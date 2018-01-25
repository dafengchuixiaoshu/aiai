#ifdef WIN32
#include <winsock2.h>
#endif

#include "xml.h"
#include "Config.h"
#include "Control.h"
#include "DBManager.h"
#include "BaseModule.h"
#include "httpserver.h"
#include "imsocket.h"

TConfig  m_Config;

TControl* GetCtrl();

extern CCheckSyncMysql g_checkSyncMysql;

THREAD_HANDLER_DECL(_CheckSyncMysqlRun, lpParam);

void HttpServer_CallBack(TSocketMsg* pInfo,size_t startPos,size_t packlen,int msgtype,TBaseParam* param)
{
    TModuleMsg*  pMsg = new TModuleMsg();
    pMsg->m_nMsgType = _MESSAGE_HTTP;
    pMsg->m_strMessage = new char[packlen+1];
    memcpy(pMsg->m_strMessage,pInfo->msginfo.Str()+startPos,packlen);
    pMsg->m_strMessage[packlen] = 0;
    pMsg->m_nMsgLength = packlen;
    pMsg->m_nSubMsgType = msgtype;
    strcpy(pMsg->m_strFromIP,pInfo->addrinfo);
    GetCtrl()->GetModule()->PutMsgToQueue(pMsg, atoi(pInfo->addrinfo));
}

void XmppServer_CallBack(TSocketMsg* pInfo,size_t startPos,size_t packlen,int msgtype,TBaseParam* param)
{
    TModuleMsg*  pMsg = new TModuleMsg();
    pMsg->m_nMsgType = _MESSAGE_IM;
    pMsg->m_strMessage = new char[packlen+1];
    memcpy(pMsg->m_strMessage,pInfo->msginfo.Str()+startPos,packlen);
    pMsg->m_strMessage[packlen] = 0;
    pMsg->m_nMsgLength = packlen;
    pMsg->m_nSubMsgType = msgtype;
    strcpy(pMsg->m_strFromIP,pInfo->addrinfo);
    pMsg->m_pParam = param;
    GetCtrl()->GetModule()->PutMsgToQueue(pMsg, atoi(pInfo->addrinfo));
}

TControl::TControl()
: m_pModule(NULL)
, m_pHttpServer(NULL)
, m_pXmppServer(NULL)
, m_pBinaryServer(NULL)
{
#ifdef WIN32
    WSAData    wsaData;
    if(NO_ERROR != WSAStartup(MAKEWORD(2,2), &wsaData))
    {
        printf("WSAStartup failed\n"); getchar();
    }
#endif
}

TControl::~TControl()
{
    if(NULL != m_pModule) delete m_pModule;

    if(NULL != m_pHttpServer) delete m_pHttpServer;

    if(NULL != m_pXmppServer) delete m_pXmppServer;
    
    if(NULL != m_pBinaryServer) delete m_pBinaryServer;

#ifdef WIN32
    WSACleanup();
#endif
}

int TControl::InitModule()
{
    if(0 != m_Config.ReadConfig())
    {
        term("readconfig failed"); exit(0);
    }

    int threadCount = m_Config.GetWorkCount();

    term("module init");

    if(NULL != m_pModule)
    {
        m_pModule->OnInit();        

        if(0 != m_pModule->StartThread())
        {
            term("start main thread fail"); exit(0);
        }

        m_pModule->OnRun();

        TModuleMsg*  pMsg = new TModuleMsg();

        pMsg->m_nMsgType = _MESSAGE_TIMER;

        m_pModule->PutMsgToQueue(pMsg);

        sleep(1);
    }

    if(m_Config.GetHttpSockInfo().m_port > 0)
    {    
        m_pHttpServer = new THttpServer(m_Config.GetHttpSockInfo().m_port,HttpServer_CallBack,NULL);

        m_pHttpServer->SetThreadCount(threadCount,(threadCount / 2) + 1);

        if(0 != m_pHttpServer->Run())
        {
            term("start http server fail"); exit(0);
        }

        term("start http server success");
    }

    if(m_Config.GetImSockInfo().m_port > 0)
    {
        if("XML" == m_Config.GetImSockInfo().m_proto)
        {
            m_pXmppServer = new TXmppServer(m_Config.GetImSockInfo().m_port,XmppServer_CallBack);

            m_pXmppServer->SetThreadCount(threadCount,(threadCount / 2) + 1); 

            if(0 != m_pXmppServer->Run())
            {
                term("start xmpp server fail"); exit(0);
            }

            term("start xmpp server success");  	
        }
        else if("BYTE" == m_Config.GetImSockInfo().m_proto)
        {
            m_pBinaryServer = new TBinaryServer(m_Config.GetImSockInfo().m_port,XmppServer_CallBack);

            m_pBinaryServer->SetThreadCount(threadCount,(threadCount / 2) + 1); 

            if(0 != m_pBinaryServer->Run())
            {
                term("start byte server fail"); exit(0);
            }

            term("start byte server success");  	
        }
        else
        {
            term("start im server failed");  	
        }
    }

    g_checkSyncMysql.m_pThread = new kernel::CTThread( _CheckSyncMysqlRun,NULL);

    for(;;)
    {
        TModuleMsg*  pMsg = new TModuleMsg();

        pMsg->m_nMsgType = _MESSAGE_TIMER;

        if(m_pModule)
        {
            m_pModule->PutMsgToQueue(pMsg);
        }

        sleep(1);

    }
    return 0;
}

void  TControl::SetModule(TBaseModule* pModule)
{
    m_pModule = pModule;
}

TBaseModule* TControl::GetModule()
{
    return m_pModule;
}

THttpServer*  TControl::GetHttpServer()
{
    return m_pHttpServer;
}

TXmppServer*  TControl::GetXmppServer()
{
    return m_pXmppServer;
}

TBinaryServer*  TControl::GetBinaryServer()
{
    return m_pBinaryServer;
}
