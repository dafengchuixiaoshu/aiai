#ifndef _HTTP_CLIENT_H
#define _HTTP_CLIENT_H

#include "tcpclient.h"

class THttpClient : public TTcpClient
{
public:

    THttpClient(char* serverIP,int serverPort,TTcpSocket_Callback* pCallback);
    
    ~THttpClient();

protected:

    virtual int GetPacklen(const char* inputData,int length,int& startPos);
};

#endif
