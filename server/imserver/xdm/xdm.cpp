//公共群组模块
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
#include "Log.h"
#include "DBManager.h"
#include "tcpsocket.h"
#include "httpmsg.h"
#include "xdm.h"
#include "zlibgzip.h"
#include "imsocket.h"

TControl* GetCtrl();
extern TConfig m_Config;

TPGModule* g_module = NULL;

char* Version="1.0.0.005";

void _System_Run()
{
    if(g_module == NULL)
    {
        g_module = new TPGModule();

        GetCtrl()->SetModule(g_module);
    }
}

THREAD_HANDLER_DECL(boddyupdate, arg)
{
    TBoddyUpdate* boddyupdate = (TBoddyUpdate*)arg;

    boddyupdate->Process();

    return 0;
}

TBoddyUpdate::TBoddyUpdate()
: m_istart(true)
, m_thread(NULL)
{
}

TBoddyUpdate::~TBoddyUpdate()
{
    if(m_istart)
    {
        m_istart = false; sleep_s(3);
    }

    if(NULL != m_thread) delete m_thread;
}

void TBoddyUpdate::OnRun()
{
    m_log = GET_LOG(boddy);

    if(NULL == m_log)
    {
        term("boddy log is null"); exit(0);
    }

    init_dbmgr(m_wbbnewdbmgrs,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,1);

    init_dbmgr(m_pUserDB,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,1);

    m_thread = new kernel::CTThread(boddyupdate, this);

    if(NULL == m_thread)
    {
        term("boddyupdate thread is null"); exit(0);
    }
}
/**
iq 通知
*/
int base64_encode(const char *src, int src_len, char *dst);

void boddy_mod(int workcount, const char* touser, TImFansModify& fansmodify)
{
    TString outstr;

    fansmodify.m_id = create_coreid(touser);

    if(fansmodify.ToBuffer(outstr))
    {
        int msglen = outstr.GetSize();

        char* enmsg = new char[msglen + (msglen >> 1)];

        base64_encode((char*)(outstr.GetBuffer()), msglen, enmsg);

        g_module->ProcessIQ(workcount, "boddyupdate",
            touser, fansmodify.m_id.c_str(), enmsg, "boddyupdate");

        delete enmsg;
    }
}

static void boddy_update_process(TDBManager* pUserManager, TDBManager* pImpresaManager, string& ver, bool isinfo)
{
    vector<ContactUpdate> contactupdates;

    static int workcount = m_Config.GetWorkCount();

    getcontactupdate(pUserManager, pImpresaManager, ver, contactupdates, isinfo);

    int upcount = contactupdates.size();

    for(int index = 0; index < upcount; ++index)
    {
        ContactUpdate& contactupdate = contactupdates[index];

        if(contactupdate._myselfinfo._loginame.empty())
        {
            continue;
        }
        else
        {
            TImFansModify fansmodify(contactupdate._myselfinfo._loginame,
                contactupdate._myselfinfo._nickname,
                contactupdate._myselfinfo._portraitcrc,
                contactupdate._myselfinfo._impresa,
                g_module->m_iqproto);

            boddy_mod(workcount, contactupdate._myselfinfo._loginame.c_str(), fansmodify);

            int contactcount = contactupdate._contactinfos.size();

            for(int i = 0; i < contactcount; ++i)
            {
                if(!contactupdate._contactinfos[i]._loginame.empty())
                {
                    boddy_mod(workcount, contactupdate._contactinfos[i]._loginame.c_str(), fansmodify);
                }
            }
        }
    }

    if(upcount > 0)
    {
        ver = contactupdates[0]._time;
    }
}

void TBoddyUpdate::Process()
{
    string cver; string iver;

    TDBManager* pUserDB = m_pUserDB[0];

    TDBManager* wbbnewmgr = m_wbbnewdbmgrs[0];

    while(m_istart)
    {
        sleep_s(3);

        //if(iver.empty())
        //{
        //    getsqltime(wbbnewmgr, iver);

        //    continue;
        //}

        //boddy_update_process(pUserDB, wbbnewmgr, iver, true);

        if(cver.empty())
        {
            getsqltime(pUserDB, cver);

            continue;
        }

        boddy_update_process(pUserDB, wbbnewmgr, cver, false);
    }
}

int TBoddyUpdate::GetPacklen(const char* inputData,int length,int& startPos)
{
    return length;
}

TPGModule::TPGModule()
: m_gmaxcount(40)
{
}

TPGModule::~TPGModule()
{
}

void TPGModule::OnInit()
{
    m_iqproto = m_Config.GetHttpSockInfo().m_proto;

    m_moduleaddr = split_to_string("%s:%d",
        m_Config.GetHttpSockInfo().m_ip.c_str(), m_Config.GetHttpSockInfo().m_port);

    m_gmaxcount = atoi(m_Config.GetXmlElem()->GetSubElem("GMAXCOUNT")->getContent());

    m_version = m_Config.GetXmlElem()->GetSubElem("HTTPSERVER")->GetSubElem("VERSION")->getContent();

    const char* addr = m_Config.GetXmlElem()->GetSubElem("UPSTREAM")->GetSubElem("STREAM")->GetAttr("ADDR")->getValue();

    start_upstream(addr, m_upaddr, m_upstreams, m_Config.GetImSockInfo().m_proto);
}

void TPGModule::OnRun()
{
    m_pglogfile = GET_LOG(pg);

    init_dbmgr(m_syncdbmgrs,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,m_Config.GetWorkCount()+1);

    init_dbmgr(m_pUserDB,m_Config.GetSqlInfo("aiai").m_ip,m_Config.GetSqlInfo("aiai").m_username,
        m_Config.GetSqlInfo("aiai").m_userpwd,m_Config.GetSqlInfo("aiai").m_dbname,m_Config.GetSqlInfo("aiai").m_port,m_Config.GetWorkCount()+1);

    m_boddyupdate.OnRun();

    {
        TMySQLParam sqlparam;

        static char sqltxt[] = "SELECT pubkey,pubvalue FROM im_pub_info WHERE type=3";

        if(0 == sync_exec_sql(m_pUserDB[0], &sqlparam, sqltxt, m_pDBLog))
        {
            term("get im_pub_info failed1"); sleep_s(1); exit(0);
        }

        int count = sqlparam.GetRecordCount();

        if(1 == count)
        {
            m_groupdelim = sqlparam.GetStrValue(0, 1);
        }
        else
        {
            term("get im_pub_info failed2"); sleep_s(1); exit(0);
        }
    }

    {
        TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

        static const char fmt[] = "REPLACE INTO im_module_info(name,addr,version,uptime) VALUES('xcap','%s','%s',now())";

        sprintf(sqltxt, fmt, m_moduleaddr.c_str(), m_version.c_str());

        if(0 == sync_exec_sql(m_pUserDB[0], &sqlparam, sqltxt, m_pDBLog))
        {
            term("replace im_module_info failed");

            exit(0);
        }
    }
}

void TPGModule::ProcessTimer(TModuleMsg* pMsg)
{
    static int count = 0;

    if(0 == count % 60)
    {
        TMySQLParam sqlparam;

        char sqltxt[1024] = { 0 };

        static const char fmt[] = "UPDATE im_module_info SET uptime=now() WHERE addr='%s'";

        sprintf(sqltxt, fmt, m_moduleaddr.c_str());

        if(0 == sync_exec_sql(m_pUserDB[0], &sqlparam, sqltxt, NULL))
        {
            term("update im_module_info:%s failed", m_moduleaddr.c_str());
        }
    }

    ++count;
}

void TPGModule::ProcessHttpMsg(TModuleMsg* pMsg)
{
    if(SOCK_TYPE_MESSAGE == pMsg->m_nSubMsgType)
    {
        THttpRequest httpmsg;

        int ret = httpmsg.Decode(pMsg->m_strMessage,pMsg->m_nMsgLength);

        const char* uri = httpmsg.GetURI();

        if(0 != ret || NULL == uri)
        {
            SendResponse403(pMsg, 500, "decode httpmsg failed");

            return;
        }

        TXMLElem* xmlelem = ParseXML(httpmsg.GetContentData());

        if(NULL == xmlelem)
        {
            SendResponse403(pMsg, 501, "decode xml ContentData failed");

            return;
        }

        if(NULL != strstr(uri, "/pg"))
        {
            if(0 == strcmp(xmlelem->getTag(), "info"))
            {
                ProcessPubGroup(xmlelem, pMsg);
            }
            else if(0 == strcmp(xmlelem->getTag(), "create"))
            {
                ProcessPubGroupCreate(xmlelem, pMsg);
            }
            else if(0 == strcmp(xmlelem->getTag(), "invite"))
            {
                ProcessPubGroupInvite(xmlelem, pMsg);
            }
            else if(0 == strcmp(xmlelem->getTag(), "quit"))
            {
                ProcessPubGroupQuit(xmlelem, pMsg);
            }
            else if(0 == strcmp(xmlelem->getTag(), "modify"))
            {
                ProcessPubGroupModify(xmlelem, pMsg);
            }
            else if(0 == strcmp(xmlelem->getTag(), "cardmodify"))
            {
                ProcessPubGroupCardModify(xmlelem, pMsg);
            }
            else if(0 == strcmp(xmlelem->getTag(), "get"))
            {
                ProcessPubGroupInfo(xmlelem, pMsg);
            }
            else
            {
                SendResponse403(pMsg, 504, "undefined pg operator");
            }
        }
        else if(NULL !=strstr(uri,"/offlinemsgack"))
        {
            ProcessPutOfflinemsgACK(xmlelem, pMsg);
        }
        else if(NULL !=strstr(uri,"/offlinemsg"))
        {
            ProcessGetOfflinemsg(xmlelem, pMsg);
        }
        else if(NULL != strstr(uri, "/contact"))
        {
            if(0 == strcmp(xmlelem->getTag(), "info"))
            {
                ProcessContact(xmlelem, pMsg);
            }
            else if(0 == strcmp(xmlelem->getTag(), "get"))
            {
                ProcessContactInfo(xmlelem, pMsg);
            }
            else if(0 == strcmp(xmlelem->getTag(), "app"))
            {
                ProcessContactAppend(xmlelem, pMsg);
            }
            else if(0 == strcmp(xmlelem->getTag(), "del"))
            {
                ProcessContactDelete(xmlelem, pMsg);
            }
            //search contact
            else if(0 == strcmp(xmlelem->getTag(), "search")){
                ProcessContactSearch(xmlelem, pMsg);
            }
            else
            {
                SendResponse403(pMsg, 505, "undefined contact operator");
            }
        }
        else if(NULL != strstr(uri, "/record"))
        {
            string tag = xmlelem->getTag();

            TXMLAttr* pattr = xmlelem->GetAttr("type");

            if(NULL == pattr)
            {
                SendResponse403(pMsg, 550, "parse type failed");
            }

            string type = pattr->getValue();

            pattr = xmlelem->GetAttr("username");

            if(NULL == pattr)
            {
                SendResponse403(pMsg, 551, "parse username failed");
            }

            string username = pattr->getValue();

            TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

            static TLogFile* dblogfile = GET_LOG(db);

            if("get" == tag)
            {
                static const char fmt[] = "SELECT opvalue FROM im_record_info WHERE name='%s' AND record='%s'";

                sprintf(sqltxt, fmt, username.c_str(), type.c_str());

                if(0 == sync_exec_sql(m_pUserDB[pMsg->m_nThreadIndex], &sqlparam, sqltxt, dblogfile))
                {
                    SendResponse403(pMsg, 553, "get opvalue failed");
                }
                else if(1 == sqlparam.GetRecordCount())
                {
                    char req[1024] = { 0 };

                    sprintf(req, "<get type=\"%s\" status=\"%s\"/>", type.c_str(), sqlparam.GetStrValue(0, 0).c_str());

                    SendResponse200(pMsg, req);
                }
                else
                {
                    SendResponse404(pMsg, 554, "get opvalue row failed");
                }
            }
            else if("post" == tag)
            {
                pattr = xmlelem->GetAttr("status");

                if(NULL == pattr)
                {
                    SendResponse403(pMsg, 552, "parse status failed");
                }

                string status = pattr->getValue();

                static const char fmt[] = "REPLACE INTO im_record_info(name,record,opvalue,uptime) VALUES('%s','%s',%s,now())";

                sprintf(sqltxt, fmt, username.c_str(), type.c_str(), status.c_str());

                if(0 == sync_exec_sql(m_pUserDB[pMsg->m_nThreadIndex], &sqlparam, sqltxt, dblogfile))
                {
                    SendResponse403(pMsg, 555, "update opvalue failed");
                }
                else
                {
                    SendResponse200(pMsg);
                }
            }
            else
            {
                SendResponse403(pMsg, 556, "parse tag failed");
            }
        }
        else if(NULL != strstr(uri, "/sns"))
        {
            TXMLAttr* pattr = xmlelem->GetAttr("type");

            if(NULL == pattr)
            {
                SendResponse403(pMsg, 506, "parse type failed");
            }

            string type = pattr->getValue();

            pattr = xmlelem->GetAttr("userid");

            if(NULL == pattr)
            {
                SendResponse403(pMsg, 507, "parse userid failed");
            }

            ContactInfo contactinfo;

            contactinfo._id = pattr->getValue();

            if(1 == getcontactinfo(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], "id", contactinfo, ""))
            {
                SendResponse200(pMsg);

                TString outstr;

                string msgid = create_coreid(contactinfo._id.c_str());

                TImSns imsns(msgid, type, m_iqproto);

                if(imsns.ToBuffer(outstr))
                {
                    int msglen = outstr.GetSize();

                    char* enmsg = new char[msglen + (msglen >> 1)];

                    base64_encode((char*)(outstr.GetBuffer()), msglen, enmsg);

                    ProcessIQ(pMsg->m_nThreadIndex, pMsg->m_strFromIP,
                        contactinfo._loginame.c_str(), msgid.c_str(), enmsg, "sns");

                    delete enmsg;
                }
            }
            else
            {
                SendResponse403(pMsg, 508, "can not find this user");
            }
        }
        else
        {
            SendResponse403(pMsg, 509, "undefined uri server name");
        }

        delete xmlelem;
    }
}

void TPGModule::SendResponse200(TModuleMsg* pMsg, string& respbody)
{
    SendResponse200(pMsg, respbody.c_str());
}

void TPGModule::SendResponse200(TModuleMsg* pMsg, const char* respbody)
{
    string response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: ";

    if(NULL == respbody)
    {
        response += "0\r\n\r\n";
    }
    else
    {
        response += to_string<int>(strlen(respbody)) + "\r\n\r\n" + respbody;
    }

    DoHttpResponse(response.c_str(), response.size(), pMsg->m_strFromIP, 1);
}
//compress with gzip
void TPGModule::SendResponse200(TModuleMsg* pMsg,const char* respbody,int resbody_length,int bgzip)
{
    char* enccontent = (char*)respbody;
    char* gzip_content = NULL;
    if(resbody_length > 0 && bgzip)
    {
        int   gzip_length = 0;
        gzip_content = gzip_compress((char*)respbody,resbody_length, gzip_length);
        if(gzip_content)
        {
            enccontent = gzip_content;
            resbody_length = gzip_length;
        }
    }


    char*  response = new char[1024 + resbody_length];
    strcpy(response,"HTTP/1.0 200 OK\r\n");
    strcat(response ,"Server: XCAP server2.0\r\n");


    if(gzip_content)
    {
        strcat(response,"Content-Encoding: gzip\r\n");
    }

    if(resbody_length > 0)
    {
        char buffer[128] ="";
        sprintf(buffer,"Content-Length: %d\r\n",resbody_length);
        strcat(response,buffer);

    }




    strcat(response,"Connection: close\r\n\r\n");

    int response_length = strlen(response);
    if(resbody_length > 0)
    {
        memcpy(response + response_length,enccontent,resbody_length);

        response_length += resbody_length;
        response[response_length] =0;

    }

    DoHttpResponse(response,response_length,pMsg->m_strFromIP, 1);

    if(gzip_content)
    {
        gzip_free(gzip_content);
    }
    delete [] response;
}

void TPGModule::SendResponse404(TModuleMsg* pMsg, int warnid, const char* warninfo)
{
    static const char respfmt[] = "HTTP/1.1 404 not find\r\nWarnid: %d\r\nWarninfo: %s\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";

    char response[1024] = { 0 }; sprintf(response, respfmt, warnid, warninfo);

    DoHttpResponse(response, strlen(response), pMsg->m_strFromIP, 1);
}

void TPGModule::SendResponse403(TModuleMsg* pMsg, int warnid, const char* warninfo, const char* app)
{
    static const char respfmt[] = "HTTP/1.1 403 Forbidden\r\nWarnid: %d\r\nWarning: %s\r\nWarninfo: %s\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";

    char response[1024] = { 0 }; sprintf(response, respfmt, warnid, (NULL == app) ? "" : app, warninfo);

    DoHttpResponse(response, strlen(response), pMsg->m_strFromIP, 1);
}

void TPGModule::ProcessIQ(int workindex, const char* addrinfo, const char* userid, string msgid, const char* iqbody, const char* type)
{
    TMySQLParam sqlparam;

    static const char fmt[] = "SELECT moduleaddr FROM im_online_info WHERE name='%s' AND prototype='%s' AND status=1 GROUP BY moduleaddr";

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, userid, m_iqproto.c_str());

    if(0 == sync_exec_sql(m_pUserDB[workindex], &sqlparam, sqltxt, m_pDBLog))
    {
        LOG_ERROR(m_pglogfile, "addrinfo: %s, type:%s, userid: %s get online failed, iq: %s", addrinfo, type, userid, iqbody);

        return;
    }

    bool status = sqlparam.GetRecordCount() > 0;

    if(strlen(iqbody) > 2048)
    {
        LOG_INFO(m_pglogfile, "addrinfo: %s, type:%s, userid: %s iq too length, iq: %s", addrinfo, type, userid, iqbody);
    }
    else
    {
        static const char fmt[] = "INSERT INTO im_msg_chat(id,fromuser,touser,type,category,body,status,recvtime) VALUES('%s','system','%s','iq','%s','%s',0,now())";

        char sqltxt[2048] = { 0 }; sprintf(sqltxt, fmt, msgid.c_str(), userid, type, iqbody);

        if(0 == sync_exec_sql(m_pUserDB[workindex], &sqlparam, sqltxt, m_pDBLog))
        {
            LOG_ERROR(m_pglogfile, "addrinfo: %s, type:%s, userid: %s, %s insert failed iq: %s", addrinfo, type, userid, status ? "online" : "offline", iqbody);

            return;
        }

        LOG_INFO(m_pglogfile, "addrinfo: %s, type:%s, userid: %s, %s iq: %s", addrinfo, type, userid, status ? "online" : "offline", iqbody);
    }

    if(status)
    {
        TImChat chat(msgid, sqlparam.GetStrValue(0, 0), m_iqproto);

        m_upstreams[workindex]->SendMsg(chat);
    }
}

static string get_xml_attr(TXMLElem* xmlelem, const char* key)
{
    TXMLAttr* attr = xmlelem->GetAttr(key);

    if(NULL != attr)
    {
        return attr->getValue();
    }

    return "";
}

static string get_xml_node(TXMLElem* xmlelem, const char* node, int index = 0)
{
    TXMLElem* pelem = xmlelem->GetSubElem(node, index);

    if(NULL != pelem)
    {
        return get_xml_attr(pelem, "id");
    }

    return "";
}

static bool get_xml_title(TDBManager* pUserManager, TDBManager* pImpresaManager, TXMLElem* xmlelem, ContactInfo& title, string& strtitle, string gid = "")
{
    title._loginame = get_xml_node(xmlelem, strtitle.c_str());

    if(title._loginame.empty()) return false;

    if(1 == getcontactinfo(pUserManager, pImpresaManager, "name", title, ""))
    {
        string cardname;

        char txt[1024] = { 0 };

        getmembername(pImpresaManager, cardname, gid, title._loginame);

        sprintf(txt, "<%s id=\"%s\" name=\"%s\" cardname=\"%s\" key=\"%s###%s\" info=\"%s\" cmsid=\"%s\"/>", ("owner"==strtitle) ? "member" : strtitle.c_str(),
            title._loginame.c_str(), title._nickname.c_str(), cardname.c_str(), title._portraitcrc.c_str(), title._facevideo.c_str(), title._impresa.c_str(), title._id.c_str());

        strtitle = txt;

        return true;
    }

    return false;
}

static int get_xml_members(TDBManager* pUserManager, TDBManager* pImpresaManager, TXMLElem* xmlelem, vector<ContactInfo>& members, string& strmembers)
{
    int index = 0;

    while(1)
    {
        ContactInfo member;

        member._loginame = get_xml_node(xmlelem, "member", index);

        if(member._loginame.empty())
        {
            break;
        }
        else if(1 == getcontactinfo(pUserManager, pImpresaManager, "name", member, ""))
        {
            ++index;

            members.push_back(member);

            char txt[1024] = { 0 };

            sprintf(txt, "<member id=\"%s\" name=\"%s\" key=\"%s###%s\" info=\"%s\" cmsid=\"%s\"/>",
                member._loginame.c_str(), member._nickname.c_str(), member._portraitcrc.c_str(), member._facevideo.c_str(), member._impresa.c_str(), member._id.c_str());

            strmembers += txt;
        }
        else
        {
            return -1;
        }
    }

    return index;
}

/**
获取群和成员列表包括增量更新
此处需要优化业务流程和数据库操作
*/
/*
void TPGModule::ProcessPubGroup(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
string ver;

if(getsqltime(m_syncdbmgrs[pMsg->m_nThreadIndex], ver))
{
map<string, ContactInfo> uids;

string uid = get_xml_attr(xmlelem, "id");

string respbody = "<pg ver=\"" + ver + "\"";

ver = get_xml_attr(xmlelem, "ver");

if(ver.empty())
{
respbody += " all=\"1\">";

vector<GroupInfo> groups;

if(getgrouplist(m_pUserDB[pMsg->m_nThreadIndex], groups, uid, "AND gtype=0 ORDER BY createtime") > 0)
{
respbody += "<ginfo>";

int groupcount = groups.size();

for(int index = 0; index < groupcount; ++index)
{
vector<GroupMemberInfo> members; int membercount = 0;

if((membercount = getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], members, split_to_string("gid='%s' AND status=1 ORDER BY createtime", groups[index]._gid.c_str()))) > 0)
{
char txt[1024] = { 0 };

static const char fmt[] = "<group gid=\"%s\" gname=\"%s\" gkey=\"%s\" gmaxcount=\"%d\" gcreator=\"%s\">";

sprintf(txt, fmt, groups[index]._gid.c_str(), groups[index]._gname.c_str(),
groups[index]._gportraitcrc.c_str(), groups[index]._gmaxcount,
groups[index]._gcreator.c_str());

string ginfo = txt;

for(int index = 0; index < membercount; ++index)
{                               
char txt[1024] = { 0 };

static const char fmt[] = "<member id=\"%s\" name=\"%s\"/>";

sprintf(txt, fmt, members[index]._uid.c_str(), members[index]._cardname.c_str());

ginfo += txt;

map<string, ContactInfo>::iterator it = uids.find(members[index]._uid);

if(uids.end() == it)
{
uids[members[index]._uid] = members[index]._contactinfo;
}
}

ginfo += "</group>";

respbody += ginfo;
}

respbody += "</ginfo>";
}
}
}
else
{
respbody += " all=\"0\">";

vector<GroupInfo> groups;

if(getgrouplist(m_pUserDB[pMsg->m_nThreadIndex], groups, uid, split_to_string("AND gtype=0 AND modifytime>'%s' ORDER BY createtime", ver.c_str())) > 0)
{
string ginfo;

int groupcount = groups.size();

if(groupcount > 0)
{
ginfo = "<ginfo>";
}

for(int index = 0; index < groupcount; ++index)
{
char txt[1024] = { 0 };

static const char fmt[] = "<group gid=\"%s\" gname=\"%s\" gkey=\"%s\" gmaxcount=\"%d\" gcreator=\"%s\"/>";

sprintf(txt, fmt, groups[index]._gid.c_str(), groups[index]._gname.c_str(),
groups[index]._gportraitcrc.c_str(), groups[index]._gmaxcount, groups[index]._gcreator.c_str());

ginfo += txt;
}

if(!ginfo.empty())
{
ginfo += "</ginfo>";
}

respbody += ginfo;
}

groups.clear(); string gmember;

if(getgrouplist(m_pUserDB[pMsg->m_nThreadIndex], groups, uid, split_to_string("AND gtype=0 ORDER BY createtime", ver.c_str())) > 0)
{
int groupcount = groups.size();

vector<GroupMemberInfo> members; int membercount = 0;

for(int index = 0; index < groupcount; ++index)
{
if((membercount = getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], members, split_to_string("gid='%s' AND modifytime>'%s' ORDER BY createtime", groups[index]._gid.c_str(), ver.c_str()))) > 0)
{
if(gmember.empty())
{
gmember = "<gmember>";
}

char txt[1024] = { 0 };

static const char fmt[] = "<group gid=\"%s\">";

sprintf(txt, fmt, groups[index]._gid.c_str());

gmember += txt;

for(int index = 0; index < membercount; ++index)
{                               
char txt[1024] = { 0 };

static const char fmt[] = "<member id=\"%s\" name=\"%s\" status=\"%s\"/>";

sprintf(txt, fmt, members[index]._uid.c_str(), members[index]._cardname.c_str(), members[index]._status.c_str());

gmember += txt;

map<string, ContactInfo>::iterator it = uids.find(members[index]._uid);

if(uids.end() == it)
{
uids[members[index]._uid] = members[index]._contactinfo;
}
}

gmember += "</group>";
}
}
}

if(!gmember.empty())
{
gmember += "</gmember>";
}

respbody += gmember;
}

respbody += "<uinfo>";

for(map<string, ContactInfo>::iterator it = uids.begin(); it != uids.end(); ++it)
{
char txt[1024] = { 0 };

static const char fmt[] = "<user id=\"%s\" name=\"%s\" key=\"%s\" info=\"%s\" cmsid=\"%s\"/>";

sprintf(txt, fmt, it->second._loginame.c_str(),
it->second._nickname.c_str(),
it->second._portraitcrc.c_str(),
it->second._impresa.c_str(),
it->second._id.c_str());

respbody += txt;
}

respbody += "</uinfo></pg>";

SendResponse200(pMsg, respbody);

return;
}

SendResponse403(pMsg, 510, "get pubgroup failed");
}
*/
void TPGModule::ProcessPubGroup(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    string ver;

    if(getsqltime(m_syncdbmgrs[pMsg->m_nThreadIndex], ver))
    {
        map<string, ContactInfo> uids;

        string uid = get_xml_attr(xmlelem, "id");

        string respbody = "<pg ver=\"" + ver + "\"";

        ver = get_xml_attr(xmlelem, "ver");

        if(ver.empty())
        {
            respbody += " all=\"1\">";

            vector<GroupMemberInfo> groups;

            if(getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], groups, split_to_string("uid='%s' AND status=1 ORDER BY createtime", uid.c_str())) > 0)
            {
                respbody += "<ginfo>";

                int groupcount = groups.size();

                for(int index = 0; index < groupcount; ++index)
                {
                    GroupInfo group;

                    if(1 == getgroupinfo(m_syncdbmgrs[pMsg->m_nThreadIndex], group, groups[index]._gid))
                    {
                        vector<GroupMemberInfo> members; int membercount = 0;

                        if((membercount = getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], members, split_to_string("gid='%s' AND status=1 ORDER BY createtime", group._gid.c_str()))) > 0)
                        {
                            char txt[1024] = { 0 };

                            static const char fmt[] = "<group gid=\"%s\" gname=\"%s\" gkey=\"%s\" gmaxcount=\"%d\" gcreator=\"%s\">";

                            sprintf(txt, fmt, group._gid.c_str(), group._gname.c_str(), group._gportraitcrc.c_str(), group._gmaxcount, group._gcreator.c_str());

                            string ginfo = txt;

                            for(int index = 0; index < membercount; ++index)
                            {                               
                                char txt[1024] = { 0 };

                                static const char fmt[] = "<member id=\"%s\" name=\"%s\"/>";

                                sprintf(txt, fmt, members[index]._uid.c_str(), members[index]._cardname.c_str());

                                ginfo += txt;

                                map<string, ContactInfo>::iterator it = uids.find(members[index]._uid);

                                if(uids.end() == it)
                                {
                                    uids[members[index]._uid] = members[index]._contactinfo;
                                }
                            }

                            ginfo += "</group>";

                            respbody += ginfo;
                        }
                    }
                }

                respbody += "</ginfo>";
            }
        }
        else
        {
            respbody += " all=\"0\">";

            vector<GroupMemberInfo> ingroups; string gmember;

            if(getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], ingroups, split_to_string("uid='%s' AND status=1 ORDER BY createtime", uid.c_str())) > 0)
            {
                string ginfo;

                int ingroupcount = ingroups.size();

                for(int index = 0; index < ingroupcount; ++index)
                {
                    GroupInfo group;

                    if(1 == getgroupinfo(m_syncdbmgrs[pMsg->m_nThreadIndex], group, ingroups[index]._gid, split_to_string("AND modifytime>'%s'", ver.c_str())))
                    {
                        if(ginfo.empty())
                        {
                            ginfo = "<ginfo>";
                        }

                        char txt[1024] = { 0 };

                        static const char fmt[] = "<group gid=\"%s\" gname=\"%s\" gkey=\"%s\" gmaxcount=\"%d\" gcreator=\"%s\"/>";

                        sprintf(txt, fmt, group._gid.c_str(), group._gname.c_str(), group._gportraitcrc.c_str(), group._gmaxcount, group._gcreator.c_str());

                        ginfo += txt;
                    }

                    vector<GroupMemberInfo> members; int membercount = 0;

                    if(ingroups[index]._createtime > ver)
                    {
                        membercount = getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], members, split_to_string("gid='%s' ORDER BY createtime", group._gid.c_str(), ver.c_str()));
                    }
                    else
                    {
                        membercount = getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], members, split_to_string("gid='%s' AND modifytime>'%s' ORDER BY createtime", group._gid.c_str(), ver.c_str()));
                    }

                    if(membercount > 0)
                    {
                        if(gmember.empty())
                        {
                            gmember = "<gmember>";
                        }

                        char txt[1024] = { 0 };

                        static const char fmt[] = "<group gid=\"%s\">";

                        sprintf(txt, fmt, group._gid.c_str());

                        gmember += txt;

                        for(int index = 0; index < membercount; ++index)
                        {                               
                            char txt[1024] = { 0 };

                            static const char fmt[] = "<member id=\"%s\" name=\"%s\" status=\"%s\"/>";

                            sprintf(txt, fmt, members[index]._uid.c_str(), members[index]._cardname.c_str(), members[index]._status.c_str());

                            gmember += txt;

                            map<string, ContactInfo>::iterator it = uids.find(members[index]._uid);

                            if(uids.end() == it)
                            {
                                uids[members[index]._uid] = members[index]._contactinfo;
                            }
                        }

                        gmember += "</group>";
                    }
                }

                if(!ginfo.empty())
                {
                    ginfo += "</ginfo>";
                }

                respbody += ginfo;
            }

            vector<GroupMemberInfo> outgroups;

            if(getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], outgroups, split_to_string("uid='%s' AND status=1 AND modifytime>'%s'", uid.c_str(), ver.c_str())) > 0)
            {
                int outgroupcount = outgroups.size();

                for(int index = 0; index < outgroupcount; ++index)
                {
                    if(gmember.empty())
                    {
                        gmember = "<gmember>";
                    }

                    char txt[1024] = { 0 };

                    GroupMemberInfo& members = outgroups[index];

                    static const char fmt[] = "<group gid=\"%s\"><member id=\"%s\" name=\"%s\" status=\"%s\"/></group>";

                    sprintf(txt, fmt, members._gid.c_str(), members._uid.c_str(), members._cardname.c_str(), members._status.c_str());

                    gmember += txt;
                }
            }

            if(!gmember.empty())
            {
                gmember += "</gmember>";
            }

            respbody += gmember;
        }

        respbody += "<uinfo>";

        for(map<string, ContactInfo>::iterator it = uids.begin(); it != uids.end(); ++it)
        {
            char txt[1024] = { 0 };

            static const char fmt[] = "<user id=\"%s\" name=\"%s\" key=\"%s###%s\" info=\"%s\" cmsid=\"%s\"/>";

            sprintf(txt, fmt, it->second._loginame.c_str(),
                it->second._nickname.c_str(),
                it->second._portraitcrc.c_str(),
                it->second._facevideo.c_str(),
                it->second._impresa.c_str(),
                it->second._id.c_str());

            respbody += txt;
        }

        respbody += "</uinfo></pg>";

        SendResponse200(pMsg, respbody);

        return;
    }

    SendResponse403(pMsg, 510, "get pubgroup failed");
}

//创建群
void TPGModule::ProcessPubGroupCreate(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    GroupInfo group;

    group._gmaxcount = m_gmaxcount;

    group._gportraitcrc = get_xml_attr(xmlelem, "gkey");

    ContactInfo title; string strtitle = "owner";

    if(get_xml_title(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], xmlelem, title, strtitle))
    {
        group._gcreator = title._loginame;
    }
    else
    {
        SendResponse403(pMsg, 511, "get title error");

        return;
    }

    group._gid = create_coreid(title._loginame.c_str());

    vector<ContactInfo> members; string strmember; int membercount = 0;

    if((membercount = get_xml_members(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], xmlelem, members, strmember)) <= 1)
    {
        SendResponse403(pMsg, 512, "member list size failed");

        return;
    }
    else if(membercount >= group._gmaxcount)//= for owner
    {
        SendResponse403(pMsg, 513, "max group count failed", split_to_string("%d", group._gmaxcount).c_str());

        return;
    }

    if(insertgroup(m_syncdbmgrs[pMsg->m_nThreadIndex], group))
    {
        if(insertmember(m_syncdbmgrs[pMsg->m_nThreadIndex], group._gid, title._loginame, 1))
        {
            for(int index = 0; index < membercount; ++index)
            {
                if(!insertmember(m_syncdbmgrs[pMsg->m_nThreadIndex], group._gid, members[index]._loginame, 0))
                {
                    //callback?

                    SendResponse403(pMsg, 514, "create groupuser failed");

                    return;
                }
            }

            char header[1024] = { 0 };

            string curtime = get_cur_time(true);

            static const char fmt[] = "<group type=\"create\" gid=\"%s\" gkey=\"%s\" gmaxcount=\"%d\" curtime=\"%s\">";

            sprintf(header, fmt, group._gid.c_str(), group._gportraitcrc.c_str(), group._gmaxcount, curtime.c_str());

            string respbody = header + strtitle + strmember + "</group>";

            SendResponse200(pMsg, respbody);

            for(int index = 0; index < membercount; ++index)
            {
                //char op[256] = { 0 };

                //string msgid = create_coreid(members[index]._loginame.c_str());

                //sprintf(op, "<iq type=\"pg\" msgid=\"%s\"><body>", msgid.c_str());

                //string iq = op; iq += header + strtitle + strmember + "</group></body></iq>";

                TString outstr;

                string msgid = create_coreid(members[index]._loginame.c_str());

                TImPGCreate pgcreate(msgid, group._gid, title._nickname, 0, m_iqproto);

                if(pgcreate.ToBuffer(outstr))
                {
                    int msglen = outstr.GetSize();

                    char* enmsg = new char[msglen + (msglen >> 1)];

                    base64_encode((char*)(outstr.GetBuffer()), msglen, enmsg);

                    ProcessIQ(pMsg->m_nThreadIndex, pMsg->m_strFromIP,
                        members[index]._loginame.c_str(), msgid.c_str(), enmsg, "creategroup");

                    delete enmsg;
                }
            }

            return;
        }
    }

    SendResponse403(pMsg, 515, "create group failed");
}

void TPGModule::ProcessPubGroupInvite(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    GroupInfo group;

    if(1 != getgroupinfo(m_syncdbmgrs[pMsg->m_nThreadIndex], group, get_xml_attr(xmlelem, "gid")))
    {
        SendResponse403(pMsg, 516, "can not find this group");

        return;
    }
    else
    {
        group._gportraitcrc = get_xml_attr(xmlelem, "gkey");
    }

    ContactInfo title; string strtitle = "inviter";

    if(get_xml_title(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], xmlelem, title, strtitle, group._gid))
    {
        if(!ismember(m_syncdbmgrs[pMsg->m_nThreadIndex], group._gid, title._loginame))
        {
            SendResponse403(pMsg, 517, "inviter is not in group");

            return;
        }
    }
    else
    {
        SendResponse403(pMsg, 518, "get title error");

        return;
    }

    vector<GroupMemberInfo> members; int membercount = 0;

    if((membercount = getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], members, split_to_string("gid='%s' AND status=1 ORDER BY createtime", group._gid.c_str()))) <= 0)
    {
        SendResponse403(pMsg, 519, "get member error");

        return;
    }

    vector<ContactInfo> invites; string strinvite; int invitecount = 0; string invitesname;

    if((invitecount = get_xml_members(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], xmlelem, invites, strinvite)) < 1)
    {
        SendResponse403(pMsg, 520, "invite list size failed");

        return;
    }
    else if(invitecount + membercount > group._gmaxcount)
    {
        SendResponse403(pMsg, 521, "max group count failed", split_to_string("%d", group._gmaxcount).c_str());

        return;
    }
    else
    {
        for(int index = 0; index < invitecount; ++index)
        {
            if(!invitesname.empty()) invitesname += m_groupdelim;//"、";

            invitesname += (invites[index]._nickname.empty()) ? invites[index]._loginame : invites[index]._nickname;

            if(ismember(m_syncdbmgrs[pMsg->m_nThreadIndex], group._gid, invites[index]._loginame))
            {
                SendResponse403(pMsg, 522, "member list failed");

                return;
            }
        }
    }

    if(upgroupinfo(m_syncdbmgrs[pMsg->m_nThreadIndex], group._gid, split_to_string("gkey='%s',modifytime=sysdate(6)", group._gportraitcrc.c_str())))
    {
        for(int index = 0; index < invitecount; ++index)
        {
            if(!insertmember(m_syncdbmgrs[pMsg->m_nThreadIndex], group._gid, invites[index]._loginame, 0))
            {
                //callback?

                SendResponse403(pMsg, 523, "invite groupuser failed");

                return;
            }
        }

        string strmemberlist;

        char header[1024] = { 0 };

        string curtime = get_cur_time(true);

        static const char fmt[] = "<group type=\"%s\" gid=\"%s\" gname=\"%s\" gkey=\"%s\" gmaxcount=\"%d\" curtime=\"%s\">\0";

        sprintf(header, fmt, "invite", group._gid.c_str(), group._gname.c_str(), group._gportraitcrc.c_str(), group._gmaxcount, curtime.c_str());

        string respbody = header + strtitle + strinvite + "</group>";

        SendResponse200(pMsg, respbody);

        string titlecardname;

        for(int index = 0; index < membercount; ++index)
        {
            char txt[1024] = { 0 };

            static const char fmt[] = "<member id=\"%s\" name=\"%s\" cardname=\"%s\" key=\"%s###%s\" info=\"%s\" cmsid=\"%s\"/>";

            sprintf(txt, fmt,
                members[index]._contactinfo._loginame.c_str(),
                members[index]._contactinfo._nickname.c_str(),
                members[index]._cardname.c_str(),
                members[index]._contactinfo._portraitcrc.c_str(),
                members[index]._contactinfo._facevideo.c_str(),
                members[index]._contactinfo._impresa.c_str(),
                members[index]._contactinfo._id.c_str());

            strmemberlist += txt;

            if(title._loginame == members[index]._uid)
            {
                titlecardname = members[index]._cardname;
            }
            else
            {
                TString outstr;

                string msgid = create_coreid(members[index]._uid.c_str());

                TImPGInvite pginvite(msgid, group._gid,
                    (titlecardname.empty()) ? title._nickname : titlecardname,
                    (titlecardname.empty()) ? 0 : 1,
                    invitesname,
                    m_iqproto);

                if(pginvite.ToBuffer(outstr))
                {
                    int msglen = outstr.GetSize();

                    char* enmsg = new char[msglen + (msglen >> 1)];

                    base64_encode((char*)(outstr.GetBuffer()), msglen, enmsg);

                    ProcessIQ(pMsg->m_nThreadIndex, pMsg->m_strFromIP,
                        members[index]._uid.c_str(), msgid.c_str(), enmsg, "inviteingroup");

                    delete enmsg;
                }
            }
        }

        sprintf(header, fmt, "create", group._gid.c_str(), group._gname.c_str(), group._gportraitcrc.c_str(), group._gmaxcount, curtime.c_str());

        for(int index = 0; index < invitecount; ++index)
        {
            TString outstr;

            string msgid = create_coreid(invites[index]._loginame.c_str());

            TImPGCreate pgcreate(msgid, group._gid,
                (titlecardname.empty()) ? title._nickname : titlecardname,
                (titlecardname.empty()) ? 0 : 1,
                m_iqproto);

            if(pgcreate.ToBuffer(outstr))
            {
                int msglen = outstr.GetSize();

                char* enmsg = new char[msglen + (msglen >> 1)];

                base64_encode((char*)(outstr.GetBuffer()), msglen, enmsg);

                ProcessIQ(pMsg->m_nThreadIndex, pMsg->m_strFromIP,
                    invites[index]._loginame.c_str(), msgid.c_str(), enmsg, "inviteoffgroup");

                delete enmsg;
            }
        }

        return;
    }

    SendResponse403(pMsg, 524, "invite group failed");
}

void TPGModule::ProcessPubGroupQuit(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    string gid = get_xml_attr(xmlelem, "gid");

    string uid = get_xml_attr(xmlelem, "id");

    string gkey = get_xml_attr(xmlelem, "gkey");    

    char header[1024] = { 0 };

    string curtime = get_cur_time(true);

    static const char fmt[] = "<group type=\"quit\" gid=\"%s\" id=\"%s\" gkey=\"%s\" curtime=\"%s\"/>";

    sprintf(header, fmt, gid.c_str(), uid.c_str(), gkey.c_str(), curtime.c_str());

    vector<GroupMemberInfo> title;

    //if(ismember(m_syncdbmgrs[pMsg->m_nThreadIndex], gid, uid))
    if(1 == getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex],
        title, split_to_string("gid='%s' AND uid='%s' AND status=1", gid.c_str(), uid.c_str())))
    {
        if(!deletemember(m_syncdbmgrs[pMsg->m_nThreadIndex], gid, gkey, uid))
        {
            SendResponse403(pMsg, 525, "quiter failed");

            return;
        }

        vector<GroupMemberInfo> members; int membercount = 0;

        if((membercount = getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], members, split_to_string("gid='%s' AND status=1 ORDER BY createtime", gid.c_str()))) > 0)
        {
            for(int index = 0; index < membercount; ++index)
            {
                TString outstr;

                string msgid = create_coreid(members[index]._uid.c_str());

                TImPGQuit pgquit(msgid, title[0]._uid, gid,
                    (title[0]._cardname.empty()) ? title[0]._contactinfo._nickname : title[0]._cardname,
                    (title[0]._cardname.empty()) ? 0 : 1,
                    gkey,
                    m_iqproto);

                if(pgquit.ToBuffer(outstr))
                {
                    int msglen = outstr.GetSize();

                    char* enmsg = new char[msglen + (msglen >> 1)];

                    base64_encode((char*)(outstr.GetBuffer()), msglen, enmsg);

                    ProcessIQ(pMsg->m_nThreadIndex, pMsg->m_strFromIP,
                        members[index]._uid.c_str(), msgid.c_str(), enmsg, "quitgroup");

                    delete enmsg;
                }            
            }
        }
    }

    SendResponse200(pMsg, header);
}

void TPGModule::ProcessPubGroupModify(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    string gid = get_xml_attr(xmlelem, "gid");

    string uid = get_xml_attr(xmlelem, "id");

    string gname = get_xml_attr(xmlelem, "gname");    

    vector<GroupMemberInfo> title;

    //if(ismember(m_syncdbmgrs[pMsg->m_nThreadIndex], gid, uid))
    if(1 == getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex],
        title, split_to_string("gid='%s' AND uid='%s' AND status=1", gid.c_str(), uid.c_str())))
        //if(ismember(m_syncdbmgrs[pMsg->m_nThreadIndex], gid, uid))
    {
        if(upgroupinfo(m_syncdbmgrs[pMsg->m_nThreadIndex], gid, split_to_string("gname='%s',modifytime=sysdate(6)", gname.c_str())))
        {
            char header[1024] = { 0 };

            string curtime = get_cur_time(true);

            static const char fmt[] = "<group type=\"modify\" gid=\"%s\" gname=\"%s\" id=\"%s\" curtime=\"%s\"/>";

            sprintf(header, fmt, gid.c_str(), gname.c_str(), uid.c_str(), curtime.c_str());

            SendResponse200(pMsg, header);

            vector<GroupMemberInfo> members; int membercount = 0;

            if((membercount = getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], members, split_to_string("gid='%s' AND status=1 ORDER BY createtime", gid.c_str()))) > 0)
            {
                for(int index = 0; index < membercount; ++index)
                {
                    if(members[index]._uid != uid)
                    {
                        TString outstr;

                        string msgid = create_coreid(members[index]._uid.c_str());

                        TImPGModify pgmodify(msgid, gid,
                            (title[0]._cardname.empty()) ? title[0]._contactinfo._nickname : title[0]._cardname,
                            (title[0]._cardname.empty()) ? 0 : 1,
                            gname,
                            m_iqproto);

                        if(pgmodify.ToBuffer(outstr))
                        {
                            int msglen = outstr.GetSize();

                            char* enmsg = new char[msglen + (msglen >> 1)];

                            base64_encode((char*)(outstr.GetBuffer()), msglen, enmsg);

                            ProcessIQ(pMsg->m_nThreadIndex, pMsg->m_strFromIP,
                                members[index]._uid.c_str(), msgid.c_str(), enmsg, "groupmodify");

                            delete enmsg;
                        }            
                    }
                }

                return;
            }
        }
    }

    SendResponse403(pMsg, 526, "modify group failed");
}

//修改群名片即群成员昵称
void TPGModule::ProcessPubGroupCardModify(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    string gid = get_xml_attr(xmlelem, "gid");

    string uid = get_xml_attr(xmlelem, "id");

    string cardname= get_xml_attr(xmlelem, "cardname");

    if(ismember(m_syncdbmgrs[pMsg->m_nThreadIndex], gid, uid))
    {
        if(upmemberinfo(m_syncdbmgrs[pMsg->m_nThreadIndex], gid, uid, split_to_string("cardname='%s',modifytime=sysdate(6)", cardname.c_str())))
        {
            char header[1024] = { 0 };

            string curtime = get_cur_time(true);

            static const char fmt[] = "<group type=\"cardmodify\" gid=\"%s\" id=\"%s\" cardname=\"%s\" curtime=\"%s\"/>";

            sprintf(header, fmt, gid.c_str(), uid.c_str(), cardname.c_str(), curtime.c_str());

            SendResponse200(pMsg, header);

            vector<GroupMemberInfo> members; int membercount = 0;

            if((membercount = getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], members, split_to_string("gid='%s' AND status=1 ORDER BY createtime", gid.c_str()))) > 0)
            {
                for(int index = 0 ;index < membercount; ++index)
                {
                    if(members[index]._uid != uid)
                    {
                        TString outstr;

                        string msgid = create_coreid(members[index]._uid.c_str());

                        TImPGCard pgcard(msgid, gid, uid, cardname, m_iqproto);

                        if(pgcard.ToBuffer(outstr))
                        {
                            int msglen = outstr.GetSize();

                            char* enmsg = new char[msglen + (msglen >> 1)];

                            base64_encode((char*)(outstr.GetBuffer()), msglen, enmsg);

                            ProcessIQ(pMsg->m_nThreadIndex, pMsg->m_strFromIP,
                                members[index]._uid.c_str(), msgid.c_str(), enmsg, "cardmodify");

                            delete enmsg;
                        }            
                    }
                }

                return;
            }
        }
    }

    SendResponse403(pMsg, 527, "modify cardname failed");
}
/**
获取群组uri对应的群信息，即根据群ID获取群信息，单个群
*/
void TPGModule::ProcessPubGroupInfo(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    string uid = get_xml_attr(xmlelem, "id");

    string gid = get_xml_attr(xmlelem, "gid");

    if(ismember(m_syncdbmgrs[pMsg->m_nThreadIndex], gid, uid))
    {
        GroupInfo group;

        if(1 == getgroupinfo(m_syncdbmgrs[pMsg->m_nThreadIndex], group, gid))
        {
            char txt[1024] = { 0 }; string response;

            static const char fmt[] = "<group gid=\"%s\" gname=\"%s\" gkey=\"%s\" gmaxcount=\"%d\" gcreator=\"%s\">";

            sprintf(txt, fmt, group._gid.c_str(), group._gname.c_str(), group._gportraitcrc.c_str(), group._gmaxcount, group._gcreator.c_str());

            response = txt;

            vector<GroupMemberInfo> members; int membercount = 0;

            if((membercount = getmemberlist(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], members, split_to_string("gid='%s' AND status=1 ORDER BY createtime", gid.c_str()))) > 0)
            {
                for(int index = 0; index < membercount; ++index)
                {
                    char txt[1024] = { 0 }; 

                    static const char fmt[] = "<member id=\"%s\" name=\"%s\" key=\"%s###%s\" info=\"%s\" cmsid=\"%s\" cardname=\"%s\"/>";

                    sprintf(txt, fmt, members[index]._uid.c_str(),
                        members[index]._contactinfo._nickname.c_str(),
                        members[index]._contactinfo._portraitcrc.c_str(),
                        members[index]._contactinfo._facevideo.c_str(),
                        members[index]._contactinfo._impresa.c_str(),
                        members[index]._contactinfo._id.c_str(),
                        members[index]._cardname.c_str());

                    response += txt;
                }

                response += "</group>";

                SendResponse200(pMsg, response);

                return;
            }
        }
    }

    SendResponse403(pMsg, 528, "get groupinfo failed");
}

/**
获取联系人，全量和增量方式
*/

static void get_uship_list(string& respbody, TDBManager* pUserManager, TDBManager* pImpresaManager, string uid, string ver, string type, bool isbuddy)
{
    vector<ContactInfo> ushiplist; int ushipcount = 0;

    if((ushipcount = getcontactlist(pUserManager, pImpresaManager, uid, ver, ushiplist, "uship", isbuddy)) > 0)
    {
        if(isbuddy)
            respbody += "<uship>";
        else
            respbody += "<fship>";

        for(int index = 0; index < ushipcount; ++index)
        {
            ContactInfo& contactinfo = ushiplist[index];

            if(contactinfo._loginame.empty())
            {
                continue;
            }
            else
            {
                char txt[1024] = { 0 };

                static const char fmt[] = "<user id=\"%s\" name=\"%s\" key=\"%s###%s\" info=\"%s\" cmsid=\"%s\" op=\"%d\" save=\"%s\" share=\"%s\" subname=\"%s\"/>";

                sprintf(txt, fmt, contactinfo._loginame.c_str(),
                    contactinfo._nickname.c_str(),
                    contactinfo._portraitcrc.c_str(),
                    contactinfo._facevideo.c_str(),
                    contactinfo._impresa.c_str(),
                    contactinfo._id.c_str(),
                    contactinfo._optype,
                    contactinfo._issave.c_str(),
                    contactinfo._isshare.c_str(),
                    contactinfo._subname.c_str());

                respbody += txt;
            }
        }

        if(isbuddy)
            respbody += "</uship>";
        else
            respbody += "</fship>";
    }
}

static void get_uinfo_list(string& respbody, TDBManager* pUserManager, TDBManager* pImpresaManager, string uid, string ver, string type, bool isbuddy)
{
    vector<ContactInfo> uinfolist; int uinfocount = 0;

    if((uinfocount = getcontactlist(pUserManager, pImpresaManager, uid, ver, uinfolist, "uinfo", isbuddy)) > 0)
    {
        if(isbuddy)
            respbody += "<uinfo>";
        else
            respbody += "<finfo>";

        for(int index = 0; index < uinfocount; ++index)
        {
            ContactInfo& contactinfo = uinfolist[index];

            if(contactinfo._loginame.empty())
            {
                continue;
            }
            else
            {
                char txt[1024] = { 0 };

                static const char fmt[] = "<user id=\"%s\" name=\"%s\" key=\"%s###%s\" info=\"%s\" cmsid=\"%s\" save=\"%s\" share=\"%s\" subname=\"%s\"/>";

                sprintf(txt, fmt, contactinfo._loginame.c_str(),
                    contactinfo._nickname.c_str(),
                    contactinfo._portraitcrc.c_str(),
                    contactinfo._facevideo.c_str(),
                    contactinfo._impresa.c_str(),
                    contactinfo._id.c_str(),
                    contactinfo._issave.c_str(),
                    contactinfo._isshare.c_str(),
                    contactinfo._subname.c_str());

                respbody += txt;
            }
        }

        if(isbuddy)
            respbody += "</uinfo>";
        else
            respbody += "</finfo>";
    }
}

void TPGModule::ProcessContact(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    string ver;

    if(getsqltime(m_syncdbmgrs[pMsg->m_nThreadIndex], ver))
    {
        string uid = get_xml_attr(xmlelem, "id");

        string respbody = "<contact ver=\"" + ver + "\"";

        ver = get_xml_attr(xmlelem, "ver");

        respbody += (ver.empty()) ? " all=\"1\">" : " all=\"0\">";

        if(!ver.empty())
        {
            get_uship_list(respbody, m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], uid, ver, "uship", true);

            get_uship_list(respbody, m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], uid, ver, "uship", false);
        }

        get_uinfo_list(respbody, m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], uid, ver, "uinfo", true);

        get_uinfo_list(respbody, m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], uid, ver, "uinfo", false);

        respbody += "</contact>";

        SendResponse200(pMsg, respbody);

        return;
    }

    SendResponse403(pMsg, 529, "get contact failed");
}

/**
获取联系人
*/
void TPGModule::ProcessContactInfo(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    ContactInfo contactinfo;
    
    ContactInfo frominfo; ContactInfo toinfo;

    toinfo._loginame = get_xml_attr(xmlelem, "to");

    frominfo._loginame = get_xml_attr(xmlelem, "from");

    contactinfo._loginame = get_xml_attr(xmlelem, "id");

    ContactInfo* pinfo = NULL;

    bool isfind = !contactinfo._loginame.empty() && 1 == getcontactinfo(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], "name", contactinfo, "");
    
    if(isfind)
    {
        pinfo = &contactinfo;
    }
    else
    {
        if(!frominfo._loginame.empty() && 1 == getcontactinfo(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], "name", frominfo, ""))
        {
            pinfo = &toinfo;

            isfind = !toinfo._loginame.empty() && 1 == getcontactinfo(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], "name", toinfo, frominfo._id);
        }
    }

    if(isfind)
    {
        char txt[1024 * 2] = { 0 };

        static const char fmt[] = "<info id=\"%s\" name=\"%s\" key=\"%s###%s\" info=\"%s\" cmsid=\"%s\" save=\"%s\" share=\"%s\" subname=\"%s\"/>";

        sprintf(txt, fmt, pinfo->_loginame.c_str(), pinfo->_nickname.c_str(),
            pinfo->_portraitcrc.c_str(), pinfo->_facevideo.c_str(), pinfo->_impresa.c_str(),
            pinfo->_id.c_str(), pinfo->_issave.c_str(), pinfo->_isshare.c_str(), pinfo->_subname.c_str());

        SendResponse200(pMsg, txt);

        return;
    }

    SendResponse403(pMsg, 530, "get contact info failed");
}

/**
添加联系人，状态通知
*/
void TPGModule::ProcessContactAppend(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    ContactInfo frominfo; ContactInfo toinfo;

    string type = get_xml_attr(xmlelem, "type");

    toinfo._loginame = get_xml_attr(xmlelem, "to");

    frominfo._loginame = get_xml_attr(xmlelem, "from");

    if(1 == getcontactinfo(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], "name", frominfo, ""))
    {
        if(1 == getcontactinfo(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], "name", toinfo, ""))
        {
            if(appendcontact(m_syncdbmgrs[pMsg->m_nThreadIndex], frominfo, toinfo) > 0)
            {
                char txt[2048] = { 0 };

                string curtime = get_cur_time(true);

                static char req[] = "<app type=\"%s\" id=\"%s\" name=\"%s\" key=\"%s###%s\" info=\"%s\" cmsid=\"%s\" curtime=\"%s\"/>\0";

                sprintf(txt, req, type.c_str(), toinfo._loginame.c_str(), toinfo._nickname.c_str(), toinfo._portraitcrc.c_str(), toinfo._facevideo.c_str(), toinfo._impresa.c_str(), toinfo._id.c_str(), curtime.c_str());

                SendResponse200(pMsg, txt);

                TString outstr;

                string msgid = create_coreid(toinfo._loginame.c_str());

                TImFansAppend imfansappend(msgid, type, frominfo._loginame, frominfo._nickname, frominfo._portraitcrc, frominfo._impresa, frominfo._id, m_iqproto);

                if(imfansappend.ToBuffer(outstr))
                {
                    int msglen = outstr.GetSize();

                    char* enmsg = new char[msglen + (msglen >> 1)];

                    base64_encode((char*)(outstr.GetBuffer()), msglen, enmsg);

                    ProcessIQ(pMsg->m_nThreadIndex, pMsg->m_strFromIP,
                        toinfo._loginame.c_str(), msgid.c_str(), enmsg, "contactappend");

                    delete enmsg;
                }

                return;
            }
        }
    }

    SendResponse403(pMsg, 531, "contact append failed");
}

/**
删除联系人通知操作
*/
void TPGModule::ProcessContactDelete(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    ContactInfo frominfo; ContactInfo toinfo;

    toinfo._loginame = get_xml_attr(xmlelem, "to");

    frominfo._loginame = get_xml_attr(xmlelem, "from");

    if(1 == getcontactinfo(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], "name", frominfo, ""))
    {
        if(1 == getcontactinfo(m_pUserDB[pMsg->m_nThreadIndex], m_syncdbmgrs[pMsg->m_nThreadIndex], "name", toinfo, ""))
        {
            if(deletecontact(m_syncdbmgrs[pMsg->m_nThreadIndex], frominfo, toinfo) > 0)
            {
                char txt[2048] = { 0 };

                string curtime = get_cur_time(true);

                static char req[] = "<del id=\"%s\" curtime=\"%s\"/>\0";

                sprintf(txt, req, toinfo._loginame.c_str(), curtime.c_str());

                SendResponse200(pMsg, txt);

                TString outstr;

                string msgid = create_coreid(frominfo._loginame.c_str());

                TImFansDelete imfansdelete(msgid, frominfo._loginame, m_iqproto);

                if(imfansdelete.ToBuffer(outstr))
                {
                    int msglen = outstr.GetSize();

                    char* enmsg = new char[msglen + (msglen >> 1)];

                    base64_encode((char*)(outstr.GetBuffer()), msglen, enmsg);

                    ProcessIQ(pMsg->m_nThreadIndex, pMsg->m_strFromIP,
                        toinfo._loginame.c_str(), msgid.c_str(), enmsg, "contactdelete");

                    delete enmsg;
                }

                return;
            }
        }
    }

    SendResponse403(pMsg, 532, "contact del failed");
}

void TPGModule::ProcessGetOfflinemsg(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    string uid  = get_xml_attr(xmlelem, "uid");
    vector<OfflineMSG> offmsgs;
    getofflinemsg(m_pUserDB[pMsg->m_nThreadIndex],offmsgs ,uid);
    if(!offmsgs.empty())
    {
        string body ="";
        body.append("<offlinemsg>\r\n");    
        body.append("<list>\r\n");
        vector<OfflineMSG>::iterator iter;
        for(iter = offmsgs.begin(); iter != offmsgs.end(); iter++)
        {
            string msg1 = "",msg2 = "";
            msg1=(*iter)._msg;
            int pos =msg1.find("</message>");
            msg2= msg1.substr(0,pos);
            msg2+="<recvtime>"+(*iter)._recvtime;
            msg2.append("</recvtime></message>\r\n");
            body.append(msg2);

        }
        body.append("</list>\r\n");
        body.append("</offlinemsg>\r\n");
        SendResponse200(pMsg, body.c_str());
    }
    else
        //SendResponse403(pMsg, "Not offline message!");
        SendResponse200(pMsg, "");
}

bool TPGModule::GetSaveAndShare(TDBManager* dbmgr, const string& from, const string& to, int& issave, int& isshare)
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

void TPGModule::ProcessPutOfflinemsgACK(TXMLElem* xmlelem, TModuleMsg* pMsg)
{
    string uid = get_xml_attr(xmlelem, "uid"); 
    //string sqlwheres ="WHERE status=0  AND type IN('chat', 'groupchat') AND touser = ";
    //sqlwheres.append("'"+uid);
    //sqlwheres.append("'");
    //sqlwheres += " AND id in(";
    string sqlwheres = "id in(";
    string msgigin ="";
    int index = 0; TXMLElem* psubelem = xmlelem->GetSubElem("list");
    while(1){
        TXMLElem* pmessage = psubelem->GetSubElem("message",index);
        if(pmessage==NULL)
            break;
        string msgid  = get_xml_attr(pmessage, "msgid");


        {
            TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

            static const char fmt[] = "SELECT fromuser,touser FROM im_msg_chat WHERE id='%s'";

            sprintf(sqltxt, fmt, msgid.c_str());

            if(0 == sync_exec_sql(m_pUserDB[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog))
            {
                LOG_INFO(m_pglogfile, "id:%s ack getmsg failed", msgid.c_str());

                ++index; continue;
            }

            string fromuser = sqlparam.GetStrValue(0, 0);
            string touser = sqlparam.GetStrValue(0, 1);

            int issave = 0; int isshare = 0;

            GetSaveAndShare(m_pUserDB[pMsg->m_nThreadIndex], fromuser, touser, issave, isshare);

            if(1 == issave || 1 == isshare)
            {
                LOG_INFO(m_pglogfile, "id:%s offline msg is save or share", msgid.c_str());

                TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

                static const char fmt[] = "UPDATE im_msg_chat SET status=1 WHERE id='%s'";

                sprintf(sqltxt, fmt, msgid.c_str());

                sync_exec_sql(m_pUserDB[pMsg->m_nThreadIndex], &sqlparam, sqltxt, m_pDBLog);

                ++index; continue;
            }            
        }

        msgigin.append("'"+msgid);
        msgigin.append("',");
        ++index;
    }

    string msg1 = "",msg2 = "";
    msg1=msgigin;
    int pos =msg1.find_last_of(",");
    msg2= msg1.substr(0,pos);
    sqlwheres.append(msg2+")");

    if(sqlwheres.empty())
        SendResponse200(pMsg, "");
    else if(1==getofflinemsg(m_pUserDB[pMsg->m_nThreadIndex],sqlwheres))
        SendResponse200(pMsg, "");
    else
        SendResponse403(pMsg, 533, "offline message read status error ,please try agent!");

}

/**
search contact or user
*/    
void TPGModule::ProcessContactSearch(TXMLElem* xmlelem, TModuleMsg* pMsg){
    ContactInfo cinfo;
    string name =get_xml_attr(xmlelem, "key");
    vector<ContactInfo> contactlist; int contactcount = 0;

    SearchContact(m_pUserDB[pMsg->m_nThreadIndex],m_syncdbmgrs[pMsg->m_nThreadIndex],name,contactlist);
    if(!contactlist.empty())
    {
        string body ="";
        body ="<searchcontact>\r\n";    
        body +="<list>\r\n";

        vector<ContactInfo>::iterator iter;
        for(iter = contactlist.begin(); iter != contactlist.end(); iter++)
        {
            string str="";
            str +="<contact id=";
            str +="\"";
            str +=(*iter)._id+"\"";
            str +=" name=";
            str +="\"";
            str +=(*iter)._loginame+"\"";
            str +="  nickname=""";
            str +="\"";
            str +=(*iter)._nickname+"\"";
            str +="  portraitcrc=""";
            str +="\"";
            str +=(*iter)._portraitcrc+"\"";
            str +=" impresa=""";
            str +="\"";
            str +=(*iter)._impresa+"\"";
            str +="  email=""";
            str +="\"";
            str +=(*iter)._email+"\"";
            str +="  mobile=""";
            str +="\"";
            str +=(*iter)._mobile+"\"";
            str +="/>\r\n";
            body.append(str);

        }
        body +="</list>\r\n";
        body +="</searchcontact>\r\n";
        SendResponse200(pMsg, body.c_str());

        //SendResponse200(pMsg,body.c_str(),body.length() ,1);
    }
    else
        SendResponse403(pMsg, 534, "Not fond user!");
    // SendResponse200(pMsg, "");

}
