#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "tcpsocket.h"

class TTcpClient : public TTcpSocket
{
public:

    TTcpClient(char* serverIP,int serverPort,TTcpSocket_Callback* pCallback);

    ~TTcpClient();

    void    UseNoneBlock(int bUsed);
    void    SetKeepAlive(int bKeepAlive);

    void    ReconnectThread();

    void    SendMsg(char* msg,int length);
    void    SendMsg(char* msg,int length,char* toip,int toport);
 
    int     Run();

private:
    kernel::CTThread*    m_pReconnectThread;

    int   m_NoneBlock;
    int   m_socketMsgType;
    char  m_serverIP[32];
    int   m_serverPort;
    int   m_bKeepAlive;    
};

#endif
