#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef WIN32
#include <afx.h>
#include <time.h>
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#endif

#include "Config.h"
#include "Control.h"
#include "BaseModule.h"
#include "Log.h"
#include "DBManager.h"
#include "httpserver.h"
#include "imsocket.h"

TControl* GetCtrl();
extern TConfig m_Config;


THREAD_HANDLER_DECL(_UpMsgThread, lpParam)
{
    TThreadParam* pThreadParam=(TThreadParam*)lpParam; 
    TBaseModule* pModule = (TBaseModule*)pThreadParam->pModule;
    int index = pThreadParam->threadIndex;
    delete pThreadParam;

    pModule->UpMsgThread(index);
    return 0;
}

int rand_base(int size)
{
    if(size <= 0) return 0;

    srand((unsigned)time(NULL));

    if(0 == size)
    {
#ifdef WIN32
        return rand();
#else
        return random();
#endif
    }
    else
    {
#ifdef WIN32
        int index = rand() % size;
#else
        int index = random() % size;
#endif

        return index;
    }
}

TModuleMsg::TModuleMsg()
{
    m_strMessage = NULL;
    m_pSqlCommand = NULL;
    m_pParam = NULL;
    m_nThreadIndex = -1;
}

TModuleMsg::~TModuleMsg()
{
    if(m_pParam) delete m_pParam;

    if(m_strMessage) delete m_strMessage;

    if(m_pSqlCommand) delete m_pSqlCommand;
}

TBaseModule::TBaseModule()
{
    m_bCanRun = 0;
    m_pDBLog = NULL;
    m_pHttpLog = NULL;
    m_pImLog = NULL;
    m_MsgQueue = NULL;
}

TBaseModule::~TBaseModule()
{
    if(0 != m_bCanRun)
    {
        m_bCanRun = 0; sleep(1);

        if(m_pThread)
        {
            for(int i = 0; i < GetWorkCount(); ++i) delete m_pThread[i];

            delete [] m_pThread;
        }
    }

    if(m_MsgQueue)
    {
        for(int i = 0; i < GetWorkCount(); ++i) delete m_MsgQueue[i];

        delete [] m_MsgQueue;
    } 

    if(m_pDBLog) delete m_pDBLog;
    if(m_pHttpLog) delete m_pHttpLog;
    if(m_pImLog) delete m_pImLog;
}

void TBaseModule::UpMsgThread(int threadIndex)
{
    TModuleMsg* pMsg;

    while(m_bCanRun)
    {
        if(0 == m_MsgQueue[threadIndex]->TryGet(pMsg))
        {
            if(pMsg->m_nMsgType == _MESSAGE_TIMER)
            {
                ProcessTimer(pMsg);
            }
            else if(pMsg->m_nMsgType == _MESSAGE_HTTP)
            {
                WriteLog(m_pHttpLog,pMsg->m_strMessage,pMsg->m_strFromIP,pMsg->m_nSubMsgType,0);
                ProcessHttpMsg(pMsg);
            }
            else if(pMsg->m_nMsgType == _MESSAGE_IM)
            {
                ProcessImMsg(pMsg);
            }

            delete pMsg;
        }    
    }
}

int TBaseModule::StartThread()
{
    m_pDBLog = GET_LOG(db);
    m_pHttpLog = GET_LOG(http);
    m_pImLog = GET_LOG(im);

    if(NULL == m_pDBLog || NULL == m_pHttpLog || NULL == m_pImLog)
    {
        printf("create db,http,im log failed\n"); exit(0);
    }

    m_bCanRun = 1;
    m_pThread = new kernel::CTThread*[GetWorkCount()];
    m_MsgQueue = new TMessageQueue*[GetWorkCount()];

    for(int i=0; i < GetWorkCount(); ++i)
    {
        m_MsgQueue[i] =new TMessageQueue();
        TThreadParam* pThreadParam = new TThreadParam;
        pThreadParam->threadIndex = i;
        pThreadParam->pModule = this;
        m_pThread[i]= new kernel::CTThread( _UpMsgThread, pThreadParam );
        term("Start thread %d success",i+1);
    }
    return 0;
}

int TBaseModule::GetWorkCount()
{
    static const int workcount = m_Config.GetWorkCount();

    return workcount;
}

int TBaseModule::PutMsgToQueue(TModuleMsg* pMsg, int index)
{
    static const int workcount = GetWorkCount();
    
    if(0 == index)
    {
        static int index = 0; ++index; if(index < 0) index = 0;

        pMsg->m_nThreadIndex = index % workcount;
    }
    else
    {
        pMsg->m_nThreadIndex = index % workcount;
    }

    if(m_MsgQueue[pMsg->m_nThreadIndex]->TryPut(pMsg) == -1)
    {
        delete pMsg; return -3;
    }

    return 0;
}

void TBaseModule::WriteLog(TLogFile* plog,char* message,char* addrInfo,int subType,int bSend)
{
    if(plog && message)	
    {
        if(bSend)
        {
            LOG_INFO(plog, "Send message[%s]:[%s]",addrInfo,(NULL==message)?"":message);
        }
        else
        {
            char* logmsg = "unkown type";

            switch(subType)
            {
            case SOCK_TYPE_MESSAGE:
                logmsg = "Receive message";
                LOG_INFO(plog, "%s[%s]:[%s]",logmsg,addrInfo,(NULL==message)?"":message);
                break;
            case SOCK_TYPE_CONNECT:
                logmsg = "Connect message";
                LOG_INFO(plog, "%s[%s]",logmsg,addrInfo);
                break;
            case SOCK_TYPE_CLOSE:
                logmsg = "Close message";
                LOG_INFO(plog, "%s[%s]",logmsg,addrInfo);
                break;
            case SOCK_TYPE_SENDFAIL:
                logmsg = "Send failed";
                LOG_ERROR(plog, "%s[%s]:[%s]",logmsg,addrInfo,(NULL==message)?"":message);
                break;
            }
        }
    }
}

void TBaseModule::DoHttpResponse(const char* message,int length,const char* addrinfo,int isclose)
{
    WriteLog(m_pHttpLog,(char*)message,(char*)addrinfo,0,1);

    static THttpServer* pServer = GetCtrl()->GetHttpServer();

    pServer->SendMsg((char*)addrinfo,(char*)message,length,isclose);  
}

void TBaseModule::DoImResponse(TBaseParam* param,const char* addrinfo,int isclose,int index)
{
    TString sendstring;

    if(NULL != param)
    {
        if(param->ToBuffer(sendstring))
        {
            DoImResponse(sendstring.GetBuffer(),sendstring.GetSize(),addrinfo,param->m_prototype,isclose,index);
        }
    }
}

void TBaseModule::DoImResponse(const char* message,int length,const char* addrinfo,string prototype,int isclose,int index)
{
    if("XML" == prototype)
    {
        static TXmppServer* xmppserver = GetCtrl()->GetXmppServer();

        if(NULL != xmppserver)
        {
            xmppserver->SendMsg((char*)addrinfo,(char*)message,length,isclose,index);
        }
    }
    else if("BYTE" == prototype)
    {
        TBinaryServer* binaryserver = GetCtrl()->GetBinaryServer();
        
        if(NULL != binaryserver)
        {
            binaryserver->SendMsg((char*)addrinfo,(char*)message,length,isclose,index);
        }
    }
}