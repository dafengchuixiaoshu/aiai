#include "BizCommon.h"
#include "CConfigure.h"
#include "CWorkerContainer.h"
#include "CWorkLogin.h"
#include "CWorkGroup.h"
#include "CWorkContact.h"
#include "CWorkMessage.h"
#include "SqliteImpl.h"
#include "immodule.h"
#include "CXmppStack.h"

#ifndef USE_SQLITE
#include "memsave.h"
#endif

CXmppStack* CXmppStack::g_pXmppStack = NULL;

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

CXmppStack::CXmppStack()
{
}

CXmppStack::~CXmppStack()
{
	sImModule.Release();
}

CXmppStack* CXmppStack::GetInstance()
{
	if (g_pXmppStack == NULL)
	{
		g_pXmppStack = new CXmppStack();
	}
	return g_pXmppStack;
}

VOID CXmppStack::Release()
{
	if (g_pXmppStack)
	{
		delete g_pXmppStack;
		g_pXmppStack = NULL;
	}
}

HRESULT CXmppStack::Start ()
{
	IConfigure* pConfigure = CConfigure::GetInstance();
	string strVal = pConfigure->ReadProfile("xmpp_name");  //读取服务器名称
	string strServerAddrName = "Cfg_" + strVal + "_Addr";  //获取服务器地址的格式
	string strServerPortName = "Cfg_" + strVal + "_Port";  //获取服务器端口的格式
	const string strAddr = pConfigure->ReadProfile (strServerAddrName);
	const string strPort = pConfigure->ReadProfile (strServerPortName);
	LOGD ("XMPP Start, strAddr=%s, strPort=%s\r\n", strAddr.c_str(), strPort.c_str());

	if (strAddr.empty() || strPort.empty())
	{
		LOGD ("Addr or Port is empty from profile\r\n");
		return E_FAIL;
	}

	BOOL nResult = sImModule.StartByte(strAddr.c_str(), atoi(strPort.c_str()), 10, OnIMModuleCallback);
	//BOOL nResult = sImModule.StartXml(strAddr.c_str(), atoi(strPort.c_str()), 10, OnIMModuleCallback);
	if (nResult == FALSE)
	{
		LOGD ("ImModule Start failed\r\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CXmppStack::Login ()
{
	IConfigure* pConfigure = CConfigure::GetInstance();
	string userid   = pConfigure->ReadProfile ("reg_id");
	string resource = pConfigure->ReadProfile ("Resource");
	string platform = pConfigure->ReadProfile ("Platform");
	string domain   = pConfigure->ReadProfile ("Domain");

	LOGD ("XMPP Login, userid=%s, resource=%s, platform=%s, domain=%s\r\n",
			userid.c_str(), resource.c_str(), platform.c_str(), domain.c_str());

	if (userid.empty() || resource.empty() || platform.empty() || domain.empty())
	{
		LOGD ("userid or resource or platform or domain is empty from profile\r\n");
		return E_FAIL;
	}

	CImLogin imLogin(userid, domain, resource, platform);
	BOOL nResult = sImModule.PushSend(imLogin);
	if (nResult == FALSE)
	{
		LOGD ("ImModule PushSend failed\r\n");
		NotifyLoginFailure ();
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CXmppStack::Logout ()
{
	LOGD ("XMPP Logout\r\n");

	CImLogout imLogout;
	BOOL nResult = sImModule.PushSend(imLogout);
	if (nResult == FALSE)
	{
		LOGD ("ImModule PushSend failed\r\n");
		NotifyLogoutFailure ();
		return E_FAIL;
	}
	return S_OK;
}

void CXmppStack::SetBlackListFlag(string& blackid, int type)
{
	CImBlackList blacklist(blackid, type);

	if(sImModule.PushSend(blacklist))
	{
		LOGD ("CXmppStack::SetBlackListFlag push successful\r\n");
	}
	else
	{
		LOGD ("CXmppStack::SetBlackListFlag push failed\r\n");
	}
}

void CXmppStack::SendImage(string& fromuser, string& touser, string& imagepath,
		string& txtbuffer, int imagewidth, int imageheight, int msgtype)
{
    char buffer[2048 + 1] = { 0 };

	string clienttime = MkIntDateTimeLong();

	string stype = Type2String ((MessageType)msgtype);

	CImMessage immessage(fromuser, touser, imagepath, txtbuffer, imagewidth, imageheight, stype);

	static const char fmt[] = "{\"type\":1,\"img\":\"%s\",\"width\":%d,\"height\":%d,\"longitude\":0,\"latitude\":0,\"text\":\"%s\",\"stype\":\"%s\"}";

	if(snprintf(buffer, 2048, fmt, imagepath.c_str(), imagewidth, imageheight, txtbuffer.c_str(), stype.c_str()) <= 0)
	{
		LOGD ("CXmppStack::SendImage snprintf failed\r\n");

		return;
	}

#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1 = StringFormat (
			"INSERT OR IGNORE INTO [messages] "
			"([msg_id], [session_id], [sender], [receiver], "
			"[type], [kind], [direction], [unread], [send], [category], "
			"[content], [client_time], [server_time]) "
			"VALUES "
			"('%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, '%s', '%s', '%s'); "
			"INSERT OR IGNORE INTO [sessions] ([session_id], [msg_id], [type]) "
			"VALUES ('%s', '%s', %d); "
			"UPDATE [sessions] "
			"SET [msg_id]='%s', [update_time]='%s' "
			"WHERE [session_id]='%s';"
			,
			immessage.m_id.c_str(), immessage.m_touser.c_str(), immessage.m_fromuser.c_str(), immessage.m_touser.c_str(),
			msgtype, 1, DT_SEND, US_READ, SS_UNSENT, "image",
			buffer, clienttime.c_str(), clienttime.c_str(),
			immessage.m_touser.c_str(), immessage.m_id.c_str(), msgtype,
			immessage.m_id.c_str(), clienttime.c_str(), immessage.m_touser.c_str());

	LOGD ("sSQL1=%s\r\n", sSQL1.c_str());

	sqliteImpl->ExecSql(sSQL1);
#else
	message msg;
	msg.msg_id = immessage.m_id;
	msg.session_id = immessage.m_touser;
	msg.sender = immessage.m_fromuser;
	msg.receiver = immessage.m_touser;
	msg.type = msgtype;
	msg.kind = 1;
	msg.direction = 1;//DT_SEND;
	msg.unread = 1;//US_READ;
	msg.send = 0;//SS_UNSENT;
	msg.category = "image";
	msg.content = buffer;
	msg.client_time = clienttime;
	msg.server_time = clienttime;

	if(!sMemSave.InsertMessage(msg))
	{

	}

	session ssn;
	ssn.session_id = immessage.m_touser;
	ssn.msg_id = immessage.m_id;
	ssn.type = msgtype;
	ssn.update_time = clienttime;

	if(!sMemSave.InsertSession(ssn))
	{

	}
#endif

	NotifySendMessageProcess (immessage.m_id, touser);

	int networkstatus = sImModule.GetNetworkStatus();

	LOGD("SendImage networkstatus: %d\n\n", networkstatus);

	if(1 == networkstatus)
	{
		if("chat" == stype && sImModule.SendLanChat(immessage))
		{
			LOGD ("CXmppStack::SendImage lanchat successful\r\n");

			return;
		}

	}

	if(sImModule.PushSend(immessage))
	{
		LOGD ("CXmppStack::SendImage push successful\r\n");
	}
	else
	{
		LOGD ("CXmppStack::SendImage push failed\r\n");

		NotifySendMessageFailure (immessage.m_id, touser);

		Start();
	}
}

void CXmppStack::SendVideo(string& fromuser, string& touser, string& videopath,
		string& faceimagepath, string& txtbuffer, string& videotitle, int videolength, int msgtype)
{
	char buffer[2048 + 1] = { 0 };

	string clienttime = MkIntDateTimeLong();

	string stype = Type2String ((MessageType)msgtype);

	CImMessage immessage(fromuser, touser, videopath, faceimagepath, txtbuffer, videotitle, videolength, stype);

	static const char fmt[] = "{\"type\":2,\"img\":\"%s\",\"video\":\"%s\",\"length\":%d,\"title\":\"%s\",\"longitude\":0,\"latitude\":0,\"text\":\"%s\",\"stype\":\"%s\"}";

	if(snprintf(buffer, 2048, fmt, faceimagepath.c_str(), videopath.c_str(),
			videolength, videotitle.c_str(), txtbuffer.c_str(), stype.c_str()) <= 0)
	{
		LOGD ("CXmppStack::SendVideo snprintf failed\r\n");

		return;
	}

#ifdef USE_SQLITE
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1 = StringFormat (
			"INSERT OR IGNORE INTO [messages] "
			"([msg_id], [session_id], [sender], [receiver], "
			"[type], [kind], [direction], [unread], [send], [category], "
			"[content], [client_time], [server_time]) "
			"VALUES "
			"('%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, '%s', '%s', '%s'); "
			"INSERT OR IGNORE INTO [sessions] ([session_id], [msg_id], [type]) "
			"VALUES ('%s', '%s', %d); "
			"UPDATE [sessions] "
			"SET [msg_id]='%s', [update_time]='%s' "
			"WHERE [session_id]='%s';"
			,
			immessage.m_id.c_str(), immessage.m_touser.c_str(), immessage.m_fromuser.c_str(), immessage.m_touser.c_str(),
			msgtype, 1, DT_SEND, US_READ, SS_UNSENT, "video",
			buffer, clienttime.c_str(), clienttime.c_str(),
			immessage.m_touser.c_str(), immessage.m_id.c_str(), msgtype,
			immessage.m_id.c_str(), clienttime.c_str(), immessage.m_touser.c_str());

	LOGD ("sSQL1=%s\r\n", sSQL1.c_str());

	sqliteImpl->ExecSql(sSQL1);
#else
	message msg;
	msg.msg_id = immessage.m_id;
	msg.session_id = immessage.m_touser;
	msg.sender = immessage.m_fromuser;
	msg.receiver = immessage.m_touser;
	msg.type = msgtype;
	msg.kind = 1;
	msg.direction = 1;//DT_SEND;
	msg.unread = 1;//US_READ;
	msg.send = 0;//SS_UNSENT;
	msg.category = "video";
	msg.content = buffer;
	msg.client_time = clienttime;
	msg.server_time = clienttime;

	session ssn;
	ssn.session_id = immessage.m_touser;
	ssn.msg_id = immessage.m_id;
	ssn.type = msgtype;
	ssn.update_time = clienttime;
#endif

	NotifySendMessageProcess (immessage.m_id, touser);

	int networkstatus = sImModule.GetNetworkStatus();

	LOGD("SendVideo networkstatus: %d\n\n", networkstatus);

	if(1 == networkstatus)
	{
		if("chat" == stype && sImModule.SendLanChat(immessage))
		{
			LOGD ("CXmppStack::SendVideo lanchat successful\r\n");

			return;
		}
	}

	if(sImModule.PushSend(immessage))
	{
		LOGD ("CXmppStack::SendVideo push successful\r\n");
	}
	else
	{
		LOGD ("CXmppStack::SendVideo push failed\r\n");

		NotifySendMessageFailure (immessage.m_id, touser);

		Start();
	}
}

void CXmppStack::OnIMModuleCallback (int msgtype, CSocketMsg& socketmsg)
{
	if (g_pXmppStack == NULL)
	{
		return;
	}

	LOGD ("XMPP IMModuleCallback msgtype=%d\r\n", msgtype);

	switch(msgtype)
    {
    case MSG_SOCKET_CONNECT:  // sock连接成功
//    case MSG_TYPE_SOCK_CONNECT:  // sock连接成功
    	g_pXmppStack->Login ();
    	break;

    case MSG_RECV_CLOSED:  // sock连接断开
//    case MSG_TYPE_SOCK_DISCONNECT:  // sock连接断开
    	g_pXmppStack->NotifyNetworkFailure ();
    	break;

    case MSG_SEND_ERROR:  // sock发送消息失败
//    case MSG_TYPE_SEND_SOCK_ERROR:  // sock发送消息失败

    case MSG_SEND_CLOSED:  // MSG_TYPE_SEND_SOCK_CLOSED:  // sock未连接或正在连接
    case MSG_SOCKET_TIMEOUT:  // MSG_TYPE_WORK_TIMEOUT:  // 发送消息没有回ACK超时
    case MSG_WORK_CLEAR:  // MSG_TYPE_WORK_CLEAR:  // 连接断开，清除发送消息队列的数据，没有收到ACK回应的消息都默认失败
    	{
    		LOGD ("imtype=%d\r\n", socketmsg.m_imtype);
    		switch (socketmsg.m_imtype)
    		{
    		case IM_CHAT:
    			{
    				const string id      = socketmsg.m_sendmsgid;
    		   		CWorkMessage*     pWorkMessage     = CWorkMessage::GetInstance();
    		   		MessageParams pMessageParams = pWorkMessage->GetMessage("", id, FALSE);
    		   		if (pMessageParams.pMsgId.empty())
    		   		{
    		   			LOGD ("Xmpp can't get one message\r\n");
    		   			break;
    		   		}

    		   		const string to      = pMessageParams.pReceiver;
    	    		LOGD ("id=%s, to=%s\r\n", id.c_str(), to.c_str());
    				g_pXmppStack->NotifySendMessageFailure (id, to);
    			}
    			break;

    		case IM_LOGIN:
    			g_pXmppStack->NotifyLoginFailure ();
    			break;

    		case IM_LOGOUT:
    			g_pXmppStack->NotifyLogoutFailure ();
    			break;

    		default:
    			// 发送ACK时出现失败，不需要抛给UI，直接忽略
    			break;
    		}
    	}
    	break;

//    case MSG_TYPE_SEND_SOCK_SUCCESS:  // sock发送消息成功
    	// waiting for ACK return
//        break;

    case MSG_RECV_MESSAGE:  // MSG_TYPE_RECV_MESSAGE:   // sock接收到新消息
    	LOGD ("imtype: %d\r\n", socketmsg.m_im->m_imtype);
        switch(socketmsg.m_im->m_imtype)
        {
        case IM_LOGIN:  // 接收到登录回应
			{
                CImLogin* imlogin = (CImLogin*)(socketmsg.m_im);

                if (imlogin && 0 == imlogin->m_status)
                {
					// success
					g_pXmppStack->NotifyLoginSuccess ();
                }
                else
                {
                	if (imlogin)
                	{
                		LOGD ("login failed: %d\r\n", imlogin->m_status);
                	}
					g_pXmppStack->NotifyLoginFailure();
                    sImModule.Release();
                }
			}
			break;

        case IM_LOGOUT:  // 接收到登出回应
			{
                CImLogout* imlogout = (CImLogout*)(socketmsg.m_im);

                if (imlogout && 0 == imlogout->m_status)
                {
					// success
					g_pXmppStack->NotifyLogoutSuccess ();
                    sImModule.Release();
                }
                else
                {
                	if (imlogout)
                	{
                		LOGD ("logout failed: %d\r\n", imlogout->m_status);
                	}
					g_pXmppStack->NotifyLogoutFailure();
                }
			}
			break;

        case IM_CHAT:  // 接收到新消息
        	{
                CImChat* imchat = (CImChat*)(socketmsg.m_im);

                if(NULL == imchat)
                {
                    LOGD ("can not find chat pointer\r\n");
                    sImModule.Release();
                }
                else
                {
                    LOGD ("do chat: [id:%s,to:%s,from:%s,type:%s,category:%s,time:%s,body:%s]\r\n",
                        imchat->m_id.c_str(), imchat->m_to.c_str(), imchat->m_from.c_str(),
                        imchat->m_type.c_str(), imchat->m_category.c_str(), imchat->m_time.c_str(),
                        imchat->m_body.c_str());
                 	g_pXmppStack->OnRecvMessage (imchat);
                }
        	}
         	break;

        case IM_ACK:  // 接收到IQ
        	{
                CImAck* imack = (CImAck*)(socketmsg.m_im);

                if(NULL == imack)
                {
                    LOGD ("can not find ack pointer\r\n");
                    sImModule.Release();
                }
                else
                {
                	const string msgid   = imack->m_id;
					const string msgtime = imack->m_time;

					LOGD ("id=%s, msgtime=%s\r\n", msgid.c_str(), msgtime.c_str());

					if (msgtime.empty())
					{
						LOGD ("Xmpp ACK server time empty\r\n");
						return;
					}

					const string servertime2 = DatetimeStrToLong (msgtime);

					CWorkMessage*     pWorkMessage     = CWorkMessage::GetInstance();
					MessageParams pMessageParams = pWorkMessage->GetMessage("", msgid.c_str(), FALSE);
					if (pMessageParams.pMsgId.empty())
					{
						LOGD ("Xmpp can't get one message\r\n");
						return;
					}

#ifdef USE_SQLITE
					SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
					const string sSQL = StringFormat (
							"UPDATE [messages] "
							"SET [send]=%d, [server_time]='%s' "
							"WHERE [msg_id]='%s'; "
							"UPDATE [sessions] "
							"SET [update_time]='%s' "
							"WHERE [session_id]='%s';"
							,
							SS_SENT, servertime2.c_str(), msgid.c_str(),
							servertime2.c_str(), pMessageParams.pSessionId.c_str());
			//		LOGD("sSQL1=%s\n", sSQL1);
					sqliteImpl->ExecSql(sSQL);
#else
					sMemSave.UpdateMessage(msgid, "send", "1");//SS_SENT
					sMemSave.UpdateMessage(msgid, "server_time", servertime2);
					sMemSave.UpdateSession(pMessageParams.pSessionId, "update_time", servertime2);
#endif

					g_pXmppStack->NotifySendMessageSuccess(pMessageParams.pMsgId, pMessageParams.pSessionId);

                }
        	}
        	break;

        case IM_KICKOUT:
             {
            	 CImKickout* imkickout = (CImKickout*)(socketmsg.m_im);

            	 if(NULL == imkickout)
            	 {
            		 LOGD ("can not find kickout pointer\r\n");
            	 }
            	 else
            	 {
            		 g_pXmppStack->NotifyLoginElsewhere ();
            	 }
             }
             break;

        case IM_PGCREATE:
			{
			 CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
			 pWorkGroup->OnNotifyOperateGroup(IM_PGCREATE,socketmsg.m_im);
			}
			break;
        case IM_PGINVITE:
			{
			 CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
			 pWorkGroup->OnNotifyOperateGroup(IM_PGINVITE,socketmsg.m_im);
			}
			break;
        case IM_PGQUIT:
			{
			 CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
			 pWorkGroup->OnNotifyOperateGroup(IM_PGQUIT,socketmsg.m_im);
			}
			break;
        case IM_PGMODIFY:
			{
			 CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
			 pWorkGroup->OnNotifyOperateGroup(IM_PGMODIFY,socketmsg.m_im);
			}
			break;
        case IM_PGCARD:
			{
			 CWorkGroup* pWorkGroup = CWorkGroup::GetInstance();
			 pWorkGroup->OnNotifyOperateGroup(IM_PGCARD,socketmsg.m_im);
			}
			break;
        case IM_SNS:
			{
			 CWorkContact* pWorkContact = CWorkContact::GetInstance();
			 pWorkContact->OnNotifyOperateContact(IM_SNS,socketmsg.m_im);
			}
			break;
        case IM_FANSMODIFY:
			{
			 CWorkContact* pWorkContact = CWorkContact::GetInstance();
			 pWorkContact->OnNotifyOperateContact(IM_FANSMODIFY,socketmsg.m_im);
			}
			break;
        case IM_FANSAPPEND:
			{
			 CWorkContact* pWorkContact = CWorkContact::GetInstance();
			 pWorkContact->OnNotifyOperateContact(IM_FANSAPPEND,socketmsg.m_im);
			}
			break;
        case IM_FANSDELETE:
			{
			 CWorkContact* pWorkContact = CWorkContact::GetInstance();
			 pWorkContact->OnNotifyOperateContact(IM_FANSDELETE,socketmsg.m_im);
			}
			break;
        case IM_BLACKLIST:
        	{
                CImBlackList* imblacklist = (CImBlackList*)(socketmsg.m_im);

                if(NULL == imblacklist)
                {
                	LOGD("can not find imblacklist pointer\r\n");
                }
                else
                {
                	LOGD("IM_BLACKLIST blackid=%s %s\r\n",
                        imblacklist->m_blackid.c_str(), (0 == imblacklist->m_type) ? "failed" : "success");
                }
        	}
        	break;

        default:
        	break;
        }
        break;

    default:
        break;
    }
}

// Recv message handlers
void CXmppStack::OnRecvMessage(CImChat* imchat)
{
	//<message msgid="53FC368C271A8026613" from="chenxi" to="chenxi" id="1409037955191835747263275"
	//type="chat" category="plain-text"><body>aGVsbG8gd29ybGQh</body></message>

	const string from       = imchat->m_from;
	const string to         = imchat->m_to;
	const string id         = imchat->m_id;
	const string type       = imchat->m_type;
	const string category   = imchat->m_category;
	const string servertime = imchat->m_time;
	const string body       = imchat->m_body;

	LOGD ("XMPP Recv New Message, from=%s, to=%s, id=%s, type=%s, category=%s, time=%s, body=%s, issave=%d, isshare=%d\r\n",
			from.c_str(), to.c_str(), id.c_str(), type.c_str(), category.c_str(),
			servertime.c_str(), body.c_str(), imchat->m_issave, imchat->m_isshare);

	if (from.empty() || to.empty() || id.empty() || type.empty() || category.empty() || body.empty())
	{
		LOGD ("XMPP Recv Bad Message\r\n");
		return;
	}

	// 根据个人还是群组还是客服，确定type和sessiodId
	enum MessageType eMessageType = String2Type (type);
	string pSessionId = from;
	switch (eMessageType)
	{
	case MT_PERSONAL:
		pSessionId = from;
		break;

	case MT_GROUP:
		pSessionId = to;
		break;

//	case MT_CUSTOM:
//		 if("system" == imchat->m_category)//获取客服小号
//		{
//			printf("do custom gettouser: [id:%s,talkid:%s,custom:%s]\n\n",
//				imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_body.c_str());
//
//			SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
//			const string sSQL1 = StringFormat (
//					"UPDATE [messages] "
//					"SET [sender]='%s' "
//					"WHERE [session_id]='%s' AND [type]=%d AND [send]=%d; "
//					,
//					imchat->m_body.c_str(), imchat->m_from.c_str(), MT_CUSTOM, SS_UNSENT);
//			LOGD("sSQL1=%s\n", sSQL1.c_str());
//			sqliteImpl->ExecSql(sSQL1);
//
//			IqParams iqParams  = {};
//			iqParams.eType     = IT_CUSTOMCHAT;
//			iqParams.pTalkId   = imchat->m_from;
//			iqParams.pSmallNum = imchat->m_body;
//
//			RspParams rspParams;
//			rspParams.eRspType  = ST_RECV_CUSTOM_SMALLNUM;
//			rspParams.pIqParams = &iqParams;
//			IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
//			IWorker* pWorker = pWorkerContainer->FindWorker(WT_MESSAGE);
//			if (pWorker)
//				pWorker->OnNotify((VOID*)&rspParams);
//			return;
//		}
//		else
//		{
//			 pSessionId = to;
//		}
//		break;
//
//	case MT_MAISHOU:
//		pSessionId = from;
//		break;
	}

	enum CategoryType eCategoryType = String2Category (category);
	const string clienttime         = MkIntDateTimeLong();
	string servertime2        = DatetimeStrToLong (servertime);
	if (servertime.empty())
	{
		servertime2 = clienttime;
	}

	if(!IfContactOrGroupExist(eMessageType, pSessionId))
	{
		LOGD("Group not Exist.\r\n");
		return;
	}
	CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
	HRESULT hResult = pWorkMessage->IsMessageExist(id);
	if (hResult == FALSE)
	{
#ifdef USE_SQLITE
		struct query_rows_s query_rows = {};
		SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
		const string sSQL1 = StringFormat (
				"INSERT OR IGNORE INTO [messages] "
				"([msg_id], [session_id], [sender], [receiver], [save], [share], "
				"[type], [kind], [direction], [unread], [send], [category], [online], "
				"[content], [client_time], [server_time]) "
				"VALUES "
				"('%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s', '%s'); "
				,
				id.c_str(), pSessionId.c_str(), from.c_str(), to.c_str(), imchat->m_issave, imchat->m_isshare,
				eMessageType, KT_ORDINARY, DT_RECV, US_UNREAD, SS_UNSENT, eCategoryType, OS_ONLINE,
				body.c_str(), clienttime.c_str(), servertime2.c_str());
		//LOGD("sSQL1=%s\r\n", sSQL1.c_str());
		sqliteImpl->ExecSql(sSQL1);

		const string sSQL2 = StringFormat ("INSERT OR IGNORE INTO [sessions] ([session_id], [msg_id], [type], [kind]) "
				"VALUES ('%s', '%s', %d, %d); ",
				pSessionId.c_str(), id.c_str(), eMessageType, KT_ORDINARY);
		//LOGD("sSQL2=%s\r\n", sSQL2.c_str());
		sqliteImpl->ExecSql(sSQL2);

		const string sSQL3 = StringFormat ("UPDATE [sessions] "
				"SET [msg_id]='%s', [unread_num]=[unread_num]+1, "
				"[update_time]='%s', [draft_changed]=%d "
				"WHERE [session_id]='%s';"
				,id.c_str(), servertime2.c_str(), DC_UNCHANGED, pSessionId.c_str());
		//LOGD("sSQL3=%s\r\n", sSQL3.c_str());
		sqliteImpl->ExecSql(sSQL3);
#else
		message msg;
		msg.msg_id = id;
		msg.session_id = pSessionId;
		msg.sender = from;
		msg.receiver = to;
		msg.type = (int)eMessageType;
		msg.kind = 1;//KT_ORDINARY;
		msg.direction = 2;//DT_RECV;
		msg.unread = 0;//US_UNREAD;
		msg.send = 0;//SS_UNSENT;
		msg.category = (int)eCategoryType;
		msg.online = 1;//OS_ONLINE;
		msg.content = body;
		msg.client_time = clienttime;
		msg.server_time = servertime2;

		if(!sMemSave.InsertMessage(msg))
		{

		}

		session ssn;
		ssn.session_id = pSessionId;
		ssn.msg_id = id;
		ssn.type = (int)eMessageType;
		ssn.update_time = servertime2;
		ssn.kind = 1;
		ssn.unread_num = ssn.unread_num+1;
		ssn.draft_changed = 0;

		if(!sMemSave.InsertSession(ssn))
		{

		}
#endif
	}
	else
	{
		LOGD ("Found it's a repetitive message, ignore it\r\n");
		return;
	}



	// retrieve contact info or group info by from and to
	map<string, string> mapData = RetrieveContactOrMember (eMessageType, from, to);

	MessageParams pMessageParams = {};
	pMessageParams.pMsgId      = id;
	pMessageParams.pSessionId  = pSessionId;
	pMessageParams.pSender     = from;
	pMessageParams.pSenderName = mapData["SenderName"];
	pMessageParams.pSenderKey  = mapData["SenderKey"];
	pMessageParams.pReceiver   = to;
	pMessageParams.pContent    = body;
	pMessageParams.pTime       = clienttime;
	pMessageParams.eType       = eMessageType;
	pMessageParams.eKind       = KT_ORDINARY;
	pMessageParams.eDirection  = DT_RECV;
	pMessageParams.eUnread     = US_UNREAD;
	pMessageParams.ePlayed     = PS_PLAYED;
	pMessageParams.eSend       = SS_UNSENT;
	pMessageParams.eCategory   = eCategoryType;
	pMessageParams.eOnline     = OS_ONLINE;

	// 抛送在线消息
	RspParams rspParams;
	rspParams.eRspType = ST_RECV_MESSAGE;
	rspParams.pMessageParams = &pMessageParams;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_MESSAGE);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}

void CXmppStack::NotifyLoginSuccess ()
{
	LOGD ("XMPP Login Success\r\n");

	RspParams rspParams;
	rspParams.eRspType = ST_LOGIN_SUCCESS;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_LOGIN);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}

void CXmppStack::NotifyLoginFailure ()
{
	LOGD ("XMPP Login Failure\r\n");

	RspParams rspParams;
	rspParams.eRspType = ST_LOGIN_FAILURE;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_LOGIN);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}

void CXmppStack::NotifyLoginVersionLow ()
{
	LOGD ("XMPP Login Version Low\r\n");

	RspParams rspParams ;
	rspParams.eRspType = ST_LOGIN_VERSIONLOW;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_LOGIN);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}

void CXmppStack::NotifyLoginElsewhere ()
{
	LOGD ("XMPP kickout.\r\n");//被踢下线

	RspParams rspParams;
	rspParams.eRspType = ST_LOGIN_ELSEWHERE;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_LOGIN);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}

void CXmppStack::NotifyLogoutSuccess ()
{
	LOGD ("XMPP Logout Success\r\n");

	RspParams rspParams ;
	rspParams.eRspType = ST_LOGOUT_SUCCESS;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_LOGIN);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}

void CXmppStack::NotifyLogoutFailure ()
{
	LOGD ("XMPP Logout Failure\r\n");

	RspParams rspParams;
	rspParams.eRspType = ST_LOGOUT_FAILURE;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_LOGIN);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}

// Network failure handlers
void CXmppStack::NotifyNetworkFailure ()
{
	LOGD ("Xmpp Network Failure\r\n");

	RspParams rspParams ;
	rspParams.eRspType = ST_NETWORK_FAILURE;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_LOGIN);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}

void CXmppStack::NotifySendMessageProcess (const string pMsgId, const string pSessionId)
{
	MessageParams* pMessageParams = new MessageParams();
	pMessageParams->pMsgId = pMsgId;
	pMessageParams->pSessionId = pSessionId;

	pthread_t pthread;	///< 线程句柄
	pthread_create (&pthread, 0, NotifySendMessageProcessRunEntry, pMessageParams);
}

void* CXmppStack::NotifySendMessageProcessRunEntry (void* pv)
{
	MessageParams* pMessageParams = (MessageParams*)pv;
	const string pMsgId = pMessageParams->pMsgId;
	const string pSessionId = pMessageParams->pSessionId;

	LOGD ("Send Message Process, pSessionId=%s, pMsgId=%s\r\n", pSessionId.c_str(), pMsgId.c_str());

	MessageParams messageParams = {};
	messageParams.pMsgId     = pMsgId;
	messageParams.pSessionId = pSessionId;

	RspParams rspParams ;
	rspParams.eRspType       = ST_SEND_MESSAGE_PROCESS;
	rspParams.pMessageParams = &messageParams;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_MESSAGE);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);

	delete pMessageParams;

#if defined(ANDROID)
	///< 全局线程回调
	on_thread_closed (pthread_self ());
#endif

	return NULL;
}

void CXmppStack::NotifySendMessageSuccess (const string pMsgId, const string pSessionId)
{
	LOGD ("Send Message Success, pSessionId=%s, pMsgId=%s\r\n", pSessionId.c_str(), pMsgId.c_str());

	MessageParams messageParams = {};
	messageParams.pMsgId     = pMsgId;
	messageParams.pSessionId = pSessionId;

	RspParams rspParams;
	rspParams.eRspType       = ST_SEND_MESSAGE_SUCCESS;
	rspParams.pMessageParams = &messageParams;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_MESSAGE);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}

void CXmppStack::NotifySendMessageFailure (const string pMsgId, const string pSessionId)
{
	MessageParams* pMessageParams = new MessageParams();
	pMessageParams->pMsgId = pMsgId;
	pMessageParams->pSessionId = pSessionId;

	pthread_t pthread;	///< 线程句柄
	pthread_create (&pthread, 0, NotifySendMessageFailureRunEntry, pMessageParams);
}

void* CXmppStack::NotifySendMessageFailureRunEntry (void* pv)
{
	sleep (1);  // Delay 1 second, for UI draw SendMessageProcess at first.

	MessageParams* pMessageParams = (MessageParams*)pv;
	const string pMsgId = pMessageParams->pMsgId;
	const string pSessionId = pMessageParams->pSessionId;

	LOGD ("Send Message Failure, pSessionId=%s, pMsgId=%s\r\n", pSessionId.c_str(), pMsgId.c_str());

	MessageParams messageParams = {};
	messageParams.pMsgId     = pMsgId;
	messageParams.pSessionId = pSessionId;

	RspParams rspParams;
	rspParams.eRspType       = ST_SEND_MESSAGE_FAILURE;
	rspParams.pMessageParams = &messageParams;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_MESSAGE);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);

	delete pMessageParams;

#if defined(ANDROID)
	///< 全局线程回调
	on_thread_closed (pthread_self ());
#endif

	return NULL;
}

