#ifndef _C_XMPPSTACK_H
#define _C_XMPPSTACK_H

#include "BizCommon.h"
#include "immodule.h"

class CXmppStack
{
public:
	CXmppStack();
	~CXmppStack();

	static CXmppStack* GetInstance();
	VOID Release();

private:
	static CXmppStack* g_pXmppStack;

public:
	HRESULT Start ();
	HRESULT Login ();
	HRESULT Logout ();

	void SendImage(string& fromuser, string& touser, string& imagepath,
			string& txtbuffer, int imagewidth, int imageheight, int msgtype);
	void SendVideo(string& fromuser, string& touser, string& videopath,
			string& faceimagepath, string& txtbuffer, string& videotitle, int videolength, int msgtype);

	void SetBlackListFlag(string& blackid, int type);

private:
	void NotifyLoginSuccess ();
	void NotifyLoginFailure ();
	void NotifyLoginElsewhere ();
	void NotifyLoginVersionLow ();
	void NotifyLogoutSuccess ();
	void NotifyLogoutFailure ();
	void NotifyNetworkFailure ();
	void NotifySendMessageProcess (const string pMsgId, const string pSessionId);
	void NotifySendMessageSuccess (const string pMsgId, const string pSessionId);
	void NotifySendMessageFailure (const string pMsgId, const string pSessionId);

private:
	static void OnIMModuleCallback (int msgtype, CSocketMsg& socketmsg);
	static void* NotifySendMessageProcessRunEntry (void* pv);
	static void* NotifySendMessageFailureRunEntry (void* pv);

private:
	void OnRecvMessage (CImChat* imchat);
};

#endif
