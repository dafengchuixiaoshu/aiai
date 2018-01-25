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
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <poll.h>
#endif

#include "tcpsocket.h"
#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct TThreadParam
{
    TTcpSocket*  pSocket;
    int          threadIndex;
};

THREAD_HANDLER_DECL(_Tcp_ReadThread,arg)
{
    TThreadParam* pParam = (TThreadParam*)arg;
    TTcpSocket* pSocket=pParam->pSocket;
    int  threadIndex = pParam->threadIndex;
    delete pParam;

    pSocket->ReadThread(threadIndex);
    return 0;
}


THREAD_HANDLER_DECL(_Tcp_SendThread,arg)
{
    TThreadParam* pParam = (TThreadParam*)arg;
    TTcpSocket* pSocket=pParam->pSocket;
    int  threadIndex = pParam->threadIndex;
    delete pParam;

    pSocket->SendThread(threadIndex);
    return 0;
}

TTcpSocket::TTcpSocket()
{
    m_sendThreadCount = 1;
    m_readThreadCount = 5;

    m_pReadThreads = NULL;
    m_pReadMutexs = NULL;
    //m_pReadQueue = NULL;
    m_SocketInfo = NULL;
    m_DeleteSockets = NULL;
    //m_ReadPacket = NULL; 

    m_pSendThreads = NULL;
    m_SendMsgQueue = NULL;

    m_pSocketMutex = NULL;

    m_bThreadRun = 0;
    m_bUseEpool = 1;

    m_nTimeout = 2;
    m_socketTimeout = 5;

    m_pSockCallback = NULL;
}

TTcpSocket::~TTcpSocket()
{
    m_bThreadRun = 0;

    sleep(1);

    if(m_pReadMutexs)
    {
        for(int i=0;i<m_readThreadCount;i++)
        {
            delete m_pReadMutexs[i];
            //delete m_pReadQueue[i];
            delete m_SocketInfo[i];
            delete m_pReadThreads[i];
            delete m_DeleteSockets[i];
            //delete [] m_ReadPacket[i];
        }

        delete [] m_pReadMutexs;
        m_pReadMutexs = NULL;

        //delete [] m_pReadQueue;
        //m_pReadQueue = NULL;

        delete [] m_SocketInfo;
        m_SocketInfo = NULL;

        delete [] m_pReadThreads;
        m_pReadThreads = NULL;

        delete [] m_DeleteSockets;
        m_DeleteSockets = NULL;

        //delete [] m_ReadPacket;
        //m_ReadPacket = NULL;
    }

    if(m_pSendThreads)
    {
        for(int j=0;j<m_sendThreadCount;j++)
        {
            delete  m_pSendThreads[j];
            delete  m_SendMsgQueue [j];
        }

        delete [] m_pSendThreads;
        m_pSendThreads = NULL;

        delete [] m_SendMsgQueue;
        m_SendMsgQueue = NULL;

    }

    if(m_pSocketMutex)
    {
        delete m_pSocketMutex;
        m_pSocketMutex = NULL;
    }
}

void TTcpSocket::SetUseEpoll(int isUsed)
{
    m_bUseEpool = isUsed;
}

int TTcpSocket::GetUseEpoll()
{
    return m_bUseEpool;
}

void TTcpSocket::setSendBufSize(int fd)
{
    int sendBufSize,sendBufLength;
    sendBufSize=512 * 1024;
    sendBufLength=sizeof sendBufSize;

    setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(char*)&sendBufSize,sendBufLength);
}

void TTcpSocket::setRecBufSize(int fd)
{
    int rcvBufSize,rcvBufLength;
    rcvBufSize=512 *1024 ;
    rcvBufLength=sizeof rcvBufSize;

    setsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char*)&rcvBufSize,rcvBufLength);
}

void TTcpSocket::setRecSendTimeOver(int fd)
{
#ifdef WIN32
    int timeout = m_socketTimeout*1000;
    setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
    setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
#else
    struct timeval timeout={m_socketTimeout,0};
    setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
    setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
#endif
}

void TTcpSocket::setLinger(int fd)
{
    struct linger sndOver;
    sndOver.l_onoff=1;
    sndOver.l_linger=0;  

    int sndOverLength=sizeof sndOver;

    setsockopt(fd,SOL_SOCKET,SO_LINGER,(char*)&sndOver,sndOverLength);
}

void TTcpSocket::setReUseAddr(int fd)
{
    int flag = 1;
    int len = sizeof(int);

    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char*)&flag,len);
}

void TTcpSocket::setNonBlock(int fd)
{
#ifdef WIN32
    unsigned long nonblocking = 1;
    ioctlsocket(fd, FIONBIO, (unsigned long*) &nonblocking);
#else
    int flags=fcntl(fd,F_GETFL,0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
}

void TTcpSocket::SetThreadCount(int readthreads,int sendthreads)
{
    if(readthreads < 1)
    {
        m_readThreadCount = 1;
    }
    else if(readthreads > 1024)
    {
        m_readThreadCount = 1024;
    }
    else
    {
        m_readThreadCount = readthreads;
    }

    if(sendthreads < 1)
    {
        m_sendThreadCount = 1;
    }
    else if(sendthreads > 1024)
    {
        m_sendThreadCount = 1024;
    }
    else 
    {
        m_sendThreadCount = sendthreads;
    }
}

void TTcpSocket::SetTimeout(int timeout)
{
    m_nTimeout = timeout;
}

void  TTcpSocket::Run()
{
    m_bThreadRun = 1;

    m_pSocketMutex = new kernel::CTMutex;

    m_pReadThreads = new kernel::CTThread*[m_readThreadCount];
    m_pReadMutexs = new kernel::CTMutex*[m_readThreadCount];
    //m_pReadQueue = new TIntQueue*[m_readThreadCount];
    m_SocketInfo = new TTcpList*[m_readThreadCount];
    m_DeleteSockets = new TTcpList*[m_readThreadCount];
    m_recvpack = new string[m_readThreadCount];

    for(int i=0;i<m_readThreadCount;i++)
    {
        m_pReadMutexs[i] = new kernel::CTMutex;
        //m_pReadQueue[i] = new TIntQueue;
        m_SocketInfo[i] = new TTcpList();
        m_DeleteSockets[i] = new TTcpList();

        TThreadParam* pParam = new TThreadParam;
        pParam->pSocket = this;
        pParam->threadIndex = i;
        m_pReadThreads[i] = new kernel::CTThread(_Tcp_ReadThread,pParam);
    }

    m_pSendThreads = new kernel::CTThread*[m_sendThreadCount];
    m_SendMsgQueue = new TSendMsgQueue*[m_sendThreadCount];

    for(int j=0;j<m_sendThreadCount;j++)
    {
        m_SendMsgQueue[j] = new TSendMsgQueue;

        TThreadParam* pParam = new TThreadParam;
        pParam->pSocket = this;
        pParam->threadIndex = j;
        m_pSendThreads[j] = new kernel::CTThread(_Tcp_SendThread,pParam);
    }
}

void  TTcpSocket::ReadThread(int index)
{
#ifndef WIN32
    if(GetUseEpoll())
    {
        //term("Read message use Epoll as thread index %d",index);
        Epoll_ReadThread(index);

        return;
    }
#endif

    fd_set ready;
    int    maxfd = 0;
    struct timeval tv;

    TTcpList mySocketList;
    TTcpList::iterator pElem;

    while(m_bThreadRun)
    {
        CheckSocketsAndFree(index);

        maxfd = 0; FD_ZERO(&ready);

        m_pReadMutexs[index]->Lock();
        pElem = m_SocketInfo[index]->begin();
        if(pElem != m_SocketInfo[index]->end())
        {
            mySocketList.push_back(*pElem);
            m_SocketInfo[index]->erase(pElem);
        }
        m_pReadMutexs[index]->Unlock();

        pElem = mySocketList.begin();
        while(pElem != mySocketList.end())
        {
            TTcpList::iterator p = pElem; ++pElem;

            TSocketMsg* pSockInfo = *p;

            if(pSockInfo->closeflag || (m_nTimeout > 0 && time(NULL) - pSockInfo->startimer > m_nTimeout * 60))
            {
                mySocketList.erase(p);
                RemoveSocketFromMap(pSockInfo,index);
            }
            else
            {
                FD_SET(pSockInfo->socketfd, &ready);
                if(pSockInfo->socketfd > maxfd)
                    maxfd = pSockInfo->socketfd;
            }
        }

        if(mySocketList.size() == 0)
        {
            sleep_ms(1);
            continue;
        }

        tv.tv_sec = 0; tv.tv_usec = 10000;

        if(select(maxfd+1,&ready,NULL,NULL,&tv) > 0)
        {
            for(pElem = mySocketList.begin();pElem != mySocketList.end();++pElem)
            {
                TSocketMsg* pSockInfo = *pElem;

                if(FD_ISSET(pSockInfo->socketfd, &ready) > 0)
                {
                    OnReadMsg(pSockInfo);
                }
            }
        }
    }
}

void  TTcpSocket::Epoll_ReadThread(int index)
{
#ifndef WIN32
    int epollFD = epoll_create(TCP_MAXEVENTSIZE);
    epoll_event events[TCP_MAXEVENTSIZE];

    TTcpList mySocketList;
    TTcpList::iterator pElem;
    while(m_bThreadRun)
    {
        CheckSocketsAndFree(index);

        m_pReadMutexs[index]->Lock();
        pElem = m_SocketInfo[index]->begin();
        if(pElem != m_SocketInfo[index]->end())
        {
            TSocketMsg* pSockInfo = *pElem;
            mySocketList.push_back(pSockInfo);
            m_SocketInfo[index]->erase(pElem);

            epoll_event ev; ev.events = EPOLLIN; ev.data.ptr = pSockInfo;

            if(epoll_ctl(epollFD, EPOLL_CTL_ADD, pSockInfo->socketfd, &ev) < 0)
            {
                CloseSocket(pSockInfo);
            }
        }
        m_pReadMutexs[index]->Unlock();

        pElem = mySocketList.begin();
        while(pElem != mySocketList.end())
        {
            TTcpList::iterator p = pElem; ++pElem;

            TSocketMsg* pSockInfo = *p;

            if(pSockInfo->closeflag || (m_nTimeout >0 && time(NULL) - pSockInfo->startimer > m_nTimeout * 60))
            {
                if (epoll_ctl(epollFD, EPOLL_CTL_DEL, pSockInfo->socketfd, NULL) < 0)
                {
                }

                mySocketList.erase(p);   
                RemoveSocketFromMap(pSockInfo,index);
            }
        }

        if(mySocketList.size() == 0)
        {
            sleep_ms(1);
            continue;
        }

        int nfds = epoll_wait(epollFD, events, TCP_MAXEVENTSIZE, 10);

        for(int i=0;i<nfds;++i)
        {
            TSocketMsg* pSockInfo = (TSocketMsg*)events[i].data.ptr;
            OnReadMsg(pSockInfo);
        }
    }
#endif
}

int TTcpSocket::OnReadMsg(TSocketMsg* pSockInfo)
{
    char message[TCP_READBUFFER+1] = { 0 };

    int len = recv(pSockInfo->socketfd,message,TCP_READBUFFER,0);

    if(len > 0)
    {
        pSockInfo->startimer=time(NULL);

        pSockInfo->msginfo.Append(message,len);

        OnRecvMsg(pSockInfo);
    }
    else
    {
        CloseSocket(pSockInfo);
    }
    return len;
}

int TTcpSocket::encodeAddrInfo(int socketfd,char* remoteip,int remoteport,char* addrinfo)
{
    static int index = 0;

    static kernel::CTMutex mutex;

    unsigned int curtime = time(NULL);

    mutex.Lock();

    sprintf(addrinfo,"%d_%s:%d_%X_%u",socketfd,remoteip,remoteport,curtime,++index);
    
    mutex.Unlock();
    
    return 1;
}

int TTcpSocket::decodeAddrInfo(int& socketfd,char* remoteip,int& remoteport,char* addrinfo)
{
    char* str1 = strchr(addrinfo,'_');
    if(str1 == NULL)
    {
        socketfd = 0;
        remoteport = 0;

        strcpy(remoteip,addrinfo);
        return 0;
    }

    socketfd = atoi(addrinfo);

    strcpy(remoteip,str1 + 1);
    int length = strlen(remoteip);
    for(int i=0;i<length;i++)
    {
        if(remoteip[i] ==':')
        {
            remoteip[i] = 0;
            remoteport = atoi(remoteip + i +1);
            return 0;
        }
    }
    return 0;
}

TSocketMsg* TTcpSocket::AddSocket(int fd,char* remoteip,int remoteport,TSocketParam* param)
{
    int index = fd % m_readThreadCount;

    TSocketMsg* pInfo = new TSocketMsg;
    pInfo->socketfd = fd;
    encodeAddrInfo(fd,remoteip,remoteport,pInfo->addrinfo);
    pInfo->startimer = time(NULL);
    pInfo->closeflag = 0;
    pInfo->readThread = index;
    pInfo->param = param;

    m_pSocketMutex->Lock();
    m_SocketMap[pInfo->addrinfo] = pInfo;
    m_pSocketMutex->Unlock();

    OnClientConnect(pInfo);

    m_pReadMutexs[index]->Lock();
    m_SocketInfo[index]->push_back(pInfo);
    m_pReadMutexs[index]->Unlock();

    //int tValue = 0;
    //m_pReadQueue[index]->TryPut(tValue);

    return pInfo;
}

void TTcpSocket::CloseSocket(TSocketMsg* sockinfo)
{
    sockinfo ->closeflag = 1;
}

int   TTcpSocket::GetConnectionSize()
{
    int size =0;

    m_pSocketMutex->Lock();
    size = m_SocketMap.size();
    m_pSocketMutex->Unlock();

    return size;
}

void TTcpSocket::RemoveSocketFromMap(TSocketMsg* pSockInfo,int index)
{
    m_pSocketMutex->Lock();
    m_SocketMap.erase(pSockInfo->addrinfo);
    m_pSocketMutex->Unlock();

    OnClientClose(pSockInfo);

    m_DeleteSockets[index]->push_back(pSockInfo);
}

void TTcpSocket::CheckSocketsAndFree(int index)
{
    list<TSocketMsg* >::iterator itrnext = m_DeleteSockets[index]->begin();

    while(itrnext != m_DeleteSockets[index]->end())
    {
        list<TSocketMsg* >::iterator itrcur = itrnext; ++itrnext;

        TSocketMsg* ptr = *itrcur;

        if(ptr->delTimes >= 2)
        {
            closesocket(ptr->socketfd);
            delete ptr;
            m_DeleteSockets[index]->erase(itrcur);
        }
        else
        {
            ++(ptr->delTimes);
        }
    }
}

int TTcpSocket::OnSendMsg(int socketfd,const char* message,int length,TSocketMsg* pSocketInfo)
{
#ifndef WIN32
    return SendMsgUsePoll(socketfd,message,length);
#else
    return SendMsgUseSelect(socketfd,message,length);
#endif
}

int TTcpSocket::SendMsgUseSelect(int socketfd,const char* message,int length)
{
    fd_set ready; FD_ZERO(&ready); FD_SET(socketfd, &ready);

    struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 10000;

    int count = 0; int temp = length;

    while(length > 0)
    {
        count = ++count % 50; if(0 == count) sleep_ms(1);

        if(select(socketfd+1,NULL,&ready,NULL,&tv) > 0)
        {
#ifndef WIN32        
            int len = send(socketfd,message,length,MSG_DONTWAIT);
#else
            int len = send(socketfd,message,length,0);
#endif
            if(len <= 0) break;

            message += len; length -= len;
        }
        else
        {
            break;
        }
    }
    
    return length;
}

int TTcpSocket::SendMsgUsePoll(int socketfd,const char* message,int length)
{
#ifndef WIN32
    struct pollfd client[1]; client[0].fd = socketfd; client[0].events = POLLOUT;

    int count = 0;

    while(length > 0)
    {
        count = ++count % 50; if(0 == count) sleep_ms(1);
        
        if(poll(client, 1, 5) > 0)
        {
            int len = send(socketfd,message,length,MSG_DONTWAIT);

            if(len <= 0) break;

            message += len; length -= len;
        }
        else
        {
            break;
        }
    }
#endif

    return length;
}

TSocketMsg* TTcpSocket::FindSocketInfo(char* addrinfo)
{
    TSocketMsg* ptr = NULL;

    m_pSocketMutex->Lock();
    map<string,TSocketMsg*>::iterator itr = m_SocketMap.find(addrinfo);
    if(itr != m_SocketMap.end()) ptr = itr->second;
    m_pSocketMutex->Unlock();

    return ptr;
}

void  TTcpSocket::CloseSocket(char* addrinfo)
{
    m_pSocketMutex->Lock();
    map<string,TSocketMsg*>::iterator itr = m_SocketMap.find(addrinfo);
    if(itr != m_SocketMap.end()) itr->second->closeflag = 1;
    m_pSocketMutex->Unlock();
}

void  TTcpSocket::SendMsgResponse(TSocketMsg * pInfo)
{
    TSocketMsg* pSocketInfo = FindSocketInfo(pInfo->addrinfo);

    size_t buflen = pInfo->msginfo.Size();

    if(pSocketInfo == NULL)
    {
        if(0 != buflen) OnSendMsgFail(pInfo);
        
        return ;
    }

    if(0 == buflen)
    {
        CloseSocket(pInfo->addrinfo);
    }
    else if(OnSendMsg(pSocketInfo->socketfd,pInfo->msginfo.Str(),buflen,pSocketInfo) != 0)
    {
        OnSendMsgFail(pInfo);
    }

    if(pInfo->closeflag)
    {
        CloseSocket(pInfo->addrinfo);
    }
}

void TTcpSocket::SendMsgConnectAlive(TSocketMsg * pInfo)
{
    TSocketMsg* pSocketInfo = NULL;

    m_pSocketMutex->Lock();
    map<string,TSocketMsg*>::iterator itr = m_SocketMap.begin();
    if(itr != m_SocketMap.end()) pSocketInfo = itr->second;
    m_pSocketMutex->Unlock();

    if(pSocketInfo == NULL)
    {
        OnSendMsgFail(pInfo);
        return;
    }

    if(OnSendMsg(pSocketInfo->socketfd,pInfo->msginfo.Str(),pInfo->msginfo.Size(),pSocketInfo) != 0)
    {
        OnSendMsgFail(pInfo);
    }

    if(pInfo->closeflag)
    {
        pSocketInfo->closeflag = 1;
    }
}

void TTcpSocket::SendMsgConnectClose(TSocketMsg * pInfo)
{
    int socketfd = ConnectToServer(pInfo->addrinfo,pInfo->port);
    if(socketfd > 0)
    {
        TSocketMsg* pSocketInfo = AddSocket(socketfd,pInfo->addrinfo,pInfo->port);
        if(OnSendMsg(socketfd,pInfo->msginfo.Str(),pInfo->msginfo.Size(),NULL) != 0)
        {
            OnSendMsgFail(pInfo);
        }
        pSocketInfo->closeflag = 1;
    }
    else
    {
        OnSendMsgFail(pInfo);
    }
}

void TTcpSocket::PutSendMsgToQueue(TSocketMsg * pInfo, int index)
{
    if(m_sendThreadCount == 1 || pInfo->msgType == MSG_CLIENT_KEEPALIVE)
    {
        m_SendMsgQueue[0]->TryPut(pInfo, index);
        return;
    }

    if(pInfo->msgType == MSG_CLIENT_KEEPCLOSE)
    {
        static int count = 0; ++count;
        if(count >= 1000000) count = 0;
        m_SendMsgQueue[count % m_sendThreadCount]->TryPut(pInfo, index);
        return;
    }

    if(pInfo->msgType = MSG_SERVER_RESPONSE)
    {
        int fd = atoi(pInfo->addrinfo);
        m_SendMsgQueue[fd % m_sendThreadCount]->TryPut(pInfo, index);
        return;
    }
}

void TTcpSocket::SendThread(int index)
{
    TSocketMsg *pInfo;
    while (m_bThreadRun)
    {
        pInfo = NULL;
        m_SendMsgQueue[index]->TryGet(pInfo);        
        if(pInfo)
        {
            if(pInfo->msgType == MSG_SERVER_RESPONSE)
            {
                SendMsgResponse(pInfo);
            }
            else if(pInfo->msgType == MSG_CLIENT_KEEPALIVE)
            {
                SendMsgConnectAlive(pInfo);
            }
            else
            {
                SendMsgConnectClose(pInfo);
            }
            delete pInfo;
        }    
    }
}

int  TTcpSocket::ConnectToServer(char* serverip,int serverport)
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0); 
    int ret = 0;
    if(socketfd <=0)
    {
        closesocket(socketfd);
        return 0;
    }

    setLinger(socketfd);
    setSendBufSize(socketfd);
    setRecBufSize(socketfd);
    setRecSendTimeOver(socketfd);

    int keepAliveFlag=1;
    int keepAliveSize=sizeof keepAliveFlag;
    setsockopt(socketfd,SOL_SOCKET,SO_KEEPALIVE,(char*)&keepAliveFlag,keepAliveSize);

    struct    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip);
    ret=connect(socketfd,(struct sockaddr *)&server,sizeof(sockaddr_in));
    if(ret<0)
    {
        closesocket(socketfd);
        return 0;
    }
    return socketfd;
}

void TTcpSocket::OnRecvMsg(TSocketMsg* pInfo)
{
    if(NULL == m_pSockCallback) return;

    while(1)
    {
        int packlen = GetPacklen(pInfo->msginfo.Str(),pInfo->msginfo.Size(),pInfo->startPos);

        if(packlen < 0)
        {
            CloseSocket(pInfo); break;
        }
        else if(packlen == 0 || packlen > pInfo->msginfo.Size())
        {
            break;
        }

        (*m_pSockCallback)(pInfo,0,packlen,SOCK_TYPE_MESSAGE,NULL);
        pInfo->msginfo.Erase(packlen); pInfo->startPos = 0;
    }
}

void TTcpSocket::OnClientConnect(TSocketMsg* pInfo)
{
    if(NULL != m_pSockCallback)
    {
        (*m_pSockCallback)(pInfo,0,0,SOCK_TYPE_CONNECT,NULL);
    }
}

void TTcpSocket::OnClientClose(TSocketMsg* pInfo)
{
    if(NULL != m_pSockCallback)
    {
        (*m_pSockCallback)(pInfo,0,0,SOCK_TYPE_CLOSE,NULL);
    }
}

void TTcpSocket::OnSendMsgFail(TSocketMsg* pInfo)
{
    if(NULL != m_pSockCallback)
    {
        (*m_pSockCallback)(pInfo,0,pInfo->msginfo.Size(),SOCK_TYPE_SENDFAIL,NULL);
    }
}

string create_coreid(const char* userid)
{
    if(NULL == userid)
    {
        return "null";
    }
    else
    {
        static kernel::CTMutex mutex;

        static int index = 0;

        char outbuf[128] = { 0 };

#ifdef WIN32
        unsigned int tickcount = GetTickCount();
#else
        struct timeval tv; gettimeofday(&tv, NULL);
        unsigned int tickcount = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif

        mutex.Lock();

        sprintf(outbuf, "%s%X%X", userid, tickcount, ++index);

        mutex.Unlock();

        return outbuf;
    }
}
