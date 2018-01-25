#ifndef _HTTP_H_
#define _HTTP_H_

#define  HTTP_METHOD_UNKNOWN  -1
#define  HTTP_METHOD_GET      0
#define  HTTP_METHOD_PUT      1
#define  HTTP_METHOD_DELETE   2
#define  HTTP_METHOD_POST   3

#define  HTTP_VERSION_0   0
#define  HTTP_VERSION_1   1

#include <string>
#include <list>
using namespace std;

struct THttpHeader
{
    int    mashKey;	
    char*  strName;
    char*  strContent;
    
    THttpHeader()
    {
       mashKey = 0;
       strName = NULL;
       strContent = NULL;
    }
    
    ~THttpHeader()
    {
        if(strName) delete [] strName;
        if(strContent) delete [] strContent;
    }
    
};

class THttpMsg
{
protected:
    THttpMsg();
    virtual ~THttpMsg();

public:
    void  SetContentType(char* contentType);
    void  SetContentData(char* data,int len=-1);
    char* GetContentType();
    char* GetContentData();
    int   GetContentLength();
    int   GetSoapAction();
    void  AddDateHeader();
    
    void  AddHeader(char* header,char* content);
    char* GetHeader(char* header);
    char* GetHeader(char* header,int index);
    void  RemoveHeader(char* header);
    void  RemoveHeader(char* header,int index);

    int   GetVersion();
    void  SetVersion(int version);

protected:
    int   GetMashKey(char* name);
    int   EncodeMessage(char* outputData,int& length);

    int   DecodeHeader(char* header,int length);
    int   DecodeMessage(char* start,char* end);

    char* m_strContentData;
    int   m_nContentLen;
    list<THttpHeader*> m_HeaderList;
    int   m_Version;
};

class THttpRequest : public THttpMsg
{
public:
    THttpRequest();
    virtual ~THttpRequest();

    void  SetMethod(int method);
    void  SetURI(char* uri);
    
    int   GetMethod();
    string GetMethodString();
    char* GetURI();

    int   Encode(char* outputData,int& length);
    
    int   DecodeReqHeader(char* requestHeader,int length);
    int   Decode(char* inputData,int length = 0);
    
    void  AddMd5Authorization(char* userName,char* realm,char* uri,char* responce);
private:
    int   m_nMethod;
    char* m_strURI;
};

class THttpResponse : public THttpMsg
{
public:
    THttpResponse();
    virtual ~THttpResponse();

    void  SetStatusCode(int statusCode);
    int   GetStatusCode();

    int   Encode(char* outputData,int& length);

    int   DecodeRespHeader(char* respHeader,int length);
    int   Decode(char* inputData,int length = 0);
    
    void  AddMd5Authenticate(char* nonce,char* realm);
private:
    int   m_nStatusCode;
};

int HttpAuthorization(const char* userPin,const char* nonce,const char* method,const char* authHeader);
string EncodeUrl(string str);
string DecodeUrl(string str);
#endif

