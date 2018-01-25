
#include "ResponseProcessor_main2.h"
#include "XCapCore.h"
#include "CWorkUpdateTime.h"
#include "CWorkGroup.h"
#include "CWorkerContainer.h"
#include "CWorker.h"
#include "BizCommon.h"
#include "Defines.h"
#include "CWorkMessage.h"
#include "MarkUp.h"
#include "CWorkContact.h"

namespace XCapStrategy
{
namespace Main  ///< 主要算法??
{

#pragma region 操作公共群组

    BOOL COperPublicGroupRespProcessor::ProcessResponse(int nHttpRespCode,int nHttpErrorCode,int nHttpExtendCode,const string& csHead,util::shared_ptr<CAutoBuffer> spRespBodyABuf)
{
	BOOL bSuccess = IsResponseOK(nHttpRespCode);
	string xmlDoc = spRespBodyABuf->ToSTLString();

	CMarkup markup;
	markup.SetDoc(xmlDoc.c_str());

	switch(m_eXO)
	{
	case XO_GET:
		{
			if(bSuccess)
			{
				CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();

				string updatetime;
				string bAllUpdate;

				markup.FindElem("pg");

				updatetime = markup.GetAttrib("ver");
				bAllUpdate = markup.GetAttrib("all");
				LOGD("GET GROUPLIST SUCCESS.UPDATETIME:%s,ALLUPDATE:%s\r\n",updatetime.c_str(),bAllUpdate.c_str());
				markup.IntoElem();

				if(bAllUpdate.compare(_T("1")) == 0)//全量更新
				{
					LOGD("GROUP ALL UPDATE.\r\n");

					std::map<string,MemberParams> memberinfomap;
					markup.IntoElem();

					if(markup.FindElem(_T("uinfo")))
					{
						markup.IntoElem();
						while(markup.FindElem(_T("user")))
						{
							string strMemberID;
							string strMemberName;
							string strMemberKey;
							string strMemberinfo;
							string strCmsID;

							strMemberID = markup.GetAttrib("id");
							strMemberName = markup.GetAttrib("name");
							strMemberKey = markup.GetAttrib("key");
							strMemberinfo = markup.GetAttrib("info");
							strCmsID = markup.GetAttrib("cmsid");

							MemberParams member;
							member.MemberID = strMemberID;
							member.MemberName = strMemberName;
							member.PortraitKey = strMemberKey;
							member.EmotionMood = strMemberinfo;
							member.CmsID = strCmsID;

							memberinfomap[strMemberID] = member;

						}
						markup.OutOfElem();
						markup.ResetMainPos();
						LOGD("GROUP ALL UPDATE uinfo.\r\n");
					}

					if(markup.FindElem(_T("ginfo")))
					{
						markup.IntoElem();
						while(markup.FindElem(_T("group")))
						{
							string strGroupName;//群名字
							string strGroupTmpName;//群临时名字
							string strGroupId;//群唯一id
							string strGroupMaxNum;//群成员上限
							string strGroupPortraitKey;//群头像

							std::vector<MemberParams> memberlist;//参数为成员id

							strGroupId = markup.GetAttrib("gid");
							strGroupName = markup.GetAttrib("gname");
							strGroupMaxNum = markup.GetAttrib("gmaxcount");
							strGroupPortraitKey = markup.GetAttrib("gkey");

							markup.IntoElem();

							while(markup.FindElem(_T("member")))
							{
								string memberid = markup.GetAttrib("id");
								string membername = markup.GetAttrib("name");
								map<string,MemberParams>::iterator it = memberinfomap.find(memberid);
								if(it != memberinfomap.end())
								{
									MemberParams memberinfo;
									memberinfo.MemberID = it->second.MemberID;
									memberinfo.MemberName = it->second.MemberName;
									memberinfo.MemberCardName = membername;
									memberinfo.CmsID = it->second.CmsID;
									memberinfo.EmotionMood = it->second.EmotionMood;
									memberinfo.PortraitKey = it->second.PortraitKey;
									memberlist.push_back(memberinfo);
								}
								else
								{
									MemberParams memberinfo;
									memberinfo.MemberID = memberid;
									memberinfo.MemberCardName = membername;
									memberlist.push_back(memberinfo);
								}

							}

							markup.OutOfElem();

							GroupInfo groupinfo;
							groupinfo.strGroupId = strGroupId;

							if(!strGroupName.empty())
							{
								groupinfo.strGroupName = strGroupName;
							}

							groupinfo.strGroupIcon = strGroupPortraitKey;
							groupinfo.memberMaxcount = strGroupMaxNum;
							groupinfo.strOwnerId = memberlist[0].MemberID;
							groupinfo.memberlist = memberlist;
							pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_ADD);
							pWorkGroup->UpdateGroupTmpName(strGroupId);

						}

						markup.OutOfElem();
						LOGD("GROUP ALL UPDATE ginfo.\r\n");
					}

					CWorkUpdateTime* pWorkUpdateTime = CWorkUpdateTime::GetInstance();
					pWorkUpdateTime->SetUpdateTime(UTT_GROUP, updatetime);

					if(!xmlDoc.empty())//联系人无变化时不通知UI
					{
						RspParams rspParams;
						rspParams.eRspType = ST_GETGROUPLIST_SUCCESS;

						IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
						IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
						if (pWorker)
							pWorker->OnNotify((VOID*)&rspParams);

						if(!m_bfromUI)
						{
							RspParams rspParams1;
							rspParams1.eRspType = ST_INNER_NOTIFY_GETGROUPLIST_SUCCESS;
							if (pWorker)
								pWorker->OnNotify((VOID*)&rspParams1);
						}

					}
				}
				else if(bAllUpdate.compare(_T("0")) == 0)//增量更新
				{
					std::map<string,MemberParams> memberinfomap;
					markup.IntoElem();

					if(markup.FindElem(_T("uinfo")))
					{
						markup.IntoElem();
						while(markup.FindElem(_T("user")))
						{
							string strMemberID;
							string strMemberName;
							string strMemberKey;
							string strMemberinfo;
							string strCmsID;

							strMemberID = markup.GetAttrib("id");
							strMemberName = markup.GetAttrib("name");
							strMemberKey = markup.GetAttrib("key");
							strMemberinfo = markup.GetAttrib("info");
							strCmsID = markup.GetAttrib("cmsid");

							MemberParams member;
							member.MemberID = strMemberID;
							member.MemberName = strMemberName;
							member.PortraitKey = strMemberKey;
							member.EmotionMood = strMemberinfo;
							member.CmsID = strCmsID;

							memberinfomap[strMemberID] = member;

						}
						markup.OutOfElem();
						markup.ResetMainPos();
						LOGD("GROUP INCREMENT UPDATE uinfo.\r\n");
					}

					if(markup.FindElem(_T("ginfo")))
					{
						markup.IntoElem();
						while(markup.FindElem(_T("group")))
						{
							string strGroupName;//群名字
							string strGroupTmpName;//群临时名字
							string strGroupId;//群唯一id
							string strGroupMaxNum;//群成员上限
							string strGroupPortraitKey;

							std::vector<MemberParams> memberlist;//参数为成员id

							strGroupId = markup.GetAttrib("gid");
							strGroupName = markup.GetAttrib("gname");
							strGroupMaxNum = markup.GetAttrib("gmaxcount");
							strGroupPortraitKey = markup.GetAttrib("gkey");

							GroupParams groupinfo = pWorkGroup->GetGroupInfo(strGroupId);

							if(groupinfo.GroupID.empty())//没有群组，插入表
							{
								GroupInfo groupinfo;
								groupinfo.strGroupId = strGroupId;
								groupinfo.strGroupName = strGroupName;
								groupinfo.memberMaxcount = strGroupMaxNum;
								groupinfo.strGroupIcon = strGroupPortraitKey;
								pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_ADD);
							}
							else//更新群信息
							{
								GroupInfo groupinfo;
								groupinfo.strGroupId = strGroupId;
								groupinfo.strGroupName = strGroupName;
								groupinfo.memberMaxcount = strGroupMaxNum;
								groupinfo.strGroupIcon = strGroupPortraitKey;
								pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_MODIFY);
							}
						}
						markup.OutOfElem();
						markup.ResetMainPos();
						LOGD("GROUP INCREMENT UPDATE ginfo.\r\n");
					}

					if(markup.FindElem(_T("gmember")))
					{
						markup.IntoElem();
						while(markup.FindElem(_T("group")))
						{
							string strGroupId;//群唯一id
							strGroupId = markup.GetAttrib("gid");
							LOGD("GroupId %s.\r\n",strGroupId.c_str());

							GroupParams groupinfo = pWorkGroup->GetGroupInfo(strGroupId);

							if(groupinfo.GroupID.empty())
							{
								continue;
							}

							markup.IntoElem();

							string strMemberId;
							string strMemberStatus;
							string strMemberName;

							while(markup.FindElem(_T("member")))
							{
								std::vector<MemberParams> memberlist;
								strMemberId = markup.GetAttrib("id");
								strMemberStatus = markup.GetAttrib("status");
								strMemberName =  markup.GetAttrib("name");
								map<string,MemberParams>::iterator it = memberinfomap.find(strMemberId);
								if(it != memberinfomap.end())
								{
									//LOGD("GROUP INCREMENT UPDATE gmember:Memberid:%s,MemberName:%s,MemberKey:%s",it->second.MemberID.c_str(),it->second.MemberName.c_str(),it->second.PortraitKey.c_str());
									MemberParams memberinfo;
									memberinfo.MemberID = it->second.MemberID;
									memberinfo.MemberName = it->second.MemberName;
									memberinfo.MemberCardName = strMemberName;
									memberinfo.CmsID = it->second.CmsID;
									memberinfo.EmotionMood = it->second.EmotionMood;
									memberinfo.PortraitKey = it->second.PortraitKey;
									memberlist.push_back(memberinfo);
								}
								else
								{
									MemberParams memberinfo;
									memberinfo.MemberID = strMemberId;
									memberinfo.MemberCardName = strMemberName;
									memberlist.push_back(memberinfo);
								}

								GroupInfo groupinfo;
								groupinfo.strGroupId = strGroupId;
								groupinfo.memberlist = memberlist;
							//	groupinfo.strOwnerId = memberlist[0].MemberID;
								BOOL ret = ProcessGroupChange(strMemberId,strMemberStatus,groupinfo);
								if(ret)
								{
									LOGD("strMemberId: %s,groupid:%s.\r\n",strMemberId.c_str(),strGroupId.c_str());
									break;
								}

							}
							markup.OutOfElem();

							pWorkGroup->UpdateGroupTmpName(strGroupId);
						}
						LOGD("GROUP INCREMENT UPDATE gmember.\r\n");
					}

				markup.OutOfElem();
				CWorkUpdateTime* pWorkUpdateTime = CWorkUpdateTime::GetInstance();
				pWorkUpdateTime->SetUpdateTime(UTT_GROUP, updatetime);
				LOGD("GROUP INCREMENT UPDATE.\r\n");

				RspParams rspParams;
				rspParams.eRspType = ST_GETGROUPLIST_SUCCESS;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);

				if(!m_bfromUI)
				{
					RspParams rspParams1;
					rspParams1.eRspType = ST_INNER_NOTIFY_GETGROUPLIST_SUCCESS;
					if (pWorker)
						pWorker->OnNotify((VOID*)&rspParams1);
					}
				}

			}
			else
			{
				LOGD("GET GROUPLIST FAILURE.ErrorCode:%d,WarnCode:%d\r\n",nHttpRespCode,nHttpErrorCode);
				RspParams rspParams;
				//rspParams.errcode = nHttpErrorCode;
				rspParams.eRspType = ST_GETGROUPLIST_FAILURE;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);
			}
			break;
		}
	case XO_INVITE:
		{
			if(bSuccess)
			{
				LOGD("INVITE GROUP SUCCESS.\r\n");
				string xmlDoc = spRespBodyABuf->ToSTLString();

				string groupid;
				string groupMaxcount;
				string groupname;
				string grouppic;
				string servertime;
				std::vector<MemberParams> memberlist;

				CMarkup markup;
				markup.SetDoc(xmlDoc.c_str());
				markup.FindElem("group");
				groupid = markup.GetAttrib("gid");
				groupname = markup.GetAttrib("gname");
				groupMaxcount = markup.GetAttrib("gmaxcount");
				grouppic = markup.GetAttrib("gkey");
				servertime = markup.GetAttrib("curtime");

				markup.IntoElem();

				MemberParams inviterinfo;
				if(markup.FindElem(_T("inviter")))
				{
					inviterinfo.MemberID =  markup.GetAttrib("id");
					inviterinfo.MemberName = markup.GetAttrib("name");
					inviterinfo.PortraitKey = markup.GetAttrib("key");
					inviterinfo.EmotionMood = markup.GetAttrib("info");
					inviterinfo.CmsID = markup.GetAttrib("cmsid");
				}

				int membercount = 0;
				while(markup.FindElem(_T("member")))
				{
					MemberParams memberinfo;
					memberinfo.MemberID = markup.GetAttrib("id");
					memberinfo.MemberName = markup.GetAttrib("name");
					memberinfo.PortraitKey = markup.GetAttrib("key");
					memberinfo.EmotionMood = markup.GetAttrib("info");
					memberinfo.CmsID = markup.GetAttrib("cmsid");

					memberlist.push_back(memberinfo);

					membercount++;
				}

				markup.OutOfElem();

				GroupInfo groupinfo;
				groupinfo.strGroupId = groupid;
				groupinfo.strGroupIcon = grouppic;
				groupinfo.memberMaxcount = groupMaxcount;
				groupinfo.memberlist = memberlist;

				CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
				pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_INVITE);
				pWorkGroup->UpdateGroupTmpName(groupid);

				string content = "你邀请";
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

				total = content + users + end;
//
//				MessageParams messageParams = {};
//				messageParams.pSender   = inviterinfo.MemberID;
//				messageParams.pReceiver = groupid;
//				messageParams.pContent  = total;
//				messageParams.eType     = MT_GROUP;
//				messageParams.eKind     = KT_NOTIFICATION;
//				messageParams.eCategory = CT_PLAIN_TEXT;
//				messageParams.pTime = servertime;

				//CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
				//pWorkMessage->WriteMessage(messageParams);

				//通知邀请加入群成功

				RspParams rspParams;
				GroupParams groupParams;
				groupParams.GroupID  = groupid;
				groupParams.GroupName  = groupname;
				groupParams.GroupPortraitKey = grouppic;
				rspParams.eRspType = ST_JOINGRUOP_SUCCESS;
				rspParams.pGroupParams = &groupParams;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);
			}
			else
			{
				LOGD("INVITE GROUP FAILURE.ErrorCode:%d,WarnCode:%d,ExtendCode:%d\r\n",nHttpRespCode,nHttpErrorCode,nHttpExtendCode);
				//通知邀请加入群失败
				RspParams rspParams;
				rspParams.errcode = INVITE_GROUP_RSP_ERR_CODE;
				rspParams.extendcode = nHttpExtendCode;
				rspParams.eRspType = ST_JOINGROUP_FAILURE;
				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);
			}

			break;
		}
	case XO_ADD:
		{
			if(bSuccess)
			{
				LOGD("ADD GROUP SUCCESS.\r\n");
				string xmlDoc = spRespBodyABuf->ToSTLString();

				string groupid;
				string groupname;
				string groupProtraitKey;
				string groupMaxcount;
				string memberid;
				string servertime;

				std::vector<MemberParams> memberlist;
				std::vector<MemberParams> Userlist;

				markup.FindElem("group");
				groupid = markup.GetAttrib("gid");
				groupname = markup.GetAttrib("gname");
				groupProtraitKey = markup.GetAttrib("gkey");
				groupMaxcount = markup.GetAttrib("gmaxcount");
				servertime = markup.GetAttrib("curtime");
				markup.IntoElem();

				int membercount = 0;

				while(markup.FindElem(_T("member")))
				{

					MemberParams memberinfo;
					memberinfo.MemberID =  markup.GetAttrib("id");
					memberinfo.MemberName = markup.GetAttrib("name");
					memberinfo.PortraitKey = markup.GetAttrib("key");
					memberinfo.EmotionMood = markup.GetAttrib("info");
					memberinfo.CmsID = markup.GetAttrib("cmsid");

					memberlist.push_back(memberinfo);
					Userlist.push_back(memberinfo);

					membercount++;
				}

				markup.OutOfElem();

				GroupInfo groupinfo;
				groupinfo.strGroupId = groupid;
				groupinfo.strGroupTmpName = groupname;
				groupinfo.strOwnerId = memberlist[0].MemberID;
				groupinfo.memberMaxcount = groupMaxcount;
				groupinfo.memberlist = memberlist;
				groupinfo.strGroupIcon = groupProtraitKey;

				CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
				pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_ADD);
				pWorkGroup->UpdateGroupTmpName(groupid);//更新群临时名

				string content = "你邀请";
				string users ="";
				string end ="加入了群聊。";
				string total ="";

				for(int i = 1; i < membercount; i++)
				{
					users += Userlist[i].MemberName;
					if(i != membercount - 1)
					{
						users += "、";
					}
				}

				total = content + users + end;

//				MessageParams messageParams = {};
//				messageParams.pSender   = groupid;
//				messageParams.pReceiver = groupid;
//				messageParams.pContent  = total;
//				messageParams.eType     = MT_GROUP;
//				messageParams.eKind     = KT_NOTIFICATION;
//				messageParams.eCategory = CT_PLAIN_TEXT;
//				messageParams.pTime = servertime;

				//CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
				//pWorkMessage->WriteMessage(messageParams);

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
				{
					//通知worker创建群主成功
					RspParams rspParams;
					GroupParams groupParams;
					groupParams.GroupID = groupid;
					groupParams.GroupName = groupname;
					groupParams.GroupPortraitKey = groupProtraitKey;
					rspParams.eRspType = ST_ADD_GROUP_SUCCESS;
					rspParams.pGroupParams = &groupParams;
					pWorker->OnNotify((VOID*)&rspParams);
				}
			}
			else
			{
				LOGD("ADD GROUP FAILURE.ErrorCode:%d,WarnCode:%d\r\n",nHttpRespCode,nHttpErrorCode);
				//通知worker创建群主失败
				RspParams rspParams;
				rspParams.errcode = CREATE_GROUP_RSP_ERR_CODE;
				rspParams.extendcode = nHttpExtendCode;
				rspParams.eRspType = ST_ADD_GROUP_FAILURE;
				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);
			}

			break;
		}
	case XO_DELETE:
		{
			if(bSuccess)
			{
				LOGD("EXIT GROUP SUCCESS.\r\n");
				string xmlDoc = spRespBodyABuf->ToSTLString();

				string groupid;
				string groupProtraitKey;
				string id;
				string name;
				std::vector<MemberParams> memberlist;

				markup.FindElem("group");
				groupid = markup.GetAttrib("gid");
				groupProtraitKey = markup.GetAttrib("gkey");
				id = markup.GetAttrib("id");//退出者id
				name = markup.GetAttrib("name");//退出者名字

				MemberParams memberinfo;
				memberinfo.MemberID = id;
				memberlist.push_back(memberinfo);

				GroupInfo groupinfo;
				groupinfo.strGroupId = groupid;
				groupinfo.strGroupIcon = groupProtraitKey;
				groupinfo.memberlist = memberlist;

				CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
				pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_DELETE,TRUE);

				CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
				pWorkMessage->DeleteSession(groupid, TRUE);

				//通知worker退出群成功
				RspParams rspParams;
				rspParams.eRspType = ST_QUIT_GROUP_SUCCESS;
				GroupParams groupParams;
				groupParams.GroupID = groupid;
				rspParams.pGroupParams = &groupParams;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);

			}
			else
			{
				LOGD("EXIT GROUP FAILURE.ErrorCode:%d,WarnCode:%d\r\n",nHttpRespCode,nHttpErrorCode);
				//通知worker退出群失败
				RspParams rspParams;
				//rspParams.errcode = nHttpErrorCode;
				rspParams.eRspType = ST_QUIT_GROUP_FAILURE;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);
			}
			break;
		}
	case XO_MODIFY:
		{
			if(bSuccess)
			{
				LOGD("MODIFY GROUP SUCCESS.\r\n");
				string xmlDoc = spRespBodyABuf->ToSTLString();

				string groupid;
				string groupname;
				string modifyid;
				string servertime;

				markup.FindElem("group");
				groupid = markup.GetAttrib("gid");
				groupname = markup.GetAttrib("gname");
				modifyid = markup.GetAttrib("id");
				servertime = markup.GetAttrib("curtime");

				GroupInfo groupinfo;
				groupinfo.strGroupId = groupid;
				groupinfo.strGroupName = groupname;

				CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
				pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_MODIFY);

				//解base64
				std::string body = Base64Decode(groupname);

				string total;
				total = StringFormat("你将群名称修改为了%s。",body.c_str());

//				MessageParams messageParams = {};
//				messageParams.pSender   = modifyid;
//				messageParams.pReceiver = groupid;
//				messageParams.pContent  = total;
//				messageParams.eType     = MT_GROUP;
//				messageParams.eKind     = KT_NOTIFICATION;
//				messageParams.eCategory = CT_PLAIN_TEXT;
//				messageParams.pTime = servertime;

				//CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
				//pWorkMessage->WriteMessage(messageParams);

				//通知worker修改群名称成功
				RspParams rspParams;

				GroupParams groupParams;
				groupParams.GroupID = groupid;
				groupParams.GroupName = body;

				rspParams.eRspType = ST_MODIFY_GROUPNAME_SUCCESS;
				rspParams.pGroupParams = &groupParams;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);

			}
			else
			{
				LOGD("MODIFY GROUP FAILURE.ErrorCode:%d,WarnCode:%d\r\n",nHttpRespCode,nHttpErrorCode);
				//通知worker修改群名称失败
				RspParams rspParams;
				//rspParams.errcode = nHttpErrorCode;
				rspParams.eRspType = ST_MODIFY_GROUPNAME_FAILURE;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);
			}
			break;
		}
	case XO_MODIFY_GROUPMEMBERNAME:
		{
			if(bSuccess)
			{
				LOGD("MODIFY GROUPMEMBER NAME SUCCESS.\r\n");
				string xmlDoc = spRespBodyABuf->ToSTLString();

				string groupid;
				string membername;
				string modifyid;

				markup.FindElem("group");
				groupid = markup.GetAttrib("gid");
				membername = markup.GetAttrib("cardname");
				modifyid = markup.GetAttrib("id");

				MemberParams member;
				member.MemberID = modifyid;
				member.MemberCardName = membername;

				std::vector<MemberParams> memberlist;
				memberlist.push_back(member);
				GroupInfo groupinfo;
				groupinfo.strGroupId = groupid;
				groupinfo.memberlist = memberlist;

				CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
				pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_MODIFY);

				GroupParams groupParams;
				groupParams.GroupID = groupid;

				std::string body = Base64Decode( membername);
				groupParams.GroupCardName = body;

				//通知worker修改群名称成功
				RspParams rspParams;
				rspParams.eRspType = ST_MODIFY_GROUPMEMBERNAME_SUCCESS;
				rspParams.pGroupParams = &groupParams;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);

			}
			else
			{
				LOGD("MODIFY GROUPMEMBERNAME FAILURE.ErrorCode:%d,WarnCode:%d\r\n",nHttpRespCode,nHttpErrorCode);
				//通知worker修改群名称失败
				RspParams rspParams;
				//rspParams.errcode = nHttpErrorCode;
				rspParams.eRspType = ST_MODIFY_GROUPMEMBERNAME_FAILURE;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_GROUP);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);
			}
			break;
		}
		case XO_GET_GROUPMEMBERLIST:
		{
			if(bSuccess)
			{
				string groupid;
				string gname;
				string gkey;
				string gmaxCount;
				string memberid;
				string membername;
				string memberkey;
				string memberinfo;
				string membercmsid;
				string membercardname;

				string xmlDoc = spRespBodyABuf->ToSTLString();

				if(markup.FindElem("group"))
				{
					std::vector<MemberParams> memberlist;

					groupid = markup.GetAttrib("gid");
					gname   = markup.GetAttrib("gname");
					gkey    = markup.GetAttrib("gkey");
					gmaxCount = markup.GetAttrib("gmaxcount");
					markup.IntoElem();

					while(markup.FindElem(_T("member")))
					{
						memberid = markup.GetAttrib("id");
						membername = markup.GetAttrib("name");
						memberkey = markup.GetAttrib("key");
						memberinfo = markup.GetAttrib("info");
						membercmsid = markup.GetAttrib("cmsid");
						membercardname = markup.GetAttrib("cardname");

						MemberParams member;
						member.MemberID = memberid;
						member.MemberName = membername;
						member.PortraitKey = memberkey;
						member.EmotionMood = memberinfo;
						member.CmsID = membercmsid;
						member.MemberCardName = membercardname;

						memberlist.push_back(member);
					}
					markup.OutOfElem();

					GroupInfo groupinfo;
					groupinfo.strGroupId = groupid;
					groupinfo.strGroupName = gname;
					groupinfo.strGroupIcon = gkey;
					groupinfo.strOwnerId = memberlist[0].MemberID;
					groupinfo.memberMaxcount = gmaxCount;
					groupinfo.memberlist = memberlist;

					CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
					pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_ADD);
					pWorkGroup->UpdateGroupTmpName(groupid);//更新群临时名

					GroupParams groupparams;
					groupparams.GroupID = groupid;
					groupparams.GroupName = gname;
					groupparams.GroupPortraitKey = gkey;

					RspParams rsp;
					rsp.eRspType = ST_NOTIFY_CREATE_GROUP;
					rsp.pGroupParams = &groupparams;
					pWorkGroup->OnNotifyGroup(&rsp);

				}
			}
			else
			{
				LOGD("GET GROUPMEMBERLIST FAILURE.ErrorCode:%d\r\n",nHttpRespCode);
			}
			break;
		}

	}

	return bSuccess;
}

#pragma endregion 操作公共群组

BOOL COperBuddyRespProcessor::ProcessResponse(int nHttpRespCode,int nHttpErrorCode,int nHttpExtendCode,const string& csHead,util::shared_ptr<CAutoBuffer> spRespBodyABuf)
{
	BOOL bSuccess = IsResponseOK(nHttpRespCode);
	string xmlDoc = spRespBodyABuf->ToSTLString();
	CMarkup markup;
	markup.SetDoc(xmlDoc.c_str());

	switch(m_eXO)
	{
	case XO_GET_BUDDYLIST:
		{
		    CWorkContact* pWorkContact = CWorkContact::GetInstance();

			if(bSuccess)
			{
				LOGD("GET　CONTACTLIST SUCCESS.\r\n");

				string strBuddyID;
				string strFanID;
				string updatetime;
				string bAllUpdate;

				markup.FindElem(_T("contact"));

				updatetime = markup.GetAttrib("ver");
				bAllUpdate = markup.GetAttrib("all");
				markup.IntoElem();

				if(bAllUpdate.compare(_T("1")) == 0)//全量更新
				{
					LOGD("GET　BUDDYLIST UPDATEALL.\r\n");

					pWorkContact->DB_DeleteBuddyList();//删除好友数据

					if(markup.FindElem(_T("uinfo")))
					{
						markup.IntoElem();
						while(markup.FindElem(_T("user")))
						{
							strBuddyID = markup.GetAttrib("id");
							BuddyParams buddy = {};
							buddy.pUserId = markup.GetAttrib("id");
							buddy.pUserName = markup.GetAttrib("name");
							buddy.pPortraitKey = markup.GetAttrib("key");
							buddy.pEmotionMood = markup.GetAttrib("info");
							buddy.pCmsId = markup.GetAttrib("cmsid");
							buddy.pSubname = markup.GetAttrib("subname");
							buddy.pIsSave = markup.GetAttrib("save");
							buddy.pIsShare = markup.GetAttrib("share");
							buddy.eIsBuddy = IBT_BUDDY;
							pWorkContact->DB_AddBuddy(buddy);
						}
						markup.OutOfElem();
						markup.ResetMainPos();
					}

					if(markup.FindElem(_T("finfo")))
					{
						markup.IntoElem();
						while(markup.FindElem(_T("user")))
						{
							strFanID = markup.GetAttrib("id");
							BuddyParams fans = {};
							fans.pUserId =  markup.GetAttrib("id");
							fans.pUserName = markup.GetAttrib("name");
							fans.pPortraitKey = markup.GetAttrib("key");
							fans.pEmotionMood = markup.GetAttrib("info");
							fans.pCmsId = markup.GetAttrib("cmsid");
							fans.pSubname = markup.GetAttrib("subname");
							fans.pIsSave = markup.GetAttrib("save");
							fans.pIsShare = markup.GetAttrib("share");
							pWorkContact->DB_AddFans(fans);
						}
						markup.OutOfElem();
					}
					markup.OutOfElem();

					CWorkUpdateTime* pWorkUpdateTime = CWorkUpdateTime::GetInstance();
					pWorkUpdateTime->SetUpdateTime(UTT_BUDDY, updatetime);

					IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
					if(!strBuddyID.empty())
					{
						//通知worker获取联系人列表成功
						RspParams rspParams;
						rspParams.eRspType = ST_LOAD_BUDDY_LIST;
						IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
						if (pWorker)
							pWorker->OnNotify((VOID*)&rspParams);
					}

					if(!strFanID.empty())
					{
						RspParams rspParams1;
						rspParams1.eRspType = ST_LOAD_FANS_LIST;
						IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
						if (pWorker)
							pWorker->OnNotify((VOID*)&rspParams1);
					}

					if(!m_bfromUI)
					{
						//通知worker进行下一步获取群组操作
						RspParams rspParams2;
						rspParams2.eRspType = ST_INNER_NOTIFY_GETBUDDYLIST_SUCCESS;
						IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
						if (pWorker)
							pWorker->OnNotify((VOID*)&rspParams2);
					}

				}
				else if(bAllUpdate.compare(_T("0")) == 0)//增量更新
				{
					LOGD("GET　BUDDYLIST UPDATE.\r\n");

					string strBuddyID;
					string strFanID;
					string strBuddyName;
					string strBuddyKey;
					string strBuddyinfo;
					string strCmsID;
					string strOperate;

					if(markup.FindElem(_T("uinfo")))
					{
						markup.IntoElem();
						while(markup.FindElem(_T("user")))
						{
							BuddyParams buddy = {};
							buddy.pUserId = markup.GetAttrib("id");
							buddy.pUserName = markup.GetAttrib("name");
							buddy.pPortraitKey = markup.GetAttrib("key");
							buddy.pEmotionMood = markup.GetAttrib("info");
							buddy.pCmsId = markup.GetAttrib("cmsid");
							buddy.eIsBuddy = IBT_BUDDY;
							pWorkContact->DB_ModifyBuddy(buddy);
						}
						markup.OutOfElem();
						markup.ResetMainPos();
					}

					if(markup.FindElem(_T("finfo")))
					{
						markup.IntoElem();
						while(markup.FindElem(_T("user")))
						{
							BuddyParams fans = {};
							fans.pUserId =  markup.GetAttrib("id");
							fans.pUserName = markup.GetAttrib("name");
							fans.pPortraitKey = markup.GetAttrib("key");
							fans.pEmotionMood = markup.GetAttrib("info");
							fans.pCmsId = markup.GetAttrib("cmsid");
							pWorkContact->DB_ModifyFans(fans);
						}
						markup.OutOfElem();
						markup.ResetMainPos();
					}

					if(markup.FindElem(_T("uship")))
					{
						markup.IntoElem();
						while(markup.FindElem(_T("user")))
						{
							strOperate = markup.GetAttrib("op");
							if(strOperate.compare(_T("0")) == 0)//添加
							{
								BuddyParams buddy = {};
								strBuddyID = markup.GetAttrib("id");
								buddy.pUserId = strBuddyID;
								buddy.pUserName = markup.GetAttrib("name");
								buddy.pPortraitKey = markup.GetAttrib("key");
								buddy.pEmotionMood = markup.GetAttrib("info");
								buddy.pCmsId = markup.GetAttrib("cmsid");
								buddy.pSubname = markup.GetAttrib("subname");
								buddy.pIsSave = markup.GetAttrib("save");
								buddy.pIsShare = markup.GetAttrib("share");
								buddy.eIsBuddy = IBT_BUDDY;
								pWorkContact->DB_AddBuddy(buddy);
							}
							else if(strOperate.compare(_T("1")) == 0)//删除
							{
								string strUserId = markup.GetAttrib("id");

								pWorkContact->DB_DeleteBuddy(strUserId);

								//CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
								//pWorkMessage->DeleteSession(strUserId, TRUE);
							}
						}
						markup.OutOfElem();
						markup.ResetMainPos();
					}

					int newfanscount = 0;
					if(markup.FindElem(_T("fship")))
					{
						markup.IntoElem();

						while(markup.FindElem(_T("user")))
						{
							strOperate = markup.GetAttrib("op");
							if(strOperate.compare(_T("0")) == 0)//添加
							{
								BuddyParams fans = {};
								strFanID = markup.GetAttrib("id");
								fans.pUserId = strFanID;
								fans.pUserName = markup.GetAttrib("name");
								fans.pPortraitKey = markup.GetAttrib("key");
								fans.pEmotionMood = markup.GetAttrib("info");
								fans.pCmsId = markup.GetAttrib("cmsid");
								fans.pSubname = markup.GetAttrib("subname");
								fans.pIsSave = markup.GetAttrib("save");
								fans.pIsShare = markup.GetAttrib("share");
								pWorkContact->DB_AddFans(fans);
								pWorkContact->AddNewFans(strFanID);//加入全局表
								newfanscount++;
								LOGD("NewFansCount:%d.\r\n",newfanscount);

							}
							else if(strOperate.compare(_T("1")) == 0)//删除
							{
								pWorkContact->DB_DeleteFans(markup.GetAttrib("id"));
								if(pWorkContact->NewFansExist(markup.GetAttrib("id")))
								{
									pWorkContact->DeleteNewFans(markup.GetAttrib("id"));
								}
							}
						}

						markup.OutOfElem();
						markup.ResetMainPos();
					}

					CWorkUpdateTime* pWorkUpdateTime = CWorkUpdateTime::GetInstance();
					pWorkUpdateTime->SetUpdateTime(UTT_BUDDY, updatetime);

					if(!strBuddyID.empty())
					{
						RspParams rspParams;
						rspParams.eRspType = ST_LOAD_BUDDY_LIST;

						IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
						IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
						if (pWorker)
							pWorker->OnNotify((VOID*)&rspParams);
						LOGD("GET　BUDDYLIST NOTIFY.\r\n");
					}

					if(!strFanID.empty())
					{
						RspParams rspParams;
						rspParams.eRspType = ST_LOAD_FANS_LIST;

						IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
						IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
						if (pWorker)
							pWorker->OnNotify((VOID*)&rspParams);
						LOGD("GET　FANSLIST NOTIFY.\r\n");
					}

					if( newfanscount > 0)
					{
						RspParams rspParams;
						rspParams.eRspType = ST_NOTIFY_NEW_FANS;

						IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
						IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
						if (pWorker)
							pWorker->OnNotify((VOID*)&rspParams);
						LOGD("GET　NEWFANS NOTIFY.\r\n");
					}

					if(!m_bfromUI)
					{
						//没有联系人的情况逻辑要往下走
						//通知worker进行下一步获取群组操作
						RspParams rspParams1;
						rspParams1.eRspType = ST_INNER_NOTIFY_GETBUDDYLIST_SUCCESS;
						IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
						IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
						if (pWorker)
							pWorker->OnNotify((VOID*)&rspParams1);
					}

				}

			}
			else
			{
				LOGD("GET　CONTACTLIST FAILURE.ErrorCode:%d\r\n",nHttpRespCode);
			}
		}
		break;
	case XO_GET_BUDDYINFO:
		{
			if(bSuccess)
			{
				LOGD("GET　BUDDYINFO SUCCESS.\r\n");

				CWorkContact* pWorkContact = CWorkContact::GetInstance();
				string xmlDoc = spRespBodyABuf->ToSTLString();

				markup.FindElem(_T("info"));

				BuddyParams buddy = {};
				buddy.pUserId = markup.GetAttrib("id");
				buddy.pUserName = markup.GetAttrib("name");
				buddy.pPortraitKey = markup.GetAttrib("key");
				buddy.pEmotionMood = markup.GetAttrib("info");
				buddy.pCmsId = markup.GetAttrib("cmsid");
				buddy.pSubname = markup.GetAttrib("subname");
				buddy.pIsSave = markup.GetAttrib("save");
				buddy.pIsShare = markup.GetAttrib("share");

				pWorkContact->DB_AddContact(buddy);

				//通知worker获取联系人信息成功
				RspParams rspParams;
				rspParams.eRspType = ST_LOAD_BUDDY;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);

			}
			else
			{
				LOGD("GET　BUDDYINFO FAILURE.ErrorCode:%d\r\n",nHttpRespCode);
			}
		}
		break;

	case XO_ADD_BUDDY:
	{
		if(bSuccess)
		{
			LOGD("ADDBUDDY SUCCESS.\r\n");
			string xmlDoc = spRespBodyABuf->ToSTLString();

			CWorkContact* pWorkContact = CWorkContact::GetInstance();

			if(markup.FindElem(_T("app")))
			{
				BuddyParams buddy = {};
				buddy.pUserId = markup.GetAttrib("id");
				buddy.pUserName = markup.GetAttrib("name");
				buddy.pPortraitKey = markup.GetAttrib("key");
				buddy.pEmotionMood = markup.GetAttrib("info");
				buddy.pCmsId = markup.GetAttrib("cmsid");
				buddy.eIsBuddy = IBT_BUDDY;
				pWorkContact->DB_AddBuddy(buddy);

				RspParams rspParams;
				rspParams.eRspType = ST_ADD_BUDDY_SUCCESS;

				BuddyParams buddyinfo;
				buddyinfo.pUserId = buddy.pUserId;

				rspParams.pBuddyParams = &buddyinfo;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);

			}
		}
		else
		{
			RspParams rspParams;
			//rspParams.errcode = nHttpErrorCode;
			rspParams.eRspType = ST_ADD_BUDDY_FAILURE;

			IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
			IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
			if (pWorker)
				pWorker->OnNotify((VOID*)&rspParams);
			LOGD("ADDBUDDY FAILED.\r\n");
		}
		break;
	}
	case XO_DELETE_BUDDY:
	{
		if(bSuccess)
		{
			LOGD("DELETEBUDDY SUCCESS.\r\n");
			string xmlDoc = spRespBodyABuf->ToSTLString();

			CWorkContact* pWorkContact = CWorkContact::GetInstance();

			if(markup.FindElem(_T("del")))
			{
				string strUserId = markup.GetAttrib("id");

				pWorkContact->DB_DeleteBuddy(strUserId);

				//CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
				//pWorkMessage->DeleteSession(strUserId, TRUE);

				RspParams rspParams;
				rspParams.eRspType = ST_DELETE_BUDDY_SUCCESS;
				//rspParams.pBuddyParams->pUserId = buddy.strUserId;

				IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
				IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
				if (pWorker)
					pWorker->OnNotify((VOID*)&rspParams);

			}
		}
		else
		{
			RspParams rspParams;
			//rspParams.errcode = nHttpErrorCode;
			rspParams.eRspType = ST_DELETE_BUDDY_FAILURE;

			IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
			IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
			if (pWorker)
				pWorker->OnNotify((VOID*)&rspParams);
			LOGD("DELETEBUDDY FAILED.\r\n");
		}
		break;
	}
	case XO_SEARCH_BUDDY:
	{
		if(bSuccess)
		{
			LOGD("SEARCH BUDDY SUCCESS.\r\n");
			string xmlDoc = spRespBodyABuf->ToSTLString();

			CWorkContact* pWorkContact = CWorkContact::GetInstance();

			if(markup.FindElem(_T("searchcontact")))
			{
				markup.IntoElem();
				if(markup.FindElem(_T("list")))
				{
					markup.IntoElem();
					BuddyList searchlist = {};
					while(markup.FindElem(_T("contact")))
					{
						BuddyParams buddy = {};
						buddy.pUserId = markup.GetAttrib("name");
						buddy.pUserName = markup.GetAttrib("nickname");
						buddy.pPortraitKey = markup.GetAttrib("portraitcrc");
						buddy.pEmotionMood = markup.GetAttrib("impresa");
						buddy.pCmsId = markup.GetAttrib("id");
						buddy.pEmail = markup.GetAttrib("email");
						buddy.pMobile = markup.GetAttrib("mobile");

						if(pWorkContact->DB_IsBuddyExist(buddy.pUserId))
						{
							buddy.eIsBuddy =  IBT_BUDDY;
						}
						else
						{
							buddy.eIsBuddy =  IBT_NOTBUDDY;
						}

						searchlist.ppBuddyParams.push_back(buddy);
					}
					searchlist.nCount = searchlist.ppBuddyParams.size();
					markup.OutOfElem();
					LOGD("SEARCH BUDDY LIST SIZE %d.\r\n",searchlist.nCount);
					pWorkContact->SetSearchContact(searchlist);

					RspParams rspParams;
					rspParams.eRspType = ST_SEARCH_BUDDY_SUCCESS;

					IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
					IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
					if (pWorker)
						pWorker->OnNotify((VOID*)&rspParams);
				}
			}
		}
		else
		{
			RspParams rspParams;
			//rspParams.errcode = nHttpErrorCode;
			rspParams.eRspType = ST_SEARCH_BUDDY_FAILURE;

			IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
			IWorker* pWorker = pWorkerContainer->FindWorker(WT_CONTACT);
			if (pWorker)
				pWorker->OnNotify((VOID*)&rspParams);
			LOGD("SEARCHBUDDY FAILED.\r\n");
		}
		break;
	}
	case XO_SET_RECV_PUSHMSG:
	{
		if(bSuccess)
		{
			LOGD("SET RECV PUSHMSG SUCCESS.\r\n");
		}
		else
		{
			LOGD("SET RECV PUSHMSG FAILURE.\r\n");
		}

		break;
	}

	case XO_GET_RECV_PUSHMSG:
	{
		if(bSuccess)
		{
			LOGD("GET RECV PUSHMSG SUCCESS.\r\n");
			string xmlDoc = spRespBodyABuf->ToSTLString();

			CWorkContact* pWorkContact = CWorkContact::GetInstance();

			if(markup.FindElem(_T("get")))
			{
				string type;
				string status;
				type = markup.GetAttrib("type");

				if(type == "pushstatus")
				{
					status = markup.GetAttrib("status");
					int value = atoi(status.c_str());
					pWorkContact->SetReceiveMessageOption(value,false);
					LOGD("SetReceiveMessageOption.\r\n");
				}
			}
		}
		else
		{
			LOGD("GET RECV PUSHMSG EMPTY.\r\n");
		}

		CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
		pWorkMessage->NotifyLoginXmpp();

		break;
	}

	default:
		break;
	}
    return S_OK;
}
    
BOOL COperPublicGroupRespProcessor::ProcessGroupChange(string id,string status,GroupInfo groupinfo)
{
	CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
	if(status.compare(_T("1")) == 0)//插入
	{
		pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_ADD);
	}
	else if(status.compare(_T("0")) == 0)
	{
		if(id.compare(GetUserSelfId()) == 0)//删除群
		{
			pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_DELETE,TRUE);
			return TRUE;
		}
		else//删除成员
		{
			pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_DELETE);
		}
	}
//	else if(status.CompareNoCase(_T("2")) == 0)//更新群昵称
//	{
//		pWorkGroup->OnNotifySaveDatabase(groupinfo,XO_MODIFY);
//	}
	return FALSE;
}

BOOL COperOfflineMsgRespProcessor::ProcessResponse(int nHttpRespCode,int nHttpErrorCode,int nHttpExtendCode,const string& csHead,util::shared_ptr<CAutoBuffer> spRespBodyABuf)
{
	BOOL bSuccess = IsResponseOK(nHttpRespCode);
	string xmlDoc = spRespBodyABuf->ToSTLString();
	CMarkup markup;
	markup.SetDoc(xmlDoc.c_str());

	switch(m_eXO)
	{
	case XO_GET_OFFLINEMSG:
		{
			if(bSuccess)
			{
				LOGD("GET　OFFLINEMSG SUCCESS.\r\n");

				string xmlDoc = spRespBodyABuf->ToSTLString();

				if(markup.FindElem(_T("offlinemsg")))
				{
					markup.IntoElem();
					if(markup.FindElem(_T("list")))
					{
						markup.IntoElem();

						list<string> msgidlist;
						MessageList msglist;
						vector<MessageParams> messagelist;
						MessageParams msgparams;

						while(markup.FindElem(_T("message")))
						{
							msgparams.pMsgId = markup.GetAttrib("msgid");
							msgparams.pSender = markup.GetAttrib("from");
							msgparams.pReceiver = markup.GetAttrib("to");
							msgparams.eType = String2Type(markup.GetAttrib("type"));
							msgparams.eCategory = String2Category(markup.GetAttrib("category"));

							markup.IntoElem();
							if(markup.FindElem(_T("body")))
							{
								msgparams.pContent = markup.GetData();
							}

							if(markup.FindElem(_T("recvtime")))
							{
								msgparams.pTime = markup.GetData();
							}

							messagelist.push_back(msgparams);
							msgidlist.push_back(msgparams.pMsgId);
							markup.OutOfElem();
						}

						msglist.nCount = messagelist.size();
						msglist.ppMessageParams = messagelist;

						CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
						pWorkMessage->SetOfflineMessageList(msglist);
						pWorkMessage->NotifySendOfflineMsgAck(msgidlist);

						markup.OutOfElem();
					}
					markup.OutOfElem();
				}
				else//没有离线消息直接登录
				{
					CWorkContact* pWorkContact = CWorkContact::GetInstance();
					pWorkContact->GetReceiveMessageFromServer();

					//CWorkMessage* pWorkMessage = CWorkMessage::GetInstance();
					//pWorkMessage->NotifyLoginXmpp();
				}

			}
			else
			{
				LOGD("GET　OFFLINEMSG FAILED. STATUSCODE:%d.\r\n",nHttpRespCode);
			}
		}
		break;
	case XO_GET_OFFLINEMSG_ACK:
		{
			if(bSuccess)
			{
				LOGD("GET　OFFLINEMSGACK SUCCESS.\r\n");
				CWorkContact* pWorkContact = CWorkContact::GetInstance();
				pWorkContact->GetReceiveMessageFromServer();
			}
			else
			{
				LOGD("GET　OFFLINEMSGACK FAILED.STATUSCODE:%d.\r\n",nHttpRespCode);
			}
		}
		break;
	default:
		break;
	}

    return TRUE;
}

}

}

