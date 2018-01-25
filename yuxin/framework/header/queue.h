#ifndef __FRAMEWORK_QUEUE_H_
#define __FRAMEWORK_QUEUE_H_

#include <deque>
using std::deque;

#include "header.h"

#ifdef WIN32
#define mutex_lock   EnterCriticalSection(&m_cs)
#define mutex_unlock LeaveCriticalSection(&m_cs)
#else
#define mutex_lock   pthread_mutex_lock(&m_mutex)
#define mutex_unlock pthread_mutex_unlock(&m_mutex)
#endif

template<typename Type> class CQueue
{
    typedef deque<Type> deque_template;

    typedef map<int, deque_template> queue_map;

    queue_map m_queuemap;

#ifdef WIN32
    CRITICAL_SECTION m_cs;
#else
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;
#endif

public:

    CQueue()
    {
#ifdef WIN32
        InitializeCriticalSection(&m_cs);
#else
        pthread_cond_init(&m_cond, NULL);
        pthread_mutex_init(&m_mutex, NULL);
#endif
    }

    ~CQueue()
    {
#ifdef WIN32
        DeleteCriticalSection(&m_cs);
#else
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);
#endif
    }

    int Size()
    {
        int size = 0;

        mutex_lock;

        typename queue_map::const_iterator it = m_queuemap.begin();

        while(m_queuemap.end() != it)
        {
            size += it->second.size();

            ++it;
        }

        mutex_unlock;

        return size;
    }

    void Push(const Type& x, int key = 0, bool isback = true)
    {
        if(key < 0)
        {
            key = 0;
        }

        mutex_lock;

        if(isback)
        {
            m_queuemap[key].push_back(x);
        }
        else
        {
            m_queuemap[key].push_front(x);
        }

        mutex_unlock;

#ifndef WIN32 
        pthread_cond_signal(&m_cond);
#endif
    }

    int Pop(Type& x, int millisecond = 0, bool ispop = true)
    {
        int key = -1;

        mutex_lock;

        if(!m_queuemap.empty())
        {
            typename queue_map::iterator it = m_queuemap.end();

            while(m_queuemap.begin() != it)
            {
                --it;

                if(!it->second.empty())
                {
                    x = it->second.front();

                    if(ispop)
                    {
                        it->second.pop_front();
                    }

                    key = it->first;

                    break;
                }
            }
        }

        if(key < 0 && millisecond >= 0)
        {
#ifndef WIN32
            if(0 == millisecond)
            {
                pthread_cond_wait(&m_cond, &m_mutex);
            }
            else
            {
                struct timespec timer;

                timer.tv_nsec = (millisecond % 1000) * 1000000;

                timer.tv_sec = time(NULL) + (millisecond /1000) ;

                pthread_cond_timedwait(&m_cond, &m_mutex, &timer);
            }
#endif
        }

        mutex_unlock;

        return key;
    }
};

#endif //__FRAMEWORK_QUEUE_H_
