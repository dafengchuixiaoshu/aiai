#ifdef WIN32
#include <afx.h>
#include <winsock2.h>
#include <process.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include "Log.h"
#include "udpserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef WIN32
#define close closesocket
#define socklen_t int
#endif
#define  MESSAGE_LEN  10 * 1024

THREAD_HANDLER_DECL(_ReceiveFunc, lpParam )
{
    TUdpServer* pServer=(TUdpServer*)lpParam;
    pServer->ReceiveFunc();
    return 0;
}

TUdpServer::TUdpServer(TUdpServerReport_CallBack* pReportFunc)
{
    m_pThread = NULL;
    m_SocketFD =0;
    m_bCanRun = 0;
    m_pReportFunc = pReportFunc;
}

TUdpServer::~TUdpServer()
{
    StopServer();
}

int TUdpServer::OpenServer(char* ip,int port)
{
    int  fd =  socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    
    if(ip == NULL)
        addr.sin_addr.s_addr = INADDR_ANY;
    else
        addr.sin_addr.s_addr = inet_addr(ip);
    
    addr.sin_port = htons(port);
    
    int  recvSize=256 * 1024;
    setsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char*)&recvSize,4);
    int  sendSize=256 * 1024;
    setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(char*)&sendSize,4);

    int res = bind (fd, (struct sockaddr *) &addr, sizeof (addr));
    if(res != 0)
    {
        close(fd);
        return 0;
    }
    
    
    return fd;
}

int TUdpServer::StartServer(char* serverip,int serverport)
{
    m_SocketFD=OpenServer(serverip,serverport);
    if(m_SocketFD == 0)
    {
        term("Open sip socket failure,ip is %s,port is %d",serverip,serverport);
        return -1;
    }
    
    m_bCanRun = 1;
    m_pThread = new kernel::CTThread( _ReceiveFunc, this );
    
    return 0;
}

void  TUdpServer::SendMsg(char* message,char* addr,int port)
{
    static int tolen=sizeof(sockaddr_in);
    int    msglen = strlen(message);

    sockaddr_in to;
    
    to.sin_family = AF_INET;
    to.sin_port = htons(port);
    to.sin_addr.s_addr =inet_addr(addr); 
    sendto(m_SocketFD,message,msglen,0,(struct sockaddr *)&to,tolen);
}

void TUdpServer::ReceiveFunc()
{
    struct timeval tv;
    sockaddr_in from;
    int from_len=sizeof(sockaddr_in);
    char message[MESSAGE_LEN+1];
    
    
    fd_set ready;
    while (m_bCanRun) 
    {
        FD_ZERO(&ready);
        FD_SET(m_SocketFD, &ready);
        
        tv.tv_sec = 0;
        tv.tv_usec = 10000;
    
        if(select(m_SocketFD + 1,&ready,NULL,NULL,&tv) >0)
        {
            if(FD_ISSET(m_SocketFD,&ready))
            {
                 int len = recvfrom(m_SocketFD,message ,MESSAGE_LEN,0,(struct sockaddr *)&from,(socklen_t*)&from_len);
                 if(len >0)
                 {
                      message[len]=0;
                  
                      char* fromIp = inet_ntoa(from.sin_addr);
                      unsigned int fromPort = ntohs(from.sin_port);
                     
                      (*m_pReportFunc)(message,fromIp,fromPort);
                 }
            }
        }
    }
}


void TUdpServer::StopServer()
{
    if (!m_bCanRun)
    {
        return;
    }
    m_bCanRun = 0;
    delete m_pThread;
    m_pThread = NULL;
}
