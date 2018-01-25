#ifdef WIN32
#include <afx.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "httpmsg.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef WIN32
#define stricmp   strcasecmp
#endif

#define COPYSTR(dest,sour) if(dest) delete dest;if(sour){dest=new char[strlen(sour)+1];strcpy(dest,sour);}

#define _HEADER_LENGTH   4096

static char _UrlChar[] = 
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,'#',0,0,0,0,'(',')','*',0,',','-','.','/',
    '0','1','2','3','4','5','6','7','8','9',':',';',0,'=',0,'?',
    '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
    'P','Q','R','S','T','U','V','W','X','Y','Z',0,0,0,0,'_',
    0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
    'p','q','r','s','t','u','v','w','x','y','z',0,0,0,'~',0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

static char HexToChar(char ch)
{
    if(ch >= 'a' && ch <= 'z')
        return ch - 87;

    if(ch >= 'A' && ch <= 'Z')
        return ch - 55;

    return ch - 48;
}

string DecodeUrl(string str)
{
    string result = "";

    int len = str.length();
    int i =0;
    for(;;)
    {
        if(i >= len)
            break;

        char ch = str[i];
        if(ch == '%')
        {
            if(i + 2 <len)
            {
                char ch1 = HexToChar(str[i+1]);
                char ch2 = HexToChar(str[i+2]);
                i += 3;

                ch = ch1 * 16 + ch2;
                result += ch;
            }
            else
            {
                result += ch;
                i++;
            }
        }
        else
        {
            result += ch;
            i++;
        }
    }
    return result;
}

string EncodeUrl(string str)
{
    string result = "";
    int   len = str.length();

    for(int i=0;i<len;i++)
    {
        char ch = str[i];
        if(_UrlChar[ch] == 0)
        {
            char temp[6];
            sprintf(temp,"%02x",ch);

            result += "%";
            result += temp;
        }
        else
        {
            result += ch;
        }
    }
    return result;
}

void TrimAll(char* sour);
	


THttpMsg::THttpMsg()
{
    m_strContentData=NULL;
    m_nContentLen =-1;
    m_Version = HTTP_VERSION_1;
}

THttpMsg::~THttpMsg()
{
    if(m_strContentData)
        delete m_strContentData;
    
    list<THttpHeader*>::iterator itrprev = m_HeaderList.begin();
    for(;;)
    {
		if(itrprev == m_HeaderList.end())
        {
            break;
        }

        list<THttpHeader*>::iterator itr = itrprev;
        itrprev ++;
        
        
        
        THttpHeader* p = *itr;
        delete p;
        m_HeaderList.erase(itr);
    }	
}

int  THttpMsg::GetMashKey(char* name)
{
    int ret =0;	
    for(int i=0;i<strlen(name);i++)
    {
        ret += (unsigned char) name[i];
        if(name[i] >= 'A' && name[i] <= 'Z')
        {
            ret += 32;
        }
    }
    return ret;
}

int  THttpMsg::GetSoapAction()
{
    if(GetHeader("SOAPAction"))
       return 1;
    return 0;
}


char* THttpMsg::GetContentType()
{
    return GetHeader("Content-Type");
}

char* THttpMsg::GetContentData()
{
    return m_strContentData;
}

int THttpMsg::GetContentLength()
{
    return m_nContentLen;
}

void  THttpMsg::SetContentType(char* contentType)
{
    assert(contentType);
    AddHeader("Content-Type",contentType);
}

int THttpMsg::GetVersion()
{
    return m_Version;
}

void THttpMsg::SetVersion(int version)
{
     m_Version = version;
}

void  THttpMsg::SetContentData(char* data,int len)
{
    assert(data);

    if(len == -1)
    {
        len = strlen(data);
    }

    
    if(m_strContentData)
    {
        delete m_strContentData;
    }

    m_strContentData = new char[len+1];
	  memset(m_strContentData, 0, len+1);
    memcpy(m_strContentData,data,len);
    m_strContentData[len]=0;

    m_nContentLen =len;
}

void THttpMsg::AddDateHeader()
{
    
    time_t atimer = time(NULL);
#ifndef WIN32
    struct tm tm_temp;
    struct tm *area = localtime_r( &atimer,&tm_temp );
#else
    struct tm *area = localtime( &atimer);
#endif

    char strDate[64];
    strcpy(strDate,asctime(area));
    strDate[strlen(strDate)-1]=0;
    
    AddHeader("Date",strDate);
}

int THttpMsg::EncodeMessage(char* outputData,int& length)
{
    list<THttpHeader*>::iterator itr = m_HeaderList.begin();
    for(;itr != m_HeaderList.end();itr++)
    {
        THttpHeader* pHeader = *itr;
        strcat(outputData,pHeader->strName);
        strcat(outputData,": ");
        strcat(outputData,pHeader->strContent);
        strcat(outputData,"\r\n");
    }
    //set content type
    if(m_strContentData)
    {
        if(GetHeader("Content-Type") == NULL)
        {
            strcat(outputData,"Content-Type: text/plain\r\n");
        }
        char lengthHeader[256];
        sprintf(lengthHeader,"Content-Length:%d\r\n",m_nContentLen);
        strcat(outputData,lengthHeader);
    }
    else
    {
        strcat(outputData,"Content-Length: 0\r\n");
    }
    
    strcat(outputData,"\r\n");

    length=strlen(outputData);
    if(m_strContentData)
    {
        memcpy(outputData+strlen(outputData),m_strContentData,m_nContentLen);
        length += m_nContentLen;
    }
    return 0;
}

void  THttpMsg::AddHeader(char* header,char* content)
{
    THttpHeader* pHeader = new THttpHeader;
    pHeader->mashKey = GetMashKey(header);
    
    pHeader->strName = new char[strlen(header) +1];
	memset(pHeader->strName, 0, strlen(header) +1);
    strcpy(pHeader->strName,header);
    
    pHeader->strContent = new char[strlen(content) +1];
	memset(pHeader->strContent, 0, strlen(content) +1);
    strcpy(pHeader->strContent,content);
            
    m_HeaderList.push_back(pHeader);
}

char* THttpMsg::GetHeader(char* header)
{
    int mashkey = GetMashKey(header);
    
    list<THttpHeader*>::iterator itr = m_HeaderList.begin();
    for(;itr != m_HeaderList.end();itr++)
    {
        THttpHeader* pHeader = *itr;
        if(pHeader->mashKey != mashkey)
            continue;

        if(stricmp(header,pHeader->strName) == 0)
            return pHeader->strContent;
    }
    return NULL;
}

char* THttpMsg::GetHeader(char* header,int index)
{
    int mashkey = GetMashKey(header);
    
    int i=0;
    list<THttpHeader*>::iterator itr = m_HeaderList.begin();
    for(;itr != m_HeaderList.end();itr++)
    {
        THttpHeader* pHeader = *itr;
        if(pHeader->mashKey != mashkey)
            continue;

        if(stricmp(header,pHeader->strName) == 0)
        {
            if(i == index)
                return pHeader->strContent;
            i++;	
        }
    }
    return NULL;
}

void THttpMsg::RemoveHeader(char* header)
{
    int mashkey = GetMashKey(header);
    
    list<THttpHeader*>::iterator itrprev = m_HeaderList.begin();
    for(;;)
    {
        list<THttpHeader*>::iterator itr = itrprev;
        if(itr == m_HeaderList.end())
            break;
        
        itrprev ++;

        THttpHeader* pHeader = *itr;
        if(pHeader->mashKey != mashkey)
            continue;
        if(stricmp(header,pHeader->strName) != 0)
            continue;

        m_HeaderList.erase(itr);
        delete pHeader;
    }
}

void THttpMsg::RemoveHeader(char* header,int index)
{
    int mashkey = GetMashKey(header);
    
    int i=0;
    list<THttpHeader*>::iterator itr = m_HeaderList.begin();
    for(;itr != m_HeaderList.end();itr++)
    {
        THttpHeader* pHeader = *itr;
        if(pHeader->mashKey != mashkey)
            continue;
        if(stricmp(header,pHeader->strName) == 0)
        {
            if(i == index)
            {
                m_HeaderList.erase(itr);
                delete pHeader;
                return;
            }
            i++;	
        }
    }
}

int  THttpMsg::DecodeHeader(char* header,int length)
{
    for(int i=0;i<length;i++)
    {
         if(header[i] == ':')
         {
             header[i] = 0;
             char* content = header +i +1;

             THttpHeader* pHeader = new THttpHeader;
             pHeader->strName = new char[i+1];
			       memset(pHeader->strName, 0, i+1);
             strcpy(pHeader->strName,header);
             
             pHeader->strContent = new char[length-i];
			       memset(pHeader->strContent,0,length-i);
             strcpy(pHeader->strContent,content);
             
             TrimAll(pHeader->strName);
             TrimAll(pHeader->strContent);
             pHeader->mashKey = GetMashKey(header);

             m_HeaderList.push_back(pHeader);
             return 0;
         }
    }
    return -1;
}

int THttpMsg::DecodeMessage(char* start,char* end)
{
    char messageHeader[_HEADER_LENGTH];

    int index =0;
    while(start <= end)
    {
        if(*start != '\r')
        {
            messageHeader[index++] = *start; 
            if(index >= _HEADER_LENGTH)
            {
                return -1;
            }
            start ++;
            continue;
        }

        start ++;
        if(*start != '\n')
        {
            return -1;
        }
        
        start ++;
        if(*start == ' ' || *start == '\t')
        {
            continue;
        }

        if(*start != '\r' )
        {
            messageHeader[index]= 0;
            if(DecodeHeader(messageHeader,index +1) != 0)
                return -1;

            index = 0;
            continue;
        }

        start ++;
        if(*start != '\n' )
            return -1;

        start ++;

        int length = end - start;
        if(length > 0)
        {
            SetContentData(start,length);
        }
        return 0;
    }

    return -1;
}


THttpRequest::THttpRequest()
{
    m_nMethod=HTTP_METHOD_UNKNOWN;
    m_strURI=NULL;
}

THttpRequest::~THttpRequest()
{
    if(m_strURI)
        delete m_strURI;
}

void  THttpRequest::SetMethod(int method)
{
    assert(method == HTTP_METHOD_GET || method == HTTP_METHOD_PUT || method==HTTP_METHOD_DELETE || method == HTTP_METHOD_POST);
    m_nMethod=method;
}

void  THttpRequest::AddMd5Authorization(char* userName,char* realm,char* uri,char* responce)
{
    char content[1024];
    sprintf(content,"Digest username=\"%s\",realm=\"%s\",uri=\"%s\",response=\"%s\"",userName,realm,uri,responce);
    AddHeader("Authorization",content);
}

void  THttpRequest::SetURI(char* uri)
{
    assert(uri);

    COPYSTR(m_strURI,uri);
}

int  THttpRequest::GetMethod()
{
    return m_nMethod;
}
string THttpRequest::GetMethodString()
{
    static char* MethodName[4]={"GET","PUT","DELETE","POST"};

    if(m_nMethod <0 || m_nMethod >= 4)
        return "";


    return MethodName[m_nMethod];
}
char* THttpRequest::GetURI()
{
    return m_strURI;
}

int  THttpRequest::Encode(char* outputData,int& length)
{
    static char* MethodName[4]={"GET","PUT","DELETE","POST"};

    if(m_nMethod == HTTP_METHOD_UNKNOWN)
        return -1;

    if(m_strURI == NULL)
        return -2;

    //set request line
    string str = EncodeUrl(m_strURI);
    sprintf(outputData,"%s %s HTTP/1.%d\r\n",MethodName[m_nMethod],str.c_str(),m_Version);

    //set body
    EncodeMessage(outputData,length);
    outputData[length]=0;
    return 0;
}


int  THttpRequest::DecodeReqHeader(char* requestHeader,int length)
{
    if(length >= 1024)
        return -1;

    char strType[1024] ="";
    char uri[1024] = "";
    char version[1024] = "";

    sscanf(requestHeader,"%s %s %s",strType,uri,version);

    if(strType[0] == 0 || uri[0] == 0 || version[0]== 0)
    {
        return -1;
    }

    if(strncmp(version,"HTTP/1.",7) !=0)
    {
        return -1;
    }

    m_Version = atoi(version + 7);
    if(m_Version != HTTP_VERSION_0 && m_Version != HTTP_VERSION_1)
        return -1;

    if(strcmp(strType,"GET") ==0)
    {
        m_nMethod=HTTP_METHOD_GET;
    }
    else if(strcmp(strType,"PUT") ==0)
    {
        m_nMethod=HTTP_METHOD_PUT;
    }
    else if(strcmp(strType,"DELETE") ==0)
    {
        m_nMethod=HTTP_METHOD_DELETE;
    }
    else if(strcmp(strType,"POST") == 0)
    {
        m_nMethod = HTTP_METHOD_POST;
    }
    else
    {
        return -1;
    }

    string str = DecodeUrl(uri);
    SetURI((char*)str.c_str());
    return 0;
}


int  THttpRequest::Decode(char* inputData,int length)
{
    if(inputData ==NULL)
        return -1;

    if(length == 0)
    {
        length = strlen(inputData);
    }

    char messageHeader[_HEADER_LENGTH];

    char* start = inputData;
    char* end = inputData + length;
    int index =0;
    while(start <= end)
    {
        if(*start == '\r')
        {
            start ++;
            if(*start != '\n')
            {
                return -1;
            }
            
            messageHeader[index] = 0;

            if(DecodeReqHeader(messageHeader,index) != 0)
                return -1;

            if(DecodeMessage(start+1,end) !=0)
                return -1;

            return 0;
        }
        else
        {
            messageHeader[index++] = *start; 
            if(index >= _HEADER_LENGTH)
            {
                return -1;
            }
            start ++;
        }
    }

    return -1;
}


struct TResponseStatus
{
    int  code;
    char* desc;
};

const TResponseStatus responseStats[] = \
{
  /* Informational 1xx */
  {100, "Continue"},
  {101, "Switching Protocols"},

  /* Successful 2xx */
  {200, "OK"},
  {201, "Created"},
  {202, "Accepted"},
  {203, "Non-Authoritative Information"},
  {204, "No Content"},
  {205, "Reset Content"},
  {206, "Partial Content"},

  /* Redirection 3xx */
  {300, "Multiple Choices"},
  {301, "Moved Permanently"},
  {302, "Found"},
  {303, "See Other"},
  {304, "Not Modified"},
  {305, "Use Proxy"},
  // 306 Unused
  {307, "Temporary Redirect"},
 
  /* Client Error 4xx */
  {400, "Bad Request"},
  {401, "Unauthorized"},
  {402, "Payment Required"},
  {403, "Forbidden"},
  {404, "Not Found"},
  {405, "Method Not Allowed"},
  {406, "Not Acceptable"},
  {407, "Proxy Authentication Required"},
  {408, "Request Timeout"},
  {409, "Conflict"},
  {410, "Gone"},
  {411, "Length Required"},
  {412, "Precondition Failed"},
  {413, "Request Entity Too Large"},
  {414, "Request-URI Too Long"},
  {415, "Unsupported Media Type"},
  {416, "Requested Range Not Satisfiable"},
  {417, "Expectation Failed"},

  /* Server Error 5xx */
  {500, "Internal Server Error"},
  {501, "Not Implemented"},
  {502, "Bad Gateway"},
  {503, "Service Unavailable"},
  {504, "Gateway Timeout"},
  {505, "HTTP Version Not Supported"}
};

const char* getStatusDescription(int code)
{
    for (int i=0; i < sizeof(responseStats) / sizeof(responseStats[0]); i++)
    {
        if (responseStats[i].code == code)
        {
            return responseStats[i].desc; 
        }
    }
    return NULL;
}

THttpResponse::THttpResponse()
{
    m_nStatusCode=0;
}

THttpResponse::~THttpResponse()
{
    
}

void  THttpResponse::AddMd5Authenticate(char* nonce,char* realm)
{
    char content[1024];
    sprintf(content,"Digest realm=\"%s\",nonce=\"%s\"",realm,nonce);
    AddHeader("WWW-Authenticate",content);
}

int  THttpResponse::Encode(char* outputData,int& length)
{
    const char* desc=getStatusDescription(m_nStatusCode);
    if(desc ==NULL)
        return -1;

    sprintf(outputData,"HTTP/1.%d %d %s\r\n",m_Version,m_nStatusCode,desc);
    EncodeMessage(outputData,length);
    outputData[length]=0;
    return 0;
}

int  THttpResponse::DecodeRespHeader(char* respHeader,int length)
{
    if(length >= 1024)
        return -1;
    
    char version[1024] = "";
    int  status = 0;

    sscanf(respHeader,"%s %d",version,&status);

    if(version[0] == 0 ||  status <= 0)
        return -1;

    if(strncmp(version,"HTTP/1.",7) !=0)
        return -1;

    m_Version = atoi(version + 7);
    if(m_Version != HTTP_VERSION_0 && m_Version != HTTP_VERSION_1)
        return -1;

    m_nStatusCode = status;
    return 0;
}
int  THttpResponse::Decode(char* inputData,int length)
{
    if(inputData ==NULL)
        return -1;

    char messageHeader[_HEADER_LENGTH];

    char* start = inputData;
    char* end = inputData + length;
    int index =0;
    while(start <= end)
    {
        if(*start == '\r')
        {
            start ++;
            if(*start != '\n')
            {
                return -1;
            }
            
            messageHeader[index] = 0;

            if(DecodeRespHeader(messageHeader,index) != 0)
                return -1;

            if(DecodeMessage(start+1,end) !=0)
                return -1;

            return 0;
        }
        else
        {
            messageHeader[index++] = *start; 
            if(index >= _HEADER_LENGTH)
            {
                return -1;
            }
            start ++;
        }
    }

    return -1;
}

void THttpResponse::SetStatusCode(int statusCode)
{
    m_nStatusCode=statusCode;
}
int THttpResponse::GetStatusCode()
{
    return m_nStatusCode;
}

