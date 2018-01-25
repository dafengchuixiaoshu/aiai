#include "sqlite3.h"
#include "BizCommon.h"
#include "CConfigure.h"
#include "SqliteImpl.h"

static int _sql_callback(void * user, int n_columns, char ** column_values, char ** column_names);

SqliteImpl* SqliteImpl::g_pSqliteImpl = NULL;

static 	bool g_dbReleaseStatus = FALSE;

SqliteImpl::SqliteImpl(void)
{
	g_dbReleaseStatus = FALSE;
	m_strDBName = "";
	m_dbHandler = NULL;
	m_dbOpenStatus = FALSE;
}

SqliteImpl::~SqliteImpl(void)
{
}

SqliteImpl* SqliteImpl::GetInstance()
{
    if (g_pSqliteImpl == NULL)
    {
    	g_pSqliteImpl = new SqliteImpl();
    	if (g_pSqliteImpl == NULL)
    	{
    		return NULL;
    	}
    }
	return g_pSqliteImpl;
}

void SqliteImpl::ReleaseInstance()
{
	if (g_pSqliteImpl)
	{
		delete g_pSqliteImpl;
		g_pSqliteImpl = NULL;
	}
}

int SqliteImpl::OpenDBConnection()
{
	LOGD ("Open DB Connection\r\n");

	if (g_dbReleaseStatus)
	{
		LOGD ("DB release\r\n");
		return (-1);
	}

	CConfigure*        pConfigure     = CConfigure::GetInstance();
	string strRootDir = pConfigure->ReadProfile ("rootdir");
	string strUserId  = pConfigure->ReadProfile ("reg_id");
	LOGD ("strRootDir=%s, strUserId=%s\r\n", strRootDir.c_str(), strUserId.c_str());
	if (strRootDir.length() > 0 && strUserId.length() > 0)
	{
		m_strDBName = GetDBDir(strRootDir, strUserId) + CONFIG_DATABASE;
		g_dbReleaseStatus = false;
	}
	else
	{
		m_strDBName = "";
		g_dbReleaseStatus = true;
	}

	int ret = 0;
	sqlite3 ** ppdb = &m_dbHandler;
	const char* dbname = m_strDBName.c_str();

	if (m_strDBName.size() == 0)
	{
		LOGD ("DB name error\r\n");
		return (-2);
	}

	if (m_dbOpenStatus)
	{
		LOGD ("Open DB %s ALREADY\r\n", dbname);
		return 0;
	}

	//打开数据库，如果数据库不存在，会建立一个数据库
	ret = sqlite3_open(dbname, ppdb);
	if (ret != SQLITE_OK)
	{
		m_dbOpenStatus = false;
		LOGD ("Open DB %s error: %s, %s\r\n", dbname, sqlite3_errmsg(*ppdb));
		return (-1);
	}

	m_dbOpenStatus = TRUE;
	LOGD ("Open DB %s OK\r\n", dbname);

	LOGD ("bThreadSafe=%d\r\n", sqlite3_threadsafe());
	return 0;
}

int SqliteImpl::CloseDBConnection()
{
	LOGD ("Close DB Connection\r\n");

	if (g_dbReleaseStatus)
	{
		LOGD ("DB release!\n");
		return (-1);
	}

	int ret = 0;
	sqlite3 * db = m_dbHandler;

	if (db == NULL)
	{
		LOGD ("DB handler error\r\n");
		return (-2);
	}

	// 关闭数据库
	ret = sqlite3_close(db);
	if (ret != SQLITE_OK)
	{
		m_dbOpenStatus = FALSE;
		LOGD ("Close DB error: %s\r\n", sqlite3_errmsg(db));
		return (-3);
	}

	m_dbOpenStatus = FALSE;
	m_dbHandler = NULL;

	LOGD ("Close DB OK\r\n");
	return 0;
}

int SqliteImpl::GetIntValue(struct query_rows_s * query_rows, int row_index, int column_index)
{
	if (row_index < 0 || column_index < 0)
		return 0;

	if (query_rows == NULL)
		return 0;

	if (row_index >= query_rows->n_rows
			|| column_index >= query_rows->query_columns[row_index].n_columns)
		return 0;

	if (query_rows->query_columns[row_index].column_values[column_index].empty())
		return 0;

	return atoi(query_rows->query_columns[row_index].column_values[column_index].c_str());
}

string SqliteImpl::GetStrValue(struct query_rows_s * query_rows, int row_index, int column_index)
{
	if (row_index < 0 || column_index < 0)
		return "";

	if (query_rows == NULL)
		return "";

	if (row_index >= query_rows->n_rows
			|| column_index >= query_rows->query_columns[row_index].n_columns)
		return "";

	return query_rows->query_columns[row_index].column_values[column_index];
}

int SqliteImpl::ExecSql(string sSQL)
{
//	LOGD ("DB exec sql\r\n");

	m_mutex.Lock();
	if (g_dbReleaseStatus)
	{
		LOGD ("DB release\r\n");
		m_mutex.Unlock();
		return (-1);
	}

	int ret = 0;
	sqlite3 * db = m_dbHandler;
	char * pErrMsg = 0;

	if (db == NULL)
	{
		LOGD ("DB handler error,sSQL=%s.\r\n",sSQL.c_str());
		m_mutex.Unlock();
		return (-2);
	}

	if (!m_dbOpenStatus)
	{
		OpenDBConnection();
		if (!m_dbOpenStatus)
		{
			LOGD ("DB %s doesn't open\r\n", m_strDBName.c_str());
			m_mutex.Unlock();
			return (-3);
		}
	}

//	sqlite3_exec(db, "begin transaction", 0, 0, 0);//显示开启一个事务
	//操作3个表，操作中间出错时可以回滚事务“rollback transaction”

	// 执行建表SQL
	ret = sqlite3_exec(db, sSQL.c_str(), 0, 0, &pErrMsg);

//	sqlite3_exec(db, "commit transaction", 0, 0, 0);//提交事务

//	result = sqlite3_exec( db, "rollback transaction", 0, 0, &zErrorMsg ); //回滚事务

	if (ret != SQLITE_OK)
	{
		LOGD ("SQL error: %s,sSQL=%s\r\n", ((pErrMsg == 0) ? ("NULL"): (pErrMsg)),sSQL.c_str());
		if (pErrMsg)
		{
			sqlite3_free(pErrMsg);
		}
		m_mutex.Unlock();
		return (-4);
	}

	m_mutex.Unlock();
	return 0;
}

int SqliteImpl::ExecSqlWithResult(string sSQL, struct query_rows_s * query_rows)
{
//	LOGD ("DB exec sql with result\r\n");

	m_mutex.Lock();
	if (g_dbReleaseStatus)
	{
		LOGD ("DB release\r\n");
		m_mutex.Unlock();
		return (-1);
	}

	int ret = 0;
	sqlite3 * db = m_dbHandler;
	char * pErrMsg = 0;

	if (db == NULL)
	{
		LOGD ("DB handler error,sSQL=%s.\r\n",sSQL.c_str());
		m_mutex.Unlock();
		return (-2);
	}

	if (!m_dbOpenStatus)
	{
		OpenDBConnection();
		if (!m_dbOpenStatus)
		{
			LOGD ("DB %s doesn't open\r\n", m_strDBName.c_str());
			m_mutex.Unlock();
			return (-3);
		}
	}

	query_rows->n_rows = 0;
	query_rows->query_columns.clear();

//	sqlite3_exec(db, "begin transaction", 0, 0, 0);//显示开启一个事务
	//操作3个表，操作中间出错时可以回滚事务“rollback transaction”

	// 查询数据表
	ret = sqlite3_exec(db, sSQL.c_str(), _sql_callback, query_rows, &pErrMsg);

//	sqlite3_exec(db, "commit transaction", 0, 0, 0);//提交事务

//	result = sqlite3_exec( db, "rollback transaction", 0, 0, &zErrorMsg ); //回滚事务

	if (ret != SQLITE_OK)
	{
		LOGD ("SQL error: %s,sSQL=%s\r\n", ((pErrMsg == 0) ? ("NULL"): (pErrMsg)),sSQL.c_str());
		if (pErrMsg)
		{
			sqlite3_free(pErrMsg);
		}
		m_mutex.Unlock();
		return (-4);
	}

	m_mutex.Unlock();
	return 0;
}

static int _sql_callback(void * user, int n_columns, char ** column_values, char ** column_names)
{
	int i;
	struct query_rows_s * query_rows = (struct query_rows_s *)user;

//	LOGD ("n_colums=%d\r\n", n_columns);

	struct query_columns_s query_columns = {};
	query_columns.n_columns = n_columns;
	for (i=0; i<n_columns; i++)
	{
		char * column_name  = *column_names++;
		char * column_value = *column_values++;
//		LOGD ("%s=%s\r\n", column_name, column_value);

		if (column_name != NULL && column_name[0] != '\0')
		{
			// do nothing
		}
		else
		{
			query_columns.n_columns--;
			continue;
		}
		if (column_value != NULL && column_value[0] != '\0')
		{
			// do nothing
		}
		else
		{
			column_value = (char*)"";
		}

		string str_column_name  = column_name;
		string str_column_value = column_value;

		query_columns.column_names.push_back(str_column_name);
		query_columns.column_values.push_back(str_column_value);
	}

	query_rows->query_columns.push_back(query_columns);
	query_rows->n_rows++;

	return 0;
}

