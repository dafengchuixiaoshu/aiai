#ifndef _EXAMPLE_HEADER_H_
#define _EXAMPLE_HEADER_H_

#include <map>
using std::map;
#include <list>
using std::list;
#include <string>
using std::string;
#include <vector>
using std::vector;

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <algorithm>

#ifdef WIN32

#include <winsock2.h>
#include <io.h>
#include <windows.h>
#include <process.h>

#define socklen_t int

#define snprintf _snprintf

#pragma warning(disable:4996)

#pragma comment(lib, "ws2_32.lib")

#else

#include <unistd.h> 
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>
#include <netdb.h>
#include <fcntl.h> 
#include <stdarg.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <poll.h>

#define closesocket close

#define stricmp  strcasecmp

#define strnicmp strncasecmp

#endif

typedef unsigned int uint;

typedef unsigned char uchar;

#define SAFE_DEL_SINGLE(p) if(NULL != (p)) { delete (p); (p) = NULL; }

#define SAFE_DEL_ARRAY(p) if(NULL != (p)) { delete[] (p); (p) = NULL; }

#endif //_EXAMPLE_HEADER_H_