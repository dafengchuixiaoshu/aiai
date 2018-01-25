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

#include "Log.h"
#include "tcpserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

THREAD_HANDLER_DECL(_Tcp_ListenThread,p)
{
    TTcpServer* pServer=(TTcpServer*)p;
    pServer->ListenThread();
    return 0;
}

TTcpServer::TTcpServer(int localport,TTcpSocket_Callback* pCallback,char* localip)
{
    m_LocalPort = localport;
    if(localip)
    {
        strcpy(m_LocalIP,localip);
    }
    else
    {
        m_LocalIP[0] =0;
    }
    
    m_ListenFD = 0;
    m_bListenRun = 0;
    
    m_pListenThread = NULL; 
    m_NoneBlock = 1;

    SetSockCallback(pCallback);
}

TTcpServer::~TTcpServer()
{
    m_bListenRun = 0;

    sleep(1);

    if(	m_pListenThread)
    {
        for(int index = 0; index < m_ListenThreadCount; ++index)
        {
            delete m_pListenThread[index];
        }

        delete m_pListenThread;
    }

    if(m_ListenFD)
    {
        closesocket(m_ListenFD);
    }
}

void TTcpServer::UseNoneBlock(int bUsed)
{
    m_NoneBlock = bUsed;
}

void  TTcpServer::SendMsg(char* addrinfo,char* msg,int length,int isclose,int index)
{
    TSocketMsg* pInfo = new TSocketMsg;
    pInfo->msgType = MSG_SERVER_RESPONSE;
    if(length>0)pInfo->msginfo.Append(msg, length);
    strcpy(pInfo->addrinfo,addrinfo);
    pInfo->closeflag = isclose;
    PutSendMsgToQueue(pInfo, index);
}

int TTcpServer::OpenServer()
{
    struct sockaddr_in my_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    
    if((m_ListenFD = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        term("Create socker error");
        return -1;
    }
    //if(m_NoneBlock)
    //{
    //    setNonBlock(m_ListenFD);
    //}

    setReUseAddr(m_ListenFD);
    setSendBufSize(m_ListenFD);
    setRecBufSize(m_ListenFD);
    setRecSendTimeOver(m_ListenFD);

    int keepAliveFlag=1;
    int keepAliveSize=sizeof keepAliveFlag;
    setsockopt(m_ListenFD,SOL_SOCKET,SO_KEEPALIVE,(char*)&keepAliveFlag,keepAliveSize);

    my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(m_LocalPort);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_ListenFD, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) 
    {
        term("Bind error  - %d",m_LocalPort);
        return -1; 
    }
    
    if (listen(m_ListenFD, 1024) == -1)
    {
        term("Listen error");
        return -1;
    }
    return 0;
}

int TTcpServer::Run()
{
    if(OpenServer() != 0)
        return -1;
    
    TTcpSocket::Run();

    m_bListenRun = 1;
    m_ListenThreadCount = m_readThreadCount>>1;
    if(m_ListenThreadCount < 1) m_ListenThreadCount = 1;
    m_mutex = new kernel::CTMutex();
    m_pListenThread = new kernel::CTThread*[m_ListenThreadCount];
    for(int index = 0; index < m_ListenThreadCount; ++index)
    {
        m_pListenThread[index]=new kernel::CTThread(_Tcp_ListenThread, this );
    }
    
    return 0;
}

void TTcpServer::ListenThread()
{
    sockaddr_in from;
    socklen_t   from_len=sizeof(sockaddr_in);
    //fd_set ready;

    //struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 10000;

    while(m_bListenRun)
    {
        //FD_ZERO(&ready); FD_SET(m_ListenFD, &ready);
        //
        //if(select(m_ListenFD + 1,&ready,NULL,NULL,&tv) >0)
        {
        //    if(FD_ISSET(m_ListenFD, &ready)>0)
            {
                m_mutex->Lock();
                int  fd = accept(m_ListenFD,(struct sockaddr *)&from,&from_len);
                m_mutex->Unlock();

                if(fd > 0)
                {
                    setRecSendTimeOver(fd);
                    
                    int keepAliveFlag=1;
                    int keepAliveSize=sizeof keepAliveFlag;
                    setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,(char*)&keepAliveFlag,keepAliveSize);

                    if(m_NoneBlock)
                    {
                        setNonBlock(fd);
                    }
                    char* fromIp = inet_ntoa(from.sin_addr);
                    unsigned int fromPort = ntohs(from.sin_port);
                    if (NULL != fromIp)
                    {
                        OnAccept(fd,fromIp,fromPort);
                    }                    
                }
            }
        }
    }
}
   
void  TTcpServer::OnAccept(int fd,char* fromip,int fromport)
{
    AddSocket(fd,fromip,fromport);
}
