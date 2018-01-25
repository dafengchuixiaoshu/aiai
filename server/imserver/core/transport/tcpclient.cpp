#ifdef WIN32
#include <afx.h>
#include <Winsock2.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "tcpclient.h"
#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

THREAD_HANDLER_DECL(_Tcp_ReconnectThread,arg)
{
    TTcpClient* pClient = (TTcpClient*)arg;
    pClient->ReconnectThread();
    return 0;
}

TTcpClient::TTcpClient(char* serverIP,int serverPort,TTcpSocket_Callback* pCallback)
{
    m_bKeepAlive = 0;
    m_socketMsgType = MSG_CLIENT_KEEPCLOSE;
    strcpy(m_serverIP,serverIP);
    m_serverPort = serverPort;
    m_pReconnectThread = NULL;
    m_NoneBlock = 1;
    SetSockCallback(pCallback);
}

TTcpClient::~TTcpClient()
{
    if(m_pReconnectThread)
    {
        delete m_pReconnectThread ;
        m_pReconnectThread = NULL;
    }
}

void TTcpClient::UseNoneBlock(int bUsed)
{
    m_NoneBlock = bUsed;
}

void TTcpClient::SetKeepAlive(int bKeepAlive)
{
    m_bKeepAlive = bKeepAlive;    
}

int TTcpClient::Run()
{
    if(m_bKeepAlive)
    {
        SetThreadCount(1,1);
        m_socketMsgType = MSG_CLIENT_KEEPALIVE;

        int fd = ConnectToServer(m_serverIP,m_serverPort);
        if(fd > 0)
        {
            if(m_NoneBlock)
            {
                setNonBlock(fd);
            }
            TTcpSocket::Run();
            AddSocket(fd,m_serverIP,m_serverPort);

            m_pReconnectThread = new kernel::CTThread(_Tcp_ReconnectThread,this);
            return 0;
        }
        else
        {
            TTcpSocket::Run();
            m_pReconnectThread = new kernel::CTThread(_Tcp_ReconnectThread,this);
            return 1;
        }
    }
    
    TTcpSocket::Run();
    return 0;
}

void TTcpClient::ReconnectThread()
{
    while(m_bThreadRun)
    {
        sleep_s(1);

        if(GetConnectionSize() != 0) continue;

        int fd = ConnectToServer(m_serverIP,m_serverPort);
        if(fd > 0)
        {
            if(m_NoneBlock)
            {
                setNonBlock(fd);
            }
            AddSocket(fd,m_serverIP,m_serverPort);
        }
    }
}

void TTcpClient::SendMsg(char* msg,int length)
{
    SendMsg(msg,length,m_serverIP,m_serverPort);
}

void TTcpClient::SendMsg(char* msg,int length,char* toip,int toport)
{
    TSocketMsg* pInfo = new TSocketMsg;
    pInfo->msgType = ((m_bKeepAlive)?MSG_CLIENT_KEEPALIVE:MSG_CLIENT_KEEPCLOSE);
    pInfo->msginfo.Append(msg, length);
    strcpy(pInfo->addrinfo,toip);
    pInfo->port = toport;
    PutSendMsgToQueue(pInfo); 
}
