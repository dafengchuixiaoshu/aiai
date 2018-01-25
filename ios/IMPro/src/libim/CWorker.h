#ifndef _C_WORKER_H
#define _C_WORKER_H

#include "BizCommon.h"
#include "IWorker.h"
#include "CObservercontainer.h"
#include "IInvoke.h"
#include "CObserver.h"

class CWorker :
	public IWorker,
	public CObserverContainer,
	public IInvoke,
	public CObserver
{
public:
	CWorker(void);
	virtual ~CWorker(void);

	virtual HRESULT Invoke (VOID* pInvoke);
	virtual HRESULT OnNotify (VOID* pNotify) { return E_NOTIMPL; }

};

#endif
