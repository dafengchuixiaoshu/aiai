#ifndef _LIBCLIENT_HEADER_H_
#define _LIBCLIENT_HEADER_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
using std::map;
#include <list>
using std::list;
#include <deque>
using std::deque;
#include <string>
using std::string;
#include <vector>
using std::vector;

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>
#include <process.h>

#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")

#define socklen_t int
#define snprintf _snprintf

#define THREAD_RETURN_TYPE unsigned __stdcall
#define mutex_lock   EnterCriticalSection(&m_cs)
#define mutex_unlock LeaveCriticalSection(&m_cs)

#else

#include <unistd.h> 
#include <errno.h>
#include <sys/socket.h>
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
#define THREAD_RETURN_TYPE void*
#define mutex_lock   pthread_mutex_lock(&m_mutex)
#define mutex_unlock pthread_mutex_unlock(&m_mutex)

#endif

typedef unsigned int uint;

typedef unsigned char uchar;

template<typename T> class CSingleton
{
public:

    CSingleton(){}

    static T& Instance()
    {
        static T instance;
        
        return instance;
    }
};

inline void sleep_s(int sec)
{
#ifdef WIN32
    Sleep(sec * 1000);
#else
    sleep(sec);
#endif
}

inline void sleep_ms(int msec)
{
#ifdef WIN32
    Sleep(msec);
#else
    usleep(msec * 1000);
#endif
}

int base64_encode(char* &outbuf, const char* inbuf, int inlen);

int base64_decode(char* &outbuf, const char* inbuf, int inlen);

inline const char* split_to_string(char* inbuf, int inlen, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    vsnprintf(inbuf, inlen, fmt, args);

    va_end(args);

    return inbuf;
}

#include <sstream>

template<typename Type> string to_string(Type num)
{
    std::stringstream ss;

    ss << num;

    return ss.str();
}

#endif //_LIBCLIENT_HEADER_H_
