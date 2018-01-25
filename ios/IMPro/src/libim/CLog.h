#ifndef _C_LOG_H
#define _C_LOG_H

#include "BizCommon.h"

#if defined(USE_LOG)

extern BOOL g_bOpenLog;

class CLog
{
public:
	CLog (void);
	~CLog (void);

	static CLog* GetInstance();

	HRESULT OpenLog(BOOL bOpenLog);

	HRESULT Init();
	VOID Release();

	HRESULT OpenFile();

	HRESULT PrintLog (const CHAR* content);
	HRESULT WriteRow (const CHAR* content);

	static HRESULT Logp (const char *fmt, ...);
	static HRESULT Logd (const char *fmt, ...);

private:
	static CLog* g_pLog;

private:
	FILE * m_fp;
};

#endif

#endif
