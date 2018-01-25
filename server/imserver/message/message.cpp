#ifdef WIN32
#include <afx.h>
#include <Winsock2.h>
#define snprintf _snprintf
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "Control.h"
#include "Config.h"
#include "Log.h"
#include "xml.h"
#include "DBManager.h"
#include "imsocket.h"
#include "message.h"

TControl* GetCtrl();

extern TConfig m_Config;

TMSModule* g_module = NULL;

char* Version="0.1.001";

void _System_Run()
{
    if(g_module == NULL)
    {
        g_module = new TMSModule();

        GetCtrl()->SetModule(g_module);
    }
}

#define MS_ERR_LOG(...)  LOG_ERROR(m_logfile, __VA_ARGS__)

#define MS_INFO_LOG(...) LOG_INFO(m_logfile, __VA_ARGS__)

#define IM_INFO_LOG(...) LOG_INFO(m_pImLog, __VA_ARGS__)

void TMSModule::OnInit()
{
    m_moduleaddr = split_to_string("%s:%d",
        m_Config.GetImSockInfo().m_ip.c_str(), m_Config.GetImSockInfo().m_port);

    m_downaddr = m_Config.GetXmlElem()->GetSubElem("DOWNSTREAM")->GetSubElem("STREAM")->GetAttr("ADDR")->getValue();

    for(int index = 0; index < 2; ++index)
    {
        const char* name = m_Config.GetXmlElem()->GetSubElem("UPSTREAM")->GetSubElem("STREAM", index)->GetAttr("NAME")->getValue();

        const char* addr = m_Config.GetXmlElem()->GetSubElem("UPSTREAM")->GetSubElem("STREAM", index)->GetAttr("ADDR")->getValue();

        if(0 == strcmp(name, "router"))
        {
            start_upstream(addr, m_upaddr, m_upstreams, m_Config.GetImSockInfo().m_proto);
        }
        else if(0 == strcmp(name, "iospush"))
        {
            start_upstream(addr, m_iospushaddr, m_iospushstreams, m_Config.GetImSockInfo().m_proto);
        }
        else
        {
            term("undefined upstream:%s", name);

            sleep_s(1);

            exit(0);
        }
    }
}

void TMSModule::OnRun()
{
    m_logfile = GET_LOG(ms);

    init_dbmgr(m_dbmgrs,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,m_Config.GetWorkCount());

    init_dbmgr(m_wbbnewdbmgrs,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,m_Config.GetWorkCount());

    {
        TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "REPLACE INTO im_module_info(name,addr,version,uptime) VALUES('message','%s','',now())";

        sprintf(sqltxt, fmt, m_moduleaddr.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[0], &sqlparam, sqltxt, m_pDBLog))
        {
            term("replace im_module_info failed");

            exit(0);
        }
    }

    term("run module success");
}

void TMSModule::ProcessTimer(TModuleMsg* pMsg)
{
    static int count = 0;

    if(0 == count % 60)
    {
        //TMySQLParam sqlparam;

        //char sqltxt[1024] = { 0 };

        //static const char fmt[] = "SELECT CASE WHEN ADDDATE(uptime, interval 3 minute) > now() THEN 1 ELSE 0 END AS status FROM im_module_info WHERE addr='%s' OR addr='%s'";

        //sprintf(sqltxt, fmt, m_upaddr.c_str(), m_iospushaddr.c_str());

        //if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, NULL))
        //{
        //    term("select im_module_info:%s failed", m_upaddr.c_str());
        //}
        //else if(2 == sqlparam.GetRecordCount())
        //{
        //    if(1 == sqlparam.GetIntValue(0, 0) && 1 == sqlparam.GetIntValue(1, 0))
        //    {
                TMySQLParam sqlparam;

                char sqltxt[1024] = { 0 };

                static const char fmt[] = "UPDATE im_module_info SET uptime=now() WHERE addr='%s'";

                sprintf(sqltxt, fmt, m_moduleaddr.c_str());

                if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, NULL))
                {
                    term("update im_module_info:%s failed", m_moduleaddr.c_str());
                }
    //        }
    //    }
    }

    ++count;
}

void TMSModule::ProcessImMsg(TModuleMsg* pMsg)
{
    if(SOCK_TYPE_MESSAGE == pMsg->m_nSubMsgType)
    {
        char* addrinfo = pMsg->m_strFromIP;

        switch(pMsg->m_pParam->m_msgtype)
        {
        case IM_ACK:
            ProcessAck(addrinfo, pMsg);
            break;

        case IM_CHAT:
            ProcessMessage(addrinfo, pMsg);
            break;

        case IM_CLOSE:
            ProcessClose(addrinfo, pMsg);
            break;

        case IM_KEEPALIVE:
            break;

        default:
            MS_ERR_LOG("addrinfo:%s can not do this type:%d", addrinfo, pMsg->m_pParam->m_msgtype);
            break;
        }
    }
}

void TMSModule::ProcessAck(const char* addrinfo, TModuleMsg* pMsg)
{
    TImAck* imack = dynamic_cast<TImAck*>(pMsg->m_pParam);

    if(NULL == imack)
    {
        MS_ERR_LOG("addrinfo:%s to ack pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, pMsg->m_pParam->m_prototype, 1);

        return;
    }

    {
        TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "SELECT fromuser,touser FROM im_msg_chat WHERE id='%s'";

        sprintf(sqltxt, fmt, imack->m_id.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
        {
            MS_ERR_LOG("id:%s ack getmsg failed", imack->m_id.c_str());

            return;
        }
        
        string fromuser = sqlparam.GetStrValue(0, 0);
        string touser = sqlparam.GetStrValue(0, 1);

        int issave = 0; int isshare = 0;

        GetSaveAndShare(m_dbmgrs[pMsg->m_nThreadIndex], fromuser, touser, issave, isshare);

        if(1 == issave || 1 == isshare)
        {
            MS_ERR_LOG("id:%s ack msg is save or share", imack->m_id.c_str());

            TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

            static const char fmt[] = "UPDATE im_msg_chat SET status=1 WHERE id='%s'";

            sprintf(sqltxt, fmt, imack->m_id.c_str());

            sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog);

            return;
        }            
    }

    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

#ifdef RECORD_MESSAGE
    static const char fmt[] = "UPDATE im_msg_chat SET status=1,sendtime=now() WHERE id='%s'";
#else
    static const char fmt[] = "DELETE FROM im_msg_chat WHERE id='%s'";
#endif

    sprintf(sqltxt, fmt, imack->m_id.c_str());

    if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
    {
        MS_ERR_LOG("id:%s ack failed", imack->m_id.c_str());
    }

    IM_INFO_LOG("ack <id='%s'>", imack->m_id.c_str());
}

void TMSModule::ProcessClose(const char* addrinfo, TModuleMsg* pMsg)
{
    TImClose* imclose = dynamic_cast<TImClose*>(pMsg->m_pParam);

    if(NULL == imclose)
    {
        MS_ERR_LOG("addrinfo:%s to close pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, pMsg->m_pParam->m_prototype, 1);
    }
    else
    {
        IM_INFO_LOG("close <user='%s' addr='%s>", addrinfo, imclose->m_user.c_str(), imclose->m_addr.c_str());

        m_upstreams[pMsg->m_nThreadIndex]->SendMsg(pMsg->m_strMessage, pMsg->m_nMsgLength);
    }
}

void TMSModule::ProcessMessage(const char* addrinfo, TModuleMsg* pMsg)
{
    TImChat* imchat = dynamic_cast<TImChat*>(pMsg->m_pParam);

    static const int max_body_size = 1024 + 256 + 128;//1024 + 1024 / 3

    if(NULL == imchat)
    {
        MS_ERR_LOG("addrinfo:%s to chat pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, pMsg->m_pParam->m_prototype, 1);

        return;
    }
    else if(0 == imchat->m_body.size() ||  imchat->m_body.size() > max_body_size)
    {
        MS_ERR_LOG("id:%s,from:%s,body:%s body is empty or too long",
            imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_body.c_str());

        return;
    }

    if(!imchat->m_addr.empty())
    {
        m_iospushstreams[pMsg->m_nThreadIndex]->SendMsg(pMsg->m_strMessage, pMsg->m_nMsgLength);
    }
    else if("groupchat" == imchat->m_type)
    {
        IM_INFO_LOG("<id='%s' to='%s' from='%s' type='%s' category='%s' body='%s'>", 
            imchat->m_id.c_str(), imchat->m_to.c_str(), imchat->m_from.c_str(), imchat->m_type.c_str(), imchat->m_category.c_str(), imchat->m_body.c_str());

        TMySQLParam sqlparam; char sqltxt[2048] = { 0 };

        static const char fmt[] = "SELECT uid FROM im_pub_groupuser WHERE status=1 AND gid='%s' AND uid<>'%s'";

        sprintf(sqltxt, fmt, imchat->m_to.c_str(), imchat->m_from.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
        {
            MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s get group user failed",
                imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());
        }
        else
        {
            int recordcount = sqlparam.GetRecordCount();

            TMySQLParam gchatsqlparam; char sqltxt[2048] = { 0 };

            static const char fmt[] = "INSERT INTO im_msg_gchat(id,fromuser,togroup,type,category,body,recvtime) VALUES('%s','%s','%s','%s','%s','%s',now())";

            sprintf(sqltxt, fmt, imchat->m_id.c_str(), imchat->m_from.c_str(),
                imchat->m_to.c_str(), imchat->m_type.c_str(), imchat->m_category.c_str(), imchat->m_body.c_str());

            if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &gchatsqlparam, sqltxt, m_pDBLog))
            {
                MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s insert gchat failed",
                    imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());
            }
            else
            {
                ProcessChatAck(imchat, pMsg, false);

                if(0 == recordcount)
                {
                    MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s no user in group",
                        imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());
                }
                else
                {
                    for(int index = 0; index < recordcount; ++index)
                    {
                        string touser = sqlparam.GetStrValue(index, 0);

                        string msgid = create_coreid(touser.c_str());

                        TImChat chat(msgid, imchat->m_from, imchat->m_to, imchat->m_type,
                            imchat->m_category, "", imchat->m_body, 0, 0, imchat->m_prototype);

                        ProcessChat(&chat, pMsg, touser, imchat->m_id);
                    }
                }
            }
        }
    }
    else if("chat" == imchat->m_type || "customchat" == imchat->m_type || "maishouchat" == imchat->m_type)
    {
        IM_INFO_LOG("<id='%s' to='%s' from='%s' type='%s' category='%s' body='%s'>",
            imchat->m_id.c_str(), imchat->m_to.c_str(), imchat->m_from.c_str(), imchat->m_type.c_str(), imchat->m_category.c_str(), imchat->m_body.c_str());

        if(ProcessChat(imchat, pMsg, imchat->m_to))
        {
            ProcessChatAck(imchat, pMsg, true);
        }
    }
    else
    {
        MS_ERR_LOG("undefined chat type:%s", imchat->m_type.c_str());

        return;
    }
}

bool TMSModule::ProcessChat(TImChat* imchat, TModuleMsg* pMsg, string& touser, string gmsg)
{
    TMySQLParam sqlparam; char sqltxt[2048] = { 0 };

    static const char fmt[] = "INSERT INTO im_msg_chat(id,fromuser,touser,togroup,gmsg,type,category,body,status,recvtime) VALUES('%s','%s','%s','%s','%s','%s','%s','%s',0,now())";

    sprintf(sqltxt, fmt, imchat->m_id.c_str(), imchat->m_from.c_str(), touser.c_str(),
        (imchat->m_to == touser) ? "" : imchat->m_to.c_str(), gmsg.c_str(), imchat->m_type.c_str(), imchat->m_category.c_str(), imchat->m_body.c_str());

    if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
    {
        MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s insert chat failed",
            imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());

        return false;
    }
    else
    {
        TMySQLParam sqlparam; char sqltxt[2048] = { 0 };

        static const char fmt[] = "SELECT moduleaddr FROM im_online_info WHERE name='%s' AND status=1 GROUP BY moduleaddr";

        sprintf(sqltxt, fmt, touser.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
        {
            MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s check online failed1",
                imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());
        }
        else if(1 == sqlparam.GetRecordCount())
        {
            TImChat chat(imchat->m_id, sqlparam.GetStrValue(0, 0), imchat->m_prototype);

            m_upstreams[pMsg->m_nThreadIndex]->SendMsg(chat);

            MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s to router",
                imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());
        }
        else if(0 == sqlparam.GetRecordCount())
        {
            TImChat chat(imchat->m_id, imchat->m_from, touser, imchat->m_type,
                imchat->m_category, "", imchat->m_body, 0, 0, imchat->m_prototype);

            m_iospushstreams[pMsg->m_nThreadIndex]->SendMsg(chat);
            
            MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s to iospush",
                imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());
        }
        else
        {
            MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s check online failed2",
                imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());
        }

        return true;
    }
}

void TMSModule::ProcessChatAck(TImChat* imchat, TModuleMsg* pMsg, bool ischat)
{
    TMySQLParam sqlparam; char sqltxt[2048] = { 0 };

    static const char fmt[] = "SELECT recvtime FROM %s WHERE id='%s'";

    sprintf(sqltxt, fmt, ischat ? "im_msg_chat" : "im_msg_gchat", imchat->m_id.c_str());

    if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
    {
        MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s chat ack failed1",
            imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());
    }
    else if(1 != sqlparam.GetRecordCount())
    {
        MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s chat ack failed2",
            imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());
    }
    else
    {
//#ifndef RECORD_MESSAGE
//        if(!ischat)
//        {
//            TMySQLParam sqlparam; char sqltxt[2048] = { 0 };
//
//            static const char fmt[] = "DELETE FROM im_msg_gchat WHERE id='%s'";
//
//            sprintf(sqltxt, fmt, imchat->m_id.c_str());
//
//            if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
//            {
//                MS_ERR_LOG("id:%s,from:%s,to:%s,body:%s gchat del failed",
//                    imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_to.c_str(), imchat->m_body.c_str());
//            }
//        }
//#endif

        TImAck imack(imchat->m_id, sqlparam.GetStrValue(0, 0), imchat->m_from, m_downaddr, imchat->m_prototype);

        m_upstreams[pMsg->m_nThreadIndex]->SendMsg(imack);
    }
}


bool TMSModule::GetSaveAndShare(TDBManager* dbmgr, const string& from, const string& to, int& issave, int& isshare)
{
    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

    static const char fmt[] ="SELECT id FROM users WHERE userName='%s' AND status=1";

    sprintf(sqltxt, fmt, from.c_str());

    if(0 != sync_exec_sql(dbmgr, &sqlparam, sqltxt, m_pDBLog))
    {
        if(1 == sqlparam.GetRecordCount())
        {
            string fromid = sqlparam.GetStrValue(0, 0);

            TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

            static const char fmt[] ="SELECT id FROM users WHERE userName='%s' AND status=1";

            sprintf(sqltxt, fmt, to.c_str());

            if(0 != sync_exec_sql(dbmgr, &sqlparam, sqltxt, m_pDBLog))
            {
                if(1 == sqlparam.GetRecordCount())
                {
                    string toid = sqlparam.GetStrValue(0, 0);

                    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

                    static const char fmt[] = "SELECT issave,isshare FROM users_friend WHERE uid=%s AND friend_id=%s";

                    sprintf(sqltxt, fmt, fromid.c_str(), toid.c_str());

                    if(0 != sync_exec_sql(dbmgr, &sqlparam, sqltxt, m_pDBLog))
                    {
                        if(sqlparam.GetRecordCount() > 0)
                        {
                            issave = sqlparam.GetIntValue(0, 0);
                            isshare = sqlparam.GetIntValue(0, 1);

                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}
