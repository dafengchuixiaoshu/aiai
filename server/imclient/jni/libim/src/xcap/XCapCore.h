//////////////////////////////////////////////////////////////////////////
///
///	@file		XCapCore.h
///	
///	@brief		XCap协议栈 HTTP 请求流程封装
///	
///	@author		yangchuanchuan 
///		
///	@version    version:	
///	
///	@data		2010/12/6
///	
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "RequestBuilder.h"
#include "ResponseProcessor.h"
#include <list>
#include "thread_pool.h"
#include "httpsocket.h"

using std::list;

using namespace XCapStrategy;
using namespace util;


class CXCapConfig
{
public:
	CXCapConfig();
	~CXCapConfig(){}
public:
	
	void SetConfig(XCAP_PROP eXP, string bstrValue);

public:

	string m_csServerAddr;		// 服务器地址
	int m_nPort;
	string m_csUserID;		// 注意：这个变量只在后台线程中使用，以避免异步
	string m_csPwd;   //用户密码
	HTTP_PROXY_INFO m_stProxyInfo;

};

enum XCAP_REQ_METHOD
{
	XCAP_HTTP_PUT,
	XCAP_HTTP_DELETE,
	XCAP_HTTP_GET,
	XCAP_HTTP_POST
};
//////////////////////////////////////////////////////////////////////////
///< 对操作的同步类型进行处理
typedef enum{ 
	SYNC_NONE = 0,			///< 无需做同步
	SYNC_BUDDYLIST,			///< 联系人同步
} SYNC_FLAG;

///< 当前的ACT状态
typedef enum{
	ACT_IDLE,					///< 当前没有运行
	ACT_RUNNING,				///< 当前正在运行
} ACT_STATE;



class CXCapCore;
class CXCapAction
{
public:
	CXCapAction():
	  m_pOwner(NULL)
	  {
		  m_eFlag = SYNC_NONE;		///< 默认无需做同步
		  m_eRunState = ACT_IDLE;	///< 默认还没有开始运行
	  }

	  SYNC_FLAG m_eFlag;
	  ACT_STATE m_eRunState;

public:
	STDMETHOD (Run) (BSTR bstrCmd = NULL) = 0;
	virtual void set_cancel_flag (char cancel) {};			// 对于大数据量的解析，设置取消状态，防止取消任务时，线程等待

	virtual BOOL StartAction(BOOL bSync = FALSE)
	{
		if (!bSync)
		{
			//加入线程池。绝大部分都是异步执行，放前面
			thread_pool*  pool = thread_pool::GetInstance();
			if (pool != NULL)
			{
				pool->AddWork ((IRun*)this, NULL, (_bstr_t )GetActionID().c_str());
				return TRUE;
			}
		}
		else
		{
			return (Run() == S_OK)?TRUE:FALSE;
		}
		return FALSE;
	}

	virtual void StopAction()
	{
		thread_pool*  pool = thread_pool::GetInstance();
		if (pool != NULL)
		{
			pool->Cancel( (_bstr_t ) (LPCTSTR ) m_strWorkID.c_str(), INFINITE, FALSE);
		}
	}

	virtual void SetNetworkStatus(bool isnet) = 0;


	void SetOwner(CXCapCore* pOwner){m_pOwner = pOwner;}

	void ActionEnd();

	string GetActionID()
	{
		m_strWorkID = RandomString ();
		return m_strWorkID;
	}

protected:
	CXCapCore* m_pOwner;
	string m_strWorkID;
	
};

class CXCapCore : 
	public CXCapConfig
{
private:
	CXCapCore()
	{
	#ifdef WIN32
	#else
		pthread_mutexattr_t ma;
		pthread_mutexattr_init(&ma);
		pthread_mutexattr_settype (&ma, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init (&m_pmutex, &ma);		
	#endif		
	}

	   ~CXCapCore () {
		#ifdef WIN32
		#else
		pthread_mutex_destroy  (&m_pmutex);
		#endif
	   }
public:
	static CXCapCore* GetInstance();
private:
	//http响应包装类
	class CXCapHttpResponse
	{
	public:
		CXCapHttpResponse():
		  nStatusCode(0),
		  nErrorCode (0),
		  spAutoBuffer(new CAutoBuffer())
		  {
		  }

	public:
		UINT nStatusCode; //Http返回的Code
		string csHead;
        util::shared_ptr<CAutoBuffer> spAutoBuffer;
        UINT nErrorCode; //服务器返回的错误信息，补充说明nStatusCode
        UINT nExtendCode;//扩展码

	};
	
	//核心类 HTTP请求
	class CRequestAction :  public CXCapAction
	{
	public:
		CRequestAction(void)
		{
			m_isnet = true;
		}
		CRequestAction(util::shared_ptr<CRequestBuilder> spRB,util::shared_ptr<CResponseProcessor> spRP)
		{
			m_spRequestBuilder = spRB;
			m_spResponseProcessor = spRP; 
			m_isnet = true;
		}
		~CRequestAction(void){}
	public:
		void SetRequestBuilder(util::shared_ptr<CRequestBuilder> spRB){ m_spRequestBuilder = spRB;  }
		void SetResponseProcessor(util::shared_ptr<CResponseProcessor> spRP)
		{
			m_spResponseProcessor = spRP; 
		}
		
		void SetReqMethod(XCAP_REQ_METHOD mt){m_nReqMthod = mt;}
		virtual void set_cancel_flag (char cancel) {
			if (m_spResponseProcessor.get () != NULL)  {
				m_spResponseProcessor->set_cancel_flag (cancel);
			}
		};

		// 对于大数据量的解析，设置取消状态，防止取消任务时，线程等待

		virtual void StopAction()
		{
			m_httpsocket.Release();
			CXCapAction::StopAction();
		}
		virtual void SetNetworkStatus(bool isnet)
		{
			m_isnet = isnet;
		}

	public:
		STDMETHOD (Run) (BSTR bstrCmd = NULL);
	protected:
        util::shared_ptr<CRequestBuilder> m_spRequestBuilder;
        util::shared_ptr<CResponseProcessor> m_spResponseProcessor;
		XCAP_REQ_METHOD m_nReqMthod;
		CHttpSocket m_httpsocket;
		bool m_isnet;
	};

public:

	void HttpGet(util::shared_ptr<CRequestBuilder> spRB,util::shared_ptr<CResponseProcessor> spRP)
	{
		HttpRequest(XCAP_HTTP_GET,spRB,spRP);
	}

	void HttpPut(util::shared_ptr<CRequestBuilder> spRB,util::shared_ptr<CResponseProcessor> spRP)
	{
		HttpRequest(XCAP_HTTP_PUT,spRB,spRP);
	}

	void HttpDelete(util::shared_ptr<CRequestBuilder> spRB,util::shared_ptr<CResponseProcessor> spRP)
	{
		HttpRequest(XCAP_HTTP_DELETE,spRB,spRP);
	}

	void HttpPost(util::shared_ptr<CRequestBuilder> spRB,util::shared_ptr<CResponseProcessor> spRP)
	{
		HttpRequest(XCAP_HTTP_POST,spRB,spRP);
	}

	BOOL HttpRequest(XCAP_OPER xEP,util::shared_ptr<CRequestBuilder> spRB,util::shared_ptr<CResponseProcessor> spRP,
		BOOL bSync = FALSE)
	{
		return HttpRequest(TranslateMethod(xEP),spRB,spRP,bSync);
	}

	BOOL HttpRequest(XCAP_REQ_METHOD mt,util::shared_ptr<CRequestBuilder> spRB,util::shared_ptr<CResponseProcessor> spRP,BOOL bSync = FALSE)
	{
		spRB->SetUserSelfId(m_csUserID);
		spRP->SetUserSelfId(m_csUserID);
		spRP->SetRequestBuilder(spRB);

		util::shared_ptr<CRequestAction> spGetReq(new CRequestAction(spRB,spRP));
		spGetReq->SetReqMethod(mt);

		return StartAction(spGetReq,bSync);
	}

	void Close();

	void SetNetworkStatus(bool isnet);

	bool CheckWait (util::shared_ptr<CXCapAction> spReqAction) {
		///< 不需要排队的请求
		if (spReqAction->m_eFlag == SYNC_NONE) {
			return false;
		}

		///< 查找当前是否有该类型的请求在运行
		list<util::shared_ptr<CXCapAction> >::iterator itB = m_listXCapAction.begin ();
		list<util::shared_ptr<CXCapAction> >::iterator itE = m_listXCapAction.end ();
		for (; itB != itE; itB ++) {
			///< 当前存在同类型的请求，阻止新的请求运行
			if ( (*itB)->m_eFlag == spReqAction->m_eFlag) {
				return true;
			}
		}

		return false;
	}
	
	BOOL StartAction(util::shared_ptr<CXCapAction> spReqAction,BOOL bSync = FALSE)
	{

		spReqAction->SetOwner(this);

		//要同步
		#ifndef WIN32
		pthread_mutex_lock (&m_pmutex);
		#endif

		bool bRunNow = !CheckWait (spReqAction);
		m_listXCapAction.push_back(spReqAction);
		
		#ifndef WIN32 
		pthread_mutex_unlock (&m_pmutex);
		#endif

		if (bRunNow) {
			spReqAction->m_eRunState = ACT_RUNNING;
			return spReqAction->StartAction(bSync);	
		}
		else {
			return TRUE;
		}
	}

	void ActionEnd(CXCapAction* pAction);

	CXCapHttpResponse ConnectXCapServer(XCAP_REQ_METHOD mt, const CHttpRequest& cReqPack,CHttpSocket& httpsocket);

    static XCAP_REQ_METHOD TranslateMethod(XCAP_OPER xEP);
private:

	typedef list<util::shared_ptr<CXCapAction> > CListXCapAction;
	CListXCapAction m_listXCapAction;

private:
#ifdef WIN32
#else
	pthread_mutex_t m_pmutex;
#endif
	
};
