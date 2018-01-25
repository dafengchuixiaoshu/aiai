#ifndef _C_SQLITE_IMPL_H
#define _C_SQLITE_IMPL_H

#include "BizCommon.h"
#include "mutex.h"

typedef int (*SQL_CALLBACK)(void * user, int n_columns, char ** column_values, char ** column_names);

struct query_columns_s
{
	int            n_columns;
	vector<string> column_names;
	vector<string> column_values;
};

struct query_rows_s
{
	int                            n_rows;
	vector<struct query_columns_s> query_columns;
};

class SqliteImpl
{
public:
	SqliteImpl(void);
	~SqliteImpl(void);

	static SqliteImpl* GetInstance();
	static void ReleaseInstance();

	static SqliteImpl* g_pSqliteImpl;

	CMutex m_mutex;
public:
	int OpenDBConnection();
	int CloseDBConnection();

	int ExecSql(string sSQL);
	int ExecSqlWithResult(string sSQL, struct query_rows_s * query_rows);

	int GetIntValue(struct query_rows_s * query_rows, int row_index, int column_index);
	string GetStrValue(struct query_rows_s * query_rows, int row_index, int column_index);

protected:
	string m_strDBName;
	sqlite3 * m_dbHandler;
	bool m_dbOpenStatus;
};

#endif

