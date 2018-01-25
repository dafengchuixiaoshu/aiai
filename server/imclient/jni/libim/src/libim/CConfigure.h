#ifndef _C_CONFIGURE_H
#define _C_CONFIGURE_H

#include "BizCommon.h"
#include "IConfigure.h"

class CConfigure :
	public IConfigure,
	public CMutexHelperBase
{
public:
	CConfigure(void);
	virtual ~CConfigure(void);

	static CConfigure* GetInstance();
	VOID Release();

	virtual const string ReadProfile (const string key);
	virtual const string WriteProfile (const string key, const string value);

public:
	static CConfigure* g_pConfigure;

protected:
	map<string, string> m_sets;
};

#endif
