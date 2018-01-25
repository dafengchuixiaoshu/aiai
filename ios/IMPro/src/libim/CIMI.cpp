#include "BizCommon.h"
#include "CObserverContainer.h"
#include "CWorkerContainer.h"
#include "CConfigure.h"
#include "CXmppStack.h"
#include "iXCapInterface.h"
#include "CWorkLogin.h"
#include "CWorkUpdateTime.h"
#include "CWorkMessage.h"
#include "CWorkGroup.h"
#include "CWorkContact.h"
#include "CLog.h"
#include "EncodeUtil.h"
#include "DBUpdate.h"
#include "CIMI.h"
#include "XcapCore.h"

#ifndef USE_SQLITE
#include "memsave.h"
#endif

CIMI* CIMI::g_imi = NULL;

CIMI::CIMI(void)
{
}

CIMI::~CIMI(void)
{
}

CIMI* CIMI::GetInstance()
{
	if (g_imi == NULL)
	{
		g_imi = new CIMI();
	}
	return g_imi;
}

HRESULT CIMI::Init()
{
	CObserverContainer* pObserverContainer = CObserverContainer::GetInstance();
	CWorkerContainer*   pWorkerContainer   = CWorkerContainer::GetInstance();
	CConfigure*         pConfigure         = CConfigure::GetInstance();
	CWorkUpdateTime*    pWorkUpdateTime    = CWorkUpdateTime::GetInstance();
	CWorkLogin*         pWorkLogin         = CWorkLogin::GetInstance();
	CWorkMessage*       pWorkMessage       = CWorkMessage::GetInstance();
	CWorkGroup*         pWorkGroup         = CWorkGroup::GetInstance();
//	CWorkBuddy*         pWorkBuddy         = CWorkBuddy::GetInstance();
	CWorkContact*       pWorkContact       = CWorkContact::GetInstance();

	pWorkerContainer->AddWorker(WT_LOGIN,   pWorkLogin);
	pWorkerContainer->AddWorker(WT_LOGOUT,  pWorkLogin);
	pWorkerContainer->AddWorker(WT_MESSAGE, pWorkMessage);
	pWorkerContainer->AddWorker(WT_GROUP,   pWorkGroup);
	pWorkerContainer->AddWorker(WT_CONTACT, pWorkContact);

	// Gbk/UTF8编码表初始化
	EncodeUtil::InitGbkU16();

	pConfigure->WriteProfile("Resource", IMI_RESOURCE);
	pConfigure->WriteProfile("Platform", IMI_PLATFORM);
	pConfigure->WriteProfile("Domain",   IMI_DEFAULT_DOMAIN);

	pConfigure->WriteProfile("gd_ver",   IMI_GD_VERSION);

	pWorkUpdateTime->Init();
	pWorkMessage->Init();
	pWorkGroup->Init();
//	pWorkBuddy->Init();
	pWorkContact->Init();

	return S_OK;
}

VOID CIMI::Release()
{
	CObserverContainer* pObserverContainer = CObserverContainer::GetInstance();
	CWorkerContainer*   pWorkerContainer   = CWorkerContainer::GetInstance();
	CConfigure*         pConfigure         = CConfigure::GetInstance();
	CWorkUpdateTime*    pWorkUpdateTime    = CWorkUpdateTime::GetInstance();
	CWorkLogin*         pWorkLogin         = CWorkLogin::GetInstance();
	CWorkMessage*       pWorkMessage       = CWorkMessage::GetInstance();
	CWorkGroup*         pWorkGroup         = CWorkGroup::GetInstance();
//	CWorkBuddy*         pWorkBuddy         = CWorkBuddy::GetInstance();
	CWorkContact*       pWorkContact       = CWorkContact::GetInstance();
	CXCapImpl*          pXCapStack         = CXCapImpl::GetInstance();
	CXmppStack*         pXmppStack         = CXmppStack::GetInstance();
	DBUpdate*           pDBUpdate          = DBUpdate::GetInstance();
#if defined(USE_LOG)
	CLog*               pLog               = CLog::GetInstance();
#endif

	pObserverContainer->Release();
	pWorkerContainer->Release();
	pConfigure->Release();
	pXCapStack->Release();
	pWorkUpdateTime->Release();
	pWorkLogin->Release();
	pWorkMessage->Release();
	pWorkGroup->Release();
//	pWorkBuddy->Release();
	pWorkContact->Release();
	pXmppStack->Release();
	pDBUpdate->Release();
#if defined(USE_LOG)
	pLog->Release();
#endif

	SqliteImpl::ReleaseInstance();

	if (g_imi)
	{
		delete g_imi;
		g_imi = NULL;
	}
}

HRESULT CIMI::SetRootDir(const string pRootDir)
{
	CConfigure*        pConfigure = CConfigure::GetInstance();

	if (pRootDir.empty())
	{
		return E_FAIL;
	}

	pConfigure->WriteProfile("rootdir", pRootDir.c_str());
	return S_OK;
}

HRESULT CIMI::SetServerAddr(const string pServerAddr, const string pServerPort)
{
	CConfigure*        pConfigure = CConfigure::GetInstance();

	if (pServerAddr.empty() || pServerPort.empty())
	{
		return E_FAIL;
	}

	const string strIpPort = StringFormat ("%s:%s", pServerAddr.c_str(), pServerPort.c_str());
	pConfigure->WriteProfile("server", strIpPort);
	return S_OK;
}

HRESULT CIMI::SetUserId(const string pUserId)
{
	CConfigure*        pConfigure = CConfigure::GetInstance();

	if (pUserId.empty())
	{
		return E_FAIL;
	}

	pConfigure->WriteProfile("reg_id", pUserId);
	return S_OK;
}

const string CIMI::GetRootDir()
{
	CConfigure*        pConfigure = CConfigure::GetInstance();
	return pConfigure->ReadProfile ("rootdir");
}

const string CIMI::GetUserId()
{
	CConfigure*        pConfigure = CConfigure::GetInstance();
	return pConfigure->ReadProfile ("reg_id");
}

HRESULT CIMI::ClearUserId()
{
	CConfigure*        pConfigure = CConfigure::GetInstance();
	pConfigure->WriteProfile("reg_id", "");
	return S_OK;
}

HRESULT CIMI::SetObserver(enum ObserverType eObserverType, IObserver* pUIObserver)
{
	CObserverContainer* pObserverContainer = CObserverContainer::GetInstance();
	return pObserverContainer->AddObserver(eObserverType, pUIObserver);
}

HRESULT CIMI::NotifyNetworkStatus(enum NetWorkStatusType eNetworkStatusType)
{
	LOGD ("CIMI::NotifyNetworkStatus 1\r\n");

	CWorkLogin*      pWorkLogin      = CWorkLogin::GetInstance();
	pWorkLogin->NotifyNetworkStatus(eNetworkStatusType);

	LOGD ("CIMI::NotifyNetworkStatus 2\r\n");

//	CWorkBuddy*      pWorkBuddy      = CWorkBuddy::GetInstance();
//	pWorkBuddy->NotifyNetworkStatus(eNetworkStatusType);

	CWorkContact*    pWorkContact    = CWorkContact::GetInstance();
	pWorkContact->NotifyNetworkStatus(eNetworkStatusType);

	LOGD ("CIMI::NotifyNetworkStatus 3\r\n");

	CWorkGroup*      pWorkGroup     = CWorkGroup::GetInstance();
	pWorkGroup->NotifyNetworkStatus(eNetworkStatusType);

	LOGD ("CIMI::NotifyNetworkStatus 4\r\n");

    return S_OK;
}

enum ConnectionStatusType CIMI::GetConnectionStatus()
{
	return ((sImModule.GetCurConnectStatus() == TRUE) ? (CST_EXIST) : (CST_NOTEXIST));
}

HRESULT CIMI::CloseConnection()
{
	sImModule.Release();
	return S_OK;
}

HRESULT CIMI::OpenLog(BOOL bOpenLog)
{
#if defined(USE_LOG)
	CLog*               pLog               = CLog::GetInstance();
	return pLog->OpenLog(bOpenLog);
#else
	return E_FAIL;
#endif
}

HRESULT CIMI::Login(ReqParams* pReqParams)
{
#if defined(USE_LOG)
	CLog*               pLog               = CLog::GetInstance();
	pLog->Init ();
	pLog->OpenFile ();
#endif

	LOGD ("libimi version=%s\n", CPPDOM_XSTR(CPPDOM_VERSION_STRING));

#ifdef USE_SQLITE
	SqliteImpl*         pSqliteImpl        = SqliteImpl::GetInstance();
	pSqliteImpl->OpenDBConnection();

	DBUpdate*           pDBUpdate          = DBUpdate::GetInstance();
	pDBUpdate->CreateGlobalsTable ();
	pDBUpdate->CheckVersion ();
#else
	CConfigure*        pConfigure     = CConfigure::GetInstance();
	string strRootDir = pConfigure->ReadProfile ("rootdir");
	string strUserId  = pConfigure->ReadProfile ("reg_id");
	if (strRootDir.length() > 0 && strUserId.length() > 0)
	{
		sMemSave.OpenMemSave(GetDBDir(strRootDir, strUserId));
	}
#endif

	CWorkUpdateTime*    pWorkUpdateTime    = CWorkUpdateTime::GetInstance();
	CWorkMessage*       pWorkMessage       = CWorkMessage::GetInstance();
	CWorkGroup*         pWorkGroup         = CWorkGroup::GetInstance();
//	CWorkBuddy*         pWorkBuddy         = CWorkBuddy::GetInstance();
	CWorkContact*       pWorkContact       = CWorkContact::GetInstance();

	pWorkUpdateTime->CreateTable ();
	pWorkMessage->CreateTable ();
	pWorkGroup->CreateTable ();
//	pWorkBuddy->CreateTable ();
	pWorkContact->CreateTable ();

	IWorkerContainer* pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* worker = pWorkerContainer->FindWorker(WT_LOGIN);
	if (worker)
		worker->Invoke((VOID*)pReqParams);
	return S_OK;
}

HRESULT CIMI::Logout(ReqParams* pReqParams)
{
	IWorkerContainer* pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* worker = pWorkerContainer->FindWorker(WT_LOGIN);
	if (worker)
		worker->Invoke((VOID*)pReqParams);

	CXCapCore* pCore = CXCapCore::GetInstance();//关闭http请求
	pCore->Close();

#ifdef USE_SQLITE
	SqliteImpl*         pSqliteImpl        = SqliteImpl::GetInstance();
	pSqliteImpl->CloseDBConnection();
#else
	sMemSave.CleanMemSave();
#endif

    ClearUserId ();
    
	return S_OK;
}

SessionList CIMI::GetSessionList ()
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->GetSessionList();
}

SessionParams CIMI::GetSession (const string& pSessionId)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->GetSession(pSessionId);
}

HRESULT CIMI::DeleteSession (const string& pSessionId, BOOL bDeleteMessageLogs)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->DeleteSession(pSessionId, bDeleteMessageLogs);
}

HRESULT CIMI::DeleteSessionList (const SessionList& pSessionList, BOOL bDeleteMessageLogs)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->DeleteSessionList(pSessionList, bDeleteMessageLogs);
}

HRESULT CIMI::ClearSession (const string& pSessionId)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->ClearSession(pSessionId);
}

HRESULT CIMI::SetSessionOption (enum SessionOptionType eOptionType, SessionParams& pSessionParams)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->SetSessionOption(eOptionType, pSessionParams);
}

HRESULT CIMI::SetSessionListReaded (const SessionList& pSessionList)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->SetSessionListReaded(pSessionList);
}

UINT CIMI::GetTotalUnreadNum ()
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->GetTotalUnreadNum();
}

HRESULT CIMI::UpdateDraft (const string& pSessionId, const string& pDraftContent)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->UpdateDraft(pSessionId, pDraftContent);
}

MessageList CIMI::GetMessageList (const string& pSessionId, const string& pLastMsgId, int nPageSize)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->GetMessageList(pSessionId, pLastMsgId, nPageSize);
}

MessageParams CIMI::GetMessage (const string& pSessionId, const string& pMsgId, BOOL bUpdateUnreadNum)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->GetMessage(pSessionId, pMsgId, bUpdateUnreadNum);
}

HRESULT CIMI::DeleteMessage (const string& pSessionId, const string& pMsgId, const string& imagekey, const string& videokey)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->DeleteMessage(pSessionId, pMsgId, imagekey, videokey);
}

HRESULT CIMI::SetAudioPlayed (const string& pSessionId, const string& pMsgId)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->SetAudioPlayed(pSessionId, pMsgId);
}

MessageList CIMI::GetGivenFieldList (const string& pSessionId, enum GivenFieldType eGivenField, enum GivenConditionType eGivenCondition)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->GetGivenFieldList(pSessionId, eGivenField, eGivenCondition);
}

MessageList& CIMI::GetOfflineMessageList (const string& pSessionId)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	return pWorkMessage->GetOfflineMessageList(pSessionId);
}

BuddyParams CIMI::GetBuddy (const string& pUserId, BOOL bLoadFromServer)
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->GetBuddy(pUserId, bLoadFromServer);
}

BuddyList CIMI::GetBuddyList ()
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->GetBuddyList();
}

HRESULT CIMI::AddBuddy (const string& pBuddyId)
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->AddBuddy(pBuddyId);
}

HRESULT CIMI::DeleteBuddy (const string& pBuddyId)
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	pWorkContact->DeleteBuddy(pBuddyId);

	//CWorkMessage*       pWorkMessage       = CWorkMessage::GetInstance();
	//pWorkMessage->DeleteSession(GetUserId(), TRUE);  // delete session and message logs

	return S_OK;
}

HRESULT CIMI::IsBuddyExist (const string& pBuddyId)
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->DB_IsBuddyExist(pBuddyId);
}

HRESULT CIMI::SearchContact (const string& key)
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->SearchContact(key);
}

BuddyList CIMI::GetSearchContact()
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->GetSearchContact();
}

BuddyParams CIMI::GetContact (const string& pUserId, BOOL bLoadFromServer)
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->GetContact(pUserId, bLoadFromServer);
}

BuddyParams CIMI::GetFans (const string& pUserId, BOOL bLoadFromServer)
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->GetFans(pUserId, bLoadFromServer);
}

BuddyList CIMI::GetFansList ()
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->GetFansList();
}

HRESULT CIMI::OperateGroup(ReqParams* pReqParams)
{
	IWorkerContainer* pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* worker = pWorkerContainer->FindWorker(WT_GROUP);
	if (worker)
		worker->Invoke((VOID*)pReqParams);
	return S_OK;
}

vector<GroupParams> CIMI::GetGroupList()
{
	CWorkGroup*      pWorkGroup = CWorkGroup::GetInstance();
	return pWorkGroup->GetGroupList();
}
GroupParams CIMI::GetGroupInfo(string groupid)
{
	CWorkGroup*      pWorkGroup = CWorkGroup::GetInstance();
	return pWorkGroup->GetGroupInfo(groupid);
}
vector<MemberParams> CIMI::GetGroupMemberList(string groupid)
{
	CWorkGroup*      pWorkGroup = CWorkGroup::GetInstance();
	return pWorkGroup->GetMemberList(groupid);
}
MemberParams CIMI::GetGroupMemberInfo(const string inMemberId,string groupid,BOOL loadfromserver)
{
	CWorkGroup*      pWorkGroup = CWorkGroup::GetInstance();
	return pWorkGroup->GetMember(inMemberId,groupid,loadfromserver);
}
int CIMI::GetNewFansCount()
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->GetNewFansCount();
}
int CIMI::ClearNewFansCount()
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	pWorkContact->ClearNewFansCount();
	return 1;
}
int CIMI::GetSnsCount(SnsType type)
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	LOGD ("CIMI::SnsType %d.\r\n",type);
	if(type == IT_HUDONG)
	{
		return pWorkContact->GetHudongNewsCount();
	}
	else if(type == IT_RECOMMEND)
	{
		return pWorkContact->GetRecommendBuddy();
	}

    return S_OK;
}
int CIMI::ClearSnsCount(SnsType type)
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	if(type == IT_HUDONG)
	{
		pWorkContact->ClearHudongNewsCount();
	}
	else if(type == IT_RECOMMEND)
	{
		pWorkContact->ClearRecommendBuddy();
	}

	return S_OK;
}
HRESULT CIMI::SetReceiveMessageOption(int brecv)
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	pWorkContact->SetReceiveMessageOption(brecv,true);
	return S_OK;
}
int CIMI::GetReceiveMessageOption()
{
	CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
	return pWorkContact->GetReceiveMessageOption();
}

void CIMI::SetSavePath(string filepath)
{
	sImModule.SetSavePath(filepath);
}

void CIMI::SetMapPoint(string& xpoint, string& ypoint)
{
	sImModule.SetMapPoint(xpoint, ypoint);
}

void CIMI::SetFileServer(string ip, int port)
{
	sImModule.SetFileServer(ip, port);
}

void CIMI::SendImage(string& fromuser, string& touser, string& imagepath,
		string& txtbuffer, int imagewidth, int imageheight, int msgtype)
{
	if(fromuser.empty() || touser.empty() || imagepath.empty())
	{
		LOGD ("CIMI::SendImage is empty, fromuser:%s,touser:%s\r\n", fromuser.c_str(), touser.c_str());
	}
	else if(txtbuffer.size() > MAX_BODY_LEN)
	{
		LOGD ("CIMI::SendImage txtbuffer is too long\r\n");
	}
	else
	{
		IWorkerContainer* pWorkerContainer = CWorkerContainer::GetInstance();

		IWorker* worker = pWorkerContainer->FindWorker(WT_MESSAGE);

		if (worker)
		{
			worker->SendImage(fromuser, touser, imagepath, txtbuffer, imagewidth, imageheight, msgtype);
		}
	}
}

void CIMI::SendVideo(string& fromuser, string& touser, string& videopath,
		string& faceimagepath, string& txtbuffer, string& videotitle, int videolength, int msgtype)
{
	if(fromuser.empty() || touser.empty() || videopath.empty())
	{
		LOGD ("CIMI::SendVideo is empty, fromuser:%s,touser:%s,videopath:%s,faceimagepath:%s\r\n",
				fromuser.c_str(), touser.c_str(), videopath.c_str(), faceimagepath.c_str());
	}
	else if(txtbuffer.size() > MAX_BODY_LEN)
	{
		LOGD ("CIMI::SendImage txtbuffer is too long\r\n");
	}
	else
	{
		IWorkerContainer* pWorkerContainer = CWorkerContainer::GetInstance();

		IWorker* worker = pWorkerContainer->FindWorker(WT_MESSAGE);

		if (worker)
		{
			worker->SendVideo(fromuser, touser, videopath, faceimagepath, txtbuffer, videotitle, videolength, msgtype);
		}
	}
}

void CIMI::SetBlackListFlag(string& blackid, int type)
{
	if(blackid.empty())
	{
		LOGD ("CIMI::SetBlackListFlag blackid is empty, blackid:%s\r\n", blackid.c_str());
	}
	else
	{
		IWorkerContainer* pWorkerContainer = CWorkerContainer::GetInstance();

		IWorker* worker = pWorkerContainer->FindWorker(WT_MESSAGE);

		if (worker)
		{
			worker->SetBlackListFlag(blackid, type);
		}
	}
}
