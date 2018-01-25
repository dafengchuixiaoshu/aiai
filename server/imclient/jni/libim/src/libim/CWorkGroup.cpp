#include "BizCommon.h"
#include "CConfigure.h"
#include "Markup.h"
#include "CObserverContainer.h"
#include "CWorkGroup.h"
#include "Defines.h"
#include "../xcap/IXCapInterface.h"
#include "CWorkLogin.h"
#include "CWorkMessage.h"
#include "CWorkContact.h"

#ifndef USE_SQLITE
#include "memsave.h"
#endif

CWorkGroup* CWorkGroup::g_pWorkGroup = NULL;

CWorkGroup::CWorkGroup(void)
{

}

CWorkGroup::~CWorkGroup(void)
{
}

CWorkGroup* CWorkGroup::GetInstance()
{
	if (g_pWorkGroup == NULL)
	{
		g_pWorkGroup = new CWorkGroup();
	}
	return g_pWorkGroup;
}

HRESULT CWorkGroup::Init()
{
	return S_OK;
}

VOID CWorkGroup::Release()
{
	if (g_pWorkGroup != NULL)
	{
		delete g_pWorkGroup;
		g_pWorkGroup = NULL;
	}
}

HRESULT CWorkGroup::CreateTable()
{
#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	if (sqliteImpl)
	{
		// 建表
		std::string sSQL1 =
				"CREATE TABLE IF NOT EXISTS [groups] ("
				"[rowid] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
				"[group_id] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[group_name] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[portraitKey] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[owner_id] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[member_max_count] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[group_tmp_name] VARCHAR(255) NOT NULL DEFAULT (''),"
				"UNIQUE ([group_id] ASC)"
				");"
				"CREATE INDEX IF NOT EXISTS [idx_groups_rowid] ON [groups] ([rowid] ASC); "
				"CREATE INDEX IF NOT EXISTS [idx_groups_groupid] ON [groups] ([group_id] ASC); ";
//		LOGD("sSQL1=%s\n", sSQL1);
		sqliteImpl->ExecSql(sSQL1);
	}
#endif
	return S_OK;
}

HRESULT CWorkGroup::Invoke (VOID* pInvoke)
{
	ReqParams* pReqParams = (ReqParams*)pInvoke;
	enum ReqType eReqType = pReqParams->eReqType;

	LOGD ("CWorkGroup::eReqType:%d\n",eReqType);
	switch (eReqType)
	{
	case RT_ADD_GROUP:
		AddGroup(pReqParams);
		break;
	case RT_INVITE_GROUP:
		InviteGroup(pReqParams);
		break;
	case RT_MODIFY_GROUP:
		ModifyGroup(pReqParams);
		break;
	case RT_REMOVE_GROUP:
		DeleteGroup(pReqParams);
		break;
	case RT_MODIFY_GROUPMEMBER_NAME:
		ModifyGroupMemberName(pReqParams);
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CWorkGroup::AddGroup(ReqParams* pReqParams)
{
	LOGD ("CWorkGroup::AddGroup GroupName %s\n",pReqParams->pGroupParams->GroupName.c_str());
	m_groupParams.GroupName = pReqParams->pGroupParams->GroupName;
	m_groupParams.GroupPortraitKey = pReqParams->pGroupParams->GroupPortraitKey;
	m_groupParams.MemberInfo.MemberID = pReqParams->pGroupParams->MemberInfo.MemberID;
	m_groupParams.MemberInfo.MemberName = pReqParams->pGroupParams->MemberInfo.MemberName;
	m_groupParams.MemberInfo.PortraitKey = pReqParams->pGroupParams->MemberInfo.PortraitKey;
	m_groupParams.Userlist = pReqParams->pGroupParams->Userlist;

	//CheckGroupListExist();
	GroupParams* groupParams = &m_groupParams;
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	xcap->CreatePublicGroup(groupParams);
	return S_OK;
}
HRESULT CWorkGroup::InviteGroup(ReqParams* pReqParams)
{
	m_groupParams.GroupID = pReqParams->pGroupParams->GroupID;
	m_groupParams.GroupName = pReqParams->pGroupParams->GroupName;
	m_groupParams.GroupPortraitKey = pReqParams->pGroupParams->GroupPortraitKey;
	m_groupParams.MemberInfo.MemberID = pReqParams->pGroupParams->MemberInfo.MemberID;
	m_groupParams.MemberInfo.MemberName = pReqParams->pGroupParams->MemberInfo.MemberName;
	m_groupParams.MemberInfo.PortraitKey = pReqParams->pGroupParams->MemberInfo.PortraitKey;
	m_groupParams.Userlist = pReqParams->pGroupParams->Userlist;

	GroupParams* groupParams = &m_groupParams;
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	xcap->InvitePublicGroup(groupParams);
	return S_OK;
}
HRESULT CWorkGroup::ModifyGroup(ReqParams* pReqParams)
{
	m_groupParams.GroupID = pReqParams->pGroupParams->GroupID;
	m_groupParams.GroupName = pReqParams->pGroupParams->GroupName;
	m_groupParams.MemberInfo.MemberID = pReqParams->pGroupParams->MemberInfo.MemberID;
	m_groupParams.MemberInfo.MemberName = pReqParams->pGroupParams->MemberInfo.MemberName;
	m_groupParams.MemberInfo.PortraitKey = pReqParams->pGroupParams->MemberInfo.PortraitKey;
	m_groupParams.Userlist = pReqParams->pGroupParams->Userlist;
	GroupParams* groupParams = &m_groupParams;
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	xcap->ModifyPublicGroup(groupParams);
	return S_OK;
}
HRESULT CWorkGroup::ModifyGroupMemberName(ReqParams* pReqParams)
{
	m_groupParams.GroupID = pReqParams->pGroupParams->GroupID;
	m_groupParams.MemberInfo.MemberID = pReqParams->pGroupParams->MemberInfo.MemberID;
	m_groupParams.MemberInfo.MemberCardName = pReqParams->pGroupParams->MemberInfo.MemberCardName;
	m_groupParams.MemberInfo.PortraitKey = pReqParams->pGroupParams->MemberInfo.PortraitKey;
	m_groupParams.Userlist = pReqParams->pGroupParams->Userlist;
	GroupParams* groupParams = &m_groupParams;
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	xcap->ModifyGroupMemberName(groupParams);
	return S_OK;
}
HRESULT CWorkGroup::DeleteGroup(ReqParams* pReqParams)
{
	m_groupParams.GroupID = pReqParams->pGroupParams->GroupID;
	m_groupParams.GroupPortraitKey = pReqParams->pGroupParams->GroupPortraitKey;
	m_groupParams.MemberInfo.MemberID = pReqParams->pGroupParams->MemberInfo.MemberID;
	m_groupParams.MemberInfo.MemberName = pReqParams->pGroupParams->MemberInfo.MemberName;
	m_groupParams.MemberInfo.PortraitKey =  pReqParams->pGroupParams->MemberInfo.PortraitKey;

	GroupParams* groupParams = &m_groupParams;
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	xcap->DeletePublicGroup(groupParams);
	return S_OK;
}
GroupParams CWorkGroup::GetGroupInfo(std::string groupid)
{
	GroupParams groupinfo;
	CWorkContact* pWorkContact = CWorkContact::GetInstance();

#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();

	if(sqliteImpl)
	{
		int nResult = E_FAIL;
		struct query_rows_s query_rows;

		string sSQL = StringFormat("SELECT [group_id],[group_name],[portraitkey] ,[owner_id],[member_max_count],[group_tmp_name] FROM [groups] WHERE [group_id]='%s'",groupid.c_str());

		nResult = sqliteImpl->ExecSqlWithResult(sSQL, &query_rows);

		if (nResult < 0 || query_rows.n_rows == 0)
		{
			LOGD ("CWorkGroup::GetGroupInfo NULL,GroupID:%s\n",groupid.c_str());
			return groupinfo;
		}

		groupinfo.GroupID = sqliteImpl->GetStrValue(&query_rows,0,0);
		string groupname = sqliteImpl->GetStrValue(&query_rows,0,1);
		string grouptmpname = sqliteImpl->GetStrValue(&query_rows,0,5);
		groupinfo.GroupPortraitKey = sqliteImpl->GetStrValue(&query_rows,0,2);
		groupinfo.OwnerID= sqliteImpl->GetStrValue(&query_rows,0,3);
		groupinfo.GroupMemberCount = FromInt(pWorkContact->DB_GetMemberCount(groupinfo.GroupID));
		if(!groupname.empty())
		{
			std::string body = Base64Decode(groupname);
			groupinfo.GroupName = body;
		}
		else
		{
			if(groupinfo.GroupMemberCount.compare(_T("1")) == 0)
			{
				groupinfo.GroupName = "群聊";
			}
			else
			{
				groupinfo.GroupName = grouptmpname;
			}
		}
		groupinfo.GroupMaxMember = sqliteImpl->GetStrValue(&query_rows,0,4);
	}
#else
	group info;
	info.group_id = groupid;

	if(1 == sMemSave.GetGroupInfo(info))
	{
		groupinfo.GroupID = info.group_id;
		string groupname = info.group_name;
		string grouptmpname = info.group_tmp_name;
		groupinfo.GroupPortraitKey = info.portraitKey;
		groupinfo.OwnerID= info.owner_id;
		groupinfo.GroupMemberCount = FromInt(pWorkContact->DB_GetMemberCount(groupinfo.GroupID));
		if(!groupname.empty())
		{
			std::string body = Base64Decode(groupname);
			groupinfo.GroupName = body;
		}
		else
		{
			if(groupinfo.GroupMemberCount.compare(_T("1")) == 0)
			{
				groupinfo.GroupName = "群聊";
			}
			else
			{
				groupinfo.GroupName = grouptmpname;
			}
		}
		groupinfo.GroupMaxMember = info.member_max_count;
	}
	else
	{
		LOGD ("CWorkGroup::GetGroupInfo error,GroupID:%s\n",groupid.c_str());
	}
#endif
	return groupinfo;
}

HRESULT CWorkGroup::LoadGroupList (BOOL fromUI)
{
	CXCapImpl* xcap = CXCapImpl::GetInstance();//从服务器获取
	return xcap->GetGroupList(fromUI);
}

std::vector<GroupParams> CWorkGroup::GetGroupList()
{
	LoadGroupList();

	vector<GroupParams> grouplist;
	CWorkContact* pWorkContact = CWorkContact::GetInstance();

#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();

	if(sqliteImpl)
	{
		int nResult = E_FAIL;
		struct query_rows_s  query_rows;

		string sSQL = StringFormat("SELECT [group_id],[group_name],[portraitkey],[owner_id],[member_max_count],[group_tmp_name] FROM [groups] ORDER BY [rowid] DESC");

		nResult = sqliteImpl->ExecSqlWithResult(sSQL, &query_rows);

		if (nResult < 0 || query_rows.n_rows == 0 )
		{
			// query nothing
			return grouplist;
		}

		for (int i=0; i < query_rows.n_rows; i++)
		{
			GroupParams groupinfo;
			groupinfo.GroupID = sqliteImpl->GetStrValue(&query_rows,i,0);
			string groupname = sqliteImpl->GetStrValue(&query_rows,i,1);
			string grouptmpname = sqliteImpl->GetStrValue(&query_rows,i,5);
			groupinfo.GroupMemberCount = FromInt(pWorkContact->DB_GetMemberCount(groupinfo.GroupID));
			if(!groupname.empty())
			{
				std::string body = Base64Decode(groupname);
				groupinfo.GroupName = body;
			}
			else
			{
				if(groupinfo.GroupMemberCount.compare(_T("1")) == 0)
				{
					groupinfo.GroupName = "群聊";
				}
				else
				{
					groupinfo.GroupName = grouptmpname;
				}
			}

			groupinfo.GroupPortraitKey = sqliteImpl->GetStrValue(&query_rows,i,2);
			groupinfo.OwnerID= sqliteImpl->GetStrValue(&query_rows,i,3);
			groupinfo.GroupMaxMember = sqliteImpl->GetStrValue(&query_rows,i,4);

			if(pWorkContact->DB_GetMemberCount(groupinfo.GroupID) > 0)//没有群成员的群不显示给UI
			{
				grouplist.push_back(groupinfo);
			}
		}
	}
#else
	int count;
	vector<group> infolist;

	if((count = sMemSave.GetGroupList(infolist)) > 0)
	{
		for (int index = 0; index < count; ++index)
		{
			GroupParams groupinfo;
			groupinfo.GroupID = infolist[index].group_id;
			string groupname = infolist[index].group_name;
			string grouptmpname = infolist[index].group_tmp_name;
			groupinfo.GroupMemberCount = FromInt(pWorkContact->DB_GetMemberCount(groupinfo.GroupID));
			if(!groupname.empty())
			{
				std::string body = Base64Decode(groupname);
				groupinfo.GroupName = body;
			}
			else
			{
				if(groupinfo.GroupMemberCount.compare(_T("1")) == 0)
				{
					groupinfo.GroupName = "群聊";
				}
				else
				{
					groupinfo.GroupName = grouptmpname;
				}
			}

			groupinfo.GroupPortraitKey = infolist[index].portraitKey;
			groupinfo.OwnerID= infolist[index].owner_id;
			groupinfo.GroupMaxMember = infolist[index].member_max_count;

			if(pWorkContact->DB_GetMemberCount(groupinfo.GroupID) > 0)//没有群成员的群不显示给UI
			{
				grouplist.push_back(groupinfo);
			}
		}
	}
#endif

	LOGD("CWorkGroup::grouplistsize:%d.\n",grouplist.size());
	return grouplist;
}

MemberParams CWorkGroup::GetMember (const string inMemberId, string groupid, BOOL loadfromserver)
{
	LOGD ("CWorkGroup::Get Member\r\n");

	MemberParams memberinfo = {};

	CWorkContact* pWorkContact = CWorkContact::GetInstance();
	memberinfo = pWorkContact->GetMember(groupid,inMemberId,loadfromserver);
	if(memberinfo.MemberCardName.empty())
	{
		memberinfo.MemberCardName = memberinfo.MemberName;
	}

	return memberinfo;
}

vector<MemberParams> CWorkGroup::GetMemberList (const string groupid)
{
	CWorkContact* pWorkContact = CWorkContact::GetInstance();
	vector<MemberParams> memberlist;
	memberlist = pWorkContact->GetMemberList(groupid);

	for (int i=0; i < memberlist.size(); i++)
	{
		if(memberlist[i].MemberCardName.empty())
		{
			memberlist[i].MemberCardName = memberlist[i].MemberName;
		}
	}

	return memberlist;
}

void CWorkGroup::UpdateGroupTmpName(std::string groupid)
{
	CWorkContact* pWorkContact = CWorkContact::GetInstance();

	vector<MemberParams> memberlist;
	memberlist = pWorkContact->GetMemberList(groupid);

    int size = memberlist.size();
	std::string grouptmpname;
	if(size >= 3)
	{
		for(int i = 0 ; i < 3; i++)
		{
			grouptmpname += memberlist[i].MemberName;
			replace_separator2(grouptmpname,"'","''");
			if(i < 2)
			{
				grouptmpname += "、";
			}
		}
	}
	else
	{
		for(int i = 0 ; i < size; i++)
		{
			grouptmpname += memberlist[i].MemberName;
			replace_separator2(grouptmpname,"'","''");
			if(i < size - 1)
			{
				grouptmpname += "、";
			}
		}
	}

#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	if (sqliteImpl)
	{
		string sSQL = StringFormat("UPDATE [groups] SET [group_tmp_name] = '%s' WHERE [group_id]='%s'",grouptmpname.c_str(),groupid.c_str());
		//LOGD("sSQL1=%s\n", sSQL);
		sqliteImpl->ExecSql(sSQL);
	}
#else
	sMemSave.UpdateGroupTempName(groupid);
#endif
}

HRESULT CWorkGroup::OnNotify (VOID* pNotify)
{
	RspParams* pRspParams = (RspParams*)pNotify;
	OnNotifyGroup(pRspParams);
	enum RspType eRstType = pRspParams->eRspType;
	LOGD ("CWorkGroup::OnNotify: eRstType %d\n", eRstType);

	if(eRstType == ST_INNER_NOTIFY_GETGROUPLIST_SUCCESS)
	{
		// Load offline message list
		CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
		pWorkMessage->GetOfflineMsg();
		LOGD ("CWorkGroup::OnNotify: Start Get Offlinemsg.\r\n");
	}

	return S_OK;
}

HRESULT CWorkGroup::OnNotifyInner (VOID* pNotify)
{
	IqParams* pIqParams = (IqParams*)pNotify;
	LOGD ("CWorkGroup::OnNotifyInner: iq eType=%d, pBody=%s\n", pIqParams->eType, pIqParams->pBody.c_str());
	CWorkContact* pWorkContact = CWorkContact::GetInstance();
	if(pIqParams->eType == IT_PG)
	{
		string content = pIqParams->pBody;
		string xmlDoc = content;
		string GroupID;
		string groupname;
		string groupMaxcount;
		string notifytype;
		string servertime;

		string id;
		string groupkey;
		string cardname;
		std::vector<MemberParams> memberlist;

		CMarkup markup;
		markup.SetDoc(xmlDoc.c_str());
		markup.FindElem("group");
		GroupID = markup.GetAttrib("gid");
		groupname = markup.GetAttrib("gname");
		groupkey = markup.GetAttrib("gkey");
		groupMaxcount = markup.GetAttrib("gmaxcount");
		notifytype = markup.GetAttrib("type");
		id = markup.GetAttrib("id");
		cardname = markup.GetAttrib("cardname");
		servertime = markup.GetAttrib("curtime");

		if(notifytype.compare(_T("create")) == 0)
		{
			markup.IntoElem();

			MemberParams inviterinfo;
			if(markup.FindElem(_T("inviter")))
			{
				inviterinfo.MemberID = markup.GetAttrib("id");
				inviterinfo.MemberName = markup.GetAttrib("name");
				inviterinfo.PortraitKey = markup.GetAttrib("key");
				inviterinfo.MemberCardName = markup.GetAttrib("cardname");
				inviterinfo.EmotionMood = markup.GetAttrib("info");
				inviterinfo.CmsID = markup.GetAttrib("cmsid");

				markup.ResetMainPos();
			}

			while(markup.FindElem(_T("member")))
			{
				MemberParams memberinfo;
				memberinfo.MemberID = markup.GetAttrib("id");
				memberinfo.MemberName = markup.GetAttrib("name");
				memberinfo.PortraitKey = markup.GetAttrib("key");
				memberinfo.MemberCardName = markup.GetAttrib("cardname");
				memberinfo.EmotionMood = markup.GetAttrib("info");
				memberinfo.CmsID = markup.GetAttrib("cmsid");

				memberlist.push_back(memberinfo);

			}
			markup.OutOfElem();

			GroupInfo groupinfo;
			groupinfo.strGroupId = GroupID;
			groupinfo.strOwnerId = memberlist[0].MemberID;
			groupinfo.strGroupName = groupname;
			groupinfo.memberlist = memberlist;
			groupinfo.memberMaxcount = groupMaxcount;
			groupinfo.strGroupIcon = groupkey;

			OnNotifySaveDatabase(groupinfo,XO_ADD);
			UpdateGroupTmpName(GroupID);//更新群临时名

			string total;

			if(!inviterinfo.MemberID.empty())
			{
				if(inviterinfo.MemberCardName.empty())
				{
					if(inviterinfo.MemberName.empty())
					{
						total = StringFormat("%s邀请你加入了群聊。",inviterinfo.MemberID.c_str());
					}
					else
					{
						total = StringFormat("%s邀请你加入了群聊。",inviterinfo.MemberName.c_str());
					}
				}
				else
				{
					std::string body = Base64Decode(inviterinfo.MemberCardName);
					total = StringFormat("%s邀请你加入了群聊。",body.c_str());
				}
			}
			else
			{
				if(memberlist[0].MemberCardName.empty())
				{
					if(memberlist[0].MemberName.empty())
					{
						total = StringFormat("%s邀请你加入了群聊。",memberlist[0].MemberID.c_str());
					}
					else
					{
						total = StringFormat("%s邀请你加入了群聊。",memberlist[0].MemberName.c_str());
					}
				}
				else
				{
					std::string body = Base64Decode(memberlist[0].MemberCardName);
					total = StringFormat("%s邀请你加入了群聊。",body.c_str());
				}
			}

//			MessageParams messageParams = {};
//			messageParams.pSender   = groupinfo.strOwnerId;
//			messageParams.pReceiver = GroupID;
//			messageParams.pContent  = total;
//			messageParams.eType     = MT_GROUP;
//			messageParams.eKind     = KT_NOTIFICATION;
//			messageParams.eCategory = CT_PLAIN_TEXT;
//			messageParams.pTime = servertime;

			//CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
			//pWorkMessage->WriteMessage(messageParams);

			m_groupParams.GroupID = GroupID;
			m_groupParams.GroupName = groupname;
			m_groupParams.GroupPortraitKey = groupkey;

			m_rspParams.eRspType = ST_NOTIFY_CREATE_GROUP;
			m_rspParams.pGroupParams = &m_groupParams;
			OnNotifyGroup(&m_rspParams);

		}
		else if(notifytype.compare(_T("invite")) == 0)
		{
			markup.IntoElem();

			MemberParams inviterinfo;
			if(markup.FindElem(_T("inviter")))
			{
				inviterinfo.MemberID =  markup.GetAttrib("id");
				inviterinfo.MemberName = markup.GetAttrib("name");
				inviterinfo.PortraitKey = markup.GetAttrib("key");
				inviterinfo.MemberCardName = markup.GetAttrib("cardname");
				inviterinfo.EmotionMood = markup.GetAttrib("info");
				inviterinfo.CmsID = markup.GetAttrib("cmsid");

				markup.ResetMainPos();
			}

			int membercount = 0;
			while(markup.FindElem(_T("member")))
			{
				MemberParams memberinfo;
				memberinfo.MemberID = markup.GetAttrib("id");
				memberinfo.MemberName = markup.GetAttrib("name");
				memberinfo.PortraitKey = markup.GetAttrib("key");
				memberinfo.MemberCardName = markup.GetAttrib("cardname");
				memberinfo.EmotionMood = markup.GetAttrib("info");
				memberinfo.CmsID = markup.GetAttrib("cmsid");

				memberlist.push_back(memberinfo);

				if(memberinfo.MemberID == inviterinfo.MemberID)
				{
					inviterinfo.MemberCardName = memberinfo.MemberCardName;
				}

				membercount++;
			}
			markup.OutOfElem();

			GroupInfo groupinfo;
			groupinfo.strGroupId = GroupID;
			groupinfo.strGroupIcon = groupkey;
			groupinfo.memberMaxcount = groupMaxcount;
			groupinfo.memberlist = memberlist;

			CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
			pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_INVITE);

			string sender;

			if(inviterinfo.MemberCardName.empty())
			{
				if(inviterinfo.MemberName.empty())
				{
					sender = inviterinfo.MemberID;
				}
				else
				{
					sender = inviterinfo.MemberName;
				}
			}
			else
			{
				std::string body = Base64Decode(inviterinfo.MemberCardName);
				sender = body;
			}

			string invite = "邀请";
			string users ="";
			string end ="加入了群聊。";
			string total ="";

			for(int i = 0; i < membercount; i++)
			{
				users += memberlist[i].MemberName;
				if(i != membercount - 1)
				{
					users += "、";
				}
			}

			total = sender + invite +  users + end;

//			MessageParams messageParams = {};
//			messageParams.pSender   = inviterinfo.MemberID;
//			messageParams.pReceiver = GroupID;
//			messageParams.pContent  = total;
//			messageParams.eType     = MT_GROUP;
//			messageParams.eKind     = KT_NOTIFICATION;
//			messageParams.eCategory = CT_PLAIN_TEXT;
//			messageParams.pTime = servertime;
//
//			CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
//			pWorkMessage->WriteMessage(messageParams);

			UpdateGroupTmpName(GroupID);//更新群临时名

			m_groupParams.GroupID = GroupID;
			m_groupParams.GroupName = groupname;
			m_groupParams.GroupPortraitKey = groupkey;

			m_rspParams.eRspType = ST_NOTIFY_REFRESH_GROUP;
			m_rspParams.pGroupParams = &m_groupParams;
			OnNotifyGroup(&m_rspParams);

		}
		else if(notifytype.compare(_T("modify")) == 0)
		{
			GroupInfo groupinfo;
			groupinfo.strGroupId = GroupID;
			groupinfo.strGroupName = groupname;

			CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
			pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_MODIFY);

			MemberParams MemberInfo;
			MemberInfo = GetMember(id, GroupID, FALSE);

			//解base64
			std::string body = Base64Decode(groupname);

			string total;
			if(MemberInfo.MemberName.empty())
			{
				total = StringFormat("%s将群名称修改为了%s。",MemberInfo.MemberID.c_str(),body.c_str());
			}
			else
			{
				total = StringFormat("%s将群名称修改为了%s。",MemberInfo.MemberName.c_str(),body.c_str());
			}

//			MessageParams messageParams = {};
//			messageParams.pSender   = id;
//			messageParams.pReceiver = GroupID;
//			messageParams.pContent  = total;
//			messageParams.eType     = MT_GROUP;
//			messageParams.eKind     = KT_NOTIFICATION;
//			messageParams.eCategory = CT_PLAIN_TEXT;
//			messageParams.pTime = servertime;
//
//			CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
//			pWorkMessage->WriteMessage(messageParams);

			m_groupParams.GroupID = GroupID;
			m_groupParams.GroupName = groupname;

			m_rspParams.eRspType = ST_NOTIFY_MODIFY_GROUP;
			m_rspParams.pGroupParams = &m_groupParams;
			OnNotifyGroup(&m_rspParams);
		}
		else if(notifytype.compare(_T("cardmodify")) == 0)
		{

			MemberParams member;
			member.MemberID = id;
			member.MemberCardName = cardname;

			std::vector<MemberParams> memberlist;
			memberlist.push_back(member);

			GroupInfo groupinfo;
			groupinfo.strGroupId = GroupID;
			groupinfo.memberlist = memberlist;

			CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
			pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_MODIFY);

			m_groupParams.GroupID = GroupID;
			std::string body = Base64Decode(cardname);
			m_groupParams.GroupCardName = body;

			m_rspParams.eRspType = ST_NOTIFY_MODIFY_GROUPMEMBERNAME;
			m_rspParams.pGroupParams = &m_groupParams;
			OnNotifyGroup(&m_rspParams);
		}
		else if(notifytype.compare(_T("quit")) == 0)
		{
			MemberParams NewUserInfo;
			NewUserInfo = GetMember(id, GroupID, FALSE);

			MemberParams memberinfo;
			memberinfo.MemberID = id;
			memberlist.push_back(memberinfo);

			GroupInfo groupinfo;
			groupinfo.strGroupId = GroupID;
			groupinfo.strGroupIcon = groupkey;
			groupinfo.memberlist = memberlist;

			CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
			pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_DELETE);

			string total;
			if(NewUserInfo.MemberName.empty())
			{
				total = StringFormat("%s退出了群。",id.c_str());
			}
			else
			{
				total = StringFormat("%s退出了群。",NewUserInfo.MemberName.c_str());
			}

//			MessageParams messageParams = {};
//			messageParams.pSender   = id;
//			messageParams.pReceiver = GroupID;
//			messageParams.pContent  = total;
//			messageParams.eType     = MT_GROUP;
//			messageParams.eKind     = KT_NOTIFICATION;
//			messageParams.eCategory = CT_PLAIN_TEXT;
//			messageParams.pTime = servertime;
//
//			CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
//			pWorkMessage->WriteMessage(messageParams);

			UpdateGroupTmpName(GroupID);//更新群临时名

			m_groupParams.GroupID = GroupID;
			m_groupParams.GroupName = groupname;
			m_groupParams.GroupPortraitKey = groupkey;

			m_rspParams.eRspType = ST_NOTIFY_QUIT_GROUP;
			m_rspParams.pGroupParams = &m_groupParams;
			OnNotifyGroup(&m_rspParams);
		}

	}

	return S_OK;
}

HRESULT CWorkGroup::OnNotifyGroup (RspParams* pRspParams)
{
	IObserverContainer*  observerContainer = CObserverContainer::GetInstance();
	IObserver* observer = observerContainer->FindObserver(OT_GROUP);
	if (observer)
	{
		observer->OnNotify((VOID*)pRspParams);
		LOGD("CWorkGroup OnNotifyGroup.\n");
	}
	else
	{
		LOGD("observer NULL.\n");
	}

	return S_OK;
}

HRESULT CWorkGroup::OnNotifySaveDatabase (GroupInfo info,XCAP_OPER oper,BOOL bself)
{
	CWorkContact* pWorkContact = CWorkContact::GetInstance();

#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();

	switch(oper)
	{
		case XO_ADD://插入数据库
		{
			if (sqliteImpl)
			{
				string sSQL1;
				sSQL1 = StringFormat("INSERT OR IGNORE INTO [groups] "
				"([group_id], [group_name],[owner_id],[portraitKey],[member_max_count],[group_tmp_name] ) "
				"VALUES "
				"('%s', '%s', '%s','%s','%s','%s');",
				info.strGroupId.c_str(),
				info.strGroupName.c_str(),
				info.strOwnerId.c_str(),
				info.strGroupIcon.c_str(),
				info.memberMaxcount.c_str(),
				info.strGroupTmpName.c_str());
				//LOGD("sSQL1=%s\n", sSQL1.c_str());
				int  nResult = sqliteImpl->ExecSql(sSQL1);

				if (nResult < 0 )
				{
					LOGD("INSERT OR IGNORE INTO [groups] failed,GroupID:%s\n",info.strGroupId.c_str());
				}

				for(int i = 0; i < info.memberlist.size(); i++)
				{
					string memberid = info.memberlist[i].MemberID.c_str();
					string membername = info.memberlist[i].MemberName.c_str();
					string memberemotion = info.memberlist[i].EmotionMood.c_str();
					string membercardname = info.memberlist[i].MemberCardName.c_str();
					string memberkey = info.memberlist[i].PortraitKey.c_str();
					string membercmsid = info.memberlist[i].CmsID.c_str();

					replace_separator2(memberid,"'","''");
					replace_separator2(membername,"'","''");
					replace_separator2(memberemotion,"'","''");
					replace_separator2(membercardname,"'","''");
					replace_separator2(memberkey,"'","''");
					replace_separator2(membercmsid,"'","''");

					BuddyParams buddy = {};
					buddy.pUserId = memberid;
					buddy.pUserName = membername;
					buddy.pPortraitKey = memberkey;
					buddy.pEmotionMood = memberemotion;
					buddy.pCmsId = membercmsid;

					pWorkContact->DB_AddMember(info.strGroupId,buddy,membercardname);
				}

//				if(!info.memberlist.empty())
//				{
//					if(!info.memberlist[0].MemberCardName.empty())
//					{
//						string memberid = info.memberlist[0].MemberID.c_str();
//						string membercardname = info.memberlist[0].MemberCardName.c_str();
//						replace_separator2(memberid,"'","''");
//						replace_separator2(membercardname,"'","''");
//
//						string sSQL;
//						sSQL = StringFormat("UPDATE [members] SET [member_card_name] ='%s' WHERE [group_id]='%s' AND [member_id]='%s'",membercardname.c_str(),info.strGroupId.c_str(),memberid.c_str());
//						//LOGD("sSQL1=%s\n", sSQL);
//						int  nResult = sqliteImpl->ExecSql(sSQL);
//
//						if (nResult < 0 )
//						{
//							LOGD("UPDATE GroupMemberName failed.GroupName:%s\n",info.strGroupName.c_str());
//						}
//					}
//				}

			}

		}
			break;
		case XO_INVITE:
		{
			if (sqliteImpl)
			{
				if(!info.strGroupIcon.empty())
				{
					string sSQL;
					sSQL = StringFormat("UPDATE [groups] SET [portraitKey] ='%s' WHERE [group_id]='%s'",info.strGroupIcon.c_str(),info.strGroupId.c_str());
					//LOGD("sSQL1=%s\n", sSQL);
					int  nResult = sqliteImpl->ExecSql(sSQL);

					if (nResult < 0 )
					{
						LOGD("UPDATE [groups] SET [portraitKey] failed.GroupID:%s\n",info.strGroupId.c_str());
					}

				}

				for(int i = 0; i < info.memberlist.size(); i++)
				{

					string memberid = info.memberlist[i].MemberID.c_str();
					string membername = info.memberlist[i].MemberName.c_str();
					string memberemotion = info.memberlist[i].EmotionMood.c_str();
					string membercardname = info.memberlist[i].MemberCardName.c_str();
					string memberkey = info.memberlist[i].PortraitKey.c_str();
					string membercmsid = info.memberlist[i].CmsID.c_str();

					replace_separator2(memberid,"'","''");
					replace_separator2(membername,"'","''");
					replace_separator2(memberemotion,"'","''");
					replace_separator2(membercardname,"'","''");
					replace_separator2(memberkey,"'","''");
					replace_separator2(membercmsid,"'","''");

					BuddyParams buddy = {};
					buddy.pUserId = memberid;
					buddy.pUserName = membername;
					buddy.pPortraitKey = memberkey;
					buddy.pEmotionMood = memberemotion;
					buddy.pCmsId = membercmsid;

					pWorkContact->DB_AddMember(info.strGroupId,buddy,membercardname);
				}
			}
		}
		break;
		case XO_DELETE:
		{
			if (sqliteImpl)
			{
				string sSQL;
				string sSQL1;

				if(bself)
				{
					sSQL = StringFormat("DELETE FROM [groups] WHERE [group_id]='%s'",
					info.strGroupId.c_str());
					//LOGD("sSQL1=%s\n", sSQL);
					int  nResult = sqliteImpl->ExecSql(sSQL);

					if (nResult < 0)
					{
						LOGD("DELETE FROM [groups] failed.GroupID:%s\n",info.strGroupId.c_str());
					}

					pWorkContact->DB_DeleteMemberList(info.strGroupId);
				}
				else
				{
					pWorkContact->DB_DeleteMember(info.strGroupId, info.memberlist[0].MemberID);

					if(!info.strGroupIcon.empty())
					{
						string sSQL;
						sSQL =StringFormat("UPDATE [groups] SET [portraitKey] ='%s' WHERE [group_id]='%s'",info.strGroupIcon.c_str(),info.strGroupId.c_str());
						//LOGD("sSQL1=%s\n", sSQL);
						int  nResult = sqliteImpl->ExecSql(sSQL);

						if (nResult < 0 )
						{
							LOGD("UPDATE [groups] SET [portraitKey] failed.GroupID:%s\n",info.strGroupId.c_str());
						}
					}
				}
			}
		}
		break;
		case XO_MODIFY:
		{
			if (sqliteImpl)
			{
				if(!info.strGroupIcon.empty())
				{
					string sSQL;
					sSQL = StringFormat("UPDATE [groups] SET [portraitKey] ='%s' WHERE [group_id]='%s'",info.strGroupIcon.c_str(),info.strGroupId.c_str());
					//LOGD("sSQL1=%s\n", sSQL);
					int  nResult = sqliteImpl->ExecSql(sSQL);

					if (nResult < 0 )
					{
						LOGD("UPDATE [groups] SET [portraitKey] failed.GroupID:%s\n",info.strGroupId.c_str());
					}
				}

				if(!info.strGroupName.empty())
				{
					string sSQL;
					sSQL = StringFormat("UPDATE [groups] SET [group_name] ='%s' WHERE [group_id]='%s'",info.strGroupName.c_str(),info.strGroupId.c_str());
					//LOGD("sSQL1=%s\n", sSQL);
					int  nResult = sqliteImpl->ExecSql(sSQL);

					if (nResult < 0 )
					{
						LOGD("UPDATE [groups] SET [group_name] failed.GroupName:%s\n",info.strGroupName.c_str());
					}
				}

				if(!info.memberMaxcount.empty())
				{
					string sSQL1;
					sSQL1 = StringFormat("UPDATE [groups] SET [member_max_count] ='%s' WHERE [group_id]='%s'",info.memberMaxcount.c_str(),info.strGroupId.c_str());
					//LOGD("sSQL1=%s\n", sSQL1);
					int  nResult = sqliteImpl->ExecSql(sSQL1);

					if (nResult < 0)
					{
						LOGD("UPDATE [groups] SET [member_max_count] failed.GroupID:%s,MemberMaxCount:%s\n",info.strGroupId.c_str(),info.memberMaxcount.c_str());
					}
				}

				if(!info.memberlist.empty())
				{
					if(!info.memberlist[0].MemberCardName.empty())
					{
						string memberid = info.memberlist[0].MemberID.c_str();
						string membercardname = info.memberlist[0].MemberCardName.c_str();
						replace_separator2(memberid,"'","''");
						replace_separator2(membercardname,"'","''");

						pWorkContact->DB_ModifyMember(info.strGroupId,memberid,membercardname);
					}
				}
			}
			break;
		}

		case XO_GET_GROUPMEMBERLIST:
		{
			for(int i = 0; i < info.memberlist.size(); i++)
			{
				string memberid = info.memberlist[i].MemberID.c_str();
				string membername = info.memberlist[i].MemberName.c_str();
				string memberemotion = info.memberlist[i].EmotionMood.c_str();
				string membercardname = info.memberlist[i].MemberCardName.c_str();
				string memberkey = info.memberlist[i].PortraitKey.c_str();
				string membercmsid = info.memberlist[i].CmsID.c_str();

				replace_separator2(memberid,"'","''");
				replace_separator2(membername,"'","''");
				replace_separator2(memberemotion,"'","''");
				replace_separator2(membercardname,"'","''");
				replace_separator2(memberkey,"'","''");
				replace_separator2(membercmsid,"'","''");

				BuddyParams buddy = {};
				buddy.pUserId = memberid;
				buddy.pUserName = membername;
				buddy.pPortraitKey = memberkey;
				buddy.pEmotionMood = memberemotion;
				buddy.pCmsId = membercmsid;

				pWorkContact->DB_AddMember(info.strGroupId,buddy,membercardname);
			}

			break;
		}
		default:
			 break;
	}
#else
	switch(oper)
	{
		case XO_ADD://插入数据库
		{
			group meminfo;
			meminfo.group_id = info.strGroupId;
			meminfo.group_name = info.strGroupName;
			meminfo.owner_id = info.strOwnerId;
			meminfo.portraitKey = info.strGroupIcon;
			meminfo.member_max_count = info.memberMaxcount;
			meminfo.group_tmp_name = info.strGroupTmpName;

			if(sMemSave.InsertGroupInfo(meminfo))
			{
				for(int i = 0; i < info.memberlist.size(); i++)
				{
					string memberid = info.memberlist[i].MemberID.c_str();
					string membername = info.memberlist[i].MemberName.c_str();
					string memberemotion = info.memberlist[i].EmotionMood.c_str();
					string membercardname = info.memberlist[i].MemberCardName.c_str();
					string memberkey = info.memberlist[i].PortraitKey.c_str();
					string membercmsid = info.memberlist[i].CmsID.c_str();

					replace_separator2(memberid,"'","''");
					replace_separator2(membername,"'","''");
					replace_separator2(memberemotion,"'","''");
					replace_separator2(membercardname,"'","''");
					replace_separator2(memberkey,"'","''");
					replace_separator2(membercmsid,"'","''");

					BuddyParams buddy = {};
					buddy.pUserId = memberid;
					buddy.pUserName = membername;
					buddy.pPortraitKey = memberkey;
					buddy.pEmotionMood = memberemotion;
					buddy.pCmsId = membercmsid;

					pWorkContact->DB_AddMember(info.strGroupId,buddy,membercardname);
				}
			}
		}
		break;
		case XO_INVITE:
		{
			if(!info.strGroupIcon.empty())
			{
				sMemSave.UpdateGroupInfoForInvite(info.strGroupId, "portraitKey", info.strGroupIcon);
			}

			for(int i = 0; i < info.memberlist.size(); i++)
			{
				string memberid = info.memberlist[i].MemberID.c_str();
				string membername = info.memberlist[i].MemberName.c_str();
				string memberemotion = info.memberlist[i].EmotionMood.c_str();
				string membercardname = info.memberlist[i].MemberCardName.c_str();
				string memberkey = info.memberlist[i].PortraitKey.c_str();
				string membercmsid = info.memberlist[i].CmsID.c_str();

				replace_separator2(memberid,"'","''");
				replace_separator2(membername,"'","''");
				replace_separator2(memberemotion,"'","''");
				replace_separator2(membercardname,"'","''");
				replace_separator2(memberkey,"'","''");
				replace_separator2(membercmsid,"'","''");

				BuddyParams buddy = {};
				buddy.pUserId = memberid;
				buddy.pUserName = membername;
				buddy.pPortraitKey = memberkey;
				buddy.pEmotionMood = memberemotion;
				buddy.pCmsId = membercmsid;

				pWorkContact->DB_AddMember(info.strGroupId,buddy,membercardname);
			}
		}
		break;
		case XO_DELETE:
		{
			if (sqliteImpl)
			{
				if(bself)
				{
					sMemSave.UpdateGroupInfoForInvite(info.strGroupId, "", "");

					pWorkContact->DB_DeleteMemberList(info.strGroupId);
				}
				else
				{
					pWorkContact->DB_DeleteMember(info.strGroupId, info.memberlist[0].MemberID);

					if(!info.strGroupIcon.empty())
					{
						sMemSave.UpdateGroupInfoForInvite(info.strGroupId, "portraitKey", info.strGroupIcon);
					}
				}
			}
		}
		break;
		case XO_MODIFY:
		{
			if(!info.strGroupIcon.empty())
			{
				sMemSave.UpdateGroupInfoForInvite(info.strGroupId, "portraitKey", info.strGroupIcon);
			}

			if(!info.strGroupName.empty())
			{
				sMemSave.UpdateGroupInfoForInvite(info.strGroupId, "group_name", info.strGroupName);
			}

			if(!info.memberMaxcount.empty())
			{
				sMemSave.UpdateGroupInfoForInvite(info.strGroupId, "member_max_count", info.memberMaxcount);
			}

			if(!info.memberlist.empty())
			{
				if(!info.memberlist[0].MemberCardName.empty())
				{
					string memberid = info.memberlist[0].MemberID.c_str();
					string membercardname = info.memberlist[0].MemberCardName.c_str();
					replace_separator2(memberid,"'","''");
					replace_separator2(membercardname,"'","''");

					pWorkContact->DB_ModifyMember(info.strGroupId,memberid,membercardname);
				}
			}
		}
		break;
		case XO_GET_GROUPMEMBERLIST:
		{
			for(int i = 0; i < info.memberlist.size(); i++)
			{
				string memberid = info.memberlist[i].MemberID.c_str();
				string membername = info.memberlist[i].MemberName.c_str();
				string memberemotion = info.memberlist[i].EmotionMood.c_str();
				string membercardname = info.memberlist[i].MemberCardName.c_str();
				string memberkey = info.memberlist[i].PortraitKey.c_str();
				string membercmsid = info.memberlist[i].CmsID.c_str();

				replace_separator2(memberid,"'","''");
				replace_separator2(membername,"'","''");
				replace_separator2(memberemotion,"'","''");
				replace_separator2(membercardname,"'","''");
				replace_separator2(memberkey,"'","''");
				replace_separator2(membercmsid,"'","''");

				BuddyParams buddy = {};
				buddy.pUserId = memberid;
				buddy.pUserName = membername;
				buddy.pPortraitKey = memberkey;
				buddy.pEmotionMood = memberemotion;
				buddy.pCmsId = membercmsid;

				pWorkContact->DB_AddMember(info.strGroupId,buddy,membercardname);
			}

			break;
		}
		default:
			 break;
	}
#endif
	return S_OK;
}
HRESULT CWorkGroup::IsGroupExist (std::string pGroupId)
{
#ifdef USE_SQLITE
	int nResult = E_FAIL;
	UINT nRowNum = 0;

	struct query_rows_s query_rows;

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	if (sqliteImpl)
	{
		string sSQL1;
		sSQL1 = StringFormat("SELECT count([group_id]) "
		"FROM [groups] "
		"WHERE [group_id]='%s'; ",pGroupId.c_str());
//		LOGD("sSQL1=%s\n", sSQL1);
		nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);
	}

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		return FALSE;
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		nRowNum = sqliteImpl->GetIntValue(&query_rows,i,0);
	}

	return ((nRowNum == 0) ? (FALSE) : (TRUE));
#else
	if(1 == sMemSave.IsGroupExist(pGroupId))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
#endif
}

HRESULT CWorkGroup::NotifyNetworkStatus(enum NetWorkStatusType eNetworkStatusType)
{
	LOGD ("CWorkGroup::NotifyNetworkStatus=%d\r\n", eNetworkStatusType);

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
void CWorkGroup::OnNotifyOperateGroup(int oper,CIm* pim)
{
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	if( pWorkMessage->IsMessageExist(pim->m_id))
	{
		LOGD("CWorkGroup::OnNotifyOperateGroup,msgid has exist.\r\n");
		return;
	}

	switch(oper)
	{
		case IM_PGCREATE://收到创建群通知
		{
			CImPGCreate* impgcreate = (CImPGCreate*)(pim);

			if(NULL == impgcreate)
			{
				LOGD("impgcreate pointer is null.");
			}
			else
			{
				string total;

				if(impgcreate->m_type == 0)//用户名
				{
					total = StringFormat("%s邀请你加入了群聊。",impgcreate->m_name.c_str());
				}
				else if(impgcreate->m_type == 1)//用户群名片
				{
					std::string body = Base64Decode(impgcreate->m_name);
					total = StringFormat("%s邀请你加入了群聊。",body.c_str());
				}

//				MessageParams messageParams = {};
//				messageParams.pSender   = impgcreate->m_gid;
//				messageParams.pReceiver = impgcreate->m_gid;
//				messageParams.pContent  = total;
//				messageParams.eType     = MT_GROUP;
//				messageParams.eKind     = KT_NOTIFICATION;
//				messageParams.eCategory = CT_PLAIN_TEXT;
//				messageParams.pTime = impgcreate->m_time;
//
//				CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
//				pWorkMessage->WriteMessage(messageParams);

				GetGroupinfoByID(impgcreate->m_gid);

			}
		}
		break;
		case IM_PGINVITE://收到拉人进群通知
		{
			CImPGInvite* impginvite = (CImPGInvite*)(pim);

			if(NULL == impginvite)
			{
				LOGD("impginvite pointer is null.");
			}
			else
			{
				string total;
				string sender;

				if(impginvite->m_type == 0)//用户名
				{
					sender = impginvite->m_name;
				}
				else if(impginvite->m_type == 1)//用户群名片
				{
					sender = Base64Decode(impginvite->m_name);
				}

				string invite = "邀请";
				string end ="加入了群聊。";
				total = sender + invite +  impginvite->m_invites + end;

//				MessageParams messageParams = {};
//				messageParams.pSender   = impginvite->m_gid;
//				messageParams.pReceiver = impginvite->m_gid;
//				messageParams.pContent  = total;
//				messageParams.eType     = MT_GROUP;
//				messageParams.eKind     = KT_NOTIFICATION;
//				messageParams.eCategory = CT_PLAIN_TEXT;
//				messageParams.pTime = impginvite->m_time;
//
//				CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
//				pWorkMessage->WriteMessage(messageParams);

				GetGroupinfoByID(impginvite->m_gid);

			}
		}
		break;
		case IM_PGQUIT://收到退群通知
		{
			CImPGQuit* impgquit = (CImPGQuit*)(pim);

			if(NULL == impgquit)
			{
				LOGD("impgquit pointer is null.");
			}
			else
			{
				std::vector<MemberParams> memberlist;

				MemberParams memberinfo;
				memberinfo.MemberID = impgquit->m_uid;
				memberlist.push_back(memberinfo);

				GroupInfo groupinfo;
				groupinfo.strGroupId = impgquit->m_gid;
				groupinfo.strGroupIcon = impgquit->m_gkey;
				groupinfo.memberlist = memberlist;

				CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
				pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_DELETE);

				string total;

				if(impgquit->m_type == 0)//用户名
				{
					total = StringFormat("%s退出了群。",impgquit->m_name.c_str());
				}
				else if(impgquit->m_type == 1)//用户群名片
				{
					string user = Base64Decode(impgquit->m_name);
					total = StringFormat("%s退出了群。",user.c_str());
				}

//				MessageParams messageParams = {};
//				messageParams.pSender   = impgquit->m_uid;
//				messageParams.pReceiver = impgquit->m_gid;
//				messageParams.pContent  = total;
//				messageParams.eType     = MT_GROUP;
//				messageParams.eKind     = KT_NOTIFICATION;
//				messageParams.eCategory = CT_PLAIN_TEXT;
//				messageParams.pTime = impgquit->m_time;
//
//				CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
//				pWorkMessage->WriteMessage(messageParams);

				UpdateGroupTmpName(impgquit->m_gid);//更新群临时名

				m_groupParams.GroupID = impgquit->m_gid;
				m_groupParams.GroupPortraitKey = impgquit->m_gkey;

				m_rspParams.eRspType = ST_NOTIFY_QUIT_GROUP;
				m_rspParams.pGroupParams = &m_groupParams;
				OnNotifyGroup(&m_rspParams);

			}
		}
		break;
		case IM_PGMODIFY://收到修改群名通知
		{
			CImPGModify* impgmodify = (CImPGModify*)(pim);

			if(NULL == impgmodify)
			{
				LOGD("impgmodify pointer is null.");
			}
			else
			{
				GroupInfo groupinfo;
				groupinfo.strGroupId = impgmodify->m_gid;
				groupinfo.strGroupName = impgmodify->m_gname;

				CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
				pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_MODIFY);

				//解base64
				std::string body = Base64Decode(impgmodify->m_gname);

				string total;
				string user;
				if(impgmodify->m_type == 0)//用户名
				{
					total = StringFormat("%s将群名称修改为了%s。",impgmodify->m_name.c_str(),body.c_str());
				}
				else if(impgmodify->m_type == 1)//用户群名片
				{
					user = Base64Decode(impgmodify->m_name);
					total = StringFormat("%s将群名称修改为了%s。",user.c_str(),body.c_str());
				}

//				MessageParams messageParams = {};
//				//messageParams.pSender   = user;
//				messageParams.pReceiver = impgmodify->m_gid;
//				messageParams.pContent  = total;
//				messageParams.eType     = MT_GROUP;
//				messageParams.eKind     = KT_NOTIFICATION;
//				messageParams.eCategory = CT_PLAIN_TEXT;
//				messageParams.pTime = impgmodify->m_time;
//
//				CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
//				pWorkMessage->WriteMessage(messageParams);

				m_groupParams.GroupID = impgmodify->m_gid;
				m_groupParams.GroupName = impgmodify->m_gname;

				m_rspParams.eRspType = ST_NOTIFY_MODIFY_GROUP;
				m_rspParams.pGroupParams = &m_groupParams;
				OnNotifyGroup(&m_rspParams);

			}
		}
		break;
		case IM_PGCARD://收到修改群名片通知
		{
			CImPGCard* impgcard = (CImPGCard*)(pim);

			if(NULL == impgcard)
			{
				LOGD("impgcard pointer is null.");
			}
			else
			{
				MemberParams member;
				member.MemberID = impgcard->m_uid;
				member.MemberCardName = impgcard->m_cardname;

				std::vector<MemberParams> memberlist;
				memberlist.push_back(member);

				GroupInfo groupinfo;
				groupinfo.strGroupId = impgcard->m_gid;
				groupinfo.memberlist = memberlist;

				CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
				pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_MODIFY);

				m_groupParams.GroupID = impgcard->m_gid;
				std::string body = Base64Decode(impgcard->m_cardname);
				m_groupParams.GroupCardName = body;

				m_rspParams.eRspType = ST_NOTIFY_MODIFY_GROUPMEMBERNAME;
				m_rspParams.pGroupParams = &m_groupParams;
				OnNotifyGroup(&m_rspParams);
			}
		}
		break;
		default:
		break;
	}
}
void CWorkGroup::GetGroupinfoByID(string groupid)
{
	CXCapImpl* xcap = CXCapImpl::GetInstance();//从服务器获取
	xcap->GetGroupMemberList(groupid);
}
