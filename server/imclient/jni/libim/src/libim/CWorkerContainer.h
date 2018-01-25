#ifndef _C_WORKER_CONTAINER_H
#define _C_WORKER_CONTAINER_H

#include "BizCommon.h"
#include "IWorker.h"
#include "IWorkerContainer.h"

using namespace std;

class CWorkerContainer : public IWorkerContainer
{
public:
	CWorkerContainer (void);
	virtual ~CWorkerContainer (void);

	static CWorkerContainer* GetInstance();
	VOID Release();

	virtual HRESULT AddWorker (INT32 key, IWorker* wkr);
	virtual HRESULT RemoveWorker (INT32 key);
	virtual IWorker* FindWorker (INT32 key);
	virtual HRESULT Notify (VOID* pNotify);
	virtual LONG    GetSize ()					{ return m_wkrs.size (); }
	virtual HRESULT ClearWorker ()				{ m_wkrs.clear (); return S_OK; }

private:
	static CWorkerContainer* g_pWorkerContainer;

protected:
	std::map<INT32, IWorker*> m_wkrs;
	unsigned int mStackLock;			// 调用次数计数	(记录Notify在堆栈中的次数，防止清除删除项时，导致更高层堆栈中对象的Notify函数错误）
};

///< 对需要线程同步的回调的处理 >
class CWorkerContainerMutex : public CWorkerContainer
{
public:
	CWorkerContainerMutex ()
	{
		pthread_mutexattr_t ma;
		pthread_mutexattr_init(&ma);
		pthread_mutexattr_settype (&ma, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init (&m_cs_oc, &ma);
	}
	virtual ~CWorkerContainerMutex ()
	{
		pthread_mutex_destroy  (&m_cs_oc);
	}
	pthread_mutex_t m_cs_oc;

public:
	virtual HRESULT AddWorker (INT32 key, IWorker* wkr)
	{
		pthread_mutex_lock (&m_cs_oc);
		HRESULT hRet = CWorkerContainer::AddWorker(key, wkr);
		pthread_mutex_unlock (&m_cs_oc);
		return hRet;
	}
	virtual HRESULT RemoveWorker (INT32 key)
	{
		pthread_mutex_lock (&m_cs_oc);
		HRESULT hRet = CWorkerContainer::RemoveWorker (key);
		pthread_mutex_unlock (&m_cs_oc);
		return hRet;
	}
	virtual IWorker* FindWorker (INT32 key)
	{
		pthread_mutex_lock (&m_cs_oc);
		IWorker* hRet = CWorkerContainer::FindWorker (key);
		pthread_mutex_unlock (&m_cs_oc);
		return hRet;
	}
	virtual LONG GetSize ()
	{
		pthread_mutex_lock (&m_cs_oc);
		LONG hRet = CWorkerContainer::GetSize ();
		pthread_mutex_unlock (&m_cs_oc);
		return hRet;
	}
	virtual HRESULT Notify (VOID* pNotify);
};

#endif
