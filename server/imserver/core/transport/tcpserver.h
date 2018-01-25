#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "tcpsocket.h"

class TTcpServer : public TTcpSocket
{
public:

    TTcpServer(int localPort,TTcpSocket_Callback* pCallback,char* localIP = NULL);

    ~TTcpServer();

    void    UseNoneBlock(int bUsed); 
    void    ListenThread();   
    
    virtual void   OnAccept(int fd,char* fromip,int fromport);

    void  SendMsg(char* addrinfo,char* msg,int length,int isclose,int index=0);
    int   Run();  

    int   GetLocalPort() { return m_LocalPort; }

protected:
    int   OpenServer();
    void  RunListenThread();

    int   m_NoneBlock;
    char  m_LocalIP[32];
    int   m_LocalPort;
    int   m_ListenFD;
    int   m_bListenRun;
    int m_ListenThreadCount;
    kernel::CTThread**  m_pListenThread;
    kernel::CTMutex* m_mutex;
};

#endif
