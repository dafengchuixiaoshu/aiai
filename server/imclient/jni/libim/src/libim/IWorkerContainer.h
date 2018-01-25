#ifndef _I_WORKER_CONTAINER_H
#define _I_WORKER_CONTAINER_H

#include "BizCommon.h"
#include "IBase.h"
#include "IWorker.h"

// 工作者容器接口
class IWorkerContainer : public IBase
{
public:
	virtual HRESULT AddWorker (INT32 key, IWorker* wkr) = 0;
	virtual HRESULT RemoveWorker (INT32 key) = 0;
	virtual IWorker* FindWorker (INT32 key) = 0;
	virtual HRESULT Notify (VOID* pNotify) = 0;
	virtual LONG GetSize () = 0;
	virtual HRESULT ClearWorker () = 0;
};

#endif
