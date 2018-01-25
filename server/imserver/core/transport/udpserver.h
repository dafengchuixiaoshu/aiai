#ifndef TRANSPORT_UDP_SERVER_H
#define TRANSPORT_UDP_SERVER_H

typedef void TUdpServerReport_CallBack(char* message,char* fromhost,int fromport);

#include "ResourceBase.h"
class TUdpServer
{
public:
    TUdpServer(TUdpServerReport_CallBack* pReportFunc);
    virtual ~TUdpServer();

    int   StartServer(char* serverip,int serverport);
    void  StopServer();
    void  SendMsg(char* message,char* addr,int port);
    
private:    
    int   OpenServer(char* ip,int port);
    
public:
    void  ReceiveFunc();
    
private:
    kernel::CTThread   *m_pThread;
    TUdpServerReport_CallBack* m_pReportFunc;
    int     m_SocketFD;
    int     m_bCanRun;
};

#endif