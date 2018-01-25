#ifndef _C_WORK_CONTACT_H
#define _C_WORK_CONTACT_H

#include "BizCommon.h"
#include "CWorker.h"
#include "immsg.h"

class CWorkContact : public CWorker
{
public:
	CWorkContact (void);
	~CWorkContact (void);

	static CWorkContact* GetInstance();

	HRESULT Init();
	VOID Release();

	HRESULT CreateTable();

	virtual HRESULT Invoke (VOID* pInvoke);
	virtual HRESULT OnNotify (VOID* pNotify);
	virtual HRESULT OnNotifyInner (VOID* pNotify);
	void    OnNotifyOperateContact(int oper,CIm* pim);

public:
	// buddy
	BuddyParams GetBuddy     (const string& pUserId, BOOL bLoadFromServer);
	BuddyList   GetBuddyList ();
	HRESULT     AddBuddy     (const string& pUserId);
	HRESULT     DeleteBuddy  (const string& pUserId);

	// fans
	BuddyParams GetFans           (const string& pUserId, BOOL bLoadFromServer);
	BuddyList   GetFansList       ();
	int         GetNewFansCount   ();
	VOID        ClearNewFansCount ();
	VOID        AddNewFans        (string fansid);
	BOOL        NewFansExist      (string fansid);
	VOID        DeleteNewFans     (string fansid);

	//互动消息
	VOID        ClearHudongNewsCount ();//清除互动消息数
	int         GetHudongNewsCount   ();//获取互动消息数
	VOID        SetHudongNewsCount   ();//加1

	//推荐好友
	VOID        ClearRecommendBuddy ();//清除互动消息数
	int         GetRecommendBuddy   ();//获取互动消息数
	VOID        SetRecommendBuddy   ();//加1

	//IOS消息推送
	HRESULT     SetReceiveMessageOption(int brecv,bool fromui = true);
	int         GetReceiveMessageOption();
	int         GetReceiveMessageFromServer();

	// member
	MemberParams           GetMember     (const string& pGroupId, const string& pUserId, BOOL bLoadFromServer);
	vector<MemberParams>   GetMemberList (const string& pGroupId);

	// contact
	BuddyParams GetContact (const string& pUserId, BOOL bLoadFromServer);

	// search from server
	HRESULT   SearchContact    (const string key);//info可能为账号，手机号，邮箱
	BuddyList GetSearchContact ();
	VOID      SetSearchContact (BuddyList pSearchlist);

	HRESULT NotifyNetworkStatus(enum NetWorkStatusType eNetworkStatusType);

public:
	// Load from server
	HRESULT LoadContact     (const string& pUserId);
	HRESULT LoadContactList (BOOL fromUI = TRUE);

public:
	// contact
	HRESULT     DB_AddContact      (BuddyParams& pBuddyParams);
	HRESULT     DB_DeleteContact   (const string& pUserId);
	HRESULT     DB_ModifyContact   (BuddyParams& pBuddyParams);
	BOOL        DB_IsContactExist  (const string& pUserId);
	BuddyParams DB_GetContact      (const string& pUserId);
	BuddyList   DB_GetContactList  (RelationParams& pRelationParams);

	// relation
	HRESULT        DB_AddRelation        (RelationParams& pRelationParams);
	HRESULT        DB_DeleteRelation     (RelationParams& pRelationParams);
	HRESULT        DB_DeleteRelationList (RelationParams& pRelationParams);
	HRESULT        DB_ModifyRelation     (RelationParams& pRelationParams);
	BOOL           DB_IsRelationExist    (RelationParams& pRelationParams);
	UINT           DB_GetRelationCount   (RelationParams& pRelationParams);
	RelationParams DB_GetRelation        (RelationParams& pRelationParams);
	RelationList   DB_GetRelationList    (RelationParams& pRelationParams);

	// buddy
	HRESULT DB_AddBuddy        (BuddyParams& pBuddyParams);
	HRESULT DB_DeleteBuddy     (const string& pUserId);
	HRESULT DB_DeleteBuddyList ();
	HRESULT DB_ModifyBuddy     (BuddyParams& pBuddyParams);
	BOOL    DB_IsBuddyExist    (const string& pUserId);

	// fans
	HRESULT DB_AddFans        (BuddyParams& pBuddyParams);
	HRESULT DB_DeleteFans     (const string& pUserId);
	HRESULT DB_DeleteFansList ();
	HRESULT DB_ModifyFans     (BuddyParams& pBuddyParams);
	BOOL    DB_IsFansExist    (const string& pUserId);

	// member
	HRESULT DB_AddMember        (const string& pGroupId, BuddyParams& pBuddyParams,const string& pGroupCardName);
	HRESULT DB_DeleteMember     (const string& pGroupId, const string& pUserId);
	HRESULT DB_DeleteMemberList (const string& pGroupId);
	HRESULT DB_ModifyMember     (const string& pGroupId, const string& pUserId,const string& pGroupCardName);
	BOOL    DB_IsMemberExist    (const string& pGroupId, const string& pUserId);
	UINT    DB_GetMemberCount   (const string& pGroupId);

private:
	HRESULT OnNotifyBuddy   (RspParams* pRspParams);
	HRESULT OnNotifyMessage (RspParams* pRspParams);
	HRESULT OnNotifyFans    (RspParams* pRspParams);

private:
	static CWorkContact* g_pWorkContact;
	BuddyList m_pSearchList;


};

#endif
