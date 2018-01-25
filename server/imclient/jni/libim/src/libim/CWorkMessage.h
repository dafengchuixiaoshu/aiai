#ifndef _C_WORK_MESSAGE_H
#define _C_WORK_MESSAGE_H

#include "BizCommon.h"
#include "CWorker.h"

class CWorkMessage : public CWorker
{
public:
	CWorkMessage (void);
	~CWorkMessage (void);

	static CWorkMessage* GetInstance();

	HRESULT Init();
	VOID Release();

	HRESULT CreateTable();

	virtual HRESULT Invoke (VOID* pInvoke);
	virtual HRESULT OnNotify (VOID* pNotify);
	virtual HRESULT OnNotifyInner (VOID* pNotify) { return S_OK; }
	virtual void SendImage(string& fromuser, string& touser, string& imagepath,
			string& txtbuffer, int imagewidth, int imageheight, int msgtype);
	virtual void SendVideo(string& fromuser, string& touser, string& videopath,
			string& faceimagepath, string& txtbuffer, string& videotitle, int videolength, int msgtype);
	virtual void SetBlackListFlag(string& blackid, int type);
public:
	SessionList   GetSessionList ();
	SessionParams GetSession (const string& pSessionId);
	HRESULT DeleteSession (const string& pSessionId, BOOL bDeleteMessageLogs);
	HRESULT DeleteSessionList (const SessionList& pSessionList, BOOL bDeleteMessageLogs);
	HRESULT ClearSession (const string& pSessionId);
	HRESULT SetSessionOption (enum SessionOptionType eOptionType, SessionParams& pSessionParams);
	HRESULT SetSessionListReaded (const SessionList& pSessionList);
	UINT    GetSessionUnreadNum (const string& pSessionId);
	UINT    GetTotalUnreadNum ();
	UINT    GetSessionCount ();
	HRESULT UpdateDraft (const string& pSessionId, const string& pDraftContent);
	MessageList   GetMessageList (const string& pSessionId, const string& pLastMsgId, int nPageSize);
	MessageParams GetMessage (const string& pSessionId, const string& pMsgId, BOOL bUpdateUnreadNum);
	HRESULT DeleteMessage (const string& pSessionId, const string& pMsgId, const string& imagekey, const string& videokey);
	UINT    GetMessageCount (const string& pSessionId);
	BOOL    IsMessageExist (const string& pMsgId);
	HRESULT SetAudioPlayed (const string& pSessionId, const string& pMsgId);
	MessageList   GetGivenFieldList (const string& pSessionId, enum GivenFieldType eGivenField, enum GivenConditionType eGivenCondition);
	MessageList&  GetOfflineMessageList (const string& pSessionId);
	HRESULT       SetOfflineMessageList (MessageList& pMessageList);
	void          GetOfflineMsg();
	void          NotifySendOfflineMsgAck(list<string> msgidlist);
	void          NotifyLoginXmpp();
	void NotifyWriteMessageSuccess (const string pMsgId, const string pSessionId);
	void NotifyWriteMessageFailure (const string pMsgId, const string pSessionId);

private:
	HRESULT OnNotifyMessage (RspParams* pRspParams);

private:
	static CWorkMessage* g_pWorkMessage;

};

#endif
