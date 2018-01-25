#ifndef __FRAMEWORK_THREAD_H_
#define __FRAMEWORK_THREAD_H_

#include "header.h"

#ifdef WIN32

const int min_thread_count = 1;

const int max_thread_count = 2;

#define THREAD_RETURN_TYPE unsigned __stdcall

#else

#define THREAD_RETURN_TYPE void*

const int min_thread_count = 1;

const int max_thread_count = sysconf(_SC_NPROCESSORS_CONF);

#endif

template<typename T> class CThreadParam
{
public:

    T* _object;

    int _index;

    CThreadParam(int index, T* object):_index(index),_object(object){}
};

typedef THREAD_RETURN_TYPE thread_address(void* param);

#define THREAD_HANDLE(function, param) THREAD_RETURN_TYPE function(void* param)

class CThread
{
    CThread(const CThread &r);

    CThread& operator= (const CThread& r);

public:

    CThread();

    ~CThread();

    bool DoStart(thread_address* address, void* param, int stacksize = 1 * 1024 * 1024);

private:

    bool m_istart;

#ifdef WIN32
    HANDLE m_handle;
#endif
};

#endif //__FRAMEWORK_THREAD_H_
