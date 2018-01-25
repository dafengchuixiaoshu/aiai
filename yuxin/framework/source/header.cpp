#include "mutex.h"

void set_block(int fd)
{
#ifdef WIN32
    unsigned long x = 0;

    ioctlsocket(fd, FIONBIO, &x);
#else
    int opts = fcntl(fd, F_GETFL, 0); 

    opts &= ~O_NONBLOCK;

    fcntl(fd, F_SETFL, opts);
#endif
}

void set_linger(int fd)
{
    struct linger ln; 

    ln.l_onoff = 1; ln.l_linger = 0;

    setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&ln, sizeof(ln));
}

void set_nonblock(int fd)
{
#ifdef WIN32
    unsigned long x = 1;

    ioctlsocket(fd, FIONBIO, &x);
#else
    int opts = fcntl(fd, F_GETFL, 0); 

    opts |= O_NONBLOCK;

    fcntl(fd, F_SETFL, opts);
#endif
}

void set_reuse_addr(int fd)
{
    int flag = 1;

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(flag));
}

void set_sendbuf_size(int fd)
{
    int size = 512 * 1024;

    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof(size));
}

void set_recvbuf_size(int fd)
{
    int size = 512 * 1024;

    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(size));
}

void set_send_timeout(int fd)
{
#ifdef WIN32
    int timeout = 30 * 1000;

    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval timeout = { 30, 0 };

    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
#endif
}

void set_recv_timeout(int fd)
{
#ifdef WIN32
    int timeout = 30 * 1000;

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval timeout = { 30, 0 };

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
}

string get_uuid(string inbuf)
{
    static CMutex mutex;

    static int index = 0;

    char outbuf[256] = { 0 };

    mutex.Lock();

    snprintf(outbuf, 255, "%s%x%x", inbuf.c_str(), get_tick_count(), ++index);

    mutex.Unlock();

    return outbuf;
}

unsigned int get_tick_count()//unsigned int64--->WIN32:%I64u,LINUX:%llu
{
#ifdef WIN32
    return GetTickCount();
#else
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

string get_cur_time(bool ispace)
{
    struct tm* timeinfo;

    time_t now = time(NULL);

#ifdef WIN32
    timeinfo = localtime(&now);
#else
    struct tm t;
    
    timeinfo = localtime_r(&now, &t);
#endif

    char curtime[32] = { 0 };

    sprintf(curtime, "%04d-%02d-%02d%s%02d:%02d:%02d",
        timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday,
        ispace ? " " : "T", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    return curtime;
}

int sys_trim(const char* &start, const char* inbuf, int inlen)
{
    if(NULL == inbuf || inlen <= 0)
    {
        return -1;
    }

    start = inbuf;

    const char* end = inbuf + inlen;

    while(start < end)
    {
        if(0 == *start)
        {
            return 0;
        }
        else if((unsigned char)*start > 0x20)
        {
            break;
        }
        else
        {
            ++start;
        }
    }

    while(start < end)
    {
        --end;

        if((unsigned char)*end > 0x20)
        {
            ++end;

            break;
        }
    }

    return end - start;
}

bool get_host_by_name(struct sockaddr_in& addr_in, const char* address)
{
    static const int max_address_len = 128;

    if(NULL == address || 0 == *address)
    {
        return false;
    }

    if(strlen(address) > max_address_len)
    {
        return false;
    }

    const char* temp = address;

    while('\0' != *temp)
    {
        if(('.' != *temp) && (*temp < '0' || *temp > '9'))
        {
            static CMutex mutex;

            mutex.Lock();

            struct hostent* myhost = gethostbyname(address);

            if(NULL == myhost)
            {
                mutex.Unlock();

                return false;
            }
            else
            {
                memcpy(&(addr_in.sin_addr), *(myhost->h_addr_list), sizeof(struct in_addr));

                mutex.Unlock();

                return true;
            }
        }

        ++temp;
    }

    addr_in.sin_addr.s_addr = inet_addr(address);

    return true;
}

void replace_separator(string& inbuf, string srcdelim, string destdelim)
{
    if(srcdelim != destdelim)
    {
        size_t srcdelimsize = srcdelim.size();

        size_t destdelimsize = destdelim.size();

        string::size_type pos = inbuf.find(srcdelim);

        while(string::npos != pos)
        {
            string sub = inbuf.substr(pos + srcdelimsize);

            inbuf.erase(pos);

            inbuf += destdelim;

            inbuf += sub;

            pos = inbuf.find(srcdelim, pos + destdelimsize);
        }
    }
}

bool split_to_string(char* inbuf, int inlen, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    bool isok = split_to_string(inbuf, inlen, fmt, args);

    va_end(args);

    return isok;
}

bool split_to_string(char* inbuf, int inlen, const char* fmt, va_list& args)
{
    if(vsnprintf(inbuf, inlen, fmt, args) > 0 && strlen(inbuf) < inlen)
    {
        return true;
    }

    return false;
}

int file_read(char* &outbuf, const char* filename)
{
    std::ifstream is(filename, std::ios::binary);

    if(is)
    {
        is.seekg(0, is.end);

        int outlen = is.tellg();

        is.seekg(0, is.beg);

        if(outlen > 0)
        {
            outbuf = new char[outlen + 1];

            if(NULL != outbuf)
            {
                is.read(outbuf, outlen);

                outbuf[outlen] = '\0';

                is.close();

                return outlen;
            }
        }

        is.close();
    }

    return 0;
}

int file_write(const char* filename, const char* inbuf, int inlen, bool isapp)
{
    std::ios_base::openmode mode = std::ios::binary;

    if(isapp)
    {
        mode |= std::ios_base::app;
    }

    std::ofstream os(filename, mode);

    if(os)
    {
        os.write(inbuf, inlen);

        os.flush();

        os.close();

        return inlen;
    }

    return 0;
}
