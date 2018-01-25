#ifndef _C_WORK_LOGIN_H
#define _C_WORK_LOGIN_H

#include "BizCommon.h"
#include "CWorker.h"

struct HttpOutData
{
	UINT nStatusCode;
	string strData;
	UINT nDataSize;

};

class CWorkLogin : public CWorker
{
public:
	CWorkLogin (void);
	~CWorkLogin (void);

public:
	static CWorkLogin* GetInstance();
	VOID Release();

	virtual HRESULT Invoke (VOID* pInvoke);
	virtual HRESULT OnNotify (VOID* pNotify);
	virtual HRESULT OnNotifyInner (VOID* pNotify) { return S_OK; }

	HRESULT NotifyNetworkStatus(enum NetWorkStatusType eNetworkStatusType);

public:
	static void* RunEntry (void* pv);

	struct HttpOutData ConnectHttpServer(string& strAddr, UINT nPort, string& strURL,
			list<string>& listHeader, string& strBody, UINT nBodyLen,
			int methodType, BOOL bSetCookie = FALSE, BOOL bSSL = FALSE);
	BOOL ParseGD(string& strBody);		///< 解析GD返回数据

	// Login operator
	HRESULT Login ();
	HRESULT Logout ();
	HRESULT CancelLogin ();
	HRESULT LoginGDServer ();
	HRESULT LoginXmppServer ();
	HRESULT LogoutXmppServer ();
	HRESULT OnNotifyLogin (RspParams* pRspParams);
	void    NotifyLoginFailure ();
	HRESULT LoadBuddyList ();

private:
	static CWorkLogin* g_pWorkLogin;
	pthread_t m_pthread;	///< 线程句柄
	BOOL m_bThreadRunning;
	BOOL m_bNeedGDLogin;
};

#endif
