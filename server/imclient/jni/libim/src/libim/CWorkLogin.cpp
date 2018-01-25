#include "BizCommon.h"
#include "CConfigure.h"
#include "Markup.h"
#include "CXmppStack.h"
#include "CWorkerContainer.h"
#include "CObserverContainer.h"
#include "CWorkLogin.h"
#include "CWorkContact.h"
#include "iXCapInterface.h"
#include "httpsocket.h"

CWorkLogin* CWorkLogin::g_pWorkLogin = NULL;

CWorkLogin::CWorkLogin(void)
{
	m_bThreadRunning = FALSE;
	m_bNeedGDLogin = TRUE;
	m_pthread = NULL;
}

CWorkLogin::~CWorkLogin(void)
{
	CancelLogin ();
	m_bThreadRunning = FALSE;
	m_bNeedGDLogin = TRUE;
}

CWorkLogin* CWorkLogin::GetInstance()
{
	if (g_pWorkLogin == NULL)
	{
		g_pWorkLogin = new CWorkLogin();
	}
	return g_pWorkLogin;
}

VOID CWorkLogin::Release()
{
	if (g_pWorkLogin != NULL)
	{
		delete g_pWorkLogin;
		g_pWorkLogin = NULL;
	}
}

HRESULT CWorkLogin::Invoke (VOID* pInvoke)
{
	ReqParams* pReqParams = (ReqParams*)pInvoke;
	enum ReqType eReqType = pReqParams->eReqType;

	switch (eReqType)
	{
	case RT_LOGIN:
		Login ();
		break;

	case RT_LOGOUT:
		Logout ();
		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT CWorkLogin::Login ()
{
	LOGD ("CWorkLogin::Login 1\r\n");

	if (m_bThreadRunning == FALSE)
	{
		LOGD ("GD Start login\r\n");
		pthread_create (&m_pthread, 0, RunEntry, this);
	}
	LOGD ("GD Start already\r\n");
	return S_OK;
}

HRESULT CWorkLogin::Logout ()
{
	LOGD ("GD logout\r\n");
//	if (m_bThreadRunning == TRUE)
//	{
//		CancelLogin ();
//	}

	return LogoutXmppServer ();
}

HRESULT CWorkLogin::LoadBuddyList ()
{
	CWorkContact*         pWorkContact         = CWorkContact::GetInstance();
	pWorkContact->LoadContactList (FALSE);
	return S_OK;
}

void* CWorkLogin::RunEntry (void* pv)
{
	HRESULT hResult;
	CWorkLogin* pWorkLogin = (CWorkLogin* ) pv;

	pWorkLogin->m_bThreadRunning = TRUE;
	pWorkLogin->m_bNeedGDLogin   = TRUE;

	pthread_detach(pthread_self());

	while (pWorkLogin->m_bNeedGDLogin)
	{
		hResult = pWorkLogin->LoginGDServer ();
		if (hResult == S_OK)
		{
			// GD login success
			break;
		}
		sleep (3*60);  // sleep 3 minutes for retry
	}

	// Load buddy list from server
	pWorkLogin->LoadBuddyList ();

END:
#if defined(ANDROID)
	///< 全局线程回调
	on_thread_closed (pthread_self ());
#endif

	pWorkLogin->m_bThreadRunning = FALSE;
	return NULL;
}

HRESULT CWorkLogin::CancelLogin ()
{
	///< 在等待线程时，释放全局锁
	pthread_join (m_pthread, NULL);
	return S_OK;
}

HRESULT CWorkLogin::LoginGDServer ()
{
	LOGD ("GD ready to login\r\n");

	// 从配置文件获取地址和端口 ip:port
	IConfigure* pConfigure = CConfigure::GetInstance();
	string strUserId = pConfigure->ReadProfile ("reg_id");
	string strGDVer  = pConfigure->ReadProfile ("gd_ver");
	string strServer = pConfigure->ReadProfile ("server");
	LOGD ("gd_userid=%s, gd_ver=%s, gd_server=%s\r\n",
			strUserId.c_str(), strGDVer.c_str(), strServer.c_str());

	// 解析出服务器地址和端口号
	string strAddr;
	UINT nPort = 0;
	vector<string> outStrVec = split(strServer, ":");
	strAddr = outStrVec[0];
	nPort = atoi(outStrVec[1].c_str());
	if (strAddr.empty() || nPort == 0 || nPort >= 65536)
	{
		LOGD ("Addr or Port params error\r\n");
		return E_FAIL;
	}
	LOGD ("Addr=%s, Port=%d\r\n", strAddr.c_str(), nPort);

	// 设定发送到Http服务器的数据
	string strURL = "/GD_NAV";
	LOGD ("gd_url=%s\r\n", strURL.c_str());

	// 设定头部数据
	string strHeader;
	list<string> listHeader;
	strHeader = StringFormat("Host: %s:%d", strAddr.c_str(), nPort);
	listHeader.push_back(strHeader);
	strHeader = StringFormat("Accept: text/xml");
	listHeader.push_back(strHeader);

	// 请求包格式封装
	string strBody = StringFormat("<gd userid=\'%s\' ver=\'%s\' />", strUserId.c_str(), strGDVer.c_str());
	LOGD ("gd_body=%s\r\n", strBody.c_str());

	//连接GD服务器
	struct HttpOutData httpOutData = ConnectHttpServer (strAddr, nPort, strURL, listHeader,
			strBody, strBody.length(), HTTP_METHOD_POST);
	//解析Http返回的数据
	if(httpOutData.nStatusCode == 200)  //200 OK
	{
		BOOL bResult = FALSE;

		//解析返回的XML数据
		bResult = ParseGD(httpOutData.strData);
		if (bResult)
		{
			LOGD("GD login success\r\n");
			return S_OK;
		}
		else
		{
			LOGD("GD login failure(parse GD failed)\r\n");
		}
	}
	else
	{
		LOGD("GD login failure(status code error), nStatusCode=%d\r\n", httpOutData.nStatusCode);
	}

	NotifyLoginFailure ();
	return E_FAIL;
}

struct HttpOutData CWorkLogin::ConnectHttpServer(string& strAddr, UINT nPort, string& strURL,
		list<string>& listHeader, string& strBody, UINT nBodyLen,
		int methodType, BOOL bSetCookie, BOOL bSSL)
{
	struct HttpOutData httpOutData = {};
	string strHttp = strAddr;

	if(bSSL)
	{
		if(strHttp.find("https://") == string::npos)
			strHttp = "https://" + strHttp + ":" + FromInt (nPort);
	}
	else
	{
		if(strHttp.find("http://") == string::npos)
			strHttp = "http://" + strHttp + ":" + FromInt (nPort);
	}

	CHttpSocket httpsocket;
	BOOL bResult = httpsocket.Start(strAddr.c_str(), nPort);
	LOGD ("HTTP Start, bResult=%d\r\n", bResult);

	int resp_code = 0;
	int err_code = 0;
	int extend_code = 0;
	string resp_body;
	if (bResult)
	{
		bResult = httpsocket.SendHttpRequest(methodType, strURL, strBody, nBodyLen, resp_code, err_code,extend_code,resp_body);
		LOGD ("HTTP Send Request, bResult=%d, resp_code=%d, err_code=%d,extend_code=%d,resp_body=%s\r\n", bResult, resp_code, err_code,extend_code,resp_body.c_str());
		if(bResult)
		{
			httpOutData.nStatusCode = resp_code;
			httpOutData.strData     = resp_body;
			httpOutData.nDataSize = resp_body.size();
		}
		else
		{
			LOGD ("HTTP Send Request Error\r\n");
		}
	}
	else
	{
		LOGD ("HTTP Start Error\r\n");
	}

	return httpOutData;
}

BOOL CWorkLogin::ParseGD(string& strBody)
{
	CConfigure*        pConfigure = CConfigure::GetInstance();

	/*
	 * <gd>
	 * <items>
	 * <item type='xcap' addr='10.0.247.194:8840'/>
	 * <item type='xmpp' addr='10.0.247.194:8800'/>
	 * </items>
	 * <pubinfo>
	 * <domain>im.izp.com</domain>
	 * </pubinfo>
	 * </gd>
	 */

	CMarkup markUP;
	markUP.SetDoc (strBody.c_str());

	if( !markUP.FindElem("gd") ) {
		return FALSE;
	}

	// 进入 items
	markUP.IntoElem();
	if( !markUP.FindElem("poolinfo") ) {
			return FALSE;
	}

	//进入item
	markUP.IntoElem();

	string strVal;
	while (markUP.FindElem("module"))
	{
		//获取属性type值
		strVal = markUP.GetAttrib ("name");

		if(strcmp(strVal.c_str(), "xcap") == 0)
		{
			string server = markUP.GetAttrib("addr");

			if (server.empty())
			{
				return FALSE;
			}

			vector<string> outStrVec = split(server, ":");

			string addr = outStrVec[0];
			string port = outStrVec[1];

			if (addr.empty())
			{
				return FALSE;
			}

			if (port.empty())
			{
				return FALSE;
			}

			//保存服务器名称
			pConfigure->WriteProfile("xcap_name", strVal);

			//获取服务器地址的格式
			string strServerAddr = _T("Cfg_") + strVal + _T("_Addr");
			//获取地址值
			pConfigure->WriteProfile(strServerAddr, addr);

			//获取服务器端口的格式
			string strServerPort = _T("Cfg_") + strVal + _T("_Port");
			//获取端口号值
			pConfigure->WriteProfile(strServerPort, port);

			//将xcap服务器地址写入协议栈
			IConfigure* pConfigure = CConfigure::GetInstance();
			string strJID = pConfigure->ReadProfile ("reg_id");

			CXCapImpl* pXCapStack = CXCapImpl::GetInstance();
			pXCapStack->put_Prop(XP_SERVER,addr.c_str());
			pXCapStack->put_Prop(XP_PORT,port.c_str());
			pXCapStack->put_Prop(XP_JID,strJID.c_str());

			LOGD ("%s:%s, %s:%s\r\n", strServerAddr.c_str(), addr.c_str(), strServerPort.c_str(), port.c_str());

		}
		else if(strcmp(strVal.c_str(), "xmpp") == 0)
		{
			string server = markUP.GetAttrib("addr");

			if (server.empty())
			{
				return FALSE;
			}

			vector<string> outStrVec = split(server, ":");

			string addr = outStrVec[0];
			string port = outStrVec[1];

			if (addr.empty())
			{
				return FALSE;
			}

			if (port.empty())
			{
				return FALSE;
			}

			//保存服务器名称
			pConfigure->WriteProfile("xmpp_name", strVal);

			//获取服务器地址的格式
			string strServerAddr = _T("Cfg_") + strVal + _T("_Addr");
			//获取地址值
			pConfigure->WriteProfile(strServerAddr, addr);

			//获取服务器端口的格式
			string strServerPort = _T("Cfg_") + strVal + _T("_Port");
			//获取端口号值
			pConfigure->WriteProfile(strServerPort, port);

			LOGD ("%s:%s, %s:%s\r\n", strServerAddr.c_str(), addr.c_str(), strServerPort.c_str(), port.c_str());

		}
	}

	markUP.OutOfElem ();
	markUP.OutOfElem ();

	// 进入 pubinfo
	markUP.IntoElem();
	if( !markUP.FindElem("pubinfo") ) {
		return FALSE;
	}

	// 进入 domain
	markUP.IntoElem();
	if( !markUP.FindElem("domain") ) {
		return FALSE;
	}

	string strDomain = markUP.GetData();

	if (strDomain.empty())
	{
		return FALSE;
	}

	//保存domain
	pConfigure->WriteProfile("domain", strDomain);

	LOGD ("domain:%s\r\n", strDomain.c_str());

	markUP.OutOfElem ();

    return TRUE;
}

HRESULT CWorkLogin::LoginXmppServer ()
{
	CXmppStack*    pXmppStack = CXmppStack::GetInstance();

	HRESULT hResult = pXmppStack->Start();
	if (hResult != S_OK)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWorkLogin::LogoutXmppServer ()
{
	CXmppStack*    pXmppStack = CXmppStack::GetInstance();
	return pXmppStack->Logout();
}

void CWorkLogin::NotifyLoginFailure ()
{
	LOGD ("XMPP Login Failure\r\n");

	RspParams rspParams;
	rspParams.eRspType = ST_LOGIN_FAILURE;
	IObserverContainer*  observerContainer = CObserverContainer::GetInstance();
	IObserver* observer = observerContainer->FindObserver(OT_LOGIN);
	if (observer)
		observer->OnNotify((VOID*)&rspParams);
}

HRESULT CWorkLogin::NotifyNetworkStatus(enum NetWorkStatusType eNetworkStatusType)
{
	LOGD ("NotifyNetworkStatus=%d\r\n", eNetworkStatusType);

	sImModule.SetNetworkStatus((int)eNetworkStatusType);

//	switch (eNetworkStatusType)
//	{
//	case NST_GOOD:
//		m_bNeedGDLogin   = TRUE;  // need GD login
//		sImModule.SetNetworkStatus(1);  // 1表示网络好
//		break;
//
//	case NST_ERROR:
//		m_bNeedGDLogin   = FALSE;  // doesn't need GD login
//		sImModule.SetNetworkStatus(0);  // 0表示网络不好
//		break;
//
//	default:
//		break;
//	}

	return S_OK;
}

HRESULT CWorkLogin::OnNotify (VOID* pNotify)
{
	RspParams* pRspParams = (RspParams*)pNotify;
	enum RspType eRspType = pRspParams->eRspType;
//	LOGD("CWorkLogin::OnNotify, eRspType=%d\r\n", eRspType);

	switch (eRspType)
	{
	case ST_NETWORK_FAILURE:
	case ST_LOGIN_SUCCESS:
	case ST_LOGIN_FAILURE:
	case ST_LOGIN_PROCESS:
	case ST_LOGIN_ELSEWHERE:
	case ST_LOGIN_VERSIONLOW:
	case ST_LOGOUT_SUCCESS:
	case ST_LOGOUT_FAILURE:
		OnNotifyLogin(pRspParams);
		break;

	default:
		break;

	}

	return S_OK;
}

HRESULT CWorkLogin::OnNotifyLogin (RspParams* pRspParams)
{
	HRESULT hResult = E_FAIL;
	IObserverContainer*  observerContainer = CObserverContainer::GetInstance();
	IObserver* observer = observerContainer->FindObserver(OT_LOGIN);
	if (observer)
	{
		hResult = observer->OnNotify((VOID*)pRspParams);
	}
	else
	{
		LOGD("OnNotifyLogin observer:%p\r\n",observer);
	}
	return hResult;
}
