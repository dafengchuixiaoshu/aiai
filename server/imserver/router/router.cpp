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
#include "router.h"
#include "base64.h"

TControl* GetCtrl();

extern TConfig m_Config;

TSSModule* g_module = NULL;

char* Version="1.0.0.004";

void _System_Run()
{
    if(g_module == NULL)
    {
        g_module = new TSSModule();

        GetCtrl()->SetModule(g_module);
    }
}

THREAD_HANDLER_DECL(getwebmsg, arg)
{
    TGetWebMsg* getwebmsg = (TGetWebMsg*)arg;

    getwebmsg->Process();

    return 0;
}

TGetWebMsg::TGetWebMsg()
: m_istart(true)
, m_thread(NULL)
{
}

TGetWebMsg::~TGetWebMsg()
{
    if(m_istart)
    {
        m_istart = false; sleep_s(3);
    }

    if(NULL != m_thread) delete m_thread;
}

void TGetWebMsg::OnRun()
{
    m_log = GET_LOG(webmsg);

    init_dbmgr(m_odddbmgrs,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,1);

    init_dbmgr(m_haiyuandbmgrs,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,1);

    m_thread = new kernel::CTThread(getwebmsg, this);

    if(NULL == m_thread)
    {
        term("web msg thread is null"); exit(0);
    }
}

void TGetWebMsg::Process()
{
    string proto = g_module->m_proto;

    TDBManager* oddmgr = m_odddbmgrs[0];

    TDBManager* haiyuandbmgr = m_haiyuandbmgrs[0];

    while(m_istart)
    {
        TMySQLParam sqlparam; sleep_s(1);

        static const char sqltxt[1024] = "SELECT touser,msgid,talkid,fromuser,body,id,dstplatform FROM message_timings WHERE sendflag=0 AND srcplatform='webc' AND dstplatform<>'webc' ORDER BY id";

        if(0 == sync_exec_sql(oddmgr, &sqlparam, sqltxt, NULL)) continue;

        int recordcount = sqlparam.GetRecordCount();

        if(0 == recordcount) continue;

        for(int index = 0; index < recordcount; ++index)
        {
            string touser = sqlparam.GetStrValue(index, 0);
            string msgid = sqlparam.GetStrValue(index, 1);
            string talkid = sqlparam.GetStrValue(index, 2);
            string fromuser = sqlparam.GetStrValue(index, 3);
            string body = sqlparam.GetStrValue(index, 4);
            string id = sqlparam.GetStrValue(index, 5);
            string dstplatform = sqlparam.GetStrValue(index, 6);

            TMySQLParam sqlparam;

            static const char fmt[] = "SELECT moduleaddr FROM im_online_info WHERE status=1 AND name='%s'";

            char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, touser.c_str());

            if(0 == sync_exec_sql(haiyuandbmgr, &sqlparam, sqltxt, NULL)) continue;

            if(1 != sqlparam.GetRecordCount()) continue;

            string moduleaddr = sqlparam.GetStrValue(0, 0);

            string::size_type pos = talkid.find("_");

            if(string::npos != pos) talkid.erase(0, pos + 1);

            if("websystem" == dstplatform)
            {
                continue;
            }
            else if("system" == dstplatform)
            {
                TMySQLParam sqlparam; char sqltxt[2048] = { 0 };

                static const char fmt[] = "INSERT INTO im_msg_chat(id,fromuser,touser,togroup,type,category,body,status,recvtime) VALUES('%s','%s','%s','%s','%s','%s','%s',0,now())";

                sprintf(sqltxt, fmt, msgid.c_str(), talkid.c_str(), touser.c_str(), "", "customchat", "system", body.c_str());

                if(0 == sync_exec_sql(haiyuandbmgr, &sqlparam, sqltxt, NULL)) continue;
            }
            else
            {
                static const char end[] = "\",\"";

                static const char type[] = "\"type\":"; static const int type_len = strlen(type);

                static const char text[] = "\"text\":\""; static const int text_len = strlen(text);

                static const char img[] = "\"img\":\""; static const int img_len = strlen(img);

                string::size_type typepos = body.find(type);

                if(string::npos == typepos) continue;

                const char* delim; int delim_len; string sendmsg;

                int typevalue = atoi(&body[typepos + type_len]);

                if(1 == typevalue)
                {
                    delim = text; delim_len = text_len;

                    static const char fmt[] = "{\"type\":1,\"text\":\"";

                    sendmsg = fmt;
                }
                else if(2 == typevalue)
                {
                    delim = img; delim_len = img_len;

                    static const char fmt[] = "{\"type\":2,\"img\":\"";

                    sendmsg = fmt;
                }
                else
                {
                    continue;
                }

                string::size_type startpos = body.find(delim);

                string::size_type endpos = body.find(end, startpos + delim_len);

                if(string::npos == startpos || string::npos == endpos) continue;

                string parsemsg = body.substr(startpos + delim_len, endpos - startpos - delim_len);

                sendmsg += parsemsg + "\"}"; int msglen = sendmsg.size();

                char* enmsg = new char[msglen + (msglen >> 1)];

                base64_encode((char*)(sendmsg.c_str()), msglen, enmsg);

                TMySQLParam sqlparam; char sqltxt[2048] = { 0 };

                static const char fmt[] = "INSERT INTO im_msg_chat(id,fromuser,touser,togroup,type,category,body,status,recvtime) VALUES('%s','%s','%s','%s','%s','%s','%s',0,now())";

                sprintf(sqltxt, fmt, msgid.c_str(), fromuser.c_str(), touser.c_str(), talkid.c_str(),
                    "customchat", (1 == typevalue) ? "plain-text" : "image", enmsg);

                if(0 == sync_exec_sql(haiyuandbmgr, &sqlparam, sqltxt, NULL)) continue;

                delete enmsg;
            }

            TString sendstring;
            
            TImChat imchat(msgid, moduleaddr, proto);

            if(imchat.ToBuffer(sendstring))
            {
                TMySQLParam sqlparam;

                static const char fmt[] = "UPDATE message_timings SET sendflag=1,sendtime=now() WHERE id=%s";

                char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, id.c_str());

                if(0 != sync_exec_sql(oddmgr, &sqlparam, sqltxt, NULL))
                {
                    LOG_INFO(m_log, "touser:%s,msgid:%s,moduleaddr:%s", touser.c_str(), msgid.c_str(), moduleaddr.c_str());

                    g_module->Transform(moduleaddr, sendstring.GetBuffer(), sendstring.GetSize());
                }
            }
        }
    }
}

int TGetWebMsg::GetPacklen(const char* inputData,int length,int& startPos)
{
    return length;
}

#define SS_ERR_LOG(...)  LOG_ERROR(m_logfile, __VA_ARGS__)

void TSSModule::OnRun()
{
    m_logfile = GET_LOG(ss);

    m_proto = m_Config.GetImSockInfo().m_proto;

    m_moduleaddr = split_to_string("%s:%d",
        m_Config.GetImSockInfo().m_ip.c_str(), m_Config.GetImSockInfo().m_port);

    init_dbmgr(m_dbmgrs,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,m_Config.GetWorkCount());

    {
        TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "REPLACE INTO im_module_info(name,addr,version,uptime) VALUES('router','%s','',now())";

        sprintf(sqltxt, fmt, m_moduleaddr.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[0], &sqlparam, sqltxt, m_pDBLog))
        {
            term("replace im_module_info failed");

            exit(0);
        }
    }

    //m_getwebmsg.OnRun();

    term("run module success");
}

void TSSModule::ProcessTimer(TModuleMsg* pMsg)
{
    static int count = 0;

    if(0 == count % 60)
    {
        TMySQLParam sqlparam;

        char sqltxt[1024] = { 0 };

        static const char fmt[] = "UPDATE im_module_info SET uptime=now() WHERE addr='%s'";

        sprintf(sqltxt, fmt, m_moduleaddr.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, NULL))
        {
            term("update im_module_info:%s failed", m_moduleaddr.c_str());
        }
    }

    if(0 == count % 180)
    {
        TMySQLParam sqlparam;

        static const char sqltxt[] = "SELECT addr FROM im_module_info WHERE name='xmpp' and adddate(uptime, interval 3 minute) > now()";

        if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, NULL) > 0)
        {
            int recordcount = sqlparam.GetRecordCount();

            for(int index = 0; index < recordcount; ++index)
            {
                m_mutex.Lock();

                string addr = sqlparam.GetStrValue(index, 0);

                if(0 == m_upstreammap.count(addr))
                {
                    string::size_type pos = addr.find(':');

                    if(string::npos == pos) continue;

                    string ip = addr.substr(0, pos);

                    int port = atoi(addr.c_str() + pos + 1);

                    static string prototype = m_Config.GetImSockInfo().m_proto;

                    TUpStream* upstream = new TUpStream(ip.c_str(), port, 0, m_moduleaddr, prototype);

                    if(NULL == upstream)
                    {
                        continue;
                    }
                    else
                    {
                        upstream->Run("ss.izp.com", "router", "linux");

                        m_upstreammap[addr] = upstream;
                    }
                }

                m_mutex.Unlock();
            }
        }
    }

    ++count;
}

void TSSModule::ProcessImMsg(TModuleMsg* pMsg)
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
            SS_ERR_LOG("addrinfo:%s can not do this type:%d", addrinfo, pMsg->m_pParam->m_msgtype);
            break;
        }
    }
}

void TSSModule::ProcessAck(const char* addrinfo, TModuleMsg* pMsg)
{
    TImAck* imack = dynamic_cast<TImAck*>(pMsg->m_pParam);

    if(NULL == imack)
    {
        SS_ERR_LOG("addrinfo:%s to ack pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, pMsg->m_pParam->m_prototype, 1);
    }
    else
    {
        m_mutex.Lock();

        upstream_map::iterator it = m_upstreammap.find(imack->m_addr);

        if(m_upstreammap.end() != it)
        {
            it->second->SendMsg(pMsg->m_strMessage, pMsg->m_nMsgLength);
        }

        m_mutex.Unlock();
    }
}

void TSSModule::ProcessClose(const char* addrinfo, TModuleMsg* pMsg)
{
    TImClose* imclose = dynamic_cast<TImClose*>(pMsg->m_pParam);

    if(NULL == imclose)
    {
        SS_ERR_LOG("addrinfo:%s to close pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, pMsg->m_pParam->m_prototype, 1);
    }
    else
    {
        m_mutex.Lock();

        upstream_map::iterator it = m_upstreammap.find(imclose->m_addr);

        if(m_upstreammap.end() != it)
        {
            it->second->SendMsg(pMsg->m_strMessage, pMsg->m_nMsgLength);
        }

        m_mutex.Unlock();
    }
}

void TSSModule::ProcessMessage(const char* addrinfo, TModuleMsg* pMsg)
{
    TImChat* imchat = dynamic_cast<TImChat*>(pMsg->m_pParam);

    static const int max_body_size = 1024 + 256 + 128;//1024 + 1024 / 3

    if(NULL == imchat)
    {
        SS_ERR_LOG("addrinfo:%s to chat pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, pMsg->m_pParam->m_prototype, 1);
    }
    else
    {
        Transform(imchat->m_addr, pMsg->m_strMessage, pMsg->m_nMsgLength);
    }
}

void TSSModule::Transform(string& moduleaddr, const char* sendbuf, int sendlen)
{
    m_mutex.Lock();

    upstream_map::iterator it = m_upstreammap.find(moduleaddr);

    if(m_upstreammap.end() != it)
    {
        it->second->SendMsg(sendbuf, sendlen);
    }

    m_mutex.Unlock();
}
