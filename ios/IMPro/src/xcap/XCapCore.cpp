
#include "XCapCore.h"


CXCapConfig::CXCapConfig()
{
	m_nPort = 80;
}

void CXCapConfig::SetConfig(XCAP_PROP eXP, string csValue)
{
	if(csValue.empty())
	{
		return;
	}

	switch (eXP)
	{
		case XP_SERVER:
		{
			m_csServerAddr = csValue;
			break;
		}
		case XP_PORT:
		{
			m_nPort = _ttoi(csValue.c_str());
			break;
		}
		case XP_PASSWORD:
		{
			m_csPwd = csValue;
			break;
		}
		case XP_JID:
		{
			m_csUserID = csValue;
			break;
		}
	}

}

void CXCapAction::ActionEnd()
{
	if (NULL != m_pOwner)
	{
		m_pOwner->ActionEnd(this);
	}
}

CXCapCore* CXCapCore::GetInstance()
{
	static CXCapCore s_cHttpReqMgr;
	return &s_cHttpReqMgr;
}

void CXCapCore::ActionEnd(CXCapAction* pAction)
{
	#ifndef WIN32
	pthread_mutex_lock (&m_pmutex);
	#endif

	SYNC_FLAG sync_flag = SYNC_NONE;		///< 记录请求类型
	util::shared_ptr<CXCapAction> new_run = NULL;			///< 新的run

	for (CListXCapAction::iterator it = m_listXCapAction.begin(); it != m_listXCapAction.end(); ++it)
	{
		if ((*it).get() == pAction)
		{
			//记录当前完成的请求类型
			sync_flag = (*it)->m_eFlag;		
			//同步操作
			m_listXCapAction.erase(it);
			break;
		}
	}

	///< 查找是否用相同类型的请求在排队
	if (sync_flag != SYNC_NONE) {
		for (CListXCapAction::iterator it = m_listXCapAction.begin(); it != m_listXCapAction.end(); ++it) {
			if ( (*it)->m_eFlag == sync_flag && (*it)->m_eRunState == ACT_IDLE) {
				new_run = (*it);
				break;
			}
		}
	}

	#ifndef WIN32
	pthread_mutex_unlock (&m_pmutex);
	#endif

	///< 运行排队的请求
	if (new_run.get() != NULL) {
		new_run->StartAction ();
	}
}

void CXCapCore::Close()
{
	list<util::shared_ptr<CXCapAction> > tmpList;

	#ifndef WIN32
	pthread_mutex_lock (&m_pmutex);
	#endif

	for (CListXCapAction::iterator it = m_listXCapAction.begin(); it != m_listXCapAction.end(); ++it)
	{
		tmpList.push_back(*it);
	}

	#ifndef WIN32
	pthread_mutex_unlock (&m_pmutex);
	#endif

	//CCurlWrapper::curl_close_all_sockets ();

	list<util::shared_ptr<CXCapAction> >::iterator itB = tmpList.begin ();
	list<util::shared_ptr<CXCapAction> >::iterator itE = tmpList.end ();

	for (; itB != itE; itB ++) {
		(*itB)->set_cancel_flag (1);
		(*itB)->StopAction();
	}
}

void CXCapCore::SetNetworkStatus(bool isnet)
{
	list<util::shared_ptr<CXCapAction> > tmpList;

	#ifndef WIN32
	pthread_mutex_lock (&m_pmutex);
	#endif

	for (CListXCapAction::iterator it = m_listXCapAction.begin(); it != m_listXCapAction.end(); ++it)
	{
		tmpList.push_back(*it);
	}

	#ifndef WIN32
	pthread_mutex_unlock (&m_pmutex);
	#endif

	list<util::shared_ptr<CXCapAction> >::iterator itB = tmpList.begin ();
	list<util::shared_ptr<CXCapAction> >::iterator itE = tmpList.end ();

	for (; itB != itE; itB ++) {
		(*itB)->SetNetworkStatus(isnet);
	}
}

CXCapCore::CXCapHttpResponse CXCapCore::ConnectXCapServer(XCAP_REQ_METHOD mt,  const CHttpRequest& cReqPack,CHttpSocket& httpsocket)
{
	CXCapHttpResponse stData;
	
	string csURL;
	csURL = StringFormat("http://%s:%d/%s", m_csServerAddr.c_str(), m_nPort, cReqPack.csReqUri.c_str());

	int iBodySize = cReqPack.csBody.length();
	const char* pBody = NULL;
	string content;
	string retcontent;

	if(iBodySize > 0)
	{
		pBody = cReqPack.csBody.c_str();
		content = pBody;
	}

	BOOL ret = httpsocket.Start(m_csServerAddr.c_str(), m_nPort);

	bool flag = false;
	int retcode = 0;
	int errcode = 0;
	int extendcode = 0;
	if(ret)
	{
		flag = httpsocket.SendHttpRequest(mt,cReqPack.csReqUri,content,iBodySize,retcode,errcode,extendcode,retcontent);

		if( flag )//返回200
		{
			stData.nStatusCode = retcode;
			stData.spAutoBuffer->CopyBuffer(retcontent.c_str(),retcontent.length());
		}
		else//返回403
		{
			stData.nStatusCode = retcode;
			stData.nErrorCode = errcode;
			stData.nExtendCode = extendcode;
		}
	}
	else
	{
		LOGD ("ConnectXCapServer Error.\r\n");
		stData.nStatusCode = retcode;
		return stData;
	}

	return stData;
}

XCAP_REQ_METHOD CXCapCore::TranslateMethod(XCAP_OPER xEP)
{
	XCAP_REQ_METHOD xm = XCAP_HTTP_GET;
	switch (xEP)
	{
	case XO_GET:
		{
			xm = XCAP_HTTP_GET;
			break;
		}
	case XO_ADD:
	case XO_MODIFY:
		{
			xm = XCAP_HTTP_PUT;
			break;
		}
	case XO_DELETE:
		{
			xm = XCAP_HTTP_DELETE;
			break;
		}
	}

	return xm;
}


STDMETHODIMP CXCapCore::CRequestAction::Run(BSTR bstrCmd)
{
	CHttpRequest cReqPack = m_spRequestBuilder->GetRequestPacket();
	CXCapHttpResponse cResp = m_pOwner->ConnectXCapServer(m_nReqMthod,cReqPack,m_httpsocket);
	m_spResponseProcessor->ProcessResponse(cResp.nStatusCode,cResp.nErrorCode,cResp.nExtendCode,cResp.csHead,cResp.spAutoBuffer);

	if(cResp.nStatusCode > 0)
	{
		//结束任务
		ActionEnd();
		LOGD ("CXCapCore::CRequestAction::Run.ConnectSuccess.\r\n");
	}
	else if (cResp.nStatusCode == 0)
	{
		while(true)
		{
			if(m_isnet)
			{
				LOGD ("CXCapCore::CRequestAction::Run.NetworkOk.Retry again.\r\n");
				CHttpRequest cReqPack = m_spRequestBuilder->GetRequestPacket();
				CXCapHttpResponse cResp = m_pOwner->ConnectXCapServer(m_nReqMthod,cReqPack,m_httpsocket);
				m_spResponseProcessor->ProcessResponse(cResp.nStatusCode,cResp.nErrorCode,cResp.nExtendCode,cResp.csHead,cResp.spAutoBuffer);

				if(cResp.nStatusCode > 0)
				{
					ActionEnd();
					LOGD ("CXCapCore::CRequestAction::Run.NetworkOk.ConnectSuccess.\r\n");
					break;
				}
			}

			sleep(3*60);//3分钟试一次
			LOGD ("CXCapCore::CRequestAction::Run.Sleep 3 minutes.\r\n");
		}
	}
    return S_OK;
}
