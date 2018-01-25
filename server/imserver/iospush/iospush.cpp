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
#include "iospush.h"
#include "base64.h"

TControl* GetCtrl();

extern TConfig m_Config;

TPushModule* g_module = NULL;

char* Version="0.1.001";

void _System_Run()
{
    if(g_module == NULL)
    {
        g_module = new TPushModule();

        GetCtrl()->SetModule(g_module);
    }
}

map<string, string> g_pubinfos;

map<string, string> g_iconinfos;

#define IM_ERR_LOG(...) LOG_ERROR(m_pImLog, __VA_ARGS__)

#define IM_INFO_LOG(...) LOG_INFO(m_pImLog, __VA_ARGS__)

THREAD_HANDLER_DECL(ios_push_msg, arg)
{
    TIosPush* iospush = (TIosPush*)arg;

    iospush->Process();

    return 0;
}

TIosPush::TIosPush(char* ip, int port, string profile)
: TTcpClient(ip, port, NULL)
, m_port(port)
, m_ip(ip)
, m_profile(profile)
, m_thread(NULL)
{
}

TIosPush::~TIosPush()
{
    if(NULL != m_thread) delete m_thread;
}

void TIosPush::OnRun()
{
    m_log = GET_LOG(iospush);

    m_thread = new kernel::CTThread(ios_push_msg, this);

    if(NULL == m_thread)
    {
        term("ios push thread is null"); exit(0);
    }
}

void TIosPush::Push(const char* f2cid, const char* body, int count)
{
    static const char fmt[] = "{'methodName':'sendMsgF2c','data':{'receiverId':%s,'badge':%d,'content':'";

    char txt[1024] = { 0 }; sprintf(txt, fmt, f2cid, count);

    string pushmsg = txt; pushmsg += body; pushmsg += "'}}";

    m_iospushmsg.TryPut(pushmsg);
}

void TIosPush::Process()
{
    int port = m_port;

    char profile[1024] = { 0 };

    char* ip = (char*)(m_ip.c_str());

    sprintf(profile, " HTTP/1.1\r\nHost: %s:%d\r\nContent-Length: 0\r\n\r\n", ip, port);

    while(1)
    {
        string pushmsg;

        if(0 == m_iospushmsg.TryGet(pushmsg))
        {
            int fd = ConnectToServer(ip, port);

            if(fd <= 0)
            {
                LOG_ERROR(m_log, "connecttoserver failed, msg: %s", pushmsg.c_str());

                continue;
            }

            int msglen = pushmsg.size();

            char* enmsg = new char[msglen + (msglen >> 1)];

            base64_encode((char*)(pushmsg.c_str()), msglen, enmsg);

            string request = "GET " + m_profile + enmsg + profile;

            int sendlen = OnSendMsg(fd, request.c_str(), request.size(), NULL);

            if(sendlen >= 0)
            {
                char recvbuffer[1024 + 1] = { 0 }; bool isuccess = false;

                int readlen = recv(fd, recvbuffer, 1024, 0);

                if(readlen > 0)
                {
                    string recvstring(recvbuffer);

                    static const char codefmt[] = "\"code\":";

                    static const int codefmt_len = strlen(codefmt);

                    string::size_type pos = recvstring.find(codefmt);

                    if(string::npos != pos)
                    {
                        const char* code = recvstring.c_str() + pos + codefmt_len;

                        int icode = atoi(code);

                        if(0 == icode)
                        {
                            isuccess = true;
                        }
                    }
                }

                LOG_INFO(m_log, "%s,pushmsg:%s,send:%s,recv:%s", isuccess ? "successful" : "failed", pushmsg.c_str(), request.c_str(), recvbuffer);
            }

            delete enmsg; closesocket(fd);
        }
    }
}

void TPushModule::OnInit()
{
    string upaddr;

    m_moduleaddr = split_to_string("%s:%d",
        m_Config.GetImSockInfo().m_ip.c_str(), m_Config.GetImSockInfo().m_port);

    const char* addr = m_Config.GetXmlElem()->GetSubElem("UPSTREAM")->GetSubElem("STREAM")->GetAttr("ADDR")->getValue();

    const char* profile = m_Config.GetXmlElem()->GetSubElem("UPSTREAM")->GetSubElem("STREAM")->GetAttr("PROFILE")->getValue();

    const char* delim = strstr(addr, ":");

    string ip; ip.append(addr, delim - addr);

    int port = atoi(++delim);

    m_iospush = new TIosPush((char*)(ip.c_str()), port, profile);

    m_iospush->OnRun();
}

void TPushModule::OnRun()
{
    m_logfile = GET_LOG(ms);

    init_dbmgr(m_dbmgrs,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,m_Config.GetWorkCount());

    {
        TMySQLParam sqlparam;

        static char sqltxt[] = "SELECT pubkey,pubvalue FROM im_pub_info WHERE type=2";

        if(0 == sync_exec_sql(m_dbmgrs[0], &sqlparam, sqltxt, m_pDBLog))
        {
            term("get im_pub_info failed1"); sleep_s(1); exit(0);
        }

        int count = sqlparam.GetRecordCount();

        if(0 == count)
        {
            term("get im_pub_info failed2"); sleep_s(1); exit(0);
        }

        for(int index = 0; index < count; ++index)
        {
            string key = sqlparam.GetStrValue(index, 0);
            string value = sqlparam.GetStrValue(index, 1);

            g_pubinfos[key] = value;
        }
    }

    {
        TMySQLParam sqlparam;

        static char sqltxt[] = "SELECT iconkey,iconvalue FROM im_icon_info";

        if(0 == sync_exec_sql(m_dbmgrs[0], &sqlparam, sqltxt, m_pDBLog))
        {
            term("get im_icon_info failed1"); sleep_s(1); exit(0);
        }

        int count = sqlparam.GetRecordCount();

        if(0 == count)
        {
            term("get im_icon_info failed2"); sleep_s(1); exit(0);
        }

        for(int index = 0; index < count; ++index)
        {
            string key = sqlparam.GetStrValue(index, 0);
            string value = sqlparam.GetStrValue(index, 1);

            g_iconinfos[key] = value;
        }
    }

    {
        TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "REPLACE INTO im_module_info(name,addr,version,uptime) VALUES('iospush','%s','',now())";

        sprintf(sqltxt, fmt, m_moduleaddr.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[0], &sqlparam, sqltxt, m_pDBLog))
        {
            term("replace im_module_info failed");

            exit(0);
        }
    }

    term("run module success");
}

void TPushModule::ProcessTimer(TModuleMsg* pMsg)
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

    ++count;
}

void TPushModule::ProcessImMsg(TModuleMsg* pMsg)
{
    if(SOCK_TYPE_MESSAGE == pMsg->m_nSubMsgType)
    {
        char* addrinfo = pMsg->m_strFromIP;

        switch(pMsg->m_pParam->m_msgtype)
        {
        case IM_CHAT:
            ProcessMessage(addrinfo, pMsg);
            break;

        case IM_KEEPALIVE:
            DoImResponse(pMsg->m_strMessage, pMsg->m_nMsgLength, addrinfo, pMsg->m_pParam->m_prototype);
            break;

        default:
            IM_INFO_LOG("addrinfo:%s can not do this type:%d", addrinfo, pMsg->m_pParam->m_msgtype);
            break;
        }
    }
}

#define MAKEBYTE    0x80 //1000 0000
#define MAKE2BYTES  0xC0 //1100 0000
#define MAKE3BYTES  0xE0 //1110 0000

int split_utf8(const char* inbuf, int inlen, int howmany)
{
    int in = 0; int out = 0;

    while(in < howmany && out < inlen)
    {
        // 1110xxxx 10xxxxxx 10xxxxxx
        if(MAKE3BYTES == (inbuf[out] & MAKE3BYTES))
        {
            out += 3;
        }
        // 110xxxxx 10xxxxxx
        else if(MAKE2BYTES == (inbuf[out] & MAKE2BYTES))
        {
            out += 2;
        }
        // 0xxxxxxx
        else if(inbuf[out] < MAKEBYTE)
        {
            ++out;
        }

        ++in;
    }

    return out;
}

static int parse_pushmsg(string& outbuf, const char* message, size_t messagelen)
{
    int typevalue = 0;

    const char* start = NULL;

    char* inbuf = new char[messagelen];

    static const char type[] = "\"type\":";

    static const int typelen = strlen(type);

    int inlen = base64_decode(message, messagelen, inbuf);

    inbuf[inlen - 2] = '\0';

    if(NULL != (start = strstr(inbuf, type)))
    {
        if(1 == (typevalue = atoi(start + typelen)))//text
        {
            static const char text[] = "\"text\":\"";

            static const int textlen = strlen(text);

            if(NULL == (start = strstr(inbuf, text)))
            {
                typevalue = 0;
            }
            else
            {
                int count = 0;

                start += textlen;

                outbuf += g_pubinfos["txtstart"];

                while('\0' != *start && count < 50)
                {
                    if('/' == *(start+1) && 'f' == *(start+1+1) && 'f' == *(start+1+4+1) && '/' == *(start+1+4+1+1+1))
                    {
                        string key;

                        key.append(start, 9);

                        start += 9;

                        map<string,string>::iterator it = g_iconinfos.find(key);

                        if(g_iconinfos.end() == it)
                        {
                            outbuf += key;

                            count += 9;
                        }
                        else
                        {
                            outbuf += it->second;

                            count += split_utf8(it->second.c_str(), it->second.size(), it->second.size());
                        }
                    }
                    else
                    {
                        ++count;

                        int len = split_utf8(start, strlen(start), 1);

                        outbuf.append(start, len);

                        start += len;
                    }
                }

                if('\0' != *start)
                {
                    outbuf += g_pubinfos["txtend"];
                }
            }
        }
    }

    return typevalue;
}

void TPushModule::ProcessMessage(const char* addrinfo, TModuleMsg* pMsg)
{
    TImChat* imchat = dynamic_cast<TImChat*>(pMsg->m_pParam);

    if(NULL == imchat)
    {
        IM_INFO_LOG("addrinfo:%s to chat pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, pMsg->m_pParam->m_prototype, 1);

        return;
    }

    IM_INFO_LOG("push msg <id='%s' to='%s' from='%s' type='%s' category='%s' body='%s'>",
        imchat->m_id.c_str(), imchat->m_to.c_str(), imchat->m_from.c_str(), imchat->m_type.c_str(), imchat->m_category.c_str(), imchat->m_body.c_str());

    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

    static const char fmt[] = "SELECT status FROM im_online_info WHERE name='%s' AND platform='ios'";// AND status=0";

    sprintf(sqltxt, fmt, imchat->m_to.c_str());

    if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog) <= 0)
    {
        IM_ERR_LOG("get platform failed <id:%s,body:%s>", imchat->m_id.c_str(), imchat->m_body.c_str());
    }
    else if(0 == sqlparam.GetRecordCount())   
    {
        IM_ERR_LOG("ignore maybe is not ios <id:%s,body:%s>", imchat->m_id.c_str(), imchat->m_body.c_str());
    }
    else
    {
        TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "SELECT opvalue FROM im_record_info WHERE name='%s' AND record='pushstatus'";

        sprintf(sqltxt, fmt, imchat->m_to.c_str());

        if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog) <= 0)
        {
            IM_ERR_LOG("get opvalue failed <id:%s,body:%s>", imchat->m_id.c_str(), imchat->m_body.c_str());
        }
        else
        {
            bool ispush = true;

            if(1 == sqlparam.GetRecordCount())
            {
                ispush = 1 != sqlparam.GetIntValue(0, 0);
            }

            if(ispush)
            {
                TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

                static const char fmt[] = "SELECT nickName FROM users WHERE userName='%s' AND status=1";

                sprintf(sqltxt, fmt, imchat->m_from.c_str());

                if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog) <= 0)
                {
                    IM_ERR_LOG("get nickName failed <id:%s,body:%s>", imchat->m_id.c_str(), imchat->m_body.c_str());
                }
                else if(1 != sqlparam.GetRecordCount())
                {
                    IM_ERR_LOG("get nickName count error <id:%s,body:%s>", imchat->m_id.c_str(), imchat->m_body.c_str());
                }
                else
                {
                    string nickname = sqlparam.GetStrValue(0, 0);

                    string outbuf = (nickname.empty()) ? imchat->m_from : nickname;

                    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

                    static const char fmt[] = "SELECT id FROM users WHERE userName='%s' AND status=1";

                    sprintf(sqltxt, fmt, imchat->m_to.c_str());

                    if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog) > 0)
                    {
                        if(1 == sqlparam.GetRecordCount())
                        {
                            int type = 0;

                            string id = sqlparam.GetStrValue(0, 0);

                            //if((type = parse_pushmsg(outbuf, imchat->m_body.c_str(), imchat->m_body.size())) > 0)
                            {
                                /*if(2 == type) outbuf += g_pubinfos["img"];
                                else if(3 == type) outbuf += g_pubinfos["audio"];
                                else if(4 == type) outbuf += g_pubinfos["video"];
                                else if(5 == type) outbuf += g_pubinfos["product"];*/

                                outbuf += g_pubinfos["message"];
                                
                                TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

                                static const char fmt[] = "SELECT count(*) FROM im_msg_chat WHERE touser='%s' AND status=0";

                                sprintf(sqltxt, fmt, imchat->m_to.c_str());

                                if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog) > 0)
                                {
                                    m_iospush->Push(id.c_str(), outbuf.c_str(), sqlparam.GetIntValue(0, 0));

                                    IM_INFO_LOG("push msg <id:%s,body:%s,outbuf:%s> ok", imchat->m_id.c_str(), imchat->m_body.c_str(), outbuf.c_str());

                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        IM_ERR_LOG("failed <id:%s,body:%s>", imchat->m_id.c_str(), imchat->m_body.c_str());
    }
}
