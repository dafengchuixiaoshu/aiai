#ifndef _C_WORK_UPDATE_TIME_H
#define _C_WORK_UPDATE_TIME_H

#include "BizCommon.h"

class CWorkUpdateTime
{
public:
	CWorkUpdateTime (void);
	~CWorkUpdateTime (void);

	static CWorkUpdateTime* GetInstance();

	HRESULT Init();
	VOID Release();

	HRESULT CreateTable();

	// 获取增量更新时间
	string GetUpdateTime(enum UpdateTimeType type);
	// 更新增量更新时间
	HRESULT SetUpdateTime(enum UpdateTimeType type, const string& strUpdateTime);

public:

private:
	static CWorkUpdateTime* g_pWorkUpdateTime;

};

#endif
