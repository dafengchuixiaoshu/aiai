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
#endif

#include "httpclient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int Http_GetPacklen(const char* inputData,int length,int& startPos);

THttpClient::THttpClient(char* serverIP,int serverPort,TTcpSocket_Callback* pCallback)
: TTcpClient(serverIP,serverPort,pCallback)
{
}

THttpClient::~THttpClient()
{   
}

int THttpClient::GetPacklen(const char* inputData,int length,int& startPos)
{
    return Http_GetPacklen(inputData,length,startPos);
}