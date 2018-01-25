#ifndef __FRAMEWORK_TCPSOCKET_H_
#define __FRAMEWORK_TCPSOCKET_H_

#include "queue.h"
#include "logger.h"
#include "opname.h"
#include "thread.h"
#include "protobyte.h"

enum
{
    OP_ACTION = 0,
    OP_SOCKET,
    OP_SERVICE,
};

enum
{
    SOCKET_CONNECT = 0,
    SOCKET_TIMEOUT,
    
    SOCKET_RECV_ERROR,
    SOCKET_RECV_CLOSED,
    SOCKET_RECV_MESSAGE,
    
    SOCKET_EPOLL_ERROR,
    
    SOCKET_SEND_START,
    SOCKET_SEND_ERROR,
    SOCKET_SEND_ACTIVE,
    SOCKET_SEND_CLOSED,
    SOCKET_SEND_DISCONNECT,
};

class CStartSocket
{
    CStartSocket();
 
    friend class CSingleton<CStartSocket>;

public:

    ~CStartSocket()
    {
#ifdef WIN32
        WSACleanup();
#endif
    }
};

class CPacket
{
    CPacket(const CPacket &r);

    CPacket& operator= (const CPacket& r);

public:

    CPacket(int serviceid, const string& addrinfo, CProtoByte* protobyte = NULL, int closeflag = 0)
        : m_addrinfo(addrinfo)
        , m_serviceid(serviceid)
        , m_closeflag(closeflag)
        , m_sockettype(SOCKET_SEND_START)//(0)
        , m_threadindex(0)
        , m_protobyte(protobyte)
    {
    }

    CPacket(int serviceid, const string& addrinfo, const string& msgid, CProtoByte* protobyte = NULL, int closeflag = 0)
        : m_msgid(msgid)
        , m_addrinfo(addrinfo)
        , m_serviceid(serviceid)
        , m_closeflag(closeflag)
        , m_sockettype(SOCKET_SEND_START)//(0)
        , m_threadindex(0)
        , m_protobyte(protobyte)
    {
    }

    ~CPacket()
    {
        SAFE_DEL_SINGLE(m_protobyte);
    }

public:

    string m_msgid;

    string m_addrinfo;

    int m_serviceid;

    int m_closeflag;

    int m_sockettype;

    int m_threadindex;

    CProtoByte* m_protobyte;
};

class CSocketInfo
{
    void Copy(const CSocketInfo &r)
    {
        m_uptime = r.m_uptime;

        m_timeout = r.m_timeout;

        m_addrinfo = r.m_addrinfo;

        m_socketfd = r.m_socketfd;

        m_closeflag = r.m_closeflag;

        m_serviceid = r.m_serviceid;
    }

public:

    CSocketInfo(const CSocketInfo &r){ Copy(r); }

    CSocketInfo(int serviceid, int socketfd, const char* ip, int port, int timeout);

    CSocketInfo& operator= (const CSocketInfo& r){ if(this != &r) Copy(r); return *this; }

    ~CSocketInfo(){ if(m_socketfd > 0){ closesocket(m_socketfd); } }

    int size(){ return m_recvbuffer.size(); }

    const char* c_str(){ return m_recvbuffer.c_str(); }

    void erase(int outlen){ m_recvbuffer.erase(0, outlen); }

    void append(const char* inbuf, int inlen){ m_recvbuffer.append(inbuf, inlen); }

public:

    int m_timeout;

    int m_socketfd;

    int m_closeflag;

    int m_serviceid;

    string m_addrinfo;

    string m_recvbuffer;

    unsigned int m_uptime;
};

typedef CQueue<CPacket*> packet_queue;

typedef list<CSocketInfo*> socket_list;

typedef CQueue<CSocketInfo*> socket_queue;

typedef map<string, CSocketInfo*> socket_map;

class CTcpSocket
{
    friend THREAD_HANDLE(tcpsocket_recvmsg, param);

    friend THREAD_HANDLE(tcpsocket_sendmsg, param);

    CTcpSocket(const CTcpSocket &r);

    CTcpSocket& operator= (const CTcpSocket& r);

    void RecvThread(int index);

    void SendThread(int index);

    bool DoRecvSocket(int sockettype, CPacket* packet);

    bool DoRecvSocket(int sockettype, CSocketInfo* socketinfo);

    void CloseSocketInfo(int sockettype, CPacket* packet);

    void CloseSocketInfo(int sockettype, const string& addrinfo);

public:

    CTcpSocket();

    virtual ~CTcpSocket();

    virtual void ReadMsg(CSocketInfo* socketinfo);

    virtual bool SendMsg(int socketfd, const char* outbuf, int outlen);

public:

    bool DoStart();

    void SetThreadCount(int threadcount);

    int GetThreadCount(){ return m_threadcount; }

    int GetRecvPacket(int index, CPacket* &packet);

    string AppendSocketInfo(CSocketInfo& socketinfo);

    bool PushSend(int serviceid, const string& addrinfo, const string& msgid, CProtoByte* protobyte, int closeflag = 0);

private:

    CLog* m_log;

    bool m_istart;

    int m_threadcount;

    CMutex m_socketmutex;

    socket_map m_socketmap;

    CMutex m_workmutexs[max_thread_count];

    CThread m_recvthreads[max_thread_count];

    CThread m_sendthreads[max_thread_count];

    packet_queue m_recvqueues[max_thread_count];

    packet_queue m_sendqueues[max_thread_count];

    socket_queue m_acceptqueues[max_thread_count];
};

#define ERR_LOG(...) LOG_ERROR(m_log, __VA_ARGS__)

#define INFO_LOG(...) LOG_INFO(m_log, __VA_ARGS__)

string Connect(CTcpSocket& tcpsocket, int serviceid, const char* serverip, int serverport, int timeout);

#endif //__FRAMEWORK_TCPSOCKET_H_
