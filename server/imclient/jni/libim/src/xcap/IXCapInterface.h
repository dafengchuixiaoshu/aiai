#ifndef _C_CXCapImpl_H
#define _C_CXCapImpl_H

#pragma once
#include "XCapCore.h"
#include "CObserverContainer.h"
#include "BizCommon.h"
#include "RequestBuilder_main2.h"
#include "ResponseProcessor_main2.h"

using namespace XCapStrategy::Main;

class CXCapInterceBase
{
	public:
		CXCapInterceBase(void){};
		virtual ~CXCapInterceBase(void){};

	STDMETHOD (put_Prop) (XCAP_PROP eXP, const char* bstrValue)
	{
		string value = bstrValue;
		CXCapCore::GetInstance()->SetConfig(eXP,value);
		return 1;
	}
};

class CXCapImpl :public CXCapInterceBase
{
public:
	CXCapImpl(void){};
	virtual ~CXCapImpl(void){};

	static CXCapImpl* GetInstance()
	{
		if (g_pXCapImpl == NULL)
		{
			g_pXCapImpl = new CXCapImpl();
		}
		return g_pXCapImpl;
	}
	static CXCapImpl* g_pXCapImpl;

	VOID Release()
	{
		if (g_pXCapImpl)
		{
			delete g_pXCapImpl;
			g_pXCapImpl = NULL;
		}
	}

	STDMETHOD (GetGroupList)(BOOL FromUI = true)
	{
        util::shared_ptr<COperPublicGroupReqBuilder>    spReqB(new COperPublicGroupReqBuilder(XO_GET));
		util::shared_ptr<COperPublicGroupRespProcessor> spRespP(new COperPublicGroupRespProcessor(XO_GET,FromUI));

		CXCapCore::GetInstance()->HttpGet(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (GetGroupMemberList)(string pGroupId)
	{
		util::shared_ptr<COperPublicGroupReqBuilder>    spReqB(new COperPublicGroupReqBuilder(XO_GET_GROUPMEMBERLIST,pGroupId));
		util::shared_ptr<COperPublicGroupRespProcessor> spRespP(new COperPublicGroupRespProcessor(XO_GET_GROUPMEMBERLIST));
		CXCapCore::GetInstance()->HttpGet(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (CreatePublicGroup) (GroupParams* groupinfo )
	{
		util::shared_ptr<COperPublicGroupReqBuilder>    spReqB(new COperPublicGroupReqBuilder(XO_ADD,groupinfo));
		util::shared_ptr<COperPublicGroupRespProcessor> spRespP(new COperPublicGroupRespProcessor(XO_ADD));

		CXCapCore::GetInstance()->HttpPut(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (InvitePublicGroup) (GroupParams* groupinfo)
	{
		util::shared_ptr<COperPublicGroupReqBuilder>    spReqB(new COperPublicGroupReqBuilder(XO_INVITE,groupinfo));
		util::shared_ptr<COperPublicGroupRespProcessor> spRespP(new COperPublicGroupRespProcessor(XO_INVITE));

		CXCapCore::GetInstance()->HttpPut(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (DeletePublicGroup)(GroupParams* groupinfo)
	{
		util::shared_ptr<COperPublicGroupReqBuilder>    spReqB(new COperPublicGroupReqBuilder(XO_DELETE,groupinfo));
		util::shared_ptr<COperPublicGroupRespProcessor> spRespP(new COperPublicGroupRespProcessor(XO_DELETE));

		CXCapCore::GetInstance()->HttpDelete(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (ModifyPublicGroup)(GroupParams* groupinfo )
	{
		util::shared_ptr<COperPublicGroupReqBuilder>    spReqB(new COperPublicGroupReqBuilder(XO_MODIFY,groupinfo));
		util::shared_ptr<COperPublicGroupRespProcessor> spRespP(new COperPublicGroupRespProcessor(XO_MODIFY));

		CXCapCore::GetInstance()->HttpPut(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (ModifyGroupMemberName)(GroupParams* groupinfo )
	{
		util::shared_ptr<COperPublicGroupReqBuilder>    spReqB(new COperPublicGroupReqBuilder(XO_MODIFY_GROUPMEMBERNAME,groupinfo));
		util::shared_ptr<COperPublicGroupRespProcessor> spRespP(new COperPublicGroupRespProcessor(XO_MODIFY_GROUPMEMBERNAME));

		CXCapCore::GetInstance()->HttpPut(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (GetBuddyList)(BOOL FromUI = true)
	{
		util::shared_ptr<COperBuddyReqBuilder>    spReqB(new COperBuddyReqBuilder(XO_GET_BUDDYLIST));
		util::shared_ptr<COperBuddyRespProcessor> spRespP(new COperBuddyRespProcessor(XO_GET_BUDDYLIST,FromUI));
		CXCapCore::GetInstance()->HttpGet(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (GetBuddyInfo)(const string pUserId)
	{
		util::shared_ptr<COperBuddyReqBuilder>    spReqB(new COperBuddyReqBuilder(XO_GET_BUDDYINFO,pUserId));
		util::shared_ptr<COperBuddyRespProcessor> spRespP(new COperBuddyRespProcessor(XO_GET_BUDDYINFO));
		CXCapCore::GetInstance()->HttpGet(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (AddBuddy)(const string& pbuddyId)
	{
		util::shared_ptr<COperBuddyReqBuilder>    spReqB(new COperBuddyReqBuilder(XO_ADD_BUDDY,pbuddyId));
		util::shared_ptr<COperBuddyRespProcessor> spRespP(new COperBuddyRespProcessor(XO_ADD_BUDDY));
		CXCapCore::GetInstance()->HttpGet(spReqB,spRespP);
		return S_OK;
	}
	STDMETHOD (DeleteBuddy)(const string& pbuddyId)
	{
		util::shared_ptr<COperBuddyReqBuilder>    spReqB(new COperBuddyReqBuilder(XO_DELETE_BUDDY,pbuddyId));
		util::shared_ptr<COperBuddyRespProcessor> spRespP(new COperBuddyRespProcessor(XO_DELETE_BUDDY));
		CXCapCore::GetInstance()->HttpGet(spReqB,spRespP);
		return S_OK;
	}
	STDMETHOD (SearchBuddy)(const string key)
	{
		util::shared_ptr<COperBuddyReqBuilder>    spReqB(new COperBuddyReqBuilder(XO_SEARCH_BUDDY,key));
		util::shared_ptr<COperBuddyRespProcessor> spRespP(new COperBuddyRespProcessor(XO_SEARCH_BUDDY));
		CXCapCore::GetInstance()->HttpGet(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (GetOfflineMsg)(const string pUserId)
	{
		util::shared_ptr<COperOfflineMsgReqBuilder>    spReqB(new COperOfflineMsgReqBuilder(XO_GET_OFFLINEMSG,pUserId));
		util::shared_ptr<COperOfflineMsgRespProcessor> spRespP(new COperOfflineMsgRespProcessor(XO_GET_OFFLINEMSG));
		CXCapCore::GetInstance()->HttpGet(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (GetOfflineMsgAck)(const string pUserId,list<string> msgidlist)
	{
		util::shared_ptr<COperOfflineMsgReqBuilder>    spReqB(new COperOfflineMsgReqBuilder(XO_GET_OFFLINEMSG_ACK,pUserId,msgidlist));
		util::shared_ptr<COperOfflineMsgRespProcessor> spRespP(new COperOfflineMsgRespProcessor(XO_GET_OFFLINEMSG_ACK));
		CXCapCore::GetInstance()->HttpPut(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (SetReceiveMessageOption)(int optionvalue)
	{
		util::shared_ptr<COperBuddyReqBuilder>    spReqB(new COperBuddyReqBuilder(XO_SET_RECV_PUSHMSG,optionvalue));
		util::shared_ptr<COperBuddyRespProcessor> spRespP(new COperBuddyRespProcessor(XO_SET_RECV_PUSHMSG));
		CXCapCore::GetInstance()->HttpGet(spReqB,spRespP);
		return S_OK;
	}

	STDMETHOD (GetReceiveMessageOption)()
	{
		util::shared_ptr<COperBuddyReqBuilder>    spReqB(new COperBuddyReqBuilder(XO_GET_RECV_PUSHMSG));
		util::shared_ptr<COperBuddyRespProcessor> spRespP(new COperBuddyRespProcessor(XO_GET_RECV_PUSHMSG));
		CXCapCore::GetInstance()->HttpGet(spReqB,spRespP);
		return S_OK;
	}

};

#endif
