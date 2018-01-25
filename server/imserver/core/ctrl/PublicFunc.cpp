#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#ifdef WIN32
#include <afx.h>
#define snprintf _snprintf
#endif

#pragma warning(disable:4786)

#include "Config.h"
#include "httpmsg.h"
#include "Common.h"
#include "hashfun.h"
#include "PublicFunc.h"
#include "xml.h"
#include "vmd5.h"
#include "ResourceBase.h"

#ifndef WIN32
#define stricmp strcasecmp
#endif

#define VALID_NONCE_TIMER 600

extern TConfig  m_Config;

string xmlEncode(string src)
{
    if(src == "")
        return "";
  
    char* pTemp = xmlTextEncode((char*)src.c_str());
    string result = pTemp;
    delete [] pTemp;
    
    return result;
}

void  HttpUrlParse(string url,string& path,string& node)
{
    char* temp = (char*)strstr(url.c_str(),"/~~");
    if(temp == NULL)
    {
        path = url;
        node ="";
    }
    else
    {
        int length = temp - url.c_str(); 

        path = url.substr(0,length);
        node = temp + 3;
    }
}

string GetHttpRequestUserName(THttpRequest* requestmsg)
{
    char* reqName = requestmsg->GetHeader("X-3GPP-Intended-Identity");
    
    if(NULL == reqName) return "";
    if(*reqName =='\"') ++reqName;

    string result = reqName;
    string::size_type pos = result.rfind('\"');
    if(string::npos != pos) result.erase(pos);

    return result;
}

string CreateNewNonce(string username,unsigned int curtime)
{
    unsigned int hash1 = BKDRHash(username.c_str());
    unsigned int hash2 = RSHash(username.c_str());    
    if(curtime == 0)
    {
        curtime = time(NULL);
    }

    char buffer[128];
    sprintf(buffer, "%u-%u-%u", curtime,(hash1 % 999) * (curtime) %999,(hash2 % 999) * (curtime) %999);
    return buffer;
}

string CreateNewID(){
	 unsigned int hash1 = BKDRHash("qwertyuiopasdfghjklzxcvbnm0123456789");
    unsigned int hash2 = RSHash("qwertyuiopasdfghjklzxcvbnm0123456789");    
    long curtime=0;
    curtime = time(NULL);

    int index = 0; static kernel::CTMutex mutex; static int count = -1;
    
    mutex.Lock();
    ++count; if(count < 0) count = 0; index = count;
    mutex.Unlock();

    static const int moduleid = m_Config.GetModuleid();

    char buffer[128] = { 0 };
    snprintf(buffer, 128, "%X%X%X%X%X", curtime,moduleid,(hash1 % 999) * (curtime) %999,(hash2 % 999) * (curtime) %999, index);
    return buffer;
	
	}


void  CreateNextNonceHeader(char* response,const char* username)
{
    string nonce = CreateNewNonce(username);
    
    char buffer[1024];
    sprintf(buffer,"Authentication-Info: nextnonce=\"%s\"\r\n",nonce.c_str());
    
    strcat(response,buffer);
}


string  CreateSipUser(string user)
{
    const char*  str = user.c_str();
    if(str[0] != 's' ||  str[1] != 'i' || str[2] != 'p' || str[3] != ':' )
        return "sip:" + user;

    return user;
}

string  CreateNoSipUser(string user)
{
    const char* str = user.c_str();
    if(str[0] == 's' &&  str[1] == 'i' && str[2] == 'p' && str[3] == ':' )
        return str + 4;

    return user;
}

string  GetTelNum(string username)
{
    int length = username.length();
    for(int i=0;i<length;i++)
    {
        if(username[i] == '@')
        {
            return username.substr(0,i);
        }
    }

    if(strncmp(username.c_str(),"+86",3) == 0)
        return username.substr(3);

    return username;
}

string int_to_str(int num)
{
    std::stringstream ss;

    ss << num;

    return ss.str();
}

void  GetUserNameDomain(string sipuser,string& username,string& domain)
{
    string src = sipuser;
    if(sipuser[0] == 's' && sipuser[1] == 'i' && sipuser[2] == 'p' && sipuser[3] == ':')
    {
        src = sipuser.substr(4);
    }
    
    int pos = src.find("@");
    if(pos == string::npos)
    {
        username = sipuser;
        domain = "";
        return;
    }

    username = src.substr(0,pos);
    domain = src.substr(pos +1);
}

void  GetJid(string sipuser,string& username)
{
    string src = sipuser;
    if(sipuser[0] == 's' && sipuser[1] == 'i' && sipuser[2] == 'p' && sipuser[3] == ':')
    {
        src = sipuser.substr(4);
    }
    
   username =src;

    //username = src.substr(0,pos);
    //domain = src.substr(pos +1);
}

string EncodeBase64(string str)
{
    int modvalue = str.length() % 3 ;
    if(modvalue == 1)
    {
        str +="  ";
    }
    else if(modvalue == 2)
    {
        str +=" ";
    }

    char buffer[1024]="";
   // base64_encode((char*)str.c_str(), str.length(),buffer);
    return buffer;
}

string DecodeBase64(string base64str)
{
    char buffer[1024]="";
    //base64_decode((char*)base64str.c_str(), base64str.length(), buffer);
    return buffer;
}

