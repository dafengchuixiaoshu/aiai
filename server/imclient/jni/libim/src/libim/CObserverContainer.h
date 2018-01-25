#ifndef _C_OBSERVER_CONTAINER_H
#define _C_OBSERVER_CONTAINER_H

#include "BizCommon.h"
#include "IObserver.h"
#include "IObserverContainer.h"

using namespace std;

class CObserverContainer : public IObserverContainer
{
public:
	CObserverContainer (void);
	virtual ~CObserverContainer (void);

	static CObserverContainer* GetInstance();
	VOID Release();

	virtual HRESULT AddObserver (INT32 key, IObserver* obv);
	virtual HRESULT RemoveObserver (INT32 key);
	virtual IObserver* FindObserver (INT32 key);
	virtual HRESULT Notify (VOID* pNotify);
	virtual LONG    GetSize ()					{ return m_obvs.size (); }
	virtual HRESULT ClearObserver ()				{ m_obvs.clear (); return S_OK; }

private:
	static CObserverContainer* g_pObserverContainer;

protected:
	std::map<INT32, IObserver*> m_obvs;
	unsigned int mStackLock;			// 调用次数计数	(记录Notify在堆栈中的次数，防止清除删除项时，导致更高层堆栈中对象的Notify函数错误）
};

///< 对需要线程同步的回调的处理 >
class CObserverContainerMutex : public CObserverContainer {
public:
	CObserverContainerMutex ()
	{
		pthread_mutexattr_t ma;
		pthread_mutexattr_init(&ma);
		pthread_mutexattr_settype (&ma, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init (&m_cs_oc, &ma);
	}
	~CObserverContainerMutex ()
	{
		pthread_mutex_destroy  (&m_cs_oc);
	}
	pthread_mutex_t m_cs_oc;

public:
	virtual HRESULT AddObserver (INT32 key, IObserver* obv)
	{
		pthread_mutex_lock (&m_cs_oc);
		HRESULT hRet = CObserverContainer::AddObserver (key, obv);
		pthread_mutex_unlock (&m_cs_oc);
		return hRet;
	}
	virtual HRESULT RemoveObserver (INT32 key)
	{
		pthread_mutex_lock (&m_cs_oc);
		HRESULT hRet = CObserverContainer::RemoveObserver (key);
		pthread_mutex_unlock (&m_cs_oc);
		return hRet;
	}
	virtual IObserver* FindObserver (INT32 key)
	{
		pthread_mutex_lock (&m_cs_oc);
		IObserver* hRet = CObserverContainer::FindObserver (key);
		pthread_mutex_unlock (&m_cs_oc);
		return hRet;
	}
	virtual HRESULT GetSize ()
	{
		pthread_mutex_lock (&m_cs_oc);
		LONG hRet = CObserverContainer::GetSize ();
		pthread_mutex_unlock (&m_cs_oc);
		return hRet;
	}
	virtual HRESULT Notify (VOID* pNotify);
};

#endif
