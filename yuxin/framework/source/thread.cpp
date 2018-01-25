#include "thread.h"

CThread::CThread()
: m_istart(false)
#ifdef WIN32
, m_handle(NULL)
#endif
{
}

CThread::~CThread()
{
#ifdef WIN32
    if(NULL != m_handle)
    {
        DWORD dwExitCode;

        GetExitCodeThread(m_handle, &dwExitCode);

        CloseHandle(m_handle);
    }
#endif
}

bool CThread::DoStart(thread_address* address, void* param, int stacksize)
{
    if(!m_istart)
    {
#ifdef WIN32
        unsigned id;

        m_handle = (HANDLE)_beginthreadex(NULL, stacksize, address, param, 0, &id);

        if(NULL != m_handle)
        {
            m_istart = true;
        }
#else
        pthread_t id;

        pthread_attr_t attr;

        pthread_attr_init(&attr);

        pthread_attr_setstacksize(&attr, stacksize);

        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        pthread_create(&id, &attr, address, param);

        pthread_attr_destroy(&attr);

        if(id > 0)
        {
            m_istart = true;
        }
#endif
    }

    return m_istart;
}
