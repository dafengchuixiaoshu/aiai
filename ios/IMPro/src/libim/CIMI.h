#ifndef _C_IMI_H
#define _C_IMI_H

#include "Defines.h"
#include "IObserver.h"

class CIMI : public IBase
{
public:
	CIMI (void);
	~CIMI (void);

	static CIMI* GetInstance();

	HRESULT Init();
	VOID Release();

	HRESULT SetRootDir(const string pRootDir);
	HRESULT SetServerAddr(const string pServerAddr, const string pServerPort);
	HRESULT SetUserId(const string pUserId);
	const string GetRootDir();
	const string GetUserId();
	HRESULT ClearUserId();
	HRESULT SetObserver(enum ObserverType eObserverType, IObserver* pUIObserver);
	HRESULT                   NotifyNetworkStatus(enum NetWorkStatusType eNetworkStatusType);
	enum ConnectionStatusType GetConnectionStatus();
	HRESULT                   CloseConnection();
	HRESULT OpenLog(BOOL bOpenOrClose);

public:
	// Login
	HRESULT Login(ReqParams* pReqParams);
	HRESULT Logout(ReqParams* pReqParams);

public:
	// Message
	SessionList   GetSessionList ();
	SessionParams GetSession (const string& pSessionId);
	HRESULT DeleteSession (const string& pSessionId, BOOL bDeleteMessageLogs);
	HRESULT DeleteSessionList (const SessionList& pSessionList, BOOL bDeleteMessageLogs);
	HRESULT ClearSession (const string& pSessionId);
	HRESULT SetSessionOption (enum SessionOptionType eOptionType, SessionParams& pSessionParams);
	HRESULT SetSessionListReaded (const SessionList& pSessionList);
	UINT    GetTotalUnreadNum ();
	HRESULT UpdateDraft (const string& pSessionId, const string& pDraftContent);
	MessageList   GetMessageList (const string& pSessionId, const string& pLastMsgId, int nPageSize);
	MessageParams GetMessage (const string& pSessionId, const string& pMsgId, BOOL bUpdateUnreadNum);
	HRESULT DeleteMessage (const string& pSessionId, const string& pMsgId, const string& imagekey, const string& videokey);
	HRESULT SetAudioPlayed (const string& pSessionId, const string& pMsgId);
	MessageList   GetGivenFieldList (const string& pSessionId, enum GivenFieldType eGivenField, enum GivenConditionType eGivenCondition);
	MessageList&  GetOfflineMessageList (const string& pSessionId);

	// Buddy
	BuddyParams GetBuddy (const string& pUserId, BOOL bLoadFromServer);
	BuddyList   GetBuddyList ();
	HRESULT     AddBuddy (const string& pBuddyId);
	HRESULT     DeleteBuddy (const string& pBuddyId);
	HRESULT     IsBuddyExist (const string& pUserId);
	HRESULT     SearchContact (const string& key);
	BuddyList   GetSearchContact();
	HRESULT     AddContact(BuddyParams& pBuddyParams);
	BuddyParams GetContact (const string& pUserId, BOOL bLoadFromServer);

	// Fans
	BuddyParams GetFans (const string& pUserId, BOOL bLoadFromServer);
	BuddyList   GetFansList ();
	int         GetNewFansCount();
	int         ClearNewFansCount();
	//hudong,Recommend buddy
	int         GetSnsCount(SnsType type);
	int         ClearSnsCount(SnsType type);

	//for IOS
	HRESULT     SetReceiveMessageOption(int brecv);
	int         GetReceiveMessageOption();


public:
	HRESULT OperateGroup(ReqParams* pReqParams);//操作群组，包括创建，邀请，修改，退出，获取群列表
	vector<GroupParams> GetGroupList();//获取群组列表
	GroupParams GetGroupInfo(string groupid);//获取群组信息
	vector<MemberParams> GetGroupMemberList(string groupid);//获取群组成员列表
	MemberParams GetGroupMemberInfo(const string inMemberId,string groupid,BOOL loadfromserver = TRUE);// 获取成员信息

public:

	void SetSavePath(string filepath);
	void SetFileServer(string ip, int port);
	void SetMapPoint(string& xpoint, string& ypoint);
	void SendImage(string& fromuser, string& touser, string& imagepath,
			string& txtbuffer, int imagewidth, int imageheight, int msgtype);
	void SendVideo(string& fromuser, string& touser, string& videopath,
			string& faceimagepath, string& txtbuffer, string& videotitle, int videolength, int msgtype);

	void SetBlackListFlag(string& blackid, int type);

private:
	static CIMI* g_imi;
	string  m_stUserId;
	string  m_stRootDir;

};

#endif
