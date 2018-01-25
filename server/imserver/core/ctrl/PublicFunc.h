#ifndef PUBLIC_FUNC_H
#define PUBLIC_FUNC_H

#include <string>
using namespace std;

class THttpRequest;

string  xmlEncode(string src);
void    HttpUrlParse(string url,string& path,string& node);
string  GetHttpRequestUserName(THttpRequest* requestmsg);

string  CreateNewNonce(string username,unsigned int curtime=0);
int     VerifyNonce(string nonce,string username);
void    CreateNextNonceHeader(char* response,const char* username);
    
string  CreateSipUser(string user);
string  CreateNoSipUser(string user);
string  GetTelNum(string username);
string  int_to_str(int num);
void    GetUserNameDomain(string sipuser,string& username,string& domain);
void    GetJid(string sipuser,string& username);
string CreateNewID();
#endif
