#include "BizCommon.h"
#include "CConfigure.h"
#include "SqliteImpl.h"
#include "CXmppStack.h"
#include "CObserverContainer.h"
#include "EncodeUtil.h"
#include "CWorkMessage.h"
#include "CWorkGroup.h"
#include "CWorkContact.h"
#include "../xcap/IXCapInterface.h"

#ifndef USE_SQLITE
#include "memsave.h"
#endif

CWorkContact* CWorkContact::g_pWorkContact = NULL;

template<class T>
static int find(vector<T> &vec, T data)
{
	int index = 0;
	typename vector<T>::iterator iter;
	for (iter = vec.begin(); iter != vec.end(); iter++)
	{
		if (*iter == data)
		{
			// found it
			return index;
		}

		index++;
	}

	// can't find
	return -1;
}

CWorkContact::CWorkContact(void)
{

}

CWorkContact::~CWorkContact(void)
{
}

CWorkContact* CWorkContact::GetInstance()
{
	if (g_pWorkContact == NULL)
	{
		g_pWorkContact = new CWorkContact();
	}
	return g_pWorkContact;
}

HRESULT CWorkContact::Init()
{
	return S_OK;
}

VOID CWorkContact::Release()
{
	if (g_pWorkContact != NULL)
	{
		delete g_pWorkContact;
		g_pWorkContact = NULL;
	}
}

HRESULT CWorkContact::CreateTable()
{
#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	if (sqliteImpl)
	{
		// 建表
		const char * sSQL1 =
				"CREATE TABLE IF NOT EXISTS [contacts] ("
				"[rowid] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
				"[user_id] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[user_name] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[user_name_gbk] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[subname] VARCHAR(255) DEFAULT (''),"
				"[issave] VARCHAR(5) NOT NULL DEFAULT (''),"
				"[isshare] VARCHAR(5) NOT NULL DEFAULT (''),"
				"[portrait_key] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[emotion_mood] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[cms_id] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[qrer_url] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[community_url] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[create_time] TIMESTAMP DEFAULT (datetime('now', 'localtime')),"
				"UNIQUE ([user_id] ASC)"
				");"
				"CREATE INDEX IF NOT EXISTS [idx_contacts_rowid] ON [contacts] ([rowid] ASC); "
				"CREATE INDEX IF NOT EXISTS [idx_contacts_userid] ON [contacts] ([user_id] ASC); "
				"CREATE TABLE IF NOT EXISTS [relations] ("
				"[rowid] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
				"[user_id] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[relation_id] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[relation_type] INTEGER NOT NULL DEFAULT (''),"
				"[group_cardname] VARCHAR(255) DEFAULT (''),"
				"[create_time] TIMESTAMP DEFAULT (datetime('now', 'localtime')),"
				"UNIQUE ([user_id], [relation_id], [relation_type])"
				");"
				"CREATE INDEX IF NOT EXISTS [idx_relations_rowid] ON [relations] ([rowid] ASC); "
				"CREATE INDEX IF NOT EXISTS [idx_relations_userid] ON [relations] ([user_id] ASC); "
				"CREATE INDEX IF NOT EXISTS [idx_relations_relationid] ON [relations] ([relation_id] ASC); "
				;
//		LOGD("sSQL1=%s\r\n", sSQL1);
		sqliteImpl->ExecSql(sSQL1);
	}
#endif
	return S_OK;
}

HRESULT CWorkContact::Invoke (VOID* pInvoke)
{
	ReqParams* pReqParams = (ReqParams*)pInvoke;
	enum ReqType eReqType = pReqParams->eReqType;

	switch (eReqType)
	{
	default:
		break;
	}

	return S_OK;
}

BuddyParams CWorkContact::GetBuddy (const string& pUserId, BOOL bLoadFromServer)
{
	if (bLoadFromServer)
	{
		LoadContact (pUserId);//更新最新信息
	}

	BOOL bResult = FALSE;
	BuddyParams pBuddyParams = {};

	bResult = DB_IsBuddyExist (pUserId);
	if (bResult == FALSE)
	{
		return pBuddyParams;
	}

	pBuddyParams = DB_GetContact (pUserId);
	return pBuddyParams;
}

BuddyList CWorkContact::GetBuddyList ()
{
	LOGD ("Get Buddy List\r\n");

	LoadContactList();

	CConfigure*        pConfigure = CConfigure::GetInstance();
	BuddyList pBuddyList = {};

	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pConfigure->ReadProfile ("reg_id");
	pRelationParams.eIsBuddyType = IBT_BUDDY;
	pBuddyList = DB_GetContactList (pRelationParams);
	return pBuddyList;
}

HRESULT CWorkContact::AddBuddy (const string& pUserId)
{
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	xcap->AddBuddy(pUserId);
	return S_OK;
}

HRESULT CWorkContact::DeleteBuddy (const string& pUserId)
{
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	xcap->DeleteBuddy(pUserId);
	return S_OK;
}

BuddyParams CWorkContact::GetFans (const string& pUserId, BOOL bLoadFromServer)
{
	if (bLoadFromServer)
	{
		LoadContact (pUserId);//更新最新信息
	}

	BOOL bResult = FALSE;
	BuddyParams pBuddyParams = {};

	bResult = DB_IsFansExist (pUserId);
	if (bResult == FALSE)
	{
		return pBuddyParams;
	}

	pBuddyParams = DB_GetContact (pUserId);
	return pBuddyParams;
}

BuddyList CWorkContact::GetFansList ()
{
	LOGD ("Get Fans List\r\n");

	LoadContactList();

	CConfigure*        pConfigure = CConfigure::GetInstance();
	BuddyList pBuddyList = {};

	RelationParams pRelationParams = {};
	pRelationParams.pRelationId  = pConfigure->ReadProfile ("reg_id");
	pRelationParams.eIsBuddyType = IBT_FANS;
	pBuddyList = DB_GetContactList (pRelationParams);
	return pBuddyList;
}


VOID CWorkContact::ClearNewFansCount()
{
#ifdef USE_SQLITE
	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat ("DELETE from [globals] WHERE [value]='newfans';");
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("DeleteNewFans success.\r\n");
	}
	else
	{
		LOGD ("DeleteNewFans failure.\r\n");
	}
#else
	sMemSave.UpdateGlobal("newfans", "", false);
#endif
}

int CWorkContact::GetNewFansCount()
{
#ifdef USE_SQLITE
	int nResult = E_FAIL;
	int count = 0;
	struct query_rows_s query_rows = {};
	// query table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat ("SELECT count([key]) from [globals] where [value]='newfans';");
//	LOGD("sSQL1=%s\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		return count;
		LOGD("query no fans.\r\n");
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		count = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}
	return count;
#else
	return sMemSave.GetGlobal("newfans");
#endif
}

VOID CWorkContact::AddNewFans(string fansid)
{
#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"INSERT OR IGNORE INTO [globals] ([key], [value]) VALUES ('%s', 'newfans');"
			,
			fansid.c_str());
//	LOGD("sSQL1=%s\n", sSQL.c_str());
	int nResult = sqliteImpl->ExecSql(sSQL1);

	if(nResult == S_OK)
	{
		LOGD ("AddNewFans success.\r\n");
	}
	else
	{
		LOGD ("AddNewFans failure.\r\n");
	}
#else
	sMemSave.UpdateGlobal("newfans", fansid, true);
#endif
}

BOOL CWorkContact::NewFansExist(string fansid)
{
#ifdef USE_SQLITE
	int nResult = E_FAIL;
	int count = 0;
	struct query_rows_s query_rows = {};
	// query table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat ("SELECT [key] from [globals] where [key]='%s';",fansid.c_str());
//	LOGD("sSQL1=%s\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD("query no new fans.\r\n");
		return FALSE;
	}
	else
	{
		LOGD("query new fans.\r\n");
		return TRUE;
	}
#endif
	return TRUE;
}

VOID CWorkContact::DeleteNewFans(string fansid)
{
#ifdef USE_SQLITE
	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"DELETE from [globals] WHERE [key]='%s';",fansid.c_str());
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("DeleteNewFans success.\r\n");
	}
	else
	{
		LOGD ("DeleteNewFans failure.\r\n");
	}
#else
	sMemSave.UpdateGlobal("newfans", fansid, false);
#endif
}

MemberParams CWorkContact::GetMember (const string& pGroupId, const string& pUserId, BOOL bLoadFromServer)
{
	if (bLoadFromServer)
	{
		LoadContact (pUserId);//更新最新信息
	}

	BOOL bResult = FALSE;
	MemberParams pMemberParams = {};

//	bResult = DB_IsMemberExist (pGroupId, pUserId);
//	if (bResult == FALSE)
//	{
//		return pMemberParams;
//	}

	BuddyParams pBuddyParams = {};
	pBuddyParams = DB_GetContact (pUserId);

	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pGroupId;
	pRelationParams.pRelationId  = pUserId;
	pRelationParams.eIsBuddyType = IBT_MEMBER;
	pRelationParams = DB_GetRelation (pRelationParams);

	pMemberParams.MemberID       = pBuddyParams.pUserId;
	pMemberParams.MemberName     = pBuddyParams.pUserName;
	pMemberParams.PortraitKey    = pBuddyParams.pPortraitKey;
	pMemberParams.EmotionMood    = pBuddyParams.pEmotionMood;
	pMemberParams.CmsID          = pBuddyParams.pCmsId;
	pMemberParams.IsBuddy        = pRelationParams.eIsBuddyType;
	pMemberParams.MemberCardName = pBuddyParams.pUserName;
	// if cardname is not empty, then use it
	if (!pRelationParams.pGroupCardName.empty())
	{
		pMemberParams.MemberCardName = pRelationParams.pGroupCardName;
	}
	return pMemberParams;
}

vector<MemberParams> CWorkContact::GetMemberList (const string& pGroupId)
{
	//LOGD ("Get Member List\r\n");

	vector<MemberParams> pMemberList;
	BuddyList pBuddyList = {};

	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pGroupId;
	pRelationParams.eIsBuddyType = IBT_MEMBER;
	pBuddyList = DB_GetContactList (pRelationParams);

	for (int i=0; i<pBuddyList.ppBuddyParams.size(); i++)
	{
		MemberParams pMemberParams = {};
		BuddyParams pBuddyParams = pBuddyList.ppBuddyParams[i];

		pMemberParams.MemberID       = pBuddyParams.pUserId;
		pMemberParams.MemberName     = pBuddyParams.pUserName;
		pMemberParams.PortraitKey    = pBuddyParams.pPortraitKey;
		pMemberParams.EmotionMood    = pBuddyParams.pEmotionMood;
		pMemberParams.CmsID          = pBuddyParams.pCmsId;
		pMemberParams.IsBuddy        = pBuddyParams.eIsBuddy;
		pMemberParams.MemberCardName = pBuddyParams.pCardName;

		pMemberList.push_back(pMemberParams);
	}

	return pMemberList;
}

BuddyParams CWorkContact::GetContact (const string& pUserId, BOOL bLoadFromServer)
{
	if (bLoadFromServer)
	{
		LoadContact (pUserId);//更新最新信息
	}

	return DB_GetContact (pUserId);
}

HRESULT CWorkContact::LoadContact (const string& pUserId)
{
	LOGD ("Load Contact From Server\r\n");

	CXCapImpl* xcap = CXCapImpl::GetInstance();//从服务器获取
	xcap->GetBuddyInfo(pUserId);
	return S_OK;
}

HRESULT CWorkContact::LoadContactList (BOOL fromUI)
{
	LOGD ("Load Contact List From Server\r\n");

	CXCapImpl* xcap = CXCapImpl::GetInstance();//从服务器获取
	return xcap->GetBuddyList(fromUI);
}

HRESULT CWorkContact::OnNotify (VOID* pNotify)
{
	RspParams* pRspParams = (RspParams*)pNotify;
	enum RspType eRspType = pRspParams->eRspType;

	switch (eRspType)
	{
	case ST_LOAD_BUDDY_LIST:
	case ST_LOAD_BUDDY:
	case ST_ADD_BUDDY_SUCCESS:
	case ST_ADD_BUDDY_FAILURE:
	case ST_DELETE_BUDDY_SUCCESS:
	case ST_DELETE_BUDDY_FAILURE:
	case ST_SEARCH_BUDDY_SUCCESS:
	case ST_SEARCH_BUDDY_FAILURE:
	case ST_NOTIFY_INVITE_BUDDY:
		OnNotifyBuddy (pRspParams);
		break;

	case ST_INNER_NOTIFY_GETBUDDYLIST_SUCCESS:
		{
			CWorkGroup*         pWorkGroup         = CWorkGroup::GetInstance();
			pWorkGroup->LoadGroupList(FALSE);
		}
		break;

	case ST_LOAD_FANS_LIST:
	case ST_LOAD_FANS:
	case ST_NOTIFY_ADD_FANS:
	case ST_NOTIFY_DELETE_FANS:
	case ST_NOTIFY_MODIFY_FANS:
	case ST_NOTIFY_NEW_FANS:
		OnNotifyFans (pRspParams);
		break;

	default:
		break;

	}

	return S_OK;
}

HRESULT CWorkContact::OnNotifyBuddy (RspParams* pRspParams)
{
	int nResult = E_FAIL;
	IObserverContainer*  observerContainer = CObserverContainer::GetInstance();
	IObserver* observer = observerContainer->FindObserver(OT_BUDDY);
	if (observer)
	{
		nResult = observer->OnNotify((VOID*)pRspParams);
	}
	else
	{
		LOGD ("Can't find BUDDY observer\n");
	}
	return nResult;
}

HRESULT CWorkContact::OnNotifyMessage (RspParams* pRspParams)
{
	int nResult = E_FAIL;
	IObserverContainer*  observerContainer = CObserverContainer::GetInstance();
	IObserver* observer = observerContainer->FindObserver(OT_MESSAGE);
	if (observer)
	{
		nResult = observer->OnNotify((VOID*)pRspParams);
	}
	else
	{
		LOGD ("CWorkContact: can't find MESSAGE observer\n");
	}
	return nResult;
}

HRESULT CWorkContact::OnNotifyFans (RspParams* pRspParams)
{
	int nResult = E_FAIL;
	IObserverContainer*  observerContainer = CObserverContainer::GetInstance();
	IObserver* observer = observerContainer->FindObserver(OT_FANS);
	if (observer)
	{
		nResult = observer->OnNotify((VOID*)pRspParams);
	}
	else
	{
		LOGD ("Can't find FANS observer\n");
	}
	return nResult;
}

HRESULT CWorkContact::OnNotifyInner (VOID* pNotify)
{
	IqParams* pIqParams = (IqParams*)pNotify;
	LOGD ("Fans: iq eType=%d\r\n", pIqParams->eType);
	if(pIqParams->eType == IT_CONTACT)
	{
		string content = pIqParams->pBody;
		string xmlDoc = content;
		string notifytype;
		string contactid;
		string contactname;
		string portraitkey;
		string moodphrase;
		string cmsid;

		CMarkup markup;
		markup.SetDoc(xmlDoc.c_str());

		if(markup.FindElem("app"))
		{
			contactid = markup.GetAttrib("id");
			contactname = markup.GetAttrib("name");
			portraitkey = markup.GetAttrib("key");
			moodphrase = markup.GetAttrib("info");
			cmsid = markup.GetAttrib("cmsid");

			RspParams rspParams;
			rspParams.pBuddyParams = new BuddyParams;
			rspParams.pBuddyParams->pUserId = contactid;
			rspParams.pBuddyParams->pUserName = contactname;
			rspParams.pBuddyParams->pPortraitKey = portraitkey;
			rspParams.pBuddyParams->pEmotionMood = moodphrase;
			rspParams.pBuddyParams->pCmsId = cmsid;

			notifytype = markup.GetAttrib("type");
			if(notifytype.compare("0") == 0)//收到添加粉丝通知
			{
				LOGD ("Received AddFans notify\r\n");

				BuddyParams buddyParams  = {};
				buddyParams.pUserId      = contactid;
				buddyParams.pUserName    = contactname;
				buddyParams.pPortraitKey = portraitkey;
				buddyParams.pEmotionMood = moodphrase;
				buddyParams.pCmsId       = cmsid;
				DB_AddFans(buddyParams);

				rspParams.eRspType = ST_NOTIFY_ADD_FANS;
				OnNotifyFans (&rspParams);//通知UI加到粉丝列表

				LoadContactList();//增量更新获取新粉丝
			}
//			else if(notifytype.compare("1") == 0)//对方同意
//			{
//
//				rspParams.eRspType = ST_NOTIFY_INVITE_BUDDY_AGREE;
//				OnNotifyBuddy (&rspParams);//通知UI加到新朋友
//				LOGD ("CWorkBuddy::OnNotifyInner,received Agree AddBuddy notify.\r\n");
//			}
//			else if(notifytype.compare("2") == 0)//对方拒绝
//			{
//				rspParams.eRspType = ST_NOTIFY_INVITE_BUDDY_REJECT;
//				OnNotifyBuddy (&rspParams);//通知UI加到新朋友
//				LOGD ("CWorkBuddy::OnNotifyInner,received Reject AddBuddy notify.\r\n");
//			}
			delete rspParams.pBuddyParams;
		}
		else if(markup.FindElem("del"))
		{
			LOGD ("Received DeleteFans notify\r\n");

			contactid = markup.GetAttrib("id");
			DB_DeleteFans(contactid);

//			if(!DB_IsBuddyExist(contactid))
//			{
//				CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
//				pWorkMessage->DeleteSession(contactid, TRUE);
//			}

			RspParams rspParams;
			rspParams.eRspType = ST_NOTIFY_DELETE_FANS;
			OnNotifyFans (&rspParams);//通知UI删除粉丝

			LoadContactList();//增量更新获取新粉丝
		}
		else if(markup.FindElem("mod"))
		{
			CConfigure*        pConfigure    = CConfigure::GetInstance();
			string strUserId  = pConfigure->ReadProfile ("reg_id");

			string strBuddyID     = markup.GetAttrib("id");
			string strBuddyName   = markup.GetAttrib("name");
			string strBuddyKey    = markup.GetAttrib("key");
			string strEmotionMood = markup.GetAttrib("info");

			BuddyParams buddy = {};
			buddy.pUserId      = strBuddyID.c_str();
			buddy.pUserName    = strBuddyName.c_str();
			buddy.pPortraitKey = strBuddyKey.c_str();
			buddy.pEmotionMood = strEmotionMood.c_str();
			DB_ModifyContact(buddy);

			if (DB_IsBuddyExist(strBuddyID))
			{
				RspParams rspParams;
				rspParams.eRspType = ST_NOTIFY_MODIFY_BUDDY;
				OnNotifyBuddy (&rspParams);//通知UI修改好友
			}
			if (DB_IsFansExist(strBuddyID))
			{
				RspParams rspParams;
				rspParams.eRspType = ST_NOTIFY_MODIFY_FANS;
				OnNotifyFans (&rspParams);//通知UI修改粉丝
			}
		}

	}
	else if(pIqParams->eType == IT_HUDONGNEWS)
	{
		SetHudongNewsCount();
		RspParams rspParams;
		rspParams.eRspType = ST_NOTIFY_HUDONG_NEWS;
		OnNotifyBuddy (&rspParams);//通知UI修改好友
	}

    return S_OK;
}

HRESULT CWorkContact::NotifyNetworkStatus(enum NetWorkStatusType eNetworkStatusType)
{
	LOGD ("CWorkContact::NotifyNetworkStatus=%d\r\n", eNetworkStatusType);

	string networkstatus;
	CXCapCore* pCore = CXCapCore::GetInstance();

	pCore->SetNetworkStatus((NST_NULL == eNetworkStatusType) ? false : true);

//	switch (eNetworkStatusType)
//	{
//	case NST_GOOD:
//		networkstatus = "1";
//		pCore->SetNetworkStatus(networkstatus);
//		break;
//	case NST_ERROR:
//		networkstatus = "0";
//		pCore->SetNetworkStatus(networkstatus);
//		break;
//	default:
//		break;
//	}

    return S_OK;
}


HRESULT CWorkContact::SearchContact(const string key)
{
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	return xcap->SearchBuddy(key);
}

BuddyList CWorkContact::GetSearchContact()
{
	return m_pSearchList;
}

VOID CWorkContact::SetSearchContact(BuddyList pSearchList)
{
	m_pSearchList = pSearchList;
}


HRESULT CWorkContact::DB_AddContact (BuddyParams& pBuddyParams)
{
	if (pBuddyParams.pUserId.empty() || pBuddyParams.pUserName.empty())
	{
		LOGD ("UserId or UserName params error\r\n");
		return E_FAIL;
	}

	int nResult = E_FAIL;

	string userid = pBuddyParams.pUserId;
	replace_separator2(userid,"'","''");

	string utf8 = pBuddyParams.pUserName;
	string gbk;
	EncodeUtil::Utf8ToGbk(utf8, gbk);
	if (gbk.empty())
	{
		gbk = pBuddyParams.pUserName;

	}
	replace_separator2(gbk,"'","''");

	string username = pBuddyParams.pUserName;

	replace_separator2(username,"'","''");

	string emotion = pBuddyParams.pEmotionMood;

	replace_separator2(emotion,"'","''");

	// 添加到数据库
#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"INSERT OR REPLACE INTO [contacts] "
			"([user_id], [user_name], [user_name_gbk], [subname], [issave], [isshare], [portrait_key], [emotion_mood], "
			"[cms_id], [qrer_url], [community_url]) "
			"VALUES "
			"('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s');"
			,
			userid.c_str(), username.c_str(), gbk.c_str(),
			pBuddyParams.pSubname.c_str(), pBuddyParams.pIsSave.c_str(), pBuddyParams.pIsShare.c_str(),
			pBuddyParams.pPortraitKey.c_str(), emotion.c_str(), pBuddyParams.pCmsId.c_str(),
			pBuddyParams.pQrerUrl.c_str(), pBuddyParams.pCommunityUrl.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSql(sSQL1);
	return nResult;
#else
	contact info;
	info.user_id = userid;
	info.user_name = username;
	info.user_name_gbk = gbk;
	info.portrait_key = pBuddyParams.pPortraitKey;
	info.emotion_mood = emotion;
	info.cms_id = pBuddyParams.pCmsId;
	info.qrer_url = pBuddyParams.pQrerUrl;
	info.community_url = pBuddyParams.pCommunityUrl;

	sMemSave.InsertContact(info);
	return 0;
#endif
}

HRESULT CWorkContact::DB_DeleteContact (const string& pUserId)
{
	if (pUserId.empty())
	{
		LOGD ("UserId params error\r\n");
		return E_FAIL;
	}

#ifdef USE_SQLITE
	// 添加到数据库
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat(
			"DELETE FROM [contacts] WHERE [user_id]='%s';"
			,
			pUserId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	return sqliteImpl->ExecSql(sSQL1);
#else
	sMemSave.UpdataContact(pUserId, "", "");
	return 0;
#endif
}

HRESULT CWorkContact::DB_ModifyContact (BuddyParams& pBuddyParams)
{
	if (pBuddyParams.pUserId.empty())
	{
		LOGD ("UserId params error\r\n");
		return E_FAIL;
	}

	string strUserName    = pBuddyParams.pUserName;
	string strPortraitKey = pBuddyParams.pPortraitKey;
	string strEmotionMood = pBuddyParams.pEmotionMood;

	string utf8 = strUserName;
	string gbk;
	EncodeUtil::Utf8ToGbk(utf8, gbk);
	if (gbk.empty())
	{
		gbk = strUserName;
	}

	replace_separator2(strUserName,"'","''");
	replace_separator2(strEmotionMood,"'","''");
	replace_separator2(gbk,"'","''");

#ifdef USE_SQLITE
	// 添加到数据库
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [contacts] "
			"SET [user_name]='%s', [user_name_gbk]='%s', [portrait_key]='%s', [emotion_mood]='%s' "
			"WHERE [user_id]='%s';"
			,
			strUserName.c_str(), gbk.c_str(), strPortraitKey.c_str(), strEmotionMood.c_str(),
			pBuddyParams.pUserId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	return sqliteImpl->ExecSql(sSQL1);
#else
	sMemSave.UpdataContact(pBuddyParams.pUserId, "user_name", strUserName);
	sMemSave.UpdataContact(pBuddyParams.pUserId, "user_name_gbk", gbk);
	sMemSave.UpdataContact(pBuddyParams.pUserId, "portrait_key", strPortraitKey);
	sMemSave.UpdataContact(pBuddyParams.pUserId, "emotion_mood", strEmotionMood);

	return 0;
#endif
}

BOOL CWorkContact::DB_IsContactExist (const string& pUserId)
{
	if (pUserId.empty())
	{
		LOGD ("UserId Params error\r\n");
		return FALSE;
	}

#ifdef USE_SQLITE
	int nResult = E_FAIL;
	UINT nRowNum = 0;
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat(
			"SELECT count([rowid]) FROM [contacts] WHERE [user_id]='%s';"
			,
			pUserId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return FALSE;
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		nRowNum       = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}

	return ((nRowNum == 0) ? (FALSE) : (TRUE));
#else
	return (0 == sMemSave.IsContactExist(pUserId)) ? (FALSE) : (TRUE);
#endif
}

BuddyParams CWorkContact::DB_GetContact (const string& pUserId)
{
	LOGD ("Get Contact, pUserId=%s\r\n", pUserId.c_str());

	BuddyParams pBuddyParams = {};

#ifdef USE_SQLITE
	int nResult = E_FAIL;

	struct query_rows_s query_rows = {};

	if (pUserId.empty())
	{
		LOGD ("UserId params error\r\n");
		return pBuddyParams;
	}

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"SELECT "
			"[user_id], [user_name], [portrait_key], [emotion_mood], [cms_id], "
			"[qrer_url], [community_url], [issave], [isshare], [subname] "
			"FROM [contacts] "
			"WHERE [user_id]='%s';"
			,
			pUserId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return pBuddyParams;
	}

	// query something
	for (int i=0; i<query_rows.n_rows; i++)
	{
		pBuddyParams.pUserId       =                   sqliteImpl->GetStrValue(&query_rows, i, 0);
		pBuddyParams.pUserName     =                   sqliteImpl->GetStrValue(&query_rows, i, 1);
		pBuddyParams.pPortraitKey  =                   sqliteImpl->GetStrValue(&query_rows, i, 2);
		pBuddyParams.pEmotionMood  =                   sqliteImpl->GetStrValue(&query_rows, i, 3);
		pBuddyParams.pCmsId        =                   sqliteImpl->GetStrValue(&query_rows, i, 4);
		pBuddyParams.pQrerUrl      =                   sqliteImpl->GetStrValue(&query_rows, i, 5);
		pBuddyParams.pCommunityUrl =                   sqliteImpl->GetStrValue(&query_rows, i, 6);
		pBuddyParams.pIsSave =                   sqliteImpl->GetStrValue(&query_rows, i, 7);
		pBuddyParams.pIsShare =                   sqliteImpl->GetStrValue(&query_rows, i, 8);
		pBuddyParams.pSubname =                   sqliteImpl->GetStrValue(&query_rows, i, 9);
	}
#else
	contact info;

	if(sMemSave.GetContact(info, pUserId))
	{
		pBuddyParams.pUserId       =                   info.user_id;
		pBuddyParams.pUserName     =                   info.user_name;
		pBuddyParams.pPortraitKey  =                   info.portrait_key;
		pBuddyParams.pEmotionMood  =                   info.emotion_mood;
		pBuddyParams.pCmsId        =                   info.cms_id;
		pBuddyParams.pQrerUrl      =                   info.qrer_url;
		pBuddyParams.pCommunityUrl =                   info.community_url;
	}
#endif

	return pBuddyParams;
}

BuddyList CWorkContact::DB_GetContactList (RelationParams& pRelationParams)
{
	//LOGD ("Get Contact List\r\n");

	int nResult = E_FAIL;
	BuddyList pBuddyList = {};
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1;
	switch (pRelationParams.eIsBuddyType)
	{
	case IBT_BUDDY:
		sSQL1 = StringFormat (
				"SELECT DISTINCT [contacts].[user_id], [user_name], [portrait_key], [emotion_mood], "
				"[cms_id], [qrer_url], [community_url], [contacts].[issave], [contacts].[isshare] "
				"FROM [contacts] "
				"LEFT JOIN [relations] ON [contacts].[user_id]=[relations].[relation_id] "
				"WHERE [relations].[user_id]='%s' AND [relations].[relation_type]=%d "
				"ORDER BY [user_name_gbk] ASC;"
				,
				pRelationParams.pUserId.c_str(), pRelationParams.eIsBuddyType);
		break;

	case IBT_FANS:
		sSQL1 = StringFormat (
				"SELECT DISTINCT [contacts].[user_id], [user_name], [portrait_key], [emotion_mood], "
				"[cms_id], [qrer_url], [community_url], [contacts].[issave], [contacts].[isshare] "
				"FROM [contacts] "
				"LEFT JOIN [relations] ON [contacts].[user_id]=[relations].[user_id] "
				"WHERE [relations].[relation_id]='%s' AND [relations].[relation_type]=%d "
				"ORDER BY [relations].[create_time] DESC;"
				,
				pRelationParams.pRelationId.c_str(), pRelationParams.eIsBuddyType);
		//LOGD("sSQL1=%s\r\n", sSQL1.c_str());
		break;

	case IBT_MEMBER:
		sSQL1 = StringFormat (
				"SELECT DISTINCT [contacts].[user_id], [user_name], [portrait_key], [emotion_mood], "
				"[cms_id], [qrer_url], [community_url], [relations].[user_id], [relations].[group_cardname], [contacts].[issave], [contacts].[isshare] "
				"FROM [contacts] "
				"LEFT JOIN [relations] ON [contacts].[user_id]=[relations].[relation_id] "
				"WHERE [relations].[user_id]='%s' AND [relations].[relation_type]=%d "
				"ORDER BY [user_name_gbk] ASC;"
				,
				pRelationParams.pUserId.c_str(), pRelationParams.eIsBuddyType);
		break;

	default:
		{
			LOGD ("eIsBuddyType params error\r\n");
			return pBuddyList;
		}
		break;
	}
//	LOGD("sSQL1=%s\r\n", sSQL1.c_str());
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return pBuddyList;
	}

	pBuddyList.nCount = query_rows.n_rows;

	for (int i=0; i<query_rows.n_rows; i++)
	{
		BuddyParams pBuddyParams = {};

		pBuddyParams.pUserId       =                   sqliteImpl->GetStrValue(&query_rows, i, 0);
		pBuddyParams.pUserName     =                   sqliteImpl->GetStrValue(&query_rows, i, 1);
		pBuddyParams.pPortraitKey  =                   sqliteImpl->GetStrValue(&query_rows, i, 2);
		pBuddyParams.pEmotionMood  =                   sqliteImpl->GetStrValue(&query_rows, i, 3);
		pBuddyParams.pCmsId        =                   sqliteImpl->GetStrValue(&query_rows, i, 4);
		pBuddyParams.pQrerUrl      =                   sqliteImpl->GetStrValue(&query_rows, i, 5);
		pBuddyParams.pCommunityUrl =                   sqliteImpl->GetStrValue(&query_rows, i, 6);

		// if querys a member, then uses group_cardname field
		if (pRelationParams.eIsBuddyType == IBT_MEMBER)
		{
			pBuddyParams.pGroupId  =                   sqliteImpl->GetStrValue(&query_rows, i, 7);
			string pGroupCardName  =                   sqliteImpl->GetStrValue(&query_rows, i, 8);
			if (!pGroupCardName.empty())
			{
				pBuddyParams.pCardName = Base64Decode(pGroupCardName);
			}

			pBuddyParams.pIsSave   =                   sqliteImpl->GetStrValue(&query_rows, i, 9);
			pBuddyParams.pIsShare  =                   sqliteImpl->GetStrValue(&query_rows, i, 10);
		}
		else
		{
			pBuddyParams.pIsSave   =                   sqliteImpl->GetStrValue(&query_rows, i, 7);
			pBuddyParams.pIsShare  =                   sqliteImpl->GetStrValue(&query_rows, i, 8);
		}

		pBuddyList.ppBuddyParams.push_back(pBuddyParams);
	}

	return pBuddyList;
}

HRESULT CWorkContact::DB_AddRelation (RelationParams& pRelationParams)
{
	if (pRelationParams.pUserId.empty() || pRelationParams.pRelationId.empty())
	{
		LOGD ("UserId or RelationId params error\r\n");
		return E_FAIL;
	}

	replace_separator2(pRelationParams.pGroupCardName,"'","''");

	int nResult = E_FAIL;

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"INSERT OR REPLACE INTO [relations] "
			"([user_id], [relation_id], [relation_type], [group_cardname]) "
			"VALUES "
			"('%s', '%s', %d, '%s');"
			,
			pRelationParams.pUserId.c_str(), pRelationParams.pRelationId.c_str(),
			pRelationParams.eIsBuddyType, pRelationParams.pGroupCardName.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSql(sSQL1);
	return nResult;
}

HRESULT CWorkContact::DB_DeleteRelation (RelationParams& pRelationParams)
{
	if (pRelationParams.pUserId.empty() || pRelationParams.pRelationId.empty())
	{
		LOGD ("UserId or RelationId params error\r\n");
		return E_FAIL;
	}

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat(
			"DELETE FROM [relations] "
			"WHERE [user_id]='%s' AND [relation_id]='%s' AND [relation_type]=%d;"
			,
			pRelationParams.pUserId.c_str(), pRelationParams.pRelationId.c_str(),
			pRelationParams.eIsBuddyType);
//	LOGD("sSQL1=%s\n", sSQL1);
	return sqliteImpl->ExecSql(sSQL1);
}

HRESULT CWorkContact::DB_DeleteRelationList (RelationParams& pRelationParams)
{
	LOGD ("Delete Relation List\r\n");

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1;
	switch (pRelationParams.eIsBuddyType)
	{
	case IBT_BUDDY:
	case IBT_MEMBER:
		sSQL1 = StringFormat (
				"DELETE FROM [relations] WHERE [user_id]='%s' AND [relation_type]=%d;"
				,
				pRelationParams.pUserId.c_str(), pRelationParams.eIsBuddyType);
		break;

	case IBT_FANS:
		sSQL1 = StringFormat (
				"DELETE FROM [relations] WHERE [relation_id]='%s' AND [relation_type]=%d;"
				,
				pRelationParams.pRelationId.c_str(), pRelationParams.eIsBuddyType);
		break;

	default:
		{
			LOGD ("eIsBuddyType params error\r\n");
			return E_FAIL;
		}
		break;
	}
//	LOGD("sSQL1=%s\n", sSQL1);
	return sqliteImpl->ExecSql(sSQL1);
}

HRESULT CWorkContact::DB_ModifyRelation (RelationParams& pRelationParams)
{
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1;
	switch (pRelationParams.eIsBuddyType)
	{
	case IBT_MEMBER:
		replace_separator2(pRelationParams.pGroupCardName,"'","''");
		sSQL1 = StringFormat (
				"UPDATE [relations] "
				"SET [group_cardname]='%s' "
				"WHERE [user_id]='%s' AND [relation_id]='%s' AND [relation_type]=%d;"
				,
				pRelationParams.pGroupCardName.c_str(),
				pRelationParams.pUserId.c_str(), pRelationParams.pRelationId.c_str(),
				pRelationParams.eIsBuddyType);
		break;

	default:
		{
			LOGD ("eIsBuddyType params error\r\n");
			return E_FAIL;
		}
		break;
	}
//	LOGD("sSQL1=%s\n", sSQL1);
	return sqliteImpl->ExecSql(sSQL1);
}

BOOL CWorkContact::DB_IsRelationExist (RelationParams& pRelationParams)
{
	if (pRelationParams.pUserId.empty() || pRelationParams.pRelationId.empty())
	{
		LOGD ("UserId or RelationId params error\r\n");
		return E_FAIL;
	}

	int nResult = E_FAIL;
	UINT nRowNum = 0;
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"SELECT count([rowid]) "
			"FROM [relations] "
			"WHERE [user_id]='%s' AND [relation_id]='%s' AND [relation_type]=%d;"
			,
			pRelationParams.pUserId.c_str(), pRelationParams.pRelationId.c_str(),
			pRelationParams.eIsBuddyType);
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		return FALSE;
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		nRowNum       = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}

	return ((nRowNum == 0) ? (FALSE) : (TRUE));
}

UINT CWorkContact::DB_GetRelationCount (RelationParams& pRelationParams)
{
	int nResult = E_FAIL;
	UINT nRowNum = 0;
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1;
	switch (pRelationParams.eIsBuddyType)
	{
	case IBT_BUDDY:
	case IBT_MEMBER:
		sSQL1 = StringFormat (
				"SELECT count([rowid]) "
				"FROM [relations] "
				"WHERE [user_id]='%s' AND [relation_type]=%d;"
				,
				pRelationParams.pUserId.c_str(), pRelationParams.eIsBuddyType);
		break;

	case IBT_FANS:
		sSQL1 = StringFormat (
				"SELECT count([rowid]) "
				"FROM [relations] "
				"WHERE [relation_id]='%s' AND [relation_type]=%d;"
				,
				pRelationParams.pRelationId.c_str(), pRelationParams.eIsBuddyType);
		break;

	default:
		{
			LOGD ("eIsBuddyType params error\r\n");
			return 0;
		}
		break;
	}
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return 0;
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		nRowNum       = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}

	return nRowNum;
}

RelationParams CWorkContact::DB_GetRelation (RelationParams& pRelationParams)
{
	int nResult = E_FAIL;
	RelationParams pRelationParams2 = {};
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"SELECT "
			"[user_id], [relation_id], [relation_type], [group_cardname] "
			"FROM [relations] "
			"WHERE [user_id]='%s' AND [relation_id]='%s' AND [relation_type]=%d;"
			,
			pRelationParams.pUserId.c_str(), pRelationParams.pRelationId.c_str(),
			pRelationParams.eIsBuddyType);
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return pRelationParams2;
	}

	// query something
	for (int i=0; i<query_rows.n_rows; i++)
	{
		pRelationParams2.pUserId        =                    sqliteImpl->GetStrValue(&query_rows, i, 0);
		pRelationParams2.pRelationId    =                    sqliteImpl->GetStrValue(&query_rows, i, 1);
		pRelationParams2.eIsBuddyType   = (enum IsBuddyType) sqliteImpl->GetIntValue(&query_rows, i, 2);
		pRelationParams2.pGroupCardName =       Base64Decode(sqliteImpl->GetStrValue(&query_rows, i, 3));
	}

	return pRelationParams2;
}

RelationList CWorkContact::DB_GetRelationList (RelationParams& pRelationParams)
{
	LOGD ("Get Relation List\r\n");

	int nResult = E_FAIL;
	RelationList pRelationList = {};
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1;
	switch (pRelationParams.eIsBuddyType)
	{
	case IBT_BUDDY:
	case IBT_MEMBER:
		sSQL1 = StringFormat (
				"SELECT "
				"[user_id], [relation_id], [relation_type], [group_cardname] "
				"FROM [relations] "
				"WHERE [user_id]='%s' AND [relation_type]=%d;"
				,
				pRelationParams.pUserId.c_str(), pRelationParams.eIsBuddyType);
		break;

	case IBT_FANS:
		sSQL1 = StringFormat (
				"SELECT "
				"[user_id], [relation_id], [relation_type], [group_cardname] "
				"FROM [relations] "
				"WHERE [relation_id]='%s' AND [relation_type]=%d;"
				,
				pRelationParams.pRelationId.c_str(), pRelationParams.eIsBuddyType);
		break;

	default:
		{
			LOGD ("eIsBuddyType params error\r\n");
			return pRelationList;
		}
		break;
	}
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return pRelationList;
	}

	pRelationList.nCount = query_rows.n_rows;

	for (int i=0; i<query_rows.n_rows; i++)
	{
		RelationParams pRelationParams = {};

		pRelationParams.pUserId        =                    sqliteImpl->GetStrValue(&query_rows, i, 0);
		pRelationParams.pRelationId    =                    sqliteImpl->GetStrValue(&query_rows, i, 1);
		pRelationParams.eIsBuddyType   = (enum IsBuddyType) sqliteImpl->GetIntValue(&query_rows, i, 2);
		pRelationParams.pGroupCardName = Base64Decode(      sqliteImpl->GetStrValue(&query_rows, i, 3));

		pRelationList.ppRelationParams.push_back(pRelationParams);
	}

	return pRelationList;
}

HRESULT CWorkContact::DB_AddBuddy (BuddyParams& pBuddyParams)
{
	HRESULT hResult = E_FAIL;
	CConfigure*        pConfigure = CConfigure::GetInstance();

	// add contact first
	hResult = DB_AddContact (pBuddyParams);
	if (hResult != S_OK)
	{
		return E_FAIL;
	}

	// second, add relation
	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pConfigure->ReadProfile ("reg_id");
	pRelationParams.pRelationId  = pBuddyParams.pUserId;
	pRelationParams.eIsBuddyType = IBT_BUDDY;
	return DB_AddRelation (pRelationParams);
}

HRESULT CWorkContact::DB_DeleteBuddy (const string& pUserId)
{
	CConfigure*        pConfigure = CConfigure::GetInstance();

	// just only delete relation
	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pConfigure->ReadProfile ("reg_id");
	pRelationParams.pRelationId  = pUserId;
	pRelationParams.eIsBuddyType = IBT_BUDDY;
	return DB_DeleteRelation (pRelationParams);
}

HRESULT CWorkContact::DB_DeleteBuddyList ()
{
	CConfigure*        pConfigure = CConfigure::GetInstance();

	// just only delete relation
	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pConfigure->ReadProfile ("reg_id");
	pRelationParams.eIsBuddyType = IBT_BUDDY;
	return DB_DeleteRelationList (pRelationParams);
}

HRESULT CWorkContact::DB_ModifyBuddy (BuddyParams& pBuddyParams)
{
	return DB_ModifyContact (pBuddyParams);
}

BOOL CWorkContact::DB_IsBuddyExist (const string& pUserId)
{
	CConfigure*        pConfigure = CConfigure::GetInstance();

	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pConfigure->ReadProfile ("reg_id");
	pRelationParams.pRelationId  = pUserId;
	pRelationParams.eIsBuddyType = IBT_BUDDY;
	return DB_IsRelationExist (pRelationParams);
}

HRESULT CWorkContact::DB_AddFans (BuddyParams& pBuddyParams)
{
	HRESULT hResult = E_FAIL;
	CConfigure*        pConfigure = CConfigure::GetInstance();

	// add contact first
	hResult = DB_AddContact (pBuddyParams);
	if (hResult != S_OK)
	{
		return E_FAIL;
	}

	// second, add relation
	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pBuddyParams.pUserId;
	pRelationParams.pRelationId  = pConfigure->ReadProfile ("reg_id");
	pRelationParams.eIsBuddyType = IBT_FANS;
	return DB_AddRelation (pRelationParams);
}

HRESULT CWorkContact::DB_DeleteFans (const string& pUserId)
{
	CConfigure*        pConfigure = CConfigure::GetInstance();

	// just only delete relation
	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pUserId;
	pRelationParams.pRelationId  = pConfigure->ReadProfile ("reg_id");
	pRelationParams.eIsBuddyType = IBT_FANS;
	return DB_DeleteRelation (pRelationParams);
}

HRESULT CWorkContact::DB_DeleteFansList ()
{
	CConfigure*        pConfigure = CConfigure::GetInstance();

	// just only delete relation
	RelationParams pRelationParams = {};
	pRelationParams.pRelationId  = pConfigure->ReadProfile ("reg_id");
	pRelationParams.eIsBuddyType = IBT_FANS;
	return DB_DeleteRelationList (pRelationParams);
}

HRESULT CWorkContact::DB_ModifyFans  (BuddyParams& pBuddyParams)
{
	return DB_ModifyContact (pBuddyParams);
}

BOOL CWorkContact::DB_IsFansExist (const string& pUserId)
{
	CConfigure*        pConfigure = CConfigure::GetInstance();

	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pUserId;
	pRelationParams.pRelationId  = pConfigure->ReadProfile ("reg_id");
	pRelationParams.eIsBuddyType = IBT_FANS;
	return DB_IsRelationExist (pRelationParams);
}

HRESULT CWorkContact::DB_AddMember (const string& pGroupId, BuddyParams& pBuddyParams,const string& pGroupCardName)
{
	HRESULT hResult = E_FAIL;

	// add contact first
	hResult = DB_AddContact (pBuddyParams);
	if (hResult != S_OK)
	{
		return E_FAIL;
	}

	// second, add relation
	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pGroupId;
	pRelationParams.pRelationId  = pBuddyParams.pUserId;
	pRelationParams.eIsBuddyType = IBT_MEMBER;
	pRelationParams.pGroupCardName = pGroupCardName;
	return DB_AddRelation (pRelationParams);
}

HRESULT CWorkContact::DB_DeleteMember (const string& pGroupId, const string& pUserId)
{
	// just only delete relation
	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pGroupId;
	pRelationParams.pRelationId  = pUserId;
	pRelationParams.eIsBuddyType = IBT_MEMBER;
	return DB_DeleteRelation (pRelationParams);
}

HRESULT CWorkContact::DB_DeleteMemberList (const string& pGroupId)
{
	// just only delete relation
	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pGroupId;
	pRelationParams.eIsBuddyType = IBT_MEMBER;
	return DB_DeleteRelationList (pRelationParams);
}

HRESULT CWorkContact::DB_ModifyMember (const string& pGroupId,const string& pUserId, const string& pGroupCardName)
{
	// just only modify relation
	RelationParams pRelationParams = {};
	pRelationParams.pUserId        = pGroupId;
	pRelationParams.pRelationId    = pUserId;
	pRelationParams.pGroupCardName = pGroupCardName;
	pRelationParams.eIsBuddyType   = IBT_MEMBER;
	return DB_ModifyRelation (pRelationParams);
}

BOOL CWorkContact::DB_IsMemberExist (const string& pGroupId, const string& pUserId)
{
	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pGroupId;
	pRelationParams.pRelationId  = pUserId;
	pRelationParams.eIsBuddyType = IBT_MEMBER;
	return DB_IsRelationExist (pRelationParams);
}

UINT CWorkContact::DB_GetMemberCount (const string& pGroupId)
{
	RelationParams pRelationParams = {};
	pRelationParams.pUserId      = pGroupId;
	pRelationParams.eIsBuddyType = IBT_MEMBER;
	return DB_GetRelationCount (pRelationParams);
}
VOID CWorkContact::ClearHudongNewsCount()
{
	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [globals] SET [value]='%d' WHERE [key]='hudongcount';",0);
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("ClearHudongNewsCount success.\r\n");
	}
	else
	{
		LOGD ("ClearHudongNewsCount failure.\r\n");
	}
}
int  CWorkContact::GetHudongNewsCount()
{
	LOGD ("CWorkContact::GetHudongNewsCount.\r\n");
	int nResult = E_FAIL;
	int count = 0;
	struct query_rows_s query_rows = {};
	// query table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat ("SELECT [value] from [globals] where [key]='hudongcount';");
//	LOGD("sSQL1=%s\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		return count;
		LOGD("query no hudongcount.\r\n");
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		count = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}
	LOGD("query hudongcount %d.\r\n",count);
	return count;
}
VOID CWorkContact::SetHudongNewsCount()
{
	int count = GetHudongNewsCount();

	if(count >= 0)
	{
		count +=1;
	}
	else
	{
		LOGD ("count less then zero.\r\n");
	}

	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [globals] SET [value]='%d' WHERE [key]='hudongcount';",count);
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SetHudongNewsCount success.\r\n");
	}
	else
	{
		LOGD ("SetHudongNewsCount failure.\r\n");
	}
}

void CWorkContact::OnNotifyOperateContact(int oper,CIm* pim)
{
	switch(oper)
	{
		case IM_SNS://收到sns
		{
			CImSns* imSns = (CImSns*)(pim);

			if(NULL == imSns)
			{
				LOGD("imSns pointer is null.\r\n");
			}
			else
			{
				if(imSns->m_subtype == "0")//互动消息
				{
					SetHudongNewsCount();
					RspParams rspParams;
					rspParams.eRspType = ST_NOTIFY_HUDONG_NEWS;
					OnNotifyBuddy (&rspParams);//通知UI收到互动消息
					LOGD("Received hudongnews notify.\r\n");
				}
				else if(imSns->m_subtype == "10")//推荐好友
				{
					SetRecommendBuddy();
					RspParams rspParams;
					rspParams.eRspType = ST_NOTIFY_RECOMMEND_BUDDY;
					OnNotifyBuddy (&rspParams);//通知UI收到互动消息
					LOGD("Received recommend buddy notify.\r\n");
				}
			}
		}
		break;
		case IM_FANSMODIFY://收到粉丝修改信息
		{
			CImFansModify* imFansModify = (CImFansModify*)(pim);

			if(NULL == imFansModify)
			{
				LOGD("imFansModify pointer is null.\r\n");
			}
			else
			{
				BuddyParams buddy = {};
				buddy.pUserId      = imFansModify->m_uid;
				buddy.pUserName    = imFansModify->m_name;
				buddy.pPortraitKey = imFansModify->m_key;
				buddy.pEmotionMood = imFansModify->m_info;
				DB_ModifyContact(buddy);

				if (DB_IsBuddyExist(imFansModify->m_uid))
				{
					RspParams rspParams;
					rspParams.eRspType = ST_NOTIFY_MODIFY_BUDDY;
					OnNotifyBuddy (&rspParams);//通知UI修改好友
				}
				if (DB_IsFansExist(imFansModify->m_uid))
				{
					RspParams rspParams;
					rspParams.eRspType = ST_NOTIFY_MODIFY_FANS;
					OnNotifyFans (&rspParams);//通知UI修改粉丝
				}
			}
		}
		break;
		case IM_FANSAPPEND://收到新粉丝修改信息
		{
			CImFansAppend* imFansAppend = (CImFansAppend*)(pim);

			if(NULL == imFansAppend)
			{
				LOGD("imFansAppend pointer is null.\r\n");
			}
			else
			{
				RspParams rspParams;
				rspParams.pBuddyParams = new BuddyParams;
				rspParams.pBuddyParams->pUserId = imFansAppend->m_uid;
				rspParams.pBuddyParams->pUserName = imFansAppend->m_name;
				rspParams.pBuddyParams->pPortraitKey = imFansAppend->m_key;
				rspParams.pBuddyParams->pEmotionMood = imFansAppend->m_info;
				rspParams.pBuddyParams->pCmsId = imFansAppend->m_cmsid;

				string notifytype;
				notifytype = imFansAppend->m_subtype;
				if(notifytype.compare("0") == 0)//收到添加粉丝通知
				{
					LOGD ("Received AddFans notify.\r\n");

					BuddyParams buddyParams  = {};
					buddyParams.pUserId      = imFansAppend->m_uid;
					buddyParams.pUserName    = imFansAppend->m_name;
					buddyParams.pPortraitKey = imFansAppend->m_key;
					buddyParams.pEmotionMood = imFansAppend->m_info;
					buddyParams.pCmsId       = imFansAppend->m_cmsid;
					DB_AddFans(buddyParams);

					rspParams.eRspType = ST_NOTIFY_ADD_FANS;
					OnNotifyFans (&rspParams);//通知UI加到粉丝列表

					LoadContactList();//增量更新获取新粉丝
				}

				delete rspParams.pBuddyParams;
			}
		}
		break;
		case IM_FANSDELETE://收到新粉丝修改信息
		{
			CImFansDelete* imFansDelete = (CImFansDelete*)(pim);

			if(NULL == imFansDelete)
			{
				LOGD("imFansDelete pointer is null.\r\n");
			}
			else
			{
				LOGD ("Received DeleteFans notify.\r\n");

				string contactid;
				contactid = imFansDelete->m_uid;
				DB_DeleteFans(contactid);

	//			if(!DB_IsBuddyExist(contactid))
	//			{
	//				CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
	//				pWorkMessage->DeleteSession(contactid, TRUE);
	//			}

				RspParams rspParams;
				rspParams.eRspType = ST_NOTIFY_DELETE_FANS;
				OnNotifyFans (&rspParams);//通知UI删除粉丝

				LoadContactList();//增量更新获取新粉丝
			}
		}
		break;

		default:
		break;
	}
}

VOID CWorkContact::ClearRecommendBuddy ()
{
	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [globals] SET [value]='%d' WHERE [key]='recommendbuddy';",0);
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("ClearRecommendBuddy success.\r\n");
	}
	else
	{
		LOGD ("ClearRecommendBuddy failure.\r\n");
	}
}
int CWorkContact::GetRecommendBuddy()
{
	LOGD ("CWorkContact::GetRecommendBuddy.\r\n");
	int nResult = E_FAIL;
	int count = 0;
	struct query_rows_s query_rows = {};
	// query table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat ("SELECT [value] from [globals] where [key]='recommendbuddy';");
//	LOGD("sSQL1=%s\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		return count;
		LOGD("query no recommendbuddy.\r\n");
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		count = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}
	//LOGD("query hudongcount %d.\r\n",count);
	return count;
}
VOID CWorkContact::SetRecommendBuddy()
{
	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [globals] SET [value]='%d' WHERE [key]='recommendbuddy';",1);
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SetRecommendBuddy success.\r\n");
	}
	else
	{
		LOGD ("SetRecommendBuddy failure.\r\n");
	}
}

HRESULT CWorkContact::SetReceiveMessageOption(int brecv,bool fromui)
{
	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [globals] SET [value]='%d' WHERE [key]='recvpushmsg';",brecv);
//	LOGD("sSQL1=%s\r\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	if (nResult == S_OK)
	{
		LOGD ("SetReceiveMessageOption success.\r\n");
	}
	else
	{
		LOGD ("SetReceiveMessageOption failure.\r\n");
	}

	if(fromui)
	{
		//通知服务器
		IConfigure* pConfigure = CConfigure::GetInstance();
		string strUserId = pConfigure->ReadProfile ("reg_id");

		CXCapImpl* xcap = CXCapImpl::GetInstance();
		xcap->SetReceiveMessageOption(brecv);
	}

	return S_OK;
}
int CWorkContact::GetReceiveMessageOption()
{
	LOGD ("CWorkContact::GetReceiveMessageOption.\r\n");
	int nResult = E_FAIL;
	int count = 0;
	struct query_rows_s query_rows = {};
	// query table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat ("SELECT [value] from [globals] where [key]='recvpushmsg';");
//	LOGD("sSQL1=%s\n", sSQL.c_str());
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		return count;
		LOGD("query no recvpushmsg.\r\n");
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		count = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}

	return count;
}
int CWorkContact::GetReceiveMessageFromServer()
{
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	xcap->GetReceiveMessageOption();
	return S_OK;
}
