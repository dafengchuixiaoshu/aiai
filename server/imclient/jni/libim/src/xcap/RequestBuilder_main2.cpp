
#include "RequestBuilder_main2.h"
#include "XCapCore.h"
#include "CWorkUpdateTime.h"
#include "CWorkGroup.h"
#include "CConfigure.h"
#include "CIMI.h"

#ifdef WIN32
#include <time.h>
#endif

namespace XCapStrategy
{
namespace Main  ///< 主要算法集
{

#pragma region COperPublicGroupReqBuilder 操作公共群组

CHttpRequest COperPublicGroupReqBuilder::GetRequestPacket()
{
	switch(m_eXO)
	{
		case XO_GET:
			{
				return BuildGetGroupList();
			}
		case XO_ADD:
			{
				return BuildCreatePulicGroup(m_groupinfo);
			}
		case XO_DELETE:
			{
				return  BuildDeletePublicGroupInfo(m_groupinfo);
			}
		case XO_MODIFY:
			{
				return 	BuildModifyGroupInfo(m_groupinfo);
			}
		case XO_MODIFY_GROUPMEMBERNAME:
		{
			return 	BuildModifyGroupMemberName(m_groupinfo);
		}
		case XO_INVITE:
		{
			return 	BuildInvitePulicGroup(m_groupinfo);
		}
		case XO_GET_GROUPMEMBERLIST:
		{
			return 	BuildGetGroupMemberList(m_pGroupId);
		}
	}

	return CHttpRequest();
}

CHttpRequest COperPublicGroupReqBuilder::BuildGetGroupList()
{
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "pg";

	CMarkup markup;
	markup.AddElem(_T("info"));

	IConfigure* pConfigure = CConfigure::GetInstance();
	string strUserId = pConfigure->ReadProfile ("reg_id");

	markup.AddAttrib(_T("id"), strUserId.c_str());

	CWorkUpdateTime* pWorkUpdateTime = CWorkUpdateTime::GetInstance();
	std::string updatetime = pWorkUpdateTime->GetUpdateTime(UTT_GROUP);
	markup.AddAttrib(_T("ver"), updatetime.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;
	LOGD ("COperPublicGroupReqBuilder: id:%s,ver:%s\r\n",GetUserSelfId().c_str(),updatetime.c_str());

	return cReqPack;
}
CHttpRequest COperPublicGroupReqBuilder::BuildGetGroupMemberList(string pGroupId)
{
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "pg";

	CMarkup markup;
	markup.AddElem(_T("get"));
	markup.AddAttrib(_T("gid"), pGroupId.c_str());

	IConfigure* pConfigure = CConfigure::GetInstance();
	string strUserId = pConfigure->ReadProfile ("reg_id");
	markup.AddAttrib(_T("id"), strUserId.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;
	LOGD("BuildGetGroupMemberList:GroupID:%s.\r\n",pGroupId.c_str());

	return cReqPack;
}

CHttpRequest COperPublicGroupReqBuilder::BuildCreatePulicGroup(GroupParams* groupinfo)
{
	LOGD ("CWorkGroup::BuildCreatePulicGroup GroupName %s.\r\n",groupinfo->GroupName.c_str());
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "pg";

	CMarkup markup;
	markup.AddElem(_T("create"));
	markup.AddAttrib(_T("gname"), groupinfo->GroupName.c_str());
	markup.AddAttrib(_T("gkey"), groupinfo->GroupPortraitKey.c_str());
	markup.IntoElem();
	markup.AddElem(_T("owner"));
	markup.AddAttrib(_T("id"), groupinfo->MemberInfo.MemberID.c_str());
	markup.AddAttrib(_T("name"), groupinfo->MemberInfo.MemberName.c_str());
	markup.AddAttrib(_T("key"), groupinfo->MemberInfo.PortraitKey.c_str());

	int membersize = groupinfo->Userlist.size();
	std::vector<MemberParams> Userlist = groupinfo->Userlist;
	for(int i = 0; i < membersize; i++)
	{
		markup.AddElem(_T("member"));
		markup.AddAttrib(_T("id"), Userlist[i].MemberID.c_str());
		markup.AddAttrib(_T("name"), Userlist[i].MemberName.c_str());
		markup.AddAttrib(_T("key"), Userlist[i].PortraitKey.c_str());

	}
	markup.OutOfElem();

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	return cReqPack;
}

CHttpRequest COperPublicGroupReqBuilder::BuildInvitePulicGroup(GroupParams* groupinfo)
{

	CHttpRequest cReqPack;
	cReqPack.csReqUri = "pg";

	CMarkup markup;
	markup.AddElem(_T("invite"));
	markup.AddAttrib(_T("gid"), groupinfo->GroupID.c_str());
	markup.AddAttrib(_T("gname"), groupinfo->GroupName.c_str());
	markup.AddAttrib(_T("gkey"), groupinfo->GroupPortraitKey.c_str());
	markup.IntoElem();

	markup.AddElem(_T("inviter"));
	markup.AddAttrib(_T("id"), groupinfo->MemberInfo.MemberID.c_str());
	markup.AddAttrib(_T("name"), groupinfo->MemberInfo.MemberName.c_str());
	markup.AddAttrib(_T("key"), groupinfo->MemberInfo.PortraitKey.c_str());

	int membersize = groupinfo->Userlist.size();

	if(membersize <= 0)
	{
		LOGD("membersize  error!\r\n");
		return cReqPack;
	}
	std::vector<MemberParams> Userlist = groupinfo->Userlist;

	for(int i = 0; i < membersize; i++)
	{
		markup.AddElem(_T("member"));
		markup.AddAttrib(_T("id"), Userlist[i].MemberID.c_str());
		markup.AddAttrib(_T("name"), Userlist[i].MemberName.c_str());
		markup.AddAttrib(_T("key"), Userlist[i].PortraitKey.c_str());
	}
	markup.OutOfElem();

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	return cReqPack;
}

CHttpRequest COperPublicGroupReqBuilder::BuildDeletePublicGroupInfo(GroupParams* groupinfo)
{
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "pg";

	CMarkup markup;
	markup.AddElem(_T("quit"));
	markup.AddAttrib(_T("gid"), groupinfo->GroupID.c_str());
	markup.AddAttrib(_T("id"), groupinfo->MemberInfo.MemberID.c_str());
	markup.AddAttrib(_T("gkey"), groupinfo->GroupPortraitKey.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	return cReqPack;

}

CHttpRequest COperPublicGroupReqBuilder::BuildModifyGroupInfo(GroupParams* groupinfo)
{
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "pg";

	CMarkup markup;
	markup.AddElem(_T("modify"));
	markup.AddAttrib(_T("gid"), groupinfo->GroupID.c_str());

	//base64群组名
	std::string groupname = Base64Encode(groupinfo->GroupName);

	markup.AddAttrib(_T("gname"), groupname.c_str());
	markup.AddAttrib(_T("id"), groupinfo->MemberInfo.MemberID.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	return cReqPack;
}
CHttpRequest COperPublicGroupReqBuilder::BuildModifyGroupMemberName(GroupParams* groupinfo)
{
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "pg";

	CMarkup markup;
	markup.AddElem(_T("cardmodify"));
	markup.AddAttrib(_T("gid"), groupinfo->GroupID.c_str());
	markup.AddAttrib(_T("id"), groupinfo->MemberInfo.MemberID.c_str());

	//base64群组名
	std::string cardname = Base64Encode(groupinfo->MemberInfo.MemberCardName);
	markup.AddAttrib(_T("cardname"), cardname.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	return cReqPack;
}

#pragma endregion COperPublicGroupReqBuilder 操作公共群组

CHttpRequest COperBuddyReqBuilder::GetRequestPacket()
{
	switch(m_eXO)
	{
		case XO_GET_BUDDYLIST:
		{
			return BuildGetBuddyList();
		}
		case XO_GET_BUDDYINFO:
		{
			return BuildGetBuddyInfo(m_pBuddyId);
		}
		case XO_ADD_BUDDY:
		{
			return BuildAddBuddy(m_pBuddyId.c_str());
		}
		case XO_DELETE_BUDDY:
		{
			return BuildDeleteBuddy(m_pBuddyId.c_str());
		}
		case XO_SEARCH_BUDDY:
		{
			return BuildSearchBuddy(m_pBuddyId);
		}
		case XO_SET_RECV_PUSHMSG:
		{
			return BuildSetRecvPushMsg(m_optionvalue);
		}
		case XO_GET_RECV_PUSHMSG:
		{
			return BuildGetRecvPushMsg();
		}

	}

	return CHttpRequest();
}
CHttpRequest COperBuddyReqBuilder::BuildSetRecvPushMsg(int value)
{
	LOGD ("COperBuddyReqBuilder: BuildSetRecvPushMsg Start.\r\n");
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "record";

	CMarkup markup;
	markup.AddElem(_T("post"));

	markup.AddAttrib(_T("type"), "pushstatus");

	IConfigure* pConfigure = CConfigure::GetInstance();
	string strUserId = pConfigure->ReadProfile ("reg_id");

	markup.AddAttrib(_T("username"), strUserId.c_str());

	string key;
	if(value == 1)
	{
		key = "1";
	}
	else if(value == 0)
	{
		key = "0";
	}

	markup.AddAttrib(_T("status"),   key.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	LOGD ("COperBuddyReqBuilder: BuildSetRecvPushMsg End\r\n");
	return cReqPack;
}
CHttpRequest COperBuddyReqBuilder::BuildGetRecvPushMsg()
{
	LOGD ("COperBuddyReqBuilder: BuildGetRecvPushMsg Start\r\n");
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "record";

	CMarkup markup;
	markup.AddElem(_T("get"));

	markup.AddAttrib(_T("type"), "pushstatus");

	IConfigure* pConfigure = CConfigure::GetInstance();
	string strUserId = pConfigure->ReadProfile ("reg_id");

	markup.AddAttrib(_T("username"), strUserId.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	LOGD ("COperBuddyReqBuilder: BuildGetRecvPushMsg End\r\n");
	return cReqPack;
}
CHttpRequest COperBuddyReqBuilder::BuildGetBuddyList()
{
	LOGD ("COperBuddyReqBuilder: BuildGetBuddyList Start\r\n");
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "contact";

	CMarkup markup;
	markup.AddElem(_T("info"));

	IConfigure* pConfigure = CConfigure::GetInstance();
	string strUserId = pConfigure->ReadProfile ("reg_id");

	markup.AddAttrib(_T("id"), strUserId.c_str());

	CWorkUpdateTime* pWorkUpdateTime = CWorkUpdateTime::GetInstance();
	std::string updatetime = pWorkUpdateTime->GetUpdateTime(UTT_BUDDY);

	markup.AddAttrib(_T("ver"), updatetime.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;
	LOGD ("COperBuddyReqBuilder: id:%s,ver:%s\r\n",GetUserSelfId().c_str(),updatetime.c_str());
	LOGD ("COperBuddyReqBuilder: BuildGetBuddyList End\r\n");
	return cReqPack;

}
CHttpRequest  COperBuddyReqBuilder::BuildGetBuddyInfo(const string pUserId)
{
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "contact";

	CIMI* pimi = CIMI::GetInstance();
	const string fromuid = pimi->GetUserId();

	CMarkup markup;
	markup.AddElem(_T("get"));
	markup.AddAttrib(_T("from"), fromuid.c_str());
	markup.AddAttrib(_T("to"), pUserId.c_str());
	//markup.AddAttrib(_T("id"), pUserId.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	return cReqPack;
}

CHttpRequest COperBuddyReqBuilder::BuildAddBuddy(const CHAR* pBuddyId)
{
	IConfigure* pConfigure = CConfigure::GetInstance();
	string strUserId = pConfigure->ReadProfile ("reg_id");

	CHttpRequest cReqPack;
	cReqPack.csReqUri = "contact";

	CMarkup markup;
	markup.AddElem(_T("app"));
	markup.AddAttrib(_T("type"), "0");
	markup.AddAttrib(_T("from"), strUserId.c_str());
	markup.AddAttrib(_T("to"), pBuddyId);

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	return cReqPack;

}
CHttpRequest COperBuddyReqBuilder::BuildDeleteBuddy(const CHAR* pBuddyId)
{
	IConfigure* pConfigure = CConfigure::GetInstance();
	string strUserId = pConfigure->ReadProfile ("reg_id");

	CHttpRequest cReqPack;
	cReqPack.csReqUri = "contact";

	CMarkup markup;
	markup.AddElem(_T("del"));
	markup.AddAttrib(_T("from"), strUserId.c_str());
	markup.AddAttrib(_T("to"), pBuddyId);

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	return cReqPack;
}
CHttpRequest COperBuddyReqBuilder::BuildSearchBuddy(const string key)
{
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "contact";

	CMarkup markup;
	markup.AddElem(_T("search"));
	markup.AddAttrib(_T("key"), key.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	return cReqPack;
}

CHttpRequest COperOfflineMsgReqBuilder::BuildGetOfflineMsg(const string pUserId)
{
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "offlinemsg";

	CMarkup markup;
	markup.AddElem(_T("offlinemsg"));
	markup.AddAttrib(_T("uid"), pUserId.c_str());

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	LOGD("COperOfflineMsgReqBuilder::BuildGetOfflineMsg.Msg:%s.\r\n",reqBody.c_str());
	return cReqPack;
}
CHttpRequest COperOfflineMsgReqBuilder::BuildGetOfflineMsgAck(const string pUserId,list<string> msgidlist)
{
	CHttpRequest cReqPack;
	cReqPack.csReqUri = "offlinemsgack";

	CMarkup markup;
	markup.AddElem(_T("offlinemsgack"));
	markup.AddAttrib(_T("uid"), pUserId.c_str());
	markup.IntoElem();
	markup.AddElem(_T("list"));
	markup.IntoElem();

	list<string>::iterator iter;

	for(iter = msgidlist.begin(); iter != msgidlist.end(); iter++)
	{
		markup.AddElem(_T("message"));
		markup.AddAttrib(_T("msgid"),(*iter).c_str());
	}

	markup.OutOfElem();
	markup.OutOfElem();

	string reqBody = markup.GetDoc();
	cReqPack.csBody = reqBody;

	LOGD("COperOfflineMsgReqBuilder::BuildGetOfflineMsgAck.Msg:%s.\r\n",reqBody.c_str());
	return cReqPack;
}

CHttpRequest COperOfflineMsgReqBuilder::GetRequestPacket()
{
	switch(m_eXO)
	{
		case XO_GET_OFFLINEMSG:
		{
			return BuildGetOfflineMsg(m_pUserId);
		}
		case XO_GET_OFFLINEMSG_ACK:
		{
			return BuildGetOfflineMsgAck(m_pUserId,m_msgidlist);
		}
		default:
			break;
	}

	return CHttpRequest();
}

}// end namespace Main

}// end namespace XCapStrategy
