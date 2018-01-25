#ifndef _LIBCLIENT_THREAD_H_
#define _LIBCLIENT_THREAD_H_

#include "header.h"

typedef THREAD_RETURN_TYPE thread_entry_address(void* param);

#define THREAD_HANDLE(function, param) THREAD_RETURN_TYPE function(void* param)

class CThread
{
    CThread(const CThread &r);

    CThread& operator= (const CThread& r);

public:

    CThread();

    ~CThread();

    bool Start(thread_entry_address* address, void* param, int stacksize = 1*1024*1024);

private:

#ifdef WIN32
    HANDLE m_handle;
#endif
};

#endif //_LIBCLIENT_THREAD_H_
