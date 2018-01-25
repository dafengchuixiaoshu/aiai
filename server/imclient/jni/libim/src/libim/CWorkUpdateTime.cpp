#include "BizCommon.h"
#include "CConfigure.h"
#include "SqliteImpl.h"
#include "CObserverContainer.h"
#include "CWorkUpdateTime.h"
#include "../xcap/IXCapInterface.h"

#ifndef USE_SQLITE
#include "memsave.h"
#endif

CWorkUpdateTime* CWorkUpdateTime::g_pWorkUpdateTime = NULL;

CWorkUpdateTime::CWorkUpdateTime(void)
{
}

CWorkUpdateTime::~CWorkUpdateTime(void)
{
}

CWorkUpdateTime* CWorkUpdateTime::GetInstance()
{
	if (g_pWorkUpdateTime == NULL)
	{
		g_pWorkUpdateTime = new CWorkUpdateTime();
	}
	return g_pWorkUpdateTime;
}

HRESULT CWorkUpdateTime::Init()
{
	return S_OK;
}

VOID CWorkUpdateTime::Release()
{
	if (g_pWorkUpdateTime != NULL)
	{
		delete g_pWorkUpdateTime;
		g_pWorkUpdateTime = NULL;
	}
}

HRESULT CWorkUpdateTime::CreateTable()
{
#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	if (sqliteImpl)
	{
		// 建表
		const char * sSQL1 =
				"CREATE TABLE IF NOT EXISTS [updatetime] ("
				"[rowid] INTEGER PRIMARY KEY NOT NULL,"
				"[group_updatetime] VARCHAR(255) NOT NULL DEFAULT (''), "
				"[buddys_updatetime] VARCHAR(255) NOT NULL DEFAULT ('')"
				");"
				"INSERT OR IGNORE INTO [updatetime] "
				"([rowid], [buddys_updatetime], [group_updatetime]) "
				"VALUES (1, '', '');"
				;
//		LOGD("sSQL1=%s\r\n", sSQL1);
		sqliteImpl->ExecSql(sSQL1);
	}
#endif
	return S_OK;
}

string CWorkUpdateTime::GetUpdateTime(enum UpdateTimeType type)
{
	LOGD ("Get Update Time, type=%d\r\n", type);

#ifdef USE_SQLITE
	string updatetime;
	struct query_rows_s query_rows = {};
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
    string sSQL1;
	switch (type)
	{
	case UTT_BUDDY:
		sSQL1 = StringFormat("SELECT [buddys_updatetime] FROM [updatetime];");
		break;

	case UTT_GROUP:
		sSQL1 = StringFormat("SELECT [group_updatetime] FROM [updatetime];");
		break;

	default:
		LOGD ("UpdateTimeType error\r\n");
		return "";
		break;
	}

	sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if ( query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return updatetime;
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		updatetime = sqliteImpl->GetStrValue(&query_rows, i, 0);
	}

	return updatetime;
#else
	return sMemSave.GetUpdateTime(type);
#endif
}

HRESULT CWorkUpdateTime::SetUpdateTime(enum UpdateTimeType type, const string& strUpdateTime)
{
	LOGD ("Set Update Time, type=%d\r\n", type);

#ifdef USE_SQLITE
	int nResult;
	struct query_rows_s query_rows = {};
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1;
	switch (type)
	{
	case UTT_BUDDY:
		sSQL1 = StringFormat("UPDATE [updatetime] SET [buddys_updatetime]='%s';", strUpdateTime.c_str());;
		break;

	case UTT_GROUP:
		sSQL1 = StringFormat("UPDATE [updatetime] SET [group_updatetime]='%s';", strUpdateTime.c_str());;
		break;

	default:
		LOGD ("UpdateTimeType error\r\n");
		return E_FAIL;
		break;
	}

	nResult = sqliteImpl->ExecSql(sSQL1);
	return nResult;
#else
	return sMemSave.SetUpdateTime(type, strUpdateTime);
#endif
}
