#ifndef _C_DB_UPDATE_H
#define _C_DB_UPDATE_H

#include "BizCommon.h"

typedef int(*update_tables_function) (void);

// 版本数据
typedef struct
{
	int                     nVersion;         // 版本号
	update_tables_function  pFunction;        // 调用函数

} version_params;

class DBUpdate
{
public:
	DBUpdate(void);
	~DBUpdate(void);

	static DBUpdate* GetInstance();
	VOID Release();

private:
	static DBUpdate* g_pDBUpdate;

public:
	int CreateGlobalsTable();
	int GetVersion();
	int CheckVersion();

	static int UpdateTablesFrom0To1();
	static int UpdateTablesFrom1To2();
	static int UpdateTablesFrom2To3();

private:
	// 0->1
	int UpdateGlobalTableFrom0To1();
	int UpdateMessageTableFrom0To1();
	int UpdateSessionTableFrom0To1();
	int UpdateMemberTableFrom0To1();

	// 1->2
	int UpdateGlobalTableFrom1To2();
	int UpdateUpdateTimeTableFrom1To2();
	int UpdateSessionTableFrom1To2();
	int UpdateMemberTableFrom1To2();

	// 2->3
	int UpdateGlobalTableFrom2To3();
	int UpdateUpdateTimeTableFrom2To3();
	int DeleteOldTableFrom2To3();
};

#endif

