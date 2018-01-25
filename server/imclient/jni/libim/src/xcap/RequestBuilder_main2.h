
#pragma once
#include "RequestBuilder.h"
#include "XCapCore.h"
#include "BizCommon.h"
#include "Markup.h"


namespace XCapStrategy
{
	namespace Main  ///< 主要算法集
	{

		class COperPublicGroupReqBuilder : public CRequestBuilder
		{
		public:
			COperPublicGroupReqBuilder(XCAP_OPER eXO):
			  m_eXO(eXO)
			  {
			  }
			COperPublicGroupReqBuilder(XCAP_OPER eXO,GroupParams* groupinfo):
			  m_eXO(eXO),m_groupinfo(groupinfo)
			  {
			  }
			COperPublicGroupReqBuilder(XCAP_OPER eXO, string pGroupId):
			  m_eXO(eXO),m_pGroupId(pGroupId),m_groupinfo(NULL)
			  {
			  }

		public:
			virtual CHttpRequest GetRequestPacket();

		private:
			CHttpRequest BuildGetGroupList();
			CHttpRequest BuildGetGroupMemberList(string pGroupId);
			CHttpRequest BuildCreatePulicGroup(GroupParams* groupinfo);
			CHttpRequest BuildInvitePulicGroup(GroupParams* groupinfo);
			CHttpRequest BuildDeletePublicGroupInfo(GroupParams* groupinfo);
			CHttpRequest BuildModifyGroupInfo(GroupParams* groupinfo);
			CHttpRequest BuildModifyGroupMemberName(GroupParams* groupinfo);

		private:
			XCAP_OPER m_eXO;
			GroupParams* m_groupinfo;
			string       m_pGroupId;
		};

		class COperBuddyReqBuilder : public CRequestBuilder
		{
		public:

			COperBuddyReqBuilder(XCAP_OPER eXO):m_eXO(eXO),m_pUserId(NULL)
			{

			}
			COperBuddyReqBuilder(XCAP_OPER eXO,int optionvalue):
			 m_eXO(eXO),m_optionvalue(optionvalue)
			 {

			 }
			COperBuddyReqBuilder(XCAP_OPER eXO,const CHAR* pUserId):
			 m_eXO(eXO),m_pUserId(pUserId)
			 {

			 }
			COperBuddyReqBuilder(XCAP_OPER eXO,const string pBuddyId):
			 m_eXO(eXO),m_pBuddyId(pBuddyId)
			 {

			 }

			COperBuddyReqBuilder(XCAP_OPER eXO,const CHAR* pUserId,list<string> msgidlist)
			:m_eXO(eXO),m_pUserId(pUserId),m_msgidlist(msgidlist)
			 {

			 }


			virtual CHttpRequest GetRequestPacket();

		private:
			CHttpRequest BuildGetBuddyList();
			CHttpRequest BuildGetBuddyInfo(const string pUserId);
			CHttpRequest BuildGetOfflineMsg(const CHAR* pUserId);
			CHttpRequest BuildGetOfflineMsgAck(const CHAR* pUserId,list<string> msgidlist);
			CHttpRequest BuildAddBuddy(const CHAR* pBuddyId);
			CHttpRequest BuildDeleteBuddy(const CHAR* pBuddyId);
			CHttpRequest BuildSearchBuddy(const string key);
			CHttpRequest BuildSetRecvPushMsg(int value);
			CHttpRequest BuildGetRecvPushMsg();
			XCAP_OPER m_eXO;
			const CHAR* m_pUserId;
			const string m_pBuddyId;
			list<string> m_msgidlist;
			int m_optionvalue;

		};

		class COperOfflineMsgReqBuilder : public CRequestBuilder
		{
		public:

			COperOfflineMsgReqBuilder(XCAP_OPER eXO,const string pUserId)
			:m_eXO(eXO),m_pUserId(pUserId)
			{

			}

			COperOfflineMsgReqBuilder(XCAP_OPER eXO,const string pUserId,list<string> msgidlist)
			:m_eXO(eXO),m_pUserId(pUserId),m_msgidlist(msgidlist)
			 {

			 }

			virtual CHttpRequest GetRequestPacket();

		private:

			CHttpRequest BuildGetOfflineMsg(const string pUserId);
			CHttpRequest BuildGetOfflineMsgAck(const string pUserId,list<string> msgidlist);
			XCAP_OPER m_eXO;
			const string m_pUserId;
			list<string> m_msgidlist;

		};

	}
}
