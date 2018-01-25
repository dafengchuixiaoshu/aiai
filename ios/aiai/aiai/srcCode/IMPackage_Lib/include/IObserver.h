#ifndef _I_OBSERVER_H
#define _I_OBSERVER_H

#include "Defines.h"
#include "IBase.h"

// 观察者接口
class IObserver : public IBase
{
public:
	virtual HRESULT OnNotify (VOID* pNotify) = 0;
};

#endif
