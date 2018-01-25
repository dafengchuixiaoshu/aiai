#ifndef   _COMMON_H
#define   _COMMON_H

#include <string>
using namespace std;

char* getLocalIpAddress();
unsigned long GetNewCseq();
string  GenerateCallID(char* localHost);
string GenerateBranch();
string GenerateTag();

#endif
