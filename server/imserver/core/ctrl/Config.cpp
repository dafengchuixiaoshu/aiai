#include "Log.h"
#include "xml.h"
#include "Config.h"

#ifndef WIN32
#define stricmp strcasecmp
#endif

extern char g_exepath[256];
extern char g_exename[256];

int  dbp_decrypt(const char* key, const unsigned char* in, int in_len, unsigned char** out, int* out_len);
int  dbp_encrypt(const char* key, const unsigned char* in, int in_len, unsigned char** out, int* out_len);

TConfig::TConfig()
: m_encrypt(0)
, m_moduleid(1)
, m_workcount(1)
, m_pXmlElem(NULL)
{
}

TConfig::~TConfig()
{
    //if(m_pXmlElem) delete m_pXmlElem;
}

int TConfig::ReadConfig()
{
    char filename[128] = { 0 };

    sprintf(filename, "%s/../config/config.system", g_exepath);
    
    FILE* fp = fopen(filename, "r");

    if(NULL == fp)
    {
        sprintf(filename, "%s/../config/config.%s\0", g_exepath, g_exename);
    
        fp = fopen(filename, "r");

        if(NULL == fp)
        {
            term("can't open config"); return 1;
        }
    }
    
    char buffer[2 * 1024] = { 0 };

    fread(buffer, 1, 2 * 1024, fp);

    fclose(fp);

    try 
    {
        m_pXmlElem = ParseXML(buffer);
        
        if(NULL == m_pXmlElem)
        {
            term("parse config fialed"); return 2;
        }
    }
    catch(TXMLExcept e)
    {
        term("can't parse config: %s", e.pStr); return 3;
    }

    TXMLElem* pTemp = m_pXmlElem->GetSubElem("ENCRYPT");
    if(pTemp && pTemp->getContent()) m_encrypt = atoi(pTemp->getContent());
    term("encrypt: %d", m_encrypt);

    pTemp = m_pXmlElem->GetSubElem("MODULEID");
    if(pTemp && pTemp->getContent()) m_moduleid = atoi(pTemp->getContent());
    term("moduleid: %d", m_moduleid);

#ifdef WIN32
    int mincount = 1;
#else
    int mincount = sysconf(_SC_NPROCESSORS_CONF);
#endif;
    pTemp =  m_pXmlElem->GetSubElem("WORKCOUNT");
    if(pTemp && pTemp->getContent()) m_workcount = atoi(pTemp->getContent());
    if(m_workcount < mincount) m_workcount = mincount;
    term("work count: %d", m_workcount);

    pTemp =  m_pXmlElem->GetSubElem("MODULENAME");
    if(pTemp && pTemp->getContent())
        m_modulename = pTemp->getContent();
    else
        m_modulename = g_exename;

    term("modulename: %s", m_modulename.c_str());

    if(0 == ReadSqlInfo())
    {
        term("read sqlinfo fialed"); return 4;
    }

    if(0 != ReadSockInfo("HTTPSERVER", m_httpsockinfo))
    {
        term("can't find HTTPSERVER sock");
    }

    if(0 != ReadSockInfo("IMSERVER", m_imsockinfo))
    {
        term("can't find IMSERVER sock");
    }

    return 0;
}

#define  READSTR(pcurelem, key, value) \
{\
    TXMLElem* p = pcurelem->GetSubElem(key);\
    if(NULL == p || NULL == p->getContent() || 0 == strlen(p->getContent()))\
{\
    term("config invalid key: %s", key);\
    return 2;\
}\
    value = p->getContent();\
}

#define READINT(pcurelem, key, value) \
{\
    TXMLElem* p = pcurelem->GetSubElem(key);\
    if(NULL == p || NULL == p->getContent() || 0 == strlen(p->getContent()))\
{\
    term("config invalid key: %s", key);\
}\
    else value = atoi(p->getContent());\
}

int TConfig::ReadSqlInfo()
{
    int index = 0; TXMLElem* psubelem = m_pXmlElem->GetSubElem("MYSQL", index);

    while(NULL != psubelem && index < max_sql_count)
    {
        READSTR(psubelem, "DESC", m_sqlinfo[index].m_desc);
        READSTR(psubelem, "IP", m_sqlinfo[index].m_ip);
        READSTR(psubelem, "USER", m_sqlinfo[index].m_username);
        READSTR(psubelem, "PIN", m_sqlinfo[index].m_userpwd);
        READSTR(psubelem, "DATABASE", m_sqlinfo[index].m_dbname);
        READINT(psubelem, "PORT", m_sqlinfo[index].m_port);

        term("sqlinfo desc: %s", m_sqlinfo[index].m_desc.c_str());
        term("   IP  : %s", m_sqlinfo[index].m_ip.c_str());
        term("   USER: %s", m_sqlinfo[index].m_username.c_str());
        term("   PIN : %s", m_sqlinfo[index].m_userpwd.c_str());
        term("   DB  : %s", m_sqlinfo[index].m_dbname.c_str());
        term("   Port: %d", m_sqlinfo[index].m_port);

        ++index; psubelem = m_pXmlElem->GetSubElem("MYSQL", index);
    }
    
    return index;
}

int TConfig::ReadSockInfo(const char* sockproto, SockInfo& sockinfo)
{
    TXMLElem* psubelem = m_pXmlElem->GetSubElem(sockproto);

    if(NULL == psubelem) return 1;
    
    string type;
    READSTR(psubelem, "TYPE", type);
    READSTR(psubelem, "PROTO", sockinfo.m_proto);
    READSTR(psubelem, "LOCALIP", sockinfo.m_ip);
    READINT(psubelem, "LOCALPORT", sockinfo.m_port);

    term("sockinfo protocol: %s", sockproto);
    term("   TYPE  : %s", type.c_str());
    term("   LOCALIP  : %s", sockinfo.m_ip.c_str());
    term("   LOCALPORT: %d", sockinfo.m_port);

    return 0;
}

int TConfig::GetModuleid()
{
    return m_moduleid;
}

int TConfig::GetWorkCount()
{
    return m_workcount;
};

TXMLElem* TConfig::GetXmlElem()
{
    return m_pXmlElem;
}

const char* TConfig::GetModulename()
{
    return m_modulename.c_str();
}

const SockInfo& TConfig::GetHttpSockInfo()
{
    return m_httpsockinfo;
}

const SockInfo& TConfig::GetImSockInfo()
{
    return m_imsockinfo;
}

const SqlInfo& TConfig::GetSqlInfo(string desc)
{
    for(int index = 0; index < max_sql_count; ++index)
    {
        if(desc == m_sqlinfo[index].m_desc) return m_sqlinfo[index];
    }

    return m_sqlinfo[max_sql_count];
}
