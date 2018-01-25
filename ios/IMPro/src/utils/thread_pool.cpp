
#if 1
//#include <wchar.h>
#include "thread_pool.h"

thread_pool* thread_pool::g_threadpool = NULL;

thread_pool* thread_pool::GetInstance()
{
	if (g_threadpool == NULL)
	{
		g_threadpool = new thread_pool();
	}
	return g_threadpool;
}

thread_pool::thread_pool(void)
: m_bInited (false)
{
}

thread_pool::~thread_pool(void)
{
	ClearAll (TRUE);
}

/************************************************************************/
/* 生成一个事物标识，用来标识一个任务                                                     
*/
/************************************************************************/
BSTR thread_pool::NewWorkId (IRun* work, BSTR bstrCmd) {
	string strId;

	///< 避免重复
	static int token = 0;
	token ++;

	std::string strCmd = (const char* ) (_bstr_t ) bstrCmd;
	if (strCmd.size () > 512) {
		strCmd = strCmd.substr (0, 512);
	}

	//strId.Format ("%x%x%s", (DWORD ) work, token, strCmd.c_str ());
	//return strId.AllocSysString ();

	strId = StringFormat("%x%x%s",(DWORD ) work, token, strCmd.c_str ());
	//return strId.c_str();

	wchar_t * wc = new wchar_t[strId.size()+sizeof(wchar_t)];
	swprintf(wc,strId.size(),L"%S",(const wchar_t*)strId.c_str()); //注意大写S。。
	return wc;
}


/************************************************************************/
/*
@param:work,逻辑处理类，比如IXcapImpl
@param:bstrCmd:命令串
@param:bstrWorkID,事务ID
*/
/************************************************************************/
STDMETHODIMP thread_pool::AddWork (IRun* work, BSTR bstrCmd, BSTR bstrWorkId) {
	if (!m_bInited) {
		m_bInited = true;
//		AddObserver (this);//自己观察自己
	}

	if (work == NULL) {
		return E_INVALIDARG;
	}

	BSTR bstrTmpId = NULL;
	if (bstrWorkId == NULL) {
		bstrTmpId = bstrWorkId = NewWorkId (work, bstrCmd);//如果workID为空，则新生成一个ID标识，格式为处理类地址：命令串
	}

	MUTEX_LOCK ();
	//判断给事务是否已经在运行线程中，或者是在等待队列中，如果满足条件，则返回。

	string strWorkId = (char*)bstrWorkId;

//	if (IsRunning (strWorkId)){		///< 重复执行
//		return E_FAIL;
//	}
	
	///< 如果线程池已满，压入等待任务
	if (IsPoolFull ()) {
		m_waiting.push_back (work_descriptor (strWorkId, bstrCmd, work, NULL));
	}

	///< 如果线程池未满，执行线程
	else {
		//创建一个任务描述对象
		work_descriptor wd (strWorkId, bstrCmd, work, NULL);//最后一个参数是线程句柄，此时还没有创建线程，所以初始化为NULL
		//创建线程
		if (RunWork (wd)) {
			m_running.push_back (wd);
		}

	}


	return S_OK;
}

BOOL thread_pool::RunWork (work_descriptor& wd) {

	thread_params* tp = new thread_params;
	tp->tp = this;
	tp->wd = new work_descriptor (wd);

	pthread_create (&wd.m_pthread, 0, RunEntry, tp);

	return wd.m_pthread != NULL;
}

///< 全局线程结束回调
//extern "C" void on_thread_closed (int tid);

/************************************************************************/
/* 线程入口                                                             */
/************************************************************************/
void* thread_pool::RunEntry (void* pv) {
	thread_params* tp = (thread_params* ) pv;
	work_descriptor wd (*tp->wd);
	thread_pool* p_this = (thread_pool* ) tp->tp;
	delete tp->wd;
	delete tp;

	wd.work->Run ( (BSTR )  wd.cmd);//执行任务接口本身的逻辑处理

	//主要是自己接收这个异步通知，以便进行对线程队列的清理

	string wrokid;
	wrokid = wd.work_id;
	pthread_t cur = p_this->get_thread_and_erase (wrokid);
	//if (cur != 0) {
		pthread_detach (cur);
	//}

#if defined(ANDROID)
	///< 全局线程回调
	on_thread_closed (pthread_self ());
#endif
	return 0;
}

STDMETHODIMP thread_pool::OnNotify (BSTR bstrNotify) {
//	CDynamicFuncWrapper df (bstrNotify);
//	string strCmd = df.getFunction ();
//
//	///< 任务完成通知
//	if (strCmd == "task_finished") {
//		///< 从执行线程列表中删除
//		EraseFinished (df.getParams (0));
//		///< 执行等待执行的任务
//		RunWaitting ();
//	}

	return S_FALSE;
}

/*
 * 返回线程id，并删除线程
 */
pthread_t thread_pool::get_thread_and_erase (string id) {
	pthread_t pthread = 0;
	pthread_mutex_lock (&m_cs);

	std::list<work_descriptor>::iterator itB = m_running.begin ();
	std::list<work_descriptor>::iterator itE = m_running.end ();
	for (; itB != itE; itB ++) {
        if(wcscmp((_bstr_t )itB->work_id.c_str(), (_bstr_t )id.c_str()) == 0){
		//if (wcscmp2(itB->work_id.c_str(),  (_bstr_t )id.c_str()) == 0) {
			pthread = itB->m_pthread;
			m_running.erase (itB);
			break;
		}
	}

	pthread_mutex_unlock (&m_cs);
	return pthread;
}

/************************************************************************/
/*
任务执行完后，从运行队列中清除
*/
/************************************************************************/
void thread_pool::EraseFinished (string strID) {
	pthread_t pthread = 0;
	pthread_mutex_lock (&m_cs);

	string id;
	std::list<work_descriptor>::iterator itB = m_running.begin ();
	std::list<work_descriptor>::iterator itE = m_running.end ();
	for (; itB != itE; itB ++) {
		if (wcscmp ((_bstr_t )itB->work_id.c_str(), (_bstr_t ) strID.c_str()) == 0) {
			pthread = itB->m_pthread;
			id = itB->work_id;
			m_running.erase (itB);
			break;
		}
	}

	pthread_mutex_unlock (&m_cs);

	if (pthread != 0) {

		pthread_join (pthread, NULL);

	}
}

/************************************************************************/
/*
判断任务是否在运行线程队列，或者等待队列中
*/
/************************************************************************/
BOOL thread_pool::IsRunning (string strID) {
	MUTEX_LOCK ();
	std::list<work_descriptor>::iterator itB = m_waiting.begin ();
	std::list<work_descriptor>::iterator itE = m_waiting.end ();
	for (; itB != itE; itB ++) {
		if (wcscmp ((_bstr_t )itB->work_id.c_str(), (_bstr_t )strID.c_str()) == 0) {
			return true;
		}
	}

	itB = m_running.begin ();
	itE = m_running.end ();	
	for (; itB != itE; itB ++) {
		if (wcscmp ((_bstr_t )itB->work_id.c_str(), (_bstr_t )strID.c_str()) == 0) {
			return true;
		}
	}

	return false;
}


/************************************************************************/
/*
从等待队列中取出一个任务，并加入到运行线程队列中
*/
/************************************************************************/
void thread_pool::RunWaitting () {
	MUTEX_LOCK ();

	///< 检查是否可执行
	if (m_waiting.empty ()) {
		return;
	}
	if (IsPoolFull ()) {
		return;
	}

	///< 取第一个等待任务
	work_descriptor wd (*m_waiting.begin ());
	m_waiting.erase (m_waiting.begin ());

	///< 执行
	if (RunWork (wd)) {
		m_running.push_back (wd);
	}
}

STDMETHODIMP thread_pool::Cancel (BSTR bstrWorkId, DWORD dwTimeWait, BOOL bForce) {
	///< 锁
	pthread_mutex_lock (&m_cs);

	///< 首先查找等待队列
	std::list<work_descriptor>::iterator itB = m_waiting.begin ();
	std::list<work_descriptor>::iterator itE = m_waiting.end ();
	for (; itB != itE; itB ++) {
		if (wcscmp ((_bstr_t )itB->work_id.c_str(), bstrWorkId) == 0) {
			m_waiting.erase (itB);
			///< 解锁
			pthread_mutex_unlock (&m_cs);
			return S_OK;
		}
	}

	///< 然后检查运行中的线程
	itB = m_running.begin ();
	itE = m_running.end ();
	for (; itB != itE; itB ++) {
		if (wcscmp ((_bstr_t )itB->work_id.c_str(), bstrWorkId) == 0) {
			pthread_t thread = itB->m_pthread;
			m_running.erase (itB);

			///< 解锁
			pthread_mutex_unlock (&m_cs);
			string wrokid;
			wrokid = (char*)bstrWorkId;
			CloseThread (&thread, dwTimeWait, bForce, wrokid);
			return S_OK;
		}
	}

	///< 解锁
	pthread_mutex_unlock (&m_cs);
	///< 没有找到，已经结束了？
	return S_OK;
}	

bool thread_pool::CloseThread (pthread_t* thread, DWORD dwTime, BOOL bForce, string strWorkId) {
	
	pthread_join (*thread, NULL);
	return true;
}

string thread_pool::FindWorkId (IRun* work) {
	MUTEX_LOCK ();

	///< 首先查找等待队列
	std::list<work_descriptor>::iterator itB = m_waiting.begin ();
	std::list<work_descriptor>::iterator itE = m_waiting.end ();
	for (; itB != itE; itB ++) {
		if (itB->work == work) {
			string workid;
			workid =  itB->work_id;
           // workid((BSTR )itB->work_id);
			return workid;
		}
	}

	///< 然后检查运行中的线程
	itB = m_running.begin ();
	itE = m_running.end ();
	for (; itB != itE; itB ++) {
		if (itB->work == work) {
			string wrokid;
			wrokid =  itB->work_id;
			return wrokid;
		}
	}	

	return string ();
}

STDMETHODIMP thread_pool::CancelAll (IRun* work, DWORD dwTimeWait, BOOL bForce) {
	do {
		string strWorkId = FindWorkId (work);
		if (strWorkId.empty()) {
			return S_OK;
		}
		Cancel ( (_bstr_t )strWorkId.c_str(), dwTimeWait, bForce);
	} while (true);

	return S_OK;
}

extern "C" void ndk_log (const char* msg);
/************************************************************************/
/*
清空运行线程队列，关闭线程
*/
/************************************************************************/
void thread_pool::ClearAll (BOOL bForce) {
	///< 锁
	pthread_mutex_lock (&m_cs);

	std::list<work_descriptor>::iterator itB = m_running.begin ();
	std::list<work_descriptor>::iterator itE = m_running.end ();
	for (; itB != itE; itB ++) {
		if (itB->m_pthread == NULL) {
			continue;
		}

		///< 解锁
		pthread_t thread = itB->m_pthread;
		pthread_mutex_unlock (&m_cs);

		///< 强行关闭
		if (bForce) {
			pthread_join (thread, NULL);
		}
		///< 阻塞等待
		else {			
			pthread_join (thread, NULL);
		}

		///< 从新加锁
		pthread_mutex_lock (&m_cs);
	}

	m_running.clear ();
	///< 解锁
	pthread_mutex_unlock (&m_cs);
}

#endif
