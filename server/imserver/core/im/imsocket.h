#ifndef _IM_SOCKET_H_
#define _IM_SOCKET_H_

#include <vector>
using std::vector;

#include "tcpserver.h"

#ifndef WIN32
#include <arpa/inet.h>
#endif

class TString
{
public:

    TString():m_rpos(0),m_wpos(0){}

    TString(const char* inbuf, int inlen);

    int GetSize(){ return m_wpos; }

    int Read(char* outbuf, int outlen);

    bool Write(const char* inbuf, int inlen);

    const char* GetBuffer(){ return &m_buffer[0]; }

    TString& operator<<(int value){ _Write<int>(htonl(value)); return *this; }

    TString& operator>>(int &value){ if(_Read<int>(value)) value = ntohl(value); return *this; }

private:

    template <typename T> bool _Read(T &value){ return sizeof(T) == Read((char*)&value, sizeof(T)); }

    template <typename T> bool _Write(T value){ return Write((const char*)&value, sizeof(T)); }

private:

    size_t m_rpos;

    size_t m_wpos;

    vector<char> m_buffer;
};

class TXmppServer : public TTcpServer
{
public:

    TXmppServer(int localPort,TTcpSocket_Callback* pCallback);

    ~TXmppServer(){}

protected:

    virtual void OnRecvMsg(TSocketMsg* pInfo);

    virtual int GetPacklen(const char* inputData,int length,int& startPos){ return -1; }
};

class TBinaryServer : public TTcpServer
{
public:

    TBinaryServer(int localPort,TTcpSocket_Callback* pCallback);

    ~TBinaryServer(){}

protected:

    virtual void OnRecvMsg(TSocketMsg* pInfo);

    virtual int GetPacklen(const char* inputData,int length,int& startPos){ return -1; }
};

class TLogFile;
class TBaseParam;

class TUpStream : public TTcpSocket
{
    int  m_port;

    char m_ip[64];

    char m_msgtype[64];

    char m_addrinfo[64];

    TLogFile* m_logfile;

    kernel::CTThread* m_thread;

    virtual int GetPacklen(const char* inputData,int length,int& startPos){ return length; }

    friend void upstream_callback(TSocketMsg* pInfo,size_t startPos,size_t packlen,int msgtype,void* param);

    string m_addr;

    string m_prototype;

    TString m_loginstring;

    TString m_keepalivestring;

public:

    TUpStream(const char* ip, int port, int index, string addr, string& prototype);

    ~TUpStream();

    bool Connect();

    void WorkThread();

    bool SendMsg(TBaseParam& param);    

    bool SendMsg(const char* msg, int length);

    void Run(string domain = "", string resource = "", string platfrom = "");
};

void start_upstream(const char* addr, string& upstr, vector<TUpStream*>& upstreams, string prototype);

#endif //_IM_SOCKET_H_
