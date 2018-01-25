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
#include "presence.h"

TControl* GetCtrl();

extern TConfig m_Config;

TPSModule* g_module = NULL;

char* Version="0.1.001";

void _System_Run()
{
    if(g_module == NULL)
    {
        g_module = new TPSModule();

        GetCtrl()->SetModule(g_module);
    }
}

#define PS_ERR_LOG(...)  LOG_ERROR(m_logfile, __VA_ARGS__)

#define PS_INFO_LOG(...)  LOG_INFO(m_logfile, __VA_ARGS__)

#define IM_INFO_LOG(...)  LOG_INFO(m_pImLog, __VA_ARGS__)

void TPSModule::OnInit()
{
    m_moduleaddr = split_to_string("%s:%d",
        m_Config.GetImSockInfo().m_ip.c_str(), m_Config.GetImSockInfo().m_port);

    m_version = m_Config.GetXmlElem()->GetSubElem("IMSERVER")->GetSubElem("VERSION")->getContent();

    const char* addr = m_Config.GetXmlElem()->GetSubElem("UPSTREAM")->GetSubElem("STREAM")->GetAttr("ADDR")->getValue();

    start_upstream(addr, m_upaddr, m_upstreams, m_Config.GetImSockInfo().m_proto);
}

void TPSModule::OnRun()
{
    m_logfile = GET_LOG(ps);

    init_dbmgr(m_dbmgrs,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,m_Config.GetWorkCount());

    init_dbmgr(m_oddmgrs,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,m_Config.GetWorkCount());

    {
        TMySQLParam sqlparam;

        char sqltxt[1024] = { 0 };

        static const char fmt[] = "UPDATE im_online_info SET status=0,uptime=now() WHERE moduleaddr='%s' AND status=1";

        sprintf(sqltxt, fmt, m_moduleaddr.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[0], &sqlparam, sqltxt, m_pDBLog))
        {
            term("update im_online_info status=0 failed");

            exit(0);
        }
    }

    {
        TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "REPLACE INTO im_module_info(name,addr,version,uptime) VALUES('xmpp','%s','%s',now())";

        sprintf(sqltxt, fmt, m_moduleaddr.c_str(), m_version.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[0], &sqlparam, sqltxt, m_pDBLog))
        {
            term("replace im_module_info failed");

            exit(0);
        }
    }
}

void TPSModule::ProcessTimer(TModuleMsg* pMsg)
{
    static int count = 0;

    if(0 == count % 60)
    {
        TMySQLParam sqlparam;

        char sqltxt[1024] = { 0 };

        static const char fmt[] = "SELECT CASE WHEN ADDDATE(uptime, interval 3 minute) > now() THEN 1 ELSE 0 END AS status FROM im_module_info WHERE addr='%s'";

        sprintf(sqltxt, fmt, m_upaddr.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, NULL))
        {
            term("select im_module_info:%s failed", m_upaddr.c_str());
        }
        else if(1 == sqlparam.GetRecordCount())
        {
            if(1 == sqlparam.GetIntValue(0, 0))
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
        }
    }

    ++count;
}

void TPSModule::ProcessImMsg(TModuleMsg* pMsg)
{
    char* addrinfo = pMsg->m_strFromIP;

    switch(pMsg->m_nSubMsgType)
    {
    case SOCK_TYPE_CONNECT:
        {
            IM_INFO_LOG("addrinfo:%s a new connection", addrinfo);

            m_mutex.Lock();

            addr_map::iterator itaddr = m_addrmap.find(addrinfo);

            if(m_addrmap.end() == itaddr)
            {
                User* user = new User();

                if(NULL == user)
                {
                    IM_INFO_LOG("addrinfo:%s user is null so close", addrinfo);

                    DoImResponse(NULL, 0, addrinfo, "XML", 1);

                    DoImResponse(NULL, 0, addrinfo, "BINARY", 1);
                }
                else
                {
                    PS_INFO_LOG("addrinfo:%s push a new user", addrinfo);

                    user->_addrinfo = addrinfo;

                    m_addrmap[addrinfo] = user;
                }
            }
            else
            {
                IM_INFO_LOG("addrinfo:%s has pushed so close", addrinfo);

                DoImResponse(NULL, 0, addrinfo, "XML", 1);

                DoImResponse(NULL, 0, addrinfo, "BINARY", 1);
            }

            m_mutex.Unlock();
        }
        break;

    case SOCK_TYPE_CLOSE:
        {
            IM_INFO_LOG("addrinfo:%s close a connection", addrinfo);

            m_mutex.Lock();

            addr_map::iterator itaddr = m_addrmap.find(addrinfo);

            if(m_addrmap.end() == itaddr)
            {
                PS_INFO_LOG("addrinfo:%s has removed", addrinfo);
            }
            else
            {
                string& username = itaddr->second->_username;

                string& resource = itaddr->second->_resource;

                user_map::iterator ituser = m_usermap.find(username);

                if(m_usermap.end() != ituser)
                {
                    resource_map& resourcemap = ituser->second;

                    resource_map::iterator itresource = resourcemap.find(resource);

                    if(resourcemap.end() != itresource)
                    {
                        TMySQLParam sqlparam;

                        char sqltxt[1024] = { 0 };

                        static const char fmt[] = "UPDATE im_online_info SET status=0,uptime=now() WHERE jid='%s%s'";

                        sprintf(sqltxt, fmt, username.c_str(), resource.c_str());

                        if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
                        {
                            PS_ERR_LOG(
                                "addrinfo:%s,jid:%s logout failed", addrinfo, itaddr->second->_jid.c_str());
                        }

                        resourcemap.erase(itresource);
                    }

                    if(resourcemap.empty())
                    {
                        m_usermap.erase(ituser);
                    }
                }

                PS_INFO_LOG(
                    "addrinfo:%s,jid:%s removed normal", addrinfo, itaddr->second->_jid.c_str());

                delete itaddr->second;

                m_addrmap.erase(itaddr);
            }

            m_mutex.Unlock();
        }
        break;

    case SOCK_TYPE_MESSAGE:
        {
            User user;

            if(IsClose(addrinfo, user))
            {
                IM_INFO_LOG("addrinfo:%s get message but socket is closed", addrinfo);
            }
            else if(user._username.empty())
            {
                if(IM_LOGIN == pMsg->m_pParam->m_msgtype)
                {
                    ProcessLogin(addrinfo, user, pMsg);
                }
                else if(IM_UPSTREAM == pMsg->m_pParam->m_msgtype)
                {
                    ProcessUpStream(addrinfo, user, pMsg);
                }
                else if(IM_KEEPALIVE == pMsg->m_pParam->m_msgtype)
                {
                    IM_INFO_LOG("addrinfo:%s keepalive and not login", addrinfo);
                }
                else if(IM_MAPPOINT == pMsg->m_pParam->m_msgtype)
                {
                    IM_INFO_LOG("addrinfo:%s mappoint and not login", addrinfo);
                }
                else if(IM_DELETE == pMsg->m_pParam->m_msgtype)
                {
                    IM_INFO_LOG("addrinfo:%s imdelete and not login", addrinfo);
                }
                else
                {
                    IM_INFO_LOG("addrinfo:%s,msgtype:%d the first message is not login", addrinfo, pMsg->m_pParam->m_msgtype);

                    DoImResponse(NULL, 0, addrinfo, "XML", 1);

                    DoImResponse(NULL, 0, addrinfo, "BINARY", 1);
                }
            }
            else
            {
                switch(pMsg->m_pParam->m_msgtype)
                {
                case IM_ACK:
                    ProcessAck(addrinfo, user, pMsg);
                    break;

                case IM_CHAT:
                    ProcessChat(addrinfo, user, pMsg);
                    break;

                case IM_CLOSE:
                    ProcessClose(addrinfo, user, pMsg);
                    break;

                case IM_LOGOUT:
                    ProcessLogout(addrinfo, user, pMsg);
                    break;

                case IM_MAPPOINT:
                    ProcessMapPoint(addrinfo, user, pMsg);
                    break;

                case IM_DELETE:
                    ProcessDelete(addrinfo, user, pMsg);
                    break;

                case IM_BLACKLIST:
                    ProcessBlackList(addrinfo, user, pMsg);
                    break;

                case IM_KEEPALIVE:
                    DoImResponse(pMsg->m_strMessage, pMsg->m_nMsgLength, addrinfo, user._prototype);
                    break;

                default:
                    PS_ERR_LOG("addrinfo:%s can not do this type:%d", addrinfo, pMsg->m_pParam->m_msgtype);
                    break;
                }
            }
        }
        break;
    }
}

bool TPSModule::IsClose(const char* addrinfo, User& user)
{
    m_mutex.Lock();

    addr_map::iterator itaddr = m_addrmap.find(addrinfo);

    if(m_addrmap.end() == itaddr)
    {
        PS_INFO_LOG("addrinfo:%s has closed", addrinfo);

        m_mutex.Unlock();

        return true;
    }
    else
    {
        user = *(itaddr->second);

        m_mutex.Unlock();

        return false;
    }
}

int base64_decode(const char *src, int src_len, char *dst);

void TPSModule::ProcessLogin(const char* addrinfo, User& user, TModuleMsg* pMsg)
{
    TImLogin* imlogin = dynamic_cast<TImLogin*>(pMsg->m_pParam);

    if(NULL == imlogin)
    {
        PS_ERR_LOG("addrinfo:%s to login pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, user._prototype, 1);

        return;
    }

    if(ProcessLogin(addrinfo, user, imlogin, pMsg->m_nThreadIndex))
    {
        TMySQLParam sqlparam; char sqltxt[2048] = { 0 };

        static const char fmt[] = "SELECT id,fromuser,touser,togroup,type,category,recvtime,body FROM im_msg_chat WHERE touser='%s' AND status=0 ORDER BY recvtime";

        sprintf(sqltxt, fmt, imlogin->m_from.c_str());

        if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog) > 0)
        {
            int recordcount = sqlparam.GetRecordCount();

            for(int index = 0; index < recordcount; ++index)
            {
                string id = sqlparam.GetStrValue(index, 0);
                string fromuser = sqlparam.GetStrValue(index, 1);
                string touser = sqlparam.GetStrValue(index, 2);
                string togroup = sqlparam.GetStrValue(index, 3);
                string type = sqlparam.GetStrValue(index, 4);
                string category = sqlparam.GetStrValue(index, 5);
                string recvtime = sqlparam.GetStrValue(index, 6);
                string body = sqlparam.GetStrValue(index, 7);

                if("iq" == type)
                {
                    int enlen = body.size();

                    char* debuf = new char[enlen];

                    int delen = base64_decode((char*)(body.c_str()), enlen, debuf);

                    DoImResponse(debuf, delen, addrinfo, imlogin->m_prototype);

                    delete debuf;
                }
                else
                {
                    int issave = 0; int isshare = 0;

                    GetSaveAndShare(m_dbmgrs[pMsg->m_nThreadIndex], fromuser, touser, issave, isshare);
                    
                    TImChat im(id, fromuser, (togroup.empty()) ? touser : togroup,
                        type, category, recvtime, body, issave, isshare, imlogin->m_prototype);

                    DoImResponse(&im, addrinfo);
                }
            }
        }
    }
}

void TPSModule::ProcessUpStream(const char* addrinfo, User& user, TModuleMsg* pMsg)
{
    TImUpStream* imupstream = dynamic_cast<TImUpStream*>(pMsg->m_pParam);

    if(NULL == imupstream)
    {
        PS_ERR_LOG("addrinfo:%s to upstream pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, user._prototype, 1);

        return;
    }

    TImLogin imlogin(imupstream->m_from, imupstream->m_domain, imupstream->m_resource, imupstream->m_platform, imupstream->m_prototype);

    ProcessLogin(addrinfo, user, &imlogin, pMsg->m_nThreadIndex);
}

bool TPSModule::ProcessLogin(const char* addrinfo, User& user, TImLogin* imlogin, int workindex)
{
    user._username = imlogin->m_from;

    user._platform = imlogin->m_platform;

    user._prototype = imlogin->m_prototype;

    user._resource = "@" + imlogin->m_domain + "/" + imlogin->m_resource;

    user._jid = user._username + user._resource;

    IM_INFO_LOG("addrinfo:%s,jid:%s,platform:%s,proto:%s login",
        addrinfo, user._jid.c_str(), user._platform.c_str(), user._prototype.c_str());

    {
        int recordcount = 0; TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "SELECT moduleaddr,prototype FROM im_online_info WHERE name='%s' AND status=1 AND moduleaddr<>'%s' GROUP BY moduleaddr";

        sprintf(sqltxt, fmt, user._username.c_str(), m_moduleaddr.c_str());

        if(0 == sync_exec_sql(m_dbmgrs[workindex], &sqlparam, sqltxt, m_pDBLog))
        {
            PS_ERR_LOG("addrinfo:%s,jid:%s check login failed", addrinfo, user._jid.c_str());

            DoImResponse(NULL, 0, addrinfo, user._prototype, 1);

            return false;
        }
        else if((recordcount = sqlparam.GetRecordCount()) > 0)
        {
            for(int index = 0; index < recordcount; ++index)
            {
                TImClose imclose(user._username, sqlparam.GetStrValue(index, 0), sqlparam.GetStrValue(index, 1));

                m_upstreams[workindex]->SendMsg(imclose);
            }
        }
    }

    {
        m_mutex.Lock();

        addr_map::iterator itaddr = m_addrmap.find(user._addrinfo);

        if(m_addrmap.end() == itaddr)
        {
            PS_INFO_LOG("addrinfo:%s has removed", addrinfo);

            m_mutex.Unlock();

            return false;
        }
        else
        {
            *(itaddr->second) = user;

            user_map::iterator ituser = m_usermap.find(user._username);

            if(m_usermap.end() == ituser)
            {
                m_usermap[user._username][user._resource] = itaddr->second;
            }
            else
            {
                resource_map& resourcemap = ituser->second;

                resource_map::iterator itresource = resourcemap.find(user._resource);

                if(resourcemap.end() != itresource)
                {
                    const char* kickoutaddrinfo = itresource->second->_addrinfo.c_str();

                    PS_INFO_LOG("addrinfo:%s,jid:%s kick out %s", addrinfo, user._jid.c_str(), kickoutaddrinfo);

                    TImKickout kickout(itresource->second->_prototype);

                    DoImResponse(&kickout, kickoutaddrinfo, 1);

                    addr_map::iterator it = m_addrmap.find(kickoutaddrinfo);

                    delete it->second;

                    m_addrmap.erase(it);
                }

                resourcemap[user._resource] = itaddr->second;
            }

            m_mutex.Unlock();
        }        
    }

    {
        TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "REPLACE INTO im_online_info(jid,name,platform,prototype,moduleaddr,status,uptime) VALUES('%s','%s','%s','%s','%s',1,now())";

        sprintf(sqltxt, fmt, user._jid.c_str(), user._username.c_str(), user._platform.c_str(), user._prototype.c_str(), m_moduleaddr.c_str());

        int status = sync_exec_sql(m_dbmgrs[workindex], &sqlparam, sqltxt, m_pDBLog);

        PS_INFO_LOG("addrinfo:%s,jid:%s login %s", addrinfo, user._jid.c_str(), (0 == status) ? "failed" : "success");

        TImLogin imlogin((0 == status) ? 1 : 0, user._prototype);

        DoImResponse(&imlogin, addrinfo, (0 == status) ? 1 : 0);

        if("linux" != user._platform)
        {
            TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

            static const char fmt[] = "INSERT INTO im_online_history(jid,name,platform,prototype,moduleaddr,status,uptime,op) VALUES('%s','%s','%s','%s','%s',1,now(),%d)";

            sprintf(sqltxt, fmt, user._jid.c_str(), user._username.c_str(), user._platform.c_str(), user._prototype.c_str(), m_moduleaddr.c_str(), status);

            sync_exec_sql(m_dbmgrs[workindex], &sqlparam, sqltxt, m_pDBLog);
        }

        if(0 == status)
            return false;
        else
            return true;
    }
}

void TPSModule::ProcessAck(const char* addrinfo, User& user, TModuleMsg* pMsg)
{
    TImAck* imack = dynamic_cast<TImAck*>(pMsg->m_pParam);

    if(NULL == imack)
    {
        PS_ERR_LOG("addrinfo:%s to ack pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, user._prototype, 1);
    }
    else if(imack->m_time.empty())
    {
        IM_INFO_LOG("addrinfo:%s,jid:%s,platform:%s,proto:%s ack to message <id='%s'>", addrinfo, 
            user._jid.c_str(), user._platform.c_str(), user._prototype.c_str(), imack->m_id.c_str());

        m_upstreams[pMsg->m_nThreadIndex]->SendMsg(pMsg->m_strMessage, pMsg->m_nMsgLength);
    }
    else
    {
        IM_INFO_LOG("addrinfo:%s,jid:%s,platform:%s,proto:%s ack to user <id='%s' time='%s'>", addrinfo,
            user._jid.c_str(), user._platform.c_str(), user._prototype.c_str(), imack->m_id.c_str(), imack->m_time.c_str());

        TImAck ack(imack->m_id, imack->m_time, user._prototype);

        m_mutex.Lock();

        user_map::iterator ituser = m_usermap.find(imack->m_name);

        if(m_usermap.end() != ituser)
        {
            resource_map& resourcemap = ituser->second;

            for(resource_map::iterator it = resourcemap.begin(); it != resourcemap.end(); ++it)
            {
                DoImResponse(&ack, it->second->_addrinfo.c_str(), 0);
            }
        }

        m_mutex.Unlock();
    }
}

void TPSModule::ProcessChat(const char* addrinfo, User& user, TModuleMsg* pMsg)
{
    TImChat* imchat = dynamic_cast<TImChat*>(pMsg->m_pParam);

    if(NULL == imchat)
    {
        PS_ERR_LOG("addrinfo:%s to chat pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, user._prototype, 1);
    }
    else if(imchat->m_addr.empty())
    {
        IM_INFO_LOG("addrinfo:%s,jid:%s,platform:%s,proto:%s send chat to message <id='%s' to='%s' from='%s' type='%s' category='%s' body='%s'>",
            addrinfo, user._jid.c_str(), user._platform.c_str(), user._prototype.c_str(),
            imchat->m_id.c_str(), imchat->m_to.c_str(), imchat->m_from.c_str(), imchat->m_type.c_str(), imchat->m_category.c_str(), imchat->m_body.c_str());

        if("customchat" == imchat->m_type)
        {
            if(ProcessWebmsg(addrinfo, user, imchat, pMsg))
            {
                TMySQLParam sqlparam; char sqltxt[2048] = { 0 };

                static const char fmt[] = "SELECT recvtime FROM message_timings WHERE msgid='%s'";

                sprintf(sqltxt, fmt, imchat->m_id.c_str());

                if(0 != sync_exec_sql(m_oddmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
                {
                    if(1 == sqlparam.GetRecordCount())
                    {
                        TImAck imack(imchat->m_id, sqlparam.GetStrValue(0, 0), imchat->m_prototype);

                        DoImResponse(&imack, addrinfo, 0);
                    }
                }
            }
        }
        else
        {
            TMySQLParam sqlparam; char sqltxt[2048] = { 0 };

            static const char fmt[] = "SELECT blackid FROM im_online_blacklist WHERE userid='%s' AND blackid='%s'";

            sprintf(sqltxt, fmt, user._username.c_str(), imchat->m_to.c_str());

            if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog) > 0)
            {
                if(1 == sqlparam.GetRecordCount())
                {
                    IM_INFO_LOG("addrinfo:%s,jid:%s,platform:%s,proto:%s send chat in blacklist <id='%s' to='%s' from='%s' type='%s' category='%s' body='%s'>",
                        addrinfo, user._jid.c_str(), user._platform.c_str(), user._prototype.c_str(),
                        imchat->m_id.c_str(), imchat->m_to.c_str(), imchat->m_from.c_str(), imchat->m_type.c_str(), imchat->m_category.c_str(), imchat->m_body.c_str());

                    TImAck imack(imchat->m_id, get_cur_time(), imchat->m_prototype);

                    DoImResponse(&imack, addrinfo, 0);

                    return;
                }
            }

            m_upstreams[pMsg->m_nThreadIndex]->SendMsg(pMsg->m_strMessage, pMsg->m_nMsgLength);                
        }
    }
    else
    {
        IM_INFO_LOG("addrinfo:%s,jid:%s,platform:%s,proto:%s recv chat <id='%s' addr='%s'>", addrinfo, 
            user._jid.c_str(), user._platform.c_str(), user._prototype.c_str(), imchat->m_id.c_str(), imchat->m_addr.c_str());

        TMySQLParam sqlparam; char sqltxt[2048] = { 0 };

        static const char fmt[] = "SELECT id,fromuser,touser,togroup,type,category,recvtime,body FROM im_msg_chat WHERE id='%s'";

        sprintf(sqltxt, fmt, imchat->m_id.c_str());

        if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog) > 0)
        {
            if(1 != sqlparam.GetRecordCount())
            {
                IM_INFO_LOG("can not find chat to user <sqltxt='%s'>", sqltxt);

                return;
            }

            string id = sqlparam.GetStrValue(0, 0);
            string fromuser = sqlparam.GetStrValue(0, 1);
            string touser = sqlparam.GetStrValue(0, 2);
            string togroup = sqlparam.GetStrValue(0, 3);
            string type = sqlparam.GetStrValue(0, 4);
            string category = sqlparam.GetStrValue(0, 5);
            string recvtime = sqlparam.GetStrValue(0, 6);
            string body = sqlparam.GetStrValue(0, 7);

            IM_INFO_LOG("get chat to user <id='%s' to='%s' from='%s' togroup='%s' type='%s' category='%s' body='%s'>",
                id.c_str(), touser.c_str(), fromuser.c_str(), togroup.c_str(), type.c_str(), category.c_str(), body.c_str());

            TString outstr;

            if("iq" == type)
            {
                int enlen = body.size();

                char* debuf = new char[enlen];

                int delen = base64_decode((char*)(body.c_str()), enlen, debuf);

                if(!outstr.Write(debuf, delen))
                {
                    delete debuf;

                    return;
                }

                delete debuf;
            }

            int issave = 0; int isshare = 0;

            GetSaveAndShare(m_dbmgrs[pMsg->m_nThreadIndex], fromuser, touser, issave, isshare);
                 
            TImChat im(id, fromuser,
                ("customchat" == type) ? (("system"==category) ? touser : togroup): ((togroup.empty()) ? touser : togroup),
                type, category, recvtime, body, issave, isshare, imchat->m_prototype);

            m_mutex.Lock();

            user_map::iterator ituser = m_usermap.find(touser);

            if(m_usermap.end() != ituser)
            {
                resource_map& resourcemap = ituser->second;

                for(resource_map::iterator it = resourcemap.begin(); it != resourcemap.end(); ++it)
                {
                    addrinfo = it->second->_addrinfo.c_str();

                    IM_INFO_LOG("addrinfo:%s,jid:%s,platform:%s,proto:%s send chat to user <id='%s' to='%s' from='%s' type='%s' category='%s' body='%s'>",
                        addrinfo, it->second->_jid.c_str(), it->second->_platform.c_str(), it->second->_prototype.c_str(),
                        id.c_str(), touser.c_str(), fromuser.c_str(), type.c_str(), category.c_str(), body.c_str());

                    if("iq" == type)
                    {
                        if(it->second->_prototype == imchat->m_prototype)
                        {
                            DoImResponse(outstr.GetBuffer(), outstr.GetSize(), addrinfo, imchat->m_prototype);
                        }
                    }
                    else
                    {
                        im.m_prototype = it->second->_prototype;

                        DoImResponse(&im, addrinfo);
                    }
                }
            }

            m_mutex.Unlock();
        }
        else
        {
            PS_ERR_LOG("send chat msg:%s to upstream again", imchat->m_id.c_str());

            m_upstreams[pMsg->m_nThreadIndex]->SendMsg(pMsg->m_strMessage, pMsg->m_nMsgLength);
        }
    }
}

void TPSModule::ProcessClose(const char* addrinfo, User& user, TModuleMsg* pMsg)
{
    TImClose* imclose = dynamic_cast<TImClose*>(pMsg->m_pParam);

    if(NULL == imclose)
    {
        PS_ERR_LOG("addrinfo:%s to close pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, user._prototype, 1);

        return;
    }

    IM_INFO_LOG("addrinfo:%s,jid:%s,platform:%s,proto:%s close <user='%s' addr='%s>",
        addrinfo, user._jid.c_str(), user._platform.c_str(), user._prototype.c_str(),
        imclose->m_user.c_str(), imclose->m_addr.c_str());

    m_mutex.Lock();

    user_map::iterator ituser = m_usermap.find(imclose->m_user);

    if(m_usermap.end() != ituser)
    {
        resource_map& resourcemap = ituser->second;

        for(resource_map::iterator it = resourcemap.begin(); it != resourcemap.end(); ++it)
        {
            addrinfo = it->second->_addrinfo.c_str();

            PS_INFO_LOG(
                "addrinfo:%s,user:%s get close so reconnect", addrinfo, it->second->_username.c_str());

            DoImResponse(NULL, 0, addrinfo, it->second->_prototype, 1);
        }
    }

    m_mutex.Unlock();
}

void TPSModule::ProcessLogout(const char* addrinfo, User& user, TModuleMsg* pMsg)
{
    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

    static const char fmt[] = "UPDATE im_online_info SET status=2,uptime=now() WHERE jid='%s'";

    sprintf(sqltxt, fmt, user._jid.c_str());

    int status = sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog);

    PS_INFO_LOG("addrinfo:%s,jid:%s logout %s", addrinfo, user._jid.c_str(), (0 == status) ? "failed" : "success");

    TImLogout imlogout((0 == status) ? 1 : 0, user._prototype);

    DoImResponse(&imlogout, addrinfo, status);

    {
        TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "INSERT INTO im_online_history(jid,name,platform,prototype,moduleaddr,status,uptime,op) VALUES('%s','%s','%s','%s','%s',2,now(),%d)";

        sprintf(sqltxt, fmt, user._jid.c_str(), user._username.c_str(), user._platform.c_str(), user._prototype.c_str(), m_moduleaddr.c_str(), status);

        sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog);
    }

    IM_INFO_LOG("addrinfo:%s,jid:%s,platform:%s,proto:%s logout",
        addrinfo, user._jid.c_str(), user._platform.c_str(), user._prototype.c_str());
}

void TPSModule::ProcessMapPoint(const char* addrinfo, User& user, TModuleMsg* pMsg)
{
    TImMapPoint* immappoint = dynamic_cast<TImMapPoint*>(pMsg->m_pParam);

    if(NULL == immappoint)
    {
        PS_ERR_LOG("addrinfo:%s to immappoint pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, user._prototype, 1);

        return;
    }

    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

    static const char fmt[] = "SELECT id FROM users WHERE userName='%s'";

    sprintf(sqltxt, fmt, user._username.c_str());

    if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog) > 0)
    {
        if(1 == sqlparam.GetRecordCount())
        {
            static const char fmt[] = "REPLACE INTO users_location(uid,longitude,latitude) VALUES(%s,%s,%s)";

            sprintf(sqltxt, fmt, sqlparam.GetStrValue(0, 0).c_str(), immappoint->m_xpoint.c_str(), immappoint->m_ypoint.c_str());

            if(sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog) > 0)
            {
                PS_INFO_LOG("addrinfo:%s,jid:%s mappoint successful[x:%s,y:%s]", addrinfo, user._jid.c_str(), immappoint->m_xpoint.c_str(), immappoint->m_ypoint.c_str());
            }
            else
            {
                PS_INFO_LOG("addrinfo:%s,jid:%s mappoint replace failed", addrinfo, user._jid.c_str());
            }
        }
        else
        {
            PS_INFO_LOG("addrinfo:%s,jid:%s mappoint find this username failed", addrinfo, user._jid.c_str());
        }
    }
    else
    {
        PS_INFO_LOG("addrinfo:%s,jid:%s mappoint get id failed", addrinfo, user._jid.c_str());
    }
}

void delete_msgkey(TDBManager* dbmgr, const string& msgid, const string& key, TLogFile* logfile)
{
    if(!key.empty())
    {
        TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "REPLACE INTO im_msg_delete(msgid,filekey,status,uptime) VALUES('%s','%s',0,now())";

        sprintf(sqltxt, fmt, msgid.c_str(), key.c_str());

        sync_exec_sql(dbmgr, &sqlparam, sqltxt, logfile);
    }
}

void TPSModule::ProcessDelete(const char* addrinfo, User& user, TModuleMsg* pMsg)
{
    TImDelete* imdelete = dynamic_cast<TImDelete*>(pMsg->m_pParam);

    if(NULL == imdelete)
    {
        PS_ERR_LOG("addrinfo:%s to imdelete pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, user._prototype, 1);

        return;
    }

    TMySQLParam sqlparam;

    static const char fmt[] = "SELECT gmsg FROM im_msg_chat WHERE id='%s'";

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, imdelete->m_msgid.c_str());

    if(0 != sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
    {
        if(1 == sqlparam.GetRecordCount())
        {
            string gmsg = sqlparam.GetStrValue(0, 0);

            if(!gmsg.empty())
            {
                TMySQLParam sqlparam;

                static const char fmt[] = "SELECT id FROM im_msg_chat WHERE gmsg='%s' AND id<>'%s'";

                char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, gmsg.c_str(), imdelete->m_msgid.c_str());

                if(0 != sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
                {
                    if(sqlparam.GetRecordCount() > 0)
                    {
                        PS_INFO_LOG("addrinfo:%s,jid:%s imdelete [%s,%s,%s] groupmsg is not the last one", addrinfo, user._jid.c_str(),
                            imdelete->m_msgid.c_str(), imdelete->m_imagekey.c_str(), imdelete->m_videokey.c_str());

                        return;
                    }
                }
            }
        }
    }

    delete_msgkey(m_dbmgrs[pMsg->m_nThreadIndex], imdelete->m_msgid, imdelete->m_imagekey, m_pDBLog);

    delete_msgkey(m_dbmgrs[pMsg->m_nThreadIndex], imdelete->m_msgid, imdelete->m_videokey, m_pDBLog);

    PS_INFO_LOG("addrinfo:%s,jid:%s imdelete [%s,%s,%s]", addrinfo, user._jid.c_str(),
        imdelete->m_msgid.c_str(), imdelete->m_imagekey.c_str(), imdelete->m_videokey.c_str());
}

void TPSModule::ProcessBlackList(const char* addrinfo, User& user, TModuleMsg* pMsg)
{
    TImBlackList* imblacklist = dynamic_cast<TImBlackList*>(pMsg->m_pParam);

    if(NULL == imblacklist)
    {
        PS_ERR_LOG("addrinfo:%s to imblacklist pointer failed", addrinfo);

        DoImResponse(NULL, 0, addrinfo, user._prototype, 1);

        return;
    }
    
    TMySQLParam sqlparam;

    char sqltxt[1024] = { 0 }; 
    
    static const char delfmt[] = "delete from im_online_blacklist where userid='%s' and blackid='%s'";

    static const char addfmt[] = "REPLACE INTO im_online_blacklist(userid,blackid,uptime) VALUES('%s','%s',now())";
    
    sprintf(sqltxt, (0 == imblacklist->m_type) ? delfmt : addfmt, user._username.c_str(), imblacklist->m_blackid.c_str());

    if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
    {
        PS_INFO_LOG("addrinfo:%s,jid:%s imblacklist failed[%s,%d]",
            addrinfo, user._jid.c_str(), imblacklist->m_blackid.c_str(), imblacklist->m_type);
    
        TImBlackList response(imblacklist->m_blackid, 0, user._prototype);

        DoImResponse(&response, addrinfo, 0);
    }
    else
    {
        PS_INFO_LOG("addrinfo:%s,jid:%s imblacklist success[%s,%d]",
            addrinfo, user._jid.c_str(), imblacklist->m_blackid.c_str(), imblacklist->m_type);

        TImBlackList response(imblacklist->m_blackid, 1, user._prototype);

        DoImResponse(&response, addrinfo, 0);
    }
}

void presence_replace_separator(string& instr, string srcdelim, string destdelim)
{
    size_t srcdelimsize = srcdelim.size();

    size_t destdelimsize = destdelim.size();

    string::size_type pos = instr.find(srcdelim);

    while(string::npos != pos)
    {
        string substr = instr.substr(pos + srcdelimsize);

        instr.erase(pos);

        instr += destdelim;

        instr += substr;

        pos = instr.find(srcdelim, pos + destdelimsize);
    }
}

string presence_replace_separator(const char* inbuf, string srcdelim, string destdelim)
{
    string instr(inbuf);

    presence_replace_separator(instr, srcdelim, destdelim);

    return instr;
}

bool TPSModule::ProcessWebmsg(const char* addrinfo, User& user, TImChat* imchat, TModuleMsg* pMsg)
{
    if(imchat->m_body.size() > 1024)
    {
        PS_ERR_LOG("webmsg addrinfo: %s msg is too length, msgid: %s", imchat->m_id.c_str());

        return false;
    }

    TMySQLParam sqlparam;

    static const char fmt[] = "SELECT nickname FROM user WHERE name='%s' AND status=1";

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, user._username.c_str());

    if(0 != sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
    {
        if(1 == sqlparam.GetRecordCount())
        {
            string fromuser = user._username;

            string nickname = sqlparam.GetStrValue(0, 0);

            if(nickname.empty()) nickname = user._username;

            string touser = imchat->m_from;

            string talkid = "pro_" + imchat->m_to;

            char* demsg = new char[imchat->m_body.size()];

            int delen = base64_decode(imchat->m_body.c_str(), imchat->m_body.size(), demsg);

            char* temp = demsg + 1;

            demsg[delen - 1] = '\0';

            static const char fmt[] = "{%s,\"date\":\"%s\",\"nick\":\"%s\",\"font\":{\"fontType\":\"&#x5B8B;&#x4F53;\",\"fontSize\":\"9pt\",\"fontWeight\":\"normal\",\"fontStyle\":\"normal\",\"textDecoration\":\"none\",\"color\":\"#000\"}}";

            string sendtemp = presence_replace_separator(temp, "\\", "\\\\");

            presence_replace_separator(sendtemp, "\"", "\\\"");

            presence_replace_separator(sendtemp, "&", "&amp;");

            presence_replace_separator(sendtemp, "<", "&lt;");

            presence_replace_separator(sendtemp, ">", "&gt;");

            presence_replace_separator(sendtemp, "'", "''");

            char body[2048] = { 0 };

            sprintf(body, fmt, sendtemp.c_str(), get_cur_time(true).c_str(), nickname.c_str());

            delete demsg;

            {
                static const char fmt[] = "INSERT INTO message_timings(fromuser,touser,msgid,recvtime,body,talkid,srcplatform) VALUES('%s','%s','%s',now(),'%s','%s','%s')";

                char sqltxt[2048] = { 0 }; sprintf(sqltxt, fmt, fromuser.c_str(), touser.c_str(), imchat->m_id.c_str(), body, talkid.c_str(), user._platform.c_str());

                TMySQLParam sqlparam;

                if(0 != sync_exec_sql(m_oddmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool TPSModule::GetSaveAndShare(TDBManager* dbmgr, const string& from, const string& to, int& issave, int& isshare)
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
