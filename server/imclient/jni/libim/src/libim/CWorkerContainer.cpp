#include "BizCommon.h"
#include "CWorkerContainer.h"

CWorkerContainer* CWorkerContainer::g_pWorkerContainer = NULL;

CWorkerContainer::CWorkerContainer(void)
{
	mStackLock = 0;
}

CWorkerContainer::~CWorkerContainer(void)
{
}

CWorkerContainer* CWorkerContainer::GetInstance()
{
	if (g_pWorkerContainer == NULL)
	{
		g_pWorkerContainer = new CWorkerContainer();
	}
	return g_pWorkerContainer;
}

VOID CWorkerContainer::Release()
{
	if (g_pWorkerContainer)
	{
		delete g_pWorkerContainer;
		g_pWorkerContainer = NULL;
	}
}

HRESULT CWorkerContainer::Notify (VOID* pNotify)
{

	// 记录this->Notify()在堆栈中的重复调用次数
	mStackLock ++;

	int nClear = -1;
	for (int i=0; i<(int)m_wkrs.size (); i++) {
		if (m_wkrs[i] != NULL) {
			m_wkrs[i]->OnNotify (pNotify);
		}
		else {
			nClear = i;
		}
	}

//	if (mStackLock == 1		// 只有this->Notify() 处于最顶层时，才释放空节点，避免更高层堆栈中的this->Notify()出现错误
//		&& nClear >= 0 && m_wkrs.size () > nClear && m_wkrs[nClear] == NULL) {
//		m_wkrs.erase (m_wkrs.begin () + nClear);
//	}

	mStackLock --;

	return S_OK;
}

HRESULT CWorkerContainer::AddWorker (INT32 key, IWorker* obv)
{
	map<INT32, IWorker*>::iterator l_it = m_wkrs.find(key);
	if(l_it == m_wkrs.end())
	{
		// not found it
		m_wkrs.insert(pair<INT32, IWorker*>(key, obv));
	}
	else
	{
		// found it
	}

	return S_OK;

}

HRESULT CWorkerContainer::RemoveWorker (INT32 key)
{
	map<INT32, IWorker*>::iterator l_it = m_wkrs.find(key);
	if(l_it == m_wkrs.end())
	{
		// not found it
		return E_FAIL;
	}
	else
	{
		// found it
		m_wkrs.erase(l_it);
	}
	return S_OK;
}

IWorker* CWorkerContainer::FindWorker (INT32 key)
{
	map<INT32, IWorker*>::iterator l_it = m_wkrs.find(key);
	if(l_it == m_wkrs.end())
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

HRESULT CWorkerContainerMutex::Notify (VOID* pNotify) {

	///< 锁
	pthread_mutex_lock (&m_cs_oc);
	mStackLock ++;
	int nClear = -1;

	for (int i=0; i<(int)m_wkrs.size (); i++) {
		if (m_wkrs[i] != NULL) {
			///< 添加引用
            IWorker* tmp_wkr = m_wkrs[i];
			///< 解锁
			pthread_mutex_unlock (&m_cs_oc);
			///< 调用回调
			tmp_wkr->OnNotify (pNotify);
			///< 重新加锁
			pthread_mutex_lock (&m_cs_oc);
		}
		else {
			nClear = i;
		}
	}

//	if (mStackLock == 1
//		&& nClear >= 0) {
//		m_wkrs.erase (m_wkrs.begin () + nClear);
//	}

	mStackLock --;
	///< 解锁
	pthread_mutex_unlock (&m_cs_oc);

	return S_OK;
}
