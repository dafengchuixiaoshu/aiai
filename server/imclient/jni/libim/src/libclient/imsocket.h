#ifndef _LIBCLIENT_IMSOCKET_H_
#define _LIBCLIENT_IMSOCKET_H_

#include "queue.h"
#include "mutex.h"
#include "thread.h"
#include "protoparse.h"

void set_linger(int fd);
void set_sendbuf_size(int fd);
void set_recvbuf_size(int fd);
void set_send_timeout(int fd);
void set_recv_timeout(int fd);
bool get_host_by_name(struct sockaddr_in& addr_in, const char* address);

enum
{
    MSG_UNDEFINED = 0,

    MSG_RECV_CLOSED,
    MSG_RECV_MESSAGE,

    MSG_SEND_ERROR,
    MSG_SEND_CLOSED,
    MSG_SEND_DISCONNECT,

    MSG_SOCKET_CONNECT,
    MSG_SOCKET_TIMEOUT,

    MSG_WORK_CLEAR,
};

class CStartSocket
{
public:

    CStartSocket()
    {
#ifdef WIN32
        WSAData    wsaData;

        if(NO_ERROR != WSAStartup(MAKEWORD(2,2), &wsaData))
        {
            printf("WSAStartup failed\n");

            exit(0);
        }
#endif
    }

    ~CStartSocket()
    {
#ifdef WIN32
        WSACleanup();
#endif
    }
};

class CSocketMsg
{
public:

    CSocketMsg()
        : m_im(NULL)
        , m_imtype(0)
        , m_sendtime(0)
    {
    }

    ~CSocketMsg()
    {
        if(NULL != m_im) delete m_im;
    }

public:

    CIm* m_im;

    int m_imtype;

    int m_sendtime;

    TString m_outstr;

    string m_sendmsgid;
};

typedef CQueue<CSocketMsg*> socketmsg_queue;

class CSocketInfo
{
    CSocketInfo(const CSocketInfo &r);

    CSocketInfo& operator= (const CSocketInfo& r);

public:

    CSocketInfo();

    ~CSocketInfo();

    int size();

    int getfd();

    void clear();

    const char* c_str();

    int getloginstatus();

    bool erase(int outlen);

    void getaddr(string& ip, int& port);
    
    bool append(const char* inbuf, int inlen);

public:

    int m_uptime;

    int m_timeout;

    int m_port;

    string m_ip;

    int m_socketfd;

    CMutex m_mutex;

    CByte m_recvbuf;

    int m_loginstatus;

    CQueue<CSocketMsg*> m_ackqueue;
};

class CImSocket
{
    friend THREAD_HANDLE(tcpsocket_recvmsg, param);

    friend THREAD_HANDLE(tcpsocket_sendmsg, param);

    void RecvThread();

    void SendThread();

protected:

    CImSocket();

    bool SendMsg(int socketfd, const char* outbuf, int outlen);

public:

    ~CImSocket();

    void Release();
    
    bool Connect();

    int GetNetworkStatus();
    
    void SetNetworkStatus(int networkstatus);

    virtual void OnCallBack(int msgtype, CSocketMsg& socketmsg, bool islanchat) = 0;

    bool PushToSendQueue(int imtype, string sendmsgid, TString& outstr);

    bool Start(const char* ip, int port, int timeout, string& prototype);

    bool DoSocket(string& recvbuffer, int msgtype, bool islanchat = false, string sendmsgid = "");

    bool GetCurConnectStatus() { return m_socketinfo.getfd() > 0 && m_networkstatus > 0; }

public:

    bool m_istart;

    string m_prototype;

    int m_networkstatus;
    
    CMutex m_networkmutex;
    
    CThread m_recvthread;
    
    CThread m_sendthread;
    
    CQueue<int> m_awuqueue;

    CQueue<string> m_ackqueue;

    CSocketInfo  m_socketinfo;

    socketmsg_queue m_sendqueue;
};

#endif //_LIBCLIENT_IMSOCKET_H_
