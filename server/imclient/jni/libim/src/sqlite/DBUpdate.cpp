#include "BizCommon.h"
#include "DBUpdate.h"

DBUpdate* DBUpdate::g_pDBUpdate = NULL;

static version_params g_version_params[] = {
		{ IMI_DB_VERSION_0, DBUpdate::UpdateTablesFrom0To1 },
		{ IMI_DB_VERSION_1, DBUpdate::UpdateTablesFrom1To2 },
		{ IMI_DB_VERSION_2, DBUpdate::UpdateTablesFrom2To3 }
};

DBUpdate::DBUpdate(void)
{
}

DBUpdate::~DBUpdate(void)
{
}

DBUpdate* DBUpdate::GetInstance()
{
    if (g_pDBUpdate == NULL)
    {
    	g_pDBUpdate = new DBUpdate();
    	if (g_pDBUpdate == NULL)
    	{
    		return NULL;
    	}
    }
	return g_pDBUpdate;
}

void DBUpdate::Release()
{
	if (g_pDBUpdate)
	{
		delete g_pDBUpdate;
		g_pDBUpdate = NULL;
	}
}

int DBUpdate::CreateGlobalsTable()
{
	int nResult = 0;

#ifdef USE_SQLITE
	// create table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"CREATE TABLE IF NOT EXISTS [globals] ("
			"[rowid] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
			"[key] VARCHAR(255) NOT NULL DEFAULT (''),"
			"[value] VARCHAR(255) NOT NULL DEFAULT (''),"
			"[create_time] TIMESTAMP DEFAULT (datetime('now', 'localtime')),"
			"UNIQUE ([key] ASC)"
			"); "
			"CREATE INDEX IF NOT EXISTS [idx_globals_rowid] ON [globals] ([rowid] ASC); "
			"CREATE INDEX IF NOT EXISTS [idx_globals_sessionid] ON [globals] ([key] ASC); "
			"INSERT OR IGNORE INTO [globals] ([key], [value]) VALUES ('version', '%d'); "
			"INSERT OR IGNORE INTO [globals] ([key], [value]) VALUES ('hudongcount', '%d');"
			"INSERT OR IGNORE INTO [globals] ([key], [value]) VALUES ('recommendbuddy','%d');"
			"INSERT OR IGNORE INTO [globals] ([key], [value]) VALUES ('recvpushmsg','%d');"
			,
			IMI_DB_VERSION,0,0,0,1);
//	LOGD("sSQL1=%s\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
#endif

	return nResult;
}

int DBUpdate::GetVersion()
{
	LOGD ("Get DB Version\r\n");

	int nResult = E_FAIL;
	int nVersion = -1;

	struct query_rows_s query_rows = {};

	// query table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat ("SELECT [value] from [globals] where [key]='version';");
//	LOGD("sSQL1=%s\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		return -1;
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		nVersion       = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}

	LOGD ("DB version=%d\r\n", nVersion);
	return nVersion;
}

int DBUpdate::CheckVersion()
{
	LOGD ("Check DB Version\r\n");

	while (TRUE)
	{
		// 获取版本号，比较版本号
		int nVersion = GetVersion();
		if (nVersion < 0 || nVersion > IMI_DB_VERSION)
		{
			// 获取的版本号不在合理的范围内，即版本号应在[0, IMI_DB_VERSION)范围内
			LOGD ("DB Version isn't right\r\n");
			return -1;
		}

		LOGD ("DB Version is right\r\n");

		// 升级数据库
		if (nVersion < IMI_DB_VERSION && g_version_params[nVersion].pFunction)
		{
			LOGD("DB Update old version %d\r\n",nVersion);
			if (g_version_params[nVersion].pFunction () < 0)
			{
				LOGD("DB Update error\r\n");
				return -1;
			}
		}
		else
		{
			break;
		}
	}

	LOGD ("DB has nothing to update\r\n");
	return 0;
}

int DBUpdate::UpdateTablesFrom0To1()
{
	LOGD ("DB Update From 0 To 1...\r\n");

	int nResult = E_FAIL;

	nResult = g_pDBUpdate->UpdateMessageTableFrom0To1 ();
	if (nResult < 0)
	{
		LOGD ("Update [messages] table from 0 to 1 failed\r\n");
		return -1;
	}

	nResult = g_pDBUpdate->UpdateSessionTableFrom0To1 ();
	if (nResult < 0)
	{
		LOGD ("Update [sessions] table from 0 to 1 failed\r\n");
		return -1;
	}

	nResult = g_pDBUpdate->UpdateMemberTableFrom0To1 ();
	if (nResult < 0)
	{
		LOGD ("Update [members] table from 0 to 1 failed\r\n");
		return -1;
	}

	// update version
	nResult = g_pDBUpdate->UpdateGlobalTableFrom0To1 ();
	if (nResult < 0)
	{
		LOGD ("Update [global] table from 0 to 1 failed\r\n");
		return -1;
	}

	LOGD ("DB Update From 0 To 1 success\r\n");
	return 0;
}

int DBUpdate::UpdateTablesFrom1To2()
{
	LOGD ("DB Update From 1 To 2...\r\n");

	int nResult = E_FAIL;

	nResult = g_pDBUpdate->UpdateSessionTableFrom1To2();
	if (nResult < 0)
	{
		LOGD ("Update [sessions] table from 1 to 2 failed\r\n");
		return -1;
	}

	nResult = g_pDBUpdate->UpdateMemberTableFrom1To2();
	if (nResult < 0)
	{
		LOGD ("Update [members] table from 1 to 2 failed\r\n");
		return -1;
	}

	nResult = g_pDBUpdate->UpdateUpdateTimeTableFrom1To2 ();
	if (nResult < 0)
	{
		LOGD ("Update [updatetime] table from 1 to 2 failed\r\n");
		return -1;
	}

	nResult = g_pDBUpdate->UpdateGlobalTableFrom1To2 ();
	if (nResult < 0)
	{
		LOGD ("Update [global] table from 1 to 2 failed\r\n");
		return -1;
	}

	LOGD ("DB Update From 1 To 2 success\r\n");

	return 0;
}

int DBUpdate::UpdateTablesFrom2To3()
{
	LOGD ("DB Update From 2 To 3...\r\n");

	int nResult = E_FAIL;

	nResult = g_pDBUpdate->UpdateGlobalTableFrom2To3 ();
	if (nResult < 0)
	{
		LOGD ("Update [global] table from 2 to 3 failed\r\n");
		return -1;
	}

	nResult = g_pDBUpdate->UpdateUpdateTimeTableFrom2To3 ();
	if (nResult < 0)
	{
		LOGD ("Update [updatetime] table from 2 to 3 failed\r\n");
		return -1;
	}

	nResult = g_pDBUpdate->DeleteOldTableFrom2To3 ();
	if (nResult < 0)
	{
		LOGD ("Delete Old Table From 2 To 3 failed\r\n");
		return -1;
	}

	LOGD ("DB Update From 2 To 3 success\r\n");

	return 0;
}

int DBUpdate::UpdateGlobalTableFrom0To1()
{
	LOGD ("Update [Global] Table From 0 To 1...\r\n");

	int nResult = E_FAIL;

	// create table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [globals] SET [value]='%d' WHERE [key]='version';"
			,
			IMI_DB_VERSION_1);
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SUCCESS\r\n");
	}
	else
	{
		LOGD ("FAILURE\r\n");
	}
	return nResult;
}

int DBUpdate::UpdateMessageTableFrom0To1()
{
	LOGD ("Update [Messages] Table From 0 To 1...\r\n");
	LOGD ("DO NOTHING\r\n");
	return S_OK;
}

int DBUpdate::UpdateSessionTableFrom0To1()
{
	LOGD ("Update [Sessions] Table From 0 To 1...\r\n");

	int nResult = E_FAIL;

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const char * sSQL1 =
			"CREATE TABLE IF NOT EXISTS [sessions_tmp] ("
			"[rowid] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
			"[session_id] VARCHAR(255) NOT NULL DEFAULT (''),"
			"[msg_id] VARCHAR(255) DEFAULT (''),"
			"[type] INTEGER NOT NULL DEFAULT (0),"
			"[kind] INTEGER NOT NULL DEFAULT (1),"
			"[unread_num] INTEGER NOT NULL DEFAULT (0),"
			"[top] INTEGER NOT NULL DEFAULT (0),"
			"[remind] INTEGER NOT NULL DEFAULT (1),"
			"[draft_content] VARCHAR(2048) DEFAULT (''),"
			"[draft_changed] INTEGER NOT NULL DEFAULT (0),"
			"[update_time] TIMESTAMP DEFAULT (''),"
			"UNIQUE ([session_id] ASC)"
			");"
			"CREATE INDEX IF NOT EXISTS [idx_sessions_rowid] ON [sessions] ([rowid] ASC); "
			"CREATE INDEX IF NOT EXISTS [idx_sessions_sessionid] ON [sessions] ([session_id] ASC); "
			"CREATE INDEX IF NOT EXISTS [idx_sessions_msgid] ON [sessions] ([msg_id] ASC); "
			"INSERT OR IGNORE INTO [sessions_tmp] "
			"SELECT [rowid], [session_id], [msg_id], [type], 0 AS [kind], [unread_num], [top], [remind],"
			"'' AS [draft_content], 0 AS [draft_changed], [update_time] "
			"from [sessions]; "
			"DROP TABLE [sessions]; "
			"ALTER TABLE [sessions_tmp] RENAME TO [sessions]; ";
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SUCCESS\r\n");
	}
	else
	{
		LOGD ("FAILURE\r\n");
	}
	return nResult;
}

int DBUpdate::UpdateMemberTableFrom0To1()
{
	LOGD ("Update [Members] Table From 0 To 1...\r\n");

	int nResult = E_FAIL;

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const char * sSQL1 =
			"CREATE TABLE IF NOT EXISTS [members_tmp] ("
			"[member_id] VARCHAR(255) NOT NULL,"
			"[member_name] VARCHAR(255),"
			"[member_card_name] VARCHAR(255),"
			"[member_portraitKey] VARCHAR(255),"
			"[member_emotionMood] VARCHAR(255),"
			"[group_id] VARCHAR(255) NOT NULL,"
			"[create_time] TIMESTAMP NOT NULL DEFAULT (((strftime('%s','now','localtime') "
			"+ (strftime('%f','now','localtime') - strftime('%S','now','localtime'))) "
			"* 1000)),"
			"PRIMARY KEY([member_id],[group_id])"
			");"
			"INSERT OR IGNORE INTO [members_tmp] "
			"SELECT [member_id], [member_name], '' AS [member_card_name], [member_portraitKey], [member_emotionMood], [group_id], [create_time]"
			"from [members]; "
			"DROP TABLE [members]; "
			"ALTER TABLE [members_tmp] RENAME TO [members]; "
			;
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SUCCESS\r\n");
	}
	else
	{
		LOGD ("FAILURE\r\n");
	}
	return nResult;
}

int DBUpdate::UpdateSessionTableFrom1To2()
{
	LOGD ("Update [Sessions] Table From 1 To 2...\r\n");

	int nResult = E_FAIL;

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const char * sSQL1 =
			"UPDATE [messages] SET [server_time]=[client_time] WHERE [server_time]=''; "
			"UPDATE [sessions] SET [update_time]="
			"(SELECT [server_time] FROM [messages] WHERE [msg_id]=[sessions].[msg_id]);";
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SUCCESS\r\n");
	}
	else
	{
		LOGD ("FAILURE\r\n");
	}
	return nResult;
}

int DBUpdate::UpdateMemberTableFrom1To2()
{
	LOGD ("Update [Members] Table From 1 To 2...\r\n");

	int nResult = E_FAIL;
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const char * sSQL1 =
			"CREATE TABLE IF NOT EXISTS [members_tmp] ("
			"[member_id]          VARCHAR(255) NOT NULL,"
			"[member_name]        VARCHAR(255),"
			"[member_card_name]   VARCHAR(255),"
			"[member_portraitKey] VARCHAR(255),"
			"[member_emotionMood] VARCHAR(255),"
			"[member_cmsid]       VARCHAR(255),"
			"[group_id]           VARCHAR(255) NOT NULL,"
			"[create_time] TIMESTAMP NOT NULL DEFAULT (((strftime('%s','now','localtime') "
			"+ (strftime('%f','now','localtime') - strftime('%S','now','localtime'))) "
			"* 1000)),"
			"PRIMARY KEY([member_id],[group_id])"
			");"
			"INSERT OR IGNORE INTO [members_tmp] "
			"SELECT [member_id], [member_name],[member_card_name],[member_portraitKey],[member_emotionMood],'' AS [member_cmsid],[group_id] , [create_time] "
			"from [members]; "
			"DROP TABLE [members]; "
			"ALTER TABLE [members_tmp] RENAME TO [members]; "
			;
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SUCCESS\r\n");
	}
	else
	{
		LOGD ("FAILURE\r\n");
	}

	string sSQL = StringFormat("SELECT [member_id] FROM [members];");
	nResult = sqliteImpl->ExecSqlWithResult(sSQL, &query_rows);

	if (nResult < 0 )
	{
		// query nothing
		LOGD ("Query member_id nothing.\r\n");
		return -1;
	}

	vector<string> memberidlist;
	for (int i = 0; i < query_rows.n_rows; i++)
	{
		memberidlist.push_back(sqliteImpl->GetStrValue(&query_rows, i, 0));
		LOGD ("memberid %s.\r\n",sqliteImpl->GetStrValue(&query_rows, i, 0).c_str());
	}

	string sSQL2;
	for(int j = 0; j < memberidlist.size();j++)
	{
		sSQL2 = StringFormat("SELECT [cms_id] from [buddys] WHERE [user_id]='%s';",memberidlist[j].c_str());
		nResult = sqliteImpl->ExecSqlWithResult(sSQL2, &query_rows);
		if (nResult < 0 )
		{
			// query nothing
			LOGD ("Query cmsid nothing\r\n");
			return -1;
		}

		if (query_rows.n_rows == 0)
		{
			continue;
		}

		string cmsid = sqliteImpl->GetStrValue(&query_rows, 0, 0);
		string sSQL3 = StringFormat("UPDATE [members] SET [member_cmsid] = '%s' WHERE [member_id]='%s';",cmsid.c_str(),memberidlist[j].c_str());
		nResult = sqliteImpl->ExecSql(sSQL3);

	}

	return nResult;
}

int DBUpdate::UpdateUpdateTimeTableFrom1To2()
{
	LOGD ("Update [UpdateTime] Table From 1 To 2...\r\n");

	int nResult = E_FAIL;

	// create table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [updatetime] SET [buddys_updatetime]=''; "
			"UPDATE [updatetime] SET [group_updatetime]='';");
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SUCCESS\r\n");
	}
	else
	{
		LOGD ("FAILURE\r\n");
	}
	return nResult;
}

int DBUpdate::UpdateGlobalTableFrom1To2()
{
	LOGD ("Update [Global] Table From 1 To 2...\r\n");

	int nResult = E_FAIL;

	// create table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [globals] SET [value]='%d' WHERE [key]='version';"
			,
			IMI_DB_VERSION_2);
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SUCCESS\r\n");
	}
	else
	{
		LOGD ("FAILURE\r\n");
	}
	return nResult;
}

int DBUpdate::UpdateGlobalTableFrom2To3()
{
	LOGD ("Update [Global] Table From 2 To 3...\r\n");

	int nResult = E_FAIL;

	// create table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [globals] SET [value]='%d' WHERE [key]='version';"
			,
			IMI_DB_VERSION_3);
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SUCCESS\r\n");
	}
	else
	{
		LOGD ("FAILURE\r\n");
	}
	return nResult;
}

int DBUpdate::UpdateUpdateTimeTableFrom2To3()
{
	LOGD ("Update [UpdateTime] Table From 2 To 3...\r\n");

	int nResult = E_FAIL;

	// create table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [updatetime] SET [buddys_updatetime]=''; "
			"UPDATE [updatetime] SET [group_updatetime]='';");
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("UpdateUpdateTimeTableFrom2To3 SUCCESS\r\n");
	}
	else
	{
		LOGD ("UpdateUpdateTimeTableFrom2To3 FAILURE\r\n");
	}
	return nResult;
}

int DBUpdate::DeleteOldTableFrom2To3()
{
	LOGD ("Delete Old Table From 2 To 3...\r\n");

	int nResult = E_FAIL;

	// create table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"DROP TABLE [buddys];"
			"DROP TABLE [fans];"
			"DROP TABLE [members];" );
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("DeleteOldTableFrom2To3 SUCCESS\r\n");
	}
	else
	{
		LOGD ("DeleteOldTableFrom2To3 FAILURE\r\n");
	}
	return nResult;
}

