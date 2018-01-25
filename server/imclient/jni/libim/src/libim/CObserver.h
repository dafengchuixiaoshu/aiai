#ifndef _C_OBSERVER_H
#define _C_OBSERVER_H

#include "BizCommon.h"
#include "IObserver.h"

// 观察者类
class CObserver : public IObserver
{
public:
	CObserver(void);
	~CObserver(void);

	virtual HRESULT OnNotify (VOID* pNotify) { return E_NOTIMPL; }

};

#endif
