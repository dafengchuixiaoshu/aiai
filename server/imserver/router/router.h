#ifndef _SS_MODULE_H_
#define _SS_MODULE_H_

#include "BaseModule.h"

class TLogFile;
class TUpStream;
class TDBManager;

class TGetWebMsg
{
public:

    TGetWebMsg();

    ~TGetWebMsg();

    void OnRun();
    
    void Process();

protected:

    virtual int GetPacklen(const char* inputData,int length,int& startPos);

private:

    bool m_istart;

    TLogFile* m_log;

    TDBManager** m_odddbmgrs;

    TDBManager** m_haiyuandbmgrs;

    kernel::CTThread* m_thread;

    kernel::BaseQueue<string> m_pushmsg;
};

class TSSModule : public TBaseModule
{
    typedef map<string, TUpStream*> upstream_map;

public:

    TSSModule(){}
    
    virtual ~TSSModule(){}

    virtual void OnRun();

    virtual void ProcessTimer(TModuleMsg* pMsg);

    virtual void ProcessImMsg(TModuleMsg* pMsg);

    void Transform(string& moduleaddr, const char* sendbuf, int sendlen);

    string m_proto;

private:

    void ProcessAck(const char* addrinfo, TModuleMsg* pMsg);

    void ProcessClose(const char* addrinfo, TModuleMsg* pMsg);

    void ProcessMessage(const char* addrinfo, TModuleMsg* pMsg);

private:

    string m_moduleaddr;

    TLogFile* m_logfile;

    TDBManager** m_dbmgrs;

    TGetWebMsg m_getwebmsg;

    kernel::CTMutex m_mutex;

    upstream_map m_upstreammap;
};

#endif //_SS_MODULE_H_
