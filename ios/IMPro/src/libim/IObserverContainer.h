#ifndef _I_OBSERVER_CONTAINER_H
#define _I_OBSERVER_CONTAINER_H

#include "BizCommon.h"
#include "IBase.h"
#include "IObserver.h"

// 观察者基类接口
class IObserverContainer : public IBase
{
public:
	virtual HRESULT AddObserver (INT32 key, IObserver* obv) = 0;
	virtual HRESULT RemoveObserver (INT32 key) = 0;
	virtual IObserver* FindObserver (INT32 key) = 0;
	virtual HRESULT Notify (VOID* pNotify) = 0;
	virtual LONG GetSize () = 0;
	virtual HRESULT ClearObserver () = 0;
};

#endif
