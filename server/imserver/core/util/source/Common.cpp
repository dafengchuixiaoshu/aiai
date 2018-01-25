#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef WIN32
#include <afx.h>
#include <Winsock2.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include "Common.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


char* getLocalIpAddress()
{
   static char myIPAddress[64];
   static int  isGet=0;
   
   if(isGet == 0)
   {
       isGet =1;
       
       char hostname[100];
       gethostname(hostname,sizeof(hostname));

       struct hostent *h;
       if ((h=gethostbyname(hostname)) == NULL)
       {
           strcpy(myIPAddress,"127.0.0.1");
       }

       struct in_addr *temp=(struct in_addr *)h->h_addr;
       strcpy(myIPAddress,inet_ntoa(*temp));
    }
    return myIPAddress;
}

unsigned long GetNewCseq()
{
    static unsigned long m_SipCseq=1000;
    if(m_SipCseq > 100000000)
       m_SipCseq=1000;

    return ++m_SipCseq;
}

string  GenerateCallID(char* localHost)
{
   char buffer[256];
   int uniqid = rand();
   sprintf(buffer,"Feinno%X%d@%s", uniqid ,time(NULL) ,localHost);

   return buffer;
}

string GenerateBranch()
{
    char branch[64];
    sprintf(branch,"%s%d","z9hG4bK",time(NULL));

    return branch;
}

string GenerateTag()
{
    int uniqid = rand();
	char buffer[512];
	sprintf(buffer,"z9hG4bK%X%d", uniqid ,time(NULL) );
	return buffer;
}
