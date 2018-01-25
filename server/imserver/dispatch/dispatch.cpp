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
#include "xml.h"
#include "DBManager.h"
#include "tcpsocket.h"
#include "dispatch.h"
#include "httpmsg.h"

TControl* GetCtrl();

extern TConfig m_Config;

TGDModule* g_module = NULL;

char* Version="0.1.001";

void _System_Run()
{
    if(g_module == NULL)
    {
        g_module = new TGDModule();

        GetCtrl()->SetModule(g_module);
    }
}

void TGDModule::OnRun()
{
    m_logfile = GET_LOG(ms);

    init_dbmgr(m_dbmgrs, m_Config.GetSqlInfo("aiai").m_ip,
        m_Config.GetSqlInfo("aiai").m_username, m_Config.GetSqlInfo("aiai").m_userpwd,
        m_Config.GetSqlInfo("aiai").m_dbname, m_Config.GetSqlInfo("aiai").m_port,
        m_Config.GetWorkCount());

    term("run gd module success");
}

void TGDModule::ProcessTimer(TModuleMsg* pMsg)
{
    static int count = 0;

    if(0 == count % 180 || m_poolmap.empty())
    {
        string pubinfo;

        GetPubInfo(pubinfo, pMsg->m_nThreadIndex);

        pool_map poolmap;

        GetModules(poolmap, pMsg->m_nThreadIndex);

        m_mutex.Lock();

        m_pubinfo = pubinfo;

        m_poolmap = poolmap;

        term("==== start print poolmap ====");

        for(pool_map::iterator itpool = m_poolmap.begin(); itpool != m_poolmap.end(); ++itpool)
        {
            const string& version = itpool->first;

            module_map& modulemap = itpool->second;

            for(module_map::iterator itmodule = modulemap.begin(); itmodule != modulemap.end(); ++itmodule)
            {
                const string& name = itmodule->first;

                const module_info& moduleinfo = itmodule->second;

                size_t size = moduleinfo.size();

                if(0 == size)
                {
                    term("version:%s,name:%s is empty", version.c_str(), name.c_str());

                    continue;
                }

                for(size_t index = 0; index < size; ++index)
                {
                    term("version:%s,name:%s,addr%s", version.c_str(), name.c_str(), moduleinfo[index].c_str());
                }
            }
        }

        term("**** end print poolmap ****");

        m_mutex.Unlock();
    }

    ++count;
}

void TGDModule::GetPubInfo(string& pubinfo, const int& workindex)
{
    TMySQLParam sqlparam;

    static char sqltxt[1024] = "SELECT pubkey,pubvalue FROM im_pub_info WHERE type=1";

    if(sync_exec_sql(m_dbmgrs[workindex], &sqlparam, sqltxt, NULL) > 0)
    {
        pubinfo = "<pubinfo>";

        int rowcount = sqlparam.GetRecordCount();

        for(int index = 0; index < rowcount; ++index)
        {
            char txt[128] = { 0 };

            static const char fmt[] = "<%s>%s</%s>";

            sprintf(txt, fmt, sqlparam.GetStrValue(index, 0).c_str(),
                sqlparam.GetStrValue(index, 1).c_str(), sqlparam.GetStrValue(index, 0).c_str());

            pubinfo += txt;
        }

        pubinfo += "</pubinfo>";
    }
}

void TGDModule::GetModules(pool_map& poolmap, const int& workindex)
{
    TMySQLParam sqlparam;

    static const char sqltxt[] = "SELECT version,CASE WHEN adddate(uptime, interval 3 minute) > now() THEN 1 ELSE 0 END AS status,addr,name FROM im_module_info";

    if(sync_exec_sql(m_dbmgrs[workindex], &sqlparam, sqltxt, NULL) > 0)
    {
        int rowcount = sqlparam.GetRecordCount();

        for(int index = 0; index < rowcount; ++index)
        {
            string version = sqlparam.GetStrValue(index, 0);

            if(version.empty())
            {
                continue;
            }
            else if(1 == sqlparam.GetIntValue(index, 1))//status
            {
                string addr = sqlparam.GetStrValue(index, 2);
                string name = sqlparam.GetStrValue(index, 3);

                if(!addr.empty())
                {
                    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

                    static const char fmt[] = "SELECT externaladdr FROM im_addr_info WHERE internaladdr='%s'";

                    sprintf(sqltxt, fmt, addr.c_str());

                    if(sync_exec_sql(m_dbmgrs[workindex], &sqlparam, sqltxt, NULL) > 0)
                    {
                        if(1 == sqlparam.GetRecordCount())
                        {
                            addr = sqlparam.GetStrValue(0, 0);
                        }
                        else
                        {
                            addr.clear();
                        }
                    }
                }

                if(name.empty() || addr.empty())
                {
                    if(0 == poolmap.count(version))
                    {
                        static module_map modulemap;
                        
                        poolmap[version] = modulemap;
                    }
                }
                else
                {
                    char info[128] = { 0 };

                    static const char fmt[] = "<module name=\"%s\" addr=\"%s\"/>";

                    sprintf(info, fmt, name.c_str(), addr.c_str());

                    poolmap[version][name].push_back(info);
                }
            }
        }
    }
}

static void get_pool_info(string& poolinfo, int hash, const module_map& modulemap)
{
    //xmpp,xcap
    if(2 == modulemap.size())
    {
        poolinfo = "<poolinfo>";

        for(module_map::const_iterator it = modulemap.begin(); it != modulemap.end(); ++it)
        {
            size_t size = it->second.size();

            if(0 == size)
            {
                poolinfo.clear();

                return;
            }

            poolinfo += it->second[hash % size];
        }

        poolinfo += "</poolinfo>";
    }
}

bool TGDModule::GetPoolInfo(string& poolinfo, int hash, string& version)
{
    m_mutex.Lock();

    if(m_poolmap.empty())
    {
        m_mutex.Unlock();

        return false;
    }

    pool_map::const_iterator it = m_poolmap.find(version);

    if(m_poolmap.end() != it)
    {
        size_t size = it->second.size();

        if(size > 0)
        {
            get_pool_info(poolinfo, hash, it->second);

            m_mutex.Unlock();

            return !poolinfo.empty();
        }
        else
        {
            m_mutex.Unlock();

            return false;
        }
    }

    it = m_poolmap.begin();

    if(it->first > version)
    {
        m_mutex.Unlock();

        return false;
    }

    it = m_poolmap.end();

    --it;

    if(it->first < version)
    {
        size_t size = it->second.size();

        if(size > 0)
        {
            get_pool_info(poolinfo, hash, it->second);

            m_mutex.Unlock();

            return !poolinfo.empty();
        }
        else
        {
            m_mutex.Unlock();

            return false;
        }
    }

    for(pool_map::const_iterator it = m_poolmap.begin(); it != m_poolmap.end(); ++it)
    {
        if(it->first > version)
        {
            --it;

            size_t size = it->second.size();

            if(size > 0)
            {
                get_pool_info(poolinfo, hash, it->second);

                m_mutex.Unlock();

                return !poolinfo.empty();
            }
            else
            {
                m_mutex.Unlock();

                return false;
            }
        }
    }

    return false;
}

#define GET_USER_INFO(s) {TXMLAttr* attr = xmlelem->GetAttr(#s); if(NULL != attr) s = attr->getValue();}

void TGDModule::ProcessHttpMsg(TModuleMsg* pMsg)
{
    if(SOCK_TYPE_MESSAGE == pMsg->m_nSubMsgType)
    {
        THttpRequest httpmsg;

        if(0 == httpmsg.Decode(pMsg->m_strMessage,pMsg->m_nMsgLength))
        {
            char* content;

            if(NULL != (content = httpmsg.GetContentData()))
            {
                TXMLElem* xmlelem;

                if(NULL != (xmlelem = ParseXML(content)))
                {
                    string ver; string userid;

                    GET_USER_INFO(ver); GET_USER_INFO(userid);

                    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

                    static const char fmt[] = "SELECT id FROM users WHERE userName='%s' AND status=1";

                    sprintf(sqltxt, fmt, userid.c_str());

                    if(0 == sync_exec_sql(m_dbmgrs[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
                    {
                        LOG_ERROR(m_logfile, "name:%s get db.user id failed", userid.c_str());
                    }
                    else
                    {
                        if(sqlparam.GetRecordCount() > 0)
                        {
                            string poolinfo;

                            string id = sqlparam.GetStrValue(0, 0);

                            if(!m_pubinfo.empty() && GetPoolInfo(poolinfo, atoi(id.c_str()), ver))
                            {
                                m_mutex.Lock();

                                string gd = "<gd>";

                                gd += poolinfo; gd += m_pubinfo;

                                m_mutex.Unlock();

                                gd += "</gd>";

                                string response = "HTTP/1.1 200 OK\r\nContent-Length: ";

                                response += to_string<size_t>(gd.size()) + "\r\n\r\n" + gd;

                                DoHttpResponse(response.c_str(), response.size(), pMsg->m_strFromIP, 1);

                                delete xmlelem;

                                return;
                            }
                        }
                    }

                    delete xmlelem;
                }
            }
        }

        static const char notfound[] = "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";

        static const int notfoundsize = strlen(notfound);

        DoHttpResponse(notfound, notfoundsize, pMsg->m_strFromIP, 1);
    }
}
