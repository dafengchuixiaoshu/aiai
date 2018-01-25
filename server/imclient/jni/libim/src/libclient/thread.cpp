#include "thread.h"

extern "C" void on_thread_closed (int tid);

CThread::CThread()
#ifdef WIN32
: m_handle(NULL)
#endif
{
}

CThread::~CThread()
{
#ifdef WIN32
    if(NULL != m_handle)
    {
        DWORD dwExitCode;
        
        GetExitCodeThread(m_handle, &dwExitCode); // should be STILL_ACTIVE = 0x00000103 = 259

        CloseHandle(m_handle);
    }
#endif
}

bool CThread::Start(thread_entry_address* address, void* param, int stacksize)
{
#ifdef WIN32

    unsigned id;

    m_handle = (HANDLE)_beginthreadex(NULL, stacksize, address, param, 0, &id);

    if(NULL == m_handle) return false;

#else

    pthread_t id;

    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    
    pthread_attr_setstacksize(&attr, stacksize);
    
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&id, &attr, address, param);
    
    pthread_attr_destroy(&attr);
    
    if(0 == id) return false;

#endif

    return true;
}
