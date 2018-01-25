#ifndef _PS_MODULE_H_
#define _PS_MODULE_H_

#include "BaseModule.h"

class TImChat;
class TLogFile;
class TUpStream;
class TDBManager;

class TMSModule : public TBaseModule
{
public:

    TMSModule(){}
    
    virtual ~TMSModule(){}

    virtual void OnInit();

    virtual void OnRun();
    
    virtual void ProcessTimer(TModuleMsg* pMsg);

    virtual void ProcessImMsg(TModuleMsg* pMsg);

private:

    void ProcessAck(const char* addrinfo, TModuleMsg* pMsg);

    void ProcessClose(const char* addrinfo, TModuleMsg* pMsg);

    void ProcessMessage(const char* addrinfo, TModuleMsg* pMsg);

    bool ProcessChat(TImChat* imchat, TModuleMsg* pMsg, string& touser, string gmsg = "");

    void ProcessChatAck(TImChat* imchat, TModuleMsg* pMsg, bool ischat);

    bool GetSaveAndShare(TDBManager* dbmgr, const string& from, const string& to, int& issave, int& isshare);

private:
    
    string m_upaddr;
    
    string m_downaddr;

    string m_moduleaddr;

    string m_iospushaddr;

    TLogFile* m_logfile;

    TDBManager** m_dbmgrs;

    TDBManager** m_wbbnewdbmgrs;

    vector<TUpStream*> m_upstreams;
    
    vector<TUpStream*> m_iospushstreams;
};

#endif //_PS_MODULE_H_
