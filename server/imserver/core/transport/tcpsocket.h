#ifndef  _TCP_SOCKET_H
#define  _TCP_SOCKET_H

#pragma warning(disable:4786)

#include "ResourceBase.h"
#include "BaseQueue.h"

#include <map>
#include <list>
#include <string>
using namespace std;

#ifndef WIN32
#define closesocket close
#else
#define socklen_t   int
#define sleep(n) Sleep(n * 1000)
#endif

#define TCP_READBUFFER     1024
#define TCP_MAXEVENTSIZE   1024 * 5 

#define MSG_CLIENT_KEEPALIVE  0
#define MSG_CLIENT_KEEPCLOSE  1
#define MSG_SERVER_RESPONSE   2

enum
{
    SOCK_TYPE_MESSAGE = 0,
    SOCK_TYPE_CONNECT,
    SOCK_TYPE_CLOSE,
    SOCK_TYPE_SENDFAIL
};

struct TMsgInfo
{
public:

    TMsgInfo():buflen(0){}

    void Append(const char* src, size_t srclen)
    {
        if(0 == srclen || NULL == src) return;

        buffer.append(src, srclen); buflen += srclen;
    }
    
    int Erase(size_t outlen)
    {
        if(buflen < outlen) return -1;
        
        buffer.erase(0, outlen); buflen -= outlen; return 0;
    }

    const char* Str()
    {
        return buffer.c_str();
    }
    
    size_t Size()
    {
        return buflen;//buffer.size();
    }

    string buffer;
    size_t buflen;
};

class TSocketParam
{
public:

    TSocketParam(){}

    virtual ~TSocketParam(){}
};

struct TSocketMsg
{
    int   socketfd;
    int   startimer;
    int   closeflag;
    int   delTimes;
    int   readThread;
    int   msgType;
    int   port;
    int   startPos;

    char  addrinfo[128];
    TMsgInfo msginfo;
    TSocketParam* param;
    char  msgsign[64];

    TSocketMsg()
    {
        socketfd = 0;
        startimer = 0;
        closeflag = 0;
        delTimes = 0;
        readThread =0;
        msgType = 0;
        port = 0;
        startPos = 0;
        addrinfo[0] = 0;
        msgsign[0] = 0;
        param = NULL;
    }

    ~TSocketMsg()
    {
        if(NULL != param) delete param;
    }
};

typedef list<TSocketMsg*> TTcpList;
typedef kernel::BaseQueue<TSocketMsg*> TSendMsgQueue;

class TBaseParam;

typedef void TTcpSocket_Callback(TSocketMsg* pInfo,size_t startPos,size_t packlen,int msgtype,TBaseParam* param);

class TTcpSocket
{
public:
    TTcpSocket();
    ~TTcpSocket();

    void  SetThreadCount(int readthreads,int sendthreads = 1);
    void  SetTimeout(int timeout);
    void  CloseSocket(TSocketMsg* sockinfo);
    void  CloseSocket(char* addrinfo);

    void  ReadThread(int index);
    void  Epoll_ReadThread(int index);
    void  SendThread(int index);

    void  SetUseEpoll(int isUsed);
    int   GetUseEpoll();

    TSocketMsg* FindSocketInfo(char* addrinfo);

    int   encodeAddrInfo(int socketfd,char* remoteip,int remoteport,char* addrinfo);
    int   decodeAddrInfo(int& socketfd,char* remoteip,int& remoteport,char* addrinfo);
    void  SetSocketTimeout(int timeout) { m_socketTimeout = timeout; }

    void  SetSockCallback(TTcpSocket_Callback* pCallback){ m_pSockCallback = pCallback; }

protected:
    void  setSendBufSize(int fd);
    void  setRecBufSize(int fd);
    void  setLinger(int fd);
    void  setReUseAddr(int fd);
    void  setNonBlock(int fd);
    void  setRecSendTimeOver(int fd);

protected:
    void    Run();
    TSocketMsg*  AddSocket(int fd,char* remoteip,int remoteport,TSocketParam* param = NULL);

    int     ConnectToServer(char* serverip,int serverport);

    virtual void   OnClientConnect(TSocketMsg* pInfo);
    virtual void   OnClientClose(TSocketMsg* pInfo);
    virtual void   OnSendMsgFail(TSocketMsg* pInfo);

    virtual int    OnReadMsg(TSocketMsg* pInfo);
    virtual int    OnSendMsg(int socketfd,const char* message,int length,TSocketMsg* pSocketInfo);
    virtual void   OnRecvMsg(TSocketMsg* pInfo);
    virtual int    GetPacklen(const char* inputData,int length,int& startPos) = 0;

private:
    void    RemoveSocketFromMap(TSocketMsg* pSockinfo,int index);
    void    CheckSocketsAndFree(int index);

protected:
    void  PutSendMsgToQueue(TSocketMsg * pInfo,int index=0);
    void  SendMsgResponse(TSocketMsg * pInfo);
    void  SendMsgConnectAlive(TSocketMsg * pInfo);
    void  SendMsgConnectClose(TSocketMsg * pInfo);

    int   SendMsgUseSelect(int socketfd,const char* message,int length);
    int   SendMsgUsePoll(int socketfd,const char* message,int length);
    int   GetConnectionSize();

protected:
    int   m_readThreadCount;
    int   m_sendThreadCount;

    string* m_recvpack;

    int   m_bThreadRun ;

    TTcpSocket_Callback*     m_pSockCallback;

private:
    int   m_bUseEpool;
    int   m_nTimeout;

    kernel::CTThread**       m_pReadThreads;
    kernel::CTMutex**        m_pReadMutexs;
    TTcpList**               m_SocketInfo;

    kernel::CTThread**       m_pSendThreads;
    TSendMsgQueue**          m_SendMsgQueue;

    kernel::CTMutex*         m_pSocketMutex;
    map<string,TSocketMsg*>    m_SocketMap;
    TTcpList**               m_DeleteSockets;

    int                      m_socketTimeout;
};

#include <sstream>

template<typename Type> string to_string(Type num)
{
    std::stringstream ss;

    ss << num;

    return ss.str();
}

string create_coreid(const char* userid);

#endif
