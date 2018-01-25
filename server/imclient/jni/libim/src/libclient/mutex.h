#ifndef _LIBCLIENT_MUTEX_H_
#define _LIBCLIENT_MUTEX_H_

#include "header.h"

#ifdef WIN32

class CMutex
{
public:

    CMutex()
    {
        InitializeCriticalSection(&m_cs);
    }

    ~CMutex()
    {
        DeleteCriticalSection(&m_cs);
    }

    void Lock()
    {
        EnterCriticalSection(&m_cs);
    }

    void Unlock()
    {
        LeaveCriticalSection(&m_cs);
    }

private:

    CRITICAL_SECTION m_cs;
};

#else

class CMutex
{
public:

    CMutex()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }

    ~CMutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    void Lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    void Unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

private:

    pthread_mutex_t m_mutex;
};

#endif //WIN32

#endif //_LIBCLIENT_MUTEX_H_
