#ifndef _LIBCLIENT_HTTPSOCKET_H_
#define _LIBCLIENT_HTTPSOCKET_H_

#include "mutex.h"
#include "protobyte.h"


class CHttpSocketInfo
{

public:

    CHttpSocketInfo();

    ~CHttpSocketInfo();

    int size();

    int getfd();

    void clear();

    const char* c_str();

    bool erase(int outlen);

    void getaddr(string& ip, int& port);
    
    void setaddr(const char* ip, int port);

    bool append(const char* inbuf, int inlen);
    

public:

    int m_port;

    string m_ip;

    int m_socketfd;

    CByte m_recvbuf;
};

class CHttpSocket
{
public:

    CHttpSocket();
    ~CHttpSocket();

    bool SendHttpRequest(int method,string uri,string body,int body_size,int& retcode,int& errcode,int& extend,string &retcontect);

    void Release();

    bool Connect();

    bool Start(const char* ip, int port);

protected:

    void set_linger(int fd);

    void set_sendbuf_size(int fd);

    void set_recvbuf_size(int fd);

    void set_blocking(int fd, bool isblock);

    void set_send_timeout(int fd);

    void set_recv_timeout(int fd);

    static bool get_host_by_name(struct sockaddr_in& addr_in, const char* address);

    bool sendrequest(char* sendbuf,int bufferlen,int& retcode,int& errcode,int& extend,string &retcontect);

    void parseresponse(string buffer,int& retcode,int& errcode,int& extend,string& body);

    CHttpSocketInfo  m_socketinfo;
    CMutex m_mutex;

};


#endif //_LIBCLIENT_IMSOCKET_H_
