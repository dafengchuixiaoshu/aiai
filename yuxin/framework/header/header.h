#ifndef __FRAMEWORK_HEADER_H_
#define __FRAMEWORK_HEADER_H_

#include <map>
using std::map;
#include <set>
using std::set;
#include <list>
using std::list;
#include <string>
using std::string;
#include <vector>
using std::vector;

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/stat.h>

#include <fstream>
#include <sstream>
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

#define sleep_ms(msec) Sleep(msec)

#define sleep_s(sec) Sleep(sec * 1000)

#else

#include <poll.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/time.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#define closesocket close

#define stricmp strcasecmp

#define strnicmp strncasecmp

#define sleep_s(sec) sleep(sec)

#define sleep_ms(msec) usleep(msec * 1000)

#endif

/***********************************************/

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

template<typename Type> string to_string(Type value)
{
    std::stringstream ss;

    ss << value;

    return ss.str();
}

template<typename Type> void erase_map(Type& erasemap)
{
    typename Type::iterator it = erasemap.begin();

    for(; it != erasemap.end(); ++it)
    {
        delete (it->second);
    }

    erasemap.clear();
}

/***********************************************/

#define SAFE_DEL_SINGLE(p) if(NULL != (p)) { delete (p); (p) = NULL; }

#define SAFE_DEL_ARRAY(p) if(NULL != (p)) { delete[] (p); (p) = NULL; }

/***********************************************/

void set_block(int fd);

void set_linger(int fd);

void set_nonblock(int fd);

void set_reuse_addr(int fd);

void set_sendbuf_size(int fd);

void set_recvbuf_size(int fd);

void set_send_timeout(int fd);

void set_recv_timeout(int fd);

/***********************************************/

string get_uuid(string inbuf);

unsigned int get_tick_count();

string get_cur_time(bool ispace = false);

int sys_trim(const char* &start, const char* inbuf, int inlen);

bool get_host_by_name(struct sockaddr_in& addr_in, const char* address);

void replace_separator(string& inbuf, string srcdelim, string destdelim);

/***********************************************/

bool split_to_string(char* inbuf, int inlen, const char* fmt, ...);

bool split_to_string(char* inbuf, int inlen, const char* fmt, va_list& args);

/***********************************************/

int file_read(char* &outbuf, const char* filename);

int file_write(const char* filename, const char* inbuf, int inlen, bool isapp = false);

/***********************************************/

int base64_encode(string& outbuf, const char* inbuf, int inlen);

int base64_decode(string& outbuf, const char* inbuf, int inlen);

/***********************************************/

int xor_encode(string& outbuf, const char* inbuf, int inlen, const char* xorbuf = NULL);

int xor_decode(string& outbuf, const char* inbuf, int inlen, const char* xorbuf = NULL);

#endif //__FRAMEWORK_HEADER_H_