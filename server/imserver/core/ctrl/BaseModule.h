#ifndef _BASE_MODULE_H
#define _BASE_MODULE_H

#include <map>
using std::map;
#include <vector>
using std::vector;
#include <string>
using std::string;

#define _MESSAGE_TIMER  1
#define _MESSAGE_HTTP   2
#define _MESSAGE_IM     3

#include "Log.h"
#include "BaseQueue.h"
#include "baseparam.h"
#include "ResourceBase.h"

class TMySQLParam;
class THttpResponse;

int rand_base(int size = 0);

class TModuleMsg
{
public:	
    int   m_nThreadIndex;

    int   m_nMsgType;
    int	  m_nSubMsgType;

    char* m_strMessage;
    int   m_nMsgLength;

    char  m_strFromIP[64];
    int   m_nFromPort;

    TBaseParam  *m_pParam;
    TMySQLParam* m_pSqlCommand;

    TModuleMsg();
    ~TModuleMsg();
};

typedef kernel::BaseQueue<TModuleMsg*>  TMessageQueue;

struct TThreadParam
{
    int threadIndex;
    void* pModule;
};

class TBaseModule
{
public:

    TBaseModule();
    virtual ~TBaseModule();

    virtual void OnInit(){}
    virtual void OnRun(){}

    virtual void ProcessTimer(TModuleMsg* pMsg){}
    virtual void ProcessHttpMsg(TModuleMsg* pMsg){}
    virtual void ProcessImMsg(TModuleMsg* pMsg){}//process xmpp msg add by chixueli 2014-04-28
    
    void UpMsgThread(int threadIndex);

    int StartThread();
    int GetWorkCount();
    int PutMsgToQueue(TModuleMsg* pMsg, int index = 0);

    void WriteLog(TLogFile* plog,char* message,char* addrInfo,int subType,int bSend);
    void DoHttpResponse(const char* message,int length,const char* addrinfo,int isclose=0);
    void DoImResponse(TBaseParam* param,const char* addrinfo,int isclose=0,int index=0);
    void DoImResponse(const char* message,int length,const char* addrinfo,string prototype="XML",int isclose=0,int index=0);

    TLogFile* GetDBLog() {return m_pDBLog;}
    TLogFile* GetHttpLog(){return m_pHttpLog;} 
    TLogFile* GetXmppLog(){return m_pImLog;} 

protected:

    int    m_bCanRun;

    TLogFile*     m_pDBLog;
    TLogFile*     m_pHttpLog;
    TLogFile*     m_pImLog;

    TMessageQueue** m_MsgQueue;
    kernel::CTThread** m_pThread;
};

#endif
