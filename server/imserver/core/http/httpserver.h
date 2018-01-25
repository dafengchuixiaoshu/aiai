#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#include "tcpserver.h"

class THttpServer : public TTcpServer
{
public:

    THttpServer(int localPort,TTcpSocket_Callback* pCallback,char* localIP = NULL);

    ~THttpServer();

protected:

    virtual int GetPacklen(const char* inputData,int length,int& startPos);
};

#endif
