#ifndef _IOSPUSH_MODULE_H_
#define _IOSPUSH_MODULE_H_

#include "tcpclient.h"
#include "BaseModule.h"

class TLogFile;
class TDBManager;

class TIosPush : public TTcpClient
{
    int m_port;

    string m_ip;

    string m_profile;

    TLogFile* m_log;

    kernel::CTThread* m_thread;

    kernel::BaseQueue<string> m_iospushmsg;

public:

    TIosPush(char* ip, int port, string profile);

    virtual ~TIosPush();

    void OnRun();

    void Process();

    void Push(const char* f2cid, const char* body, int count);

protected:

    virtual int GetPacklen(const char* inputData,int length,int& startPos){ return length; }
};

class TPushModule : public TBaseModule
{
public:

    TPushModule(){}

    virtual ~TPushModule(){}

    virtual void OnInit();

    virtual void OnRun();

    virtual void ProcessTimer(TModuleMsg* pMsg);

    virtual void ProcessImMsg(TModuleMsg* pMsg);

private:

    void ProcessMessage(const char* addrinfo, TModuleMsg* pMsg);

private:

    TIosPush* m_iospush;

    string m_moduleaddr;

    TLogFile* m_logfile;

    TDBManager** m_dbmgrs;
};

#endif //_IOSPUSH_MODULE_H_
