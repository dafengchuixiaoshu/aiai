#ifdef WIN32
#include <afx.h>
#include <Winsock2.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <strings.h>
#endif

#include "httpserver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int Http_GetPacklen(const char* inputData,int length,int& startPos)
{
    int headerLength = 0;
    int bodyLength  = 0;

    for(int i=startPos; i<length; ++i)
    {
        if(inputData[i] == '\r')
        {
            if(length - i < 4)
            {
                startPos = i; return 0;
            }

            if(inputData[i + 1] =='\n' && inputData[i + 2] =='\r' && inputData[i + 3] =='\n')
            {
                headerLength = i + 4; break;
            }
        }
    }

    for(int j=0; j<headerLength; ++j)
    {
        if(inputData[j] == '\r' && inputData[j+1] == '\n')
        {
#ifdef WIN32
            if(strnicmp(inputData+j+2,"content-length:",15)==0)
#else
            if(strncasecmp(inputData+j+2,"content-length:",15)==0)
#endif
            {
                bodyLength = atoi(inputData+j+17); break;
            }
        }
    }

    return headerLength + bodyLength;
}

THttpServer::THttpServer(int localPort,TTcpSocket_Callback* pCallback,char* localIP)
: TTcpServer(localPort,pCallback,localIP)
{
}

THttpServer::~THttpServer()
{   
}

int THttpServer::GetPacklen(const char* inputData,int length,int& startPos)
{
    return Http_GetPacklen(inputData,length,startPos);
}