#ifndef _LIBCLIENT_QUEUE_H_
#define _LIBCLIENT_QUEUE_H_

#include "header.h"

template<typename T> class CQueue
{
    typedef deque<T> deque_template;

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

        typename queue_map::iterator it = m_queuemap.begin();

        while(m_queuemap.end() != it)
        {
            size += it->second.size();

            ++it;
        }

        mutex_unlock;

        return size;
    }

    int Push(const T& x, int key = 0, bool isback = true)
    {
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

        return 0;
    }

    int Pop(T& x, int millisecond = 0, bool ispop = true)
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

                    if(ispop) it->second.pop_front();

                    key = it->first;

                    break;
                }
            }
        }

        if(key < 0)
        {
#ifdef WIN32
            sleep_ms(1);
#else
            if(0 == millisecond)
            {
                pthread_cond_wait(&m_cond, &m_mutex);
            }
            else if(millisecond > 0)
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

#endif //_LIBCLIENT_QUEUE_H_
