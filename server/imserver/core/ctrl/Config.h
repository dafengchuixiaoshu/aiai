#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
using std::string;

struct SockInfo
{
    SockInfo():m_port(0){}

    int  m_port;
    string m_ip;
    string m_proto;
};

struct SqlInfo
{
    SqlInfo():m_port(0){}

    int m_port;
    string m_ip;
    string m_desc;
    string m_dbname;
    string m_userpwd;
    string m_username;
};

class TXMLElem;

class TConfig
{
public:

    TConfig();
    ~TConfig();

    int ReadConfig();

    int ReadSqlInfo();

    int ReadSockInfo(const char* sockproto, SockInfo& sockinfo);

    int GetModuleid();

    int GetWorkCount();

    TXMLElem* GetXmlElem();

    const char* GetModulename();

    const SockInfo& GetHttpSockInfo();

    const SockInfo& GetImSockInfo();

    const SqlInfo& GetSqlInfo(string desc);

private:

    int m_encrypt;

    int m_moduleid;

    int m_workcount;

    TXMLElem* m_pXmlElem;

    string m_modulename;

    static const int max_sql_count = 10;

    SqlInfo m_sqlinfo[max_sql_count + 1];

    SockInfo m_httpsockinfo;

    SockInfo m_imsockinfo;
};

#endif //_CONFIG_H_
