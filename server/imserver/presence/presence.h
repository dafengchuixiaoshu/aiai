#ifndef _PS_MODULE_H_
#define _PS_MODULE_H_

#include "BaseModule.h"

struct User
{
    User(){}

    User(const User& r){ copy(r); }

    User& operator=(const User& r){ if(this != &r) copy(r); return *this; }

    string _jid;

    string _addrinfo;

    string _username;

    string _resource;

    string _platform;
    
    string _prototype;

private:

    void copy(const User& r)
    {
        _jid = r._jid;

        _addrinfo = r._addrinfo;

        _username = r._username;

        _resource = r._resource;

        _platform = r._platform;

        _prototype = r._prototype;
    }
};

class TLogFile;
class TUpStream;
class TDBManager;

class TPSModule : public TBaseModule
{
    typedef map<string, User*> addr_map;

    typedef map<string, User*> resource_map;

    typedef map<string, resource_map> user_map;

    addr_map m_addrmap;

    user_map m_usermap;

    kernel::CTMutex m_mutex;

public:

    TPSModule(){}

    virtual ~TPSModule(){}

    virtual void OnInit();

    virtual void OnRun();

    virtual void ProcessTimer(TModuleMsg* pMsg);

    virtual void ProcessImMsg(TModuleMsg* pMsg);

private:

    bool IsClose(const char* addrinfo, User& user);

    void ProcessAck(const char* addrinfo, User& user, TModuleMsg* pMsg);

    void ProcessChat(const char* addrinfo, User& user, TModuleMsg* pMsg);

    void ProcessClose(const char* addrinfo, User& user, TModuleMsg* pMsg);

    void ProcessLogin(const char* addrinfo, User& user, TModuleMsg* pMsg);

    void ProcessLogout(const char* addrinfo, User& user, TModuleMsg* pMsg);

    void ProcessUpStream(const char* addrinfo, User& user, TModuleMsg* pMsg);

    void ProcessMapPoint(const char* addrinfo, User& user, TModuleMsg* pMsg);

    void ProcessDelete(const char* addrinfo, User& user, TModuleMsg* pMsg);

    void ProcessBlackList(const char* addrinfo, User& user, TModuleMsg* pMsg);

    bool ProcessLogin(const char* addrinfo, User& user, TImLogin* imlogin, int workindex);

    bool ProcessWebmsg(const char* addrinfo, User& user, TImChat* imchat, TModuleMsg* pMsg);

    bool GetSaveAndShare(TDBManager* dbmgr, const string& from, const string& to, int& issave, int& isshare);

private:

    string m_upaddr;

    string m_version;

    string m_moduleaddr;

    TLogFile* m_logfile;

    TDBManager** m_dbmgrs;

    TDBManager** m_oddmgrs;

    vector<TUpStream*> m_upstreams;
};

#endif //_PS_MODULE_H_
