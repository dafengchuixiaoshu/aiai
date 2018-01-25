#include "BizCommon.h"
#include "CObserverContainer.h"

CObserverContainer* CObserverContainer::g_pObserverContainer = NULL;

CObserverContainer::CObserverContainer(void)
{
	mStackLock = 0;
}

CObserverContainer::~CObserverContainer(void)
{
}

CObserverContainer* CObserverContainer::GetInstance()
{
	if (g_pObserverContainer == NULL)
	{
		g_pObserverContainer = new CObserverContainer();
	}
	return g_pObserverContainer;
}

VOID CObserverContainer::Release()
{
	if (g_pObserverContainer)
	{
		delete g_pObserverContainer;
		g_pObserverContainer = NULL;
	}
}

HRESULT CObserverContainer::Notify (VOID* pNotify)
{

	// 记录this->Notify()在堆栈中的重复调用次数
	mStackLock ++;

	int nClear = -1;
	for (int i=0; i<(int)m_obvs.size (); i++) {
		if (m_obvs[i] != NULL) {
			m_obvs[i]->OnNotify (pNotify);
		}
		else {
			nClear = i;
		}
	}

//	if (mStackLock == 1		// 只有this->Notify() 处于最顶层时，才释放空节点，避免更高层堆栈中的this->Notify()出现错误
//		&& nClear >= 0 && m_obvs.size () > nClear && m_obvs[nClear] == NULL) {
//		m_obvs.erase (m_obvs.begin () + nClear);
//	}

	mStackLock --;

	return S_OK;
}

HRESULT CObserverContainer::AddObserver (INT32 key, IObserver* obv)
{
	if(NULL == obv)
	{
		LOGD ("CObserverContainer::AddObserver obv is null\r\n");
	}
	else
	{
		map<INT32, IObserver*>::iterator l_it = m_obvs.find(key);
		if(l_it == m_obvs.end())
		{
			// not found it
			m_obvs.insert(pair<INT32, IObserver*>(key, obv));
		}
		else
		{
			// found it
			m_obvs[key] = obv;
		}
	}

	return S_OK;

}

HRESULT CObserverContainer::RemoveObserver (INT32 key)
{
	map<INT32, IObserver*>::iterator l_it = m_obvs.find(key);
	if(l_it == m_obvs.end())
	{
		// not found it
		return E_FAIL;
	}
	else
	{
		// found it
		m_obvs.erase(l_it);
	}
	return S_OK;
}

IObserver* CObserverContainer::FindObserver (INT32 key)
{
	map<INT32, IObserver*>::iterator l_it = m_obvs.find(key);
	if(l_it == m_obvs.end())
	{
		// not found it
		return NULL;
	}
	else
	{
		// found it
	}
	return l_it->second;
}

HRESULT CObserverContainerMutex::Notify (VOID* pNotify)
{
	///< 锁
	pthread_mutex_lock (&m_cs_oc);
	mStackLock ++;
	int nClear = -1;

	for (int i=0; i<(int)m_obvs.size (); i++) {
		if (m_obvs[i] != NULL) {
			///< 添加引用
			IObserver* tmp_obv = m_obvs[i];
			///< 解锁
			pthread_mutex_unlock (&m_cs_oc);
			///< 调用回调
			tmp_obv->OnNotify (pNotify);
			///< 重新加锁
			pthread_mutex_lock (&m_cs_oc);
		}
		else {
			nClear = i;
		}
	}

//	if (mStackLock == 1
//		&& nClear >= 0) {
//		m_obvs.erase (m_obvs.begin () + nClear);
//	}

	mStackLock --;
	///< 解锁
	pthread_mutex_unlock (&m_cs_oc);

	return S_OK;
}
