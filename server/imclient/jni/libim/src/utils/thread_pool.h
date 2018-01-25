#pragma once
#include "MutexHelper.h"
#include "CObserverContainer.h"
#include "IObserver.h"
#include <list>
#include "bstr_t.h"

class thread_pool;

class IRun
{
	public:
	/** 线程实现函数
	* @param bstrCmd			该参数未向线程池添加任务时，提供的参数，线程池透明回调回来
	*/
	virtual HRESULT Run (BSTR bstrCmd) = 0;
	virtual ~IRun();
};

class IThreadPool
{
	public:
	/** 添加任务
	* @param work				任务接口
	* @param bstrCmd			透明回传参数
	* @param bstrWorkId			任务ID（如果为空，内部自动生成ID）
	*/
	virtual HRESULT AddWork (IRun* work, BSTR bstrCmd, BSTR bstrWorkId) = 0;
	/** 取消指定ID的任务
	* @param bstrWorkId			任务ID
	* @param dwTimeWait			等待时长
	* @param bForce				是否超时强制关闭（杀线程）
	*/
	virtual HRESULT Cancel (BSTR bstrWorkId, DWORD dwTimeWait, BOOL bForce) = 0;
	/** 取消指定任务对象的全部任务
	* @param work				任务接口指针
	* @param dwTimeWait			等待时长
	* @param bForce				是否超时强制关闭（杀线程）
	*/
	virtual HRESULT CancelAll (IRun* work, DWORD dwTimeWait, BOOL bForce) = 0;

	virtual ~IThreadPool();
};

//用来描述一个任务，里面包含任务参数，和任务线程。
class work_descriptor 
{
public:
	work_descriptor () : work (NULL) {
		memset (&m_pthread, 0, sizeof (m_pthread));
	}
	work_descriptor (string bstrWorkId, BSTR bstrCmd, IRun* w, pthread_t t) {
		work_id = bstrWorkId;
		cmd = bstrCmd == NULL ? L"" : bstrCmd;
		work = w;
		m_pthread = t;
	}
	work_descriptor (const work_descriptor& wd) {
		*this = wd;
	}
	work_descriptor& operator= (const work_descriptor& wd) {
		work_id = wd.work_id;
		cmd = wd.cmd;
		work = wd.work;
		m_pthread = wd.m_pthread;
		return *this;
	}

	string work_id;		///< 任务 id
	_bstr_t cmd;			///< 任务参数
	IRun* work;				///< 任务接口
	pthread_t m_pthread;	///< 线程句柄
};

struct thread_params {
	work_descriptor* wd;
	thread_pool* tp;
};

class thread_pool :
	public CMutexHelperBase
	//public IObserver,
	//public IObserverContainer,
{
private:
	static thread_pool* g_threadpool;
public:
	thread_pool(void);
	 ~thread_pool(void);

	///< 允许的最大线程数
	enum { MAX_THREAD_SIZE = 100 };

	static thread_pool* GetInstance();

	STDMETHOD (AddWork) (IRun* work, BSTR bstrCmd, BSTR bstrWorkId);
	STDMETHOD (Cancel) (BSTR bstrWorkId, DWORD dwTimeWait, BOOL bForce);
	STDMETHOD (CancelAll) (IRun* work, DWORD dwTimeWait, BOOL bForce);
	STDMETHOD (OnNotify) (BSTR bstrNotify);

	BOOL IsPoolFull ()					{ MUTEX_LOCK (); return m_running.size () >=  MAX_THREAD_SIZE; }
	BOOL RunWork (work_descriptor& wd);
	BOOL IsRunning (string strID);

	BSTR NewWorkId (IRun* work, BSTR bstrCmd);
	static void* RunEntry (void* pv);

	void EraseFinished (string strID);
	void RunWaitting ();
	bool CloseThread (pthread_t* hThread, DWORD dwTime, BOOL bForce, string strWorkId);

	string FindWorkId (IRun* work);
	void ClearAll (BOOL bForce);

	pthread_t get_thread_and_erase (string id);

protected:
	std::list<work_descriptor> m_running;
	std::list<work_descriptor> m_waiting;
	bool m_bInited;
};
