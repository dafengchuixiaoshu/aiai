#ifndef QUEUE_H
#define QUEUE_H

#ifdef WIN32
#include "ResourceBase.h"
#else
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#endif

#include <map>
#include <list>
using std::map;
using std::list;

#ifdef WIN32
#define _BaseQueue_Lock    m_lock.Lock();
#define _BaseQueue_Unlock  m_lock.Unlock();
#else
#define _BaseQueue_Lock    pthread_mutex_lock(&mutex);
#define _BaseQueue_Unlock  pthread_mutex_unlock(&mutex);
#endif

namespace kernel
{
    template<class T> class BaseQueue
    {
        typedef map< int,list<T> > queue_map;

    public:

        BaseQueue();

        ~BaseQueue();

        int TryPut(T& data, int index = 1);

        int TryGet(T& data, int millisecond = 0);

        int GetSize();

    private:

#ifndef WIN32
        pthread_cond_t more;
        pthread_mutex_t mutex;
#else
        CTCriticalSection m_lock;
#endif

        queue_map m_queuemap;
    };

    template<class T> BaseQueue<T>::BaseQueue()
    {
#ifndef WIN32
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&more, NULL);
#endif
    }

    template<class T> BaseQueue<T>::~BaseQueue()
    {
    }

    template<class T> int BaseQueue<T>::TryPut(T& data, int index)
    {
        _BaseQueue_Lock;

        m_queuemap[index].push_back(data);

        _BaseQueue_Unlock;

#ifndef WIN32
        pthread_cond_signal(&more);
#endif

        return 0;
    }

    template<class T> int BaseQueue<T>::TryGet(T& data, int millisecond)
    {
        _BaseQueue_Lock;

        if(!m_queuemap.empty())
        {
            typename queue_map::iterator it = m_queuemap.end();

            while(1)
            {
                --it;

                if(!it->second.empty())
                {
                    data = it->second.front();

                    it->second.pop_front();

                    _BaseQueue_Unlock; return 0 ;
                }

                if(it == m_queuemap.begin()) break;
            }
        }

#ifdef WIN32
        //sleep_ms(1);
#else
        if(millisecond <= 0)
        {
            pthread_cond_wait(&more,&mutex);
        }
        else
        {
            struct timespec timer;
            timer.tv_sec=time(0)+ (millisecond /1000) ;
            timer.tv_nsec= (millisecond % 1000) * 1000000;
            pthread_cond_timedwait(&more,&mutex,&timer);
        }
#endif

        _BaseQueue_Unlock; return -1;
    }

    template<class T> int BaseQueue<T>::GetSize()
    {
        int datasize = 0;

        _BaseQueue_Lock;

        typename queue_map::iterator it = m_queuemap.begin();

        while(it != m_queuemap.end())
        {
            datasize += it->second.size(); ++it;
        }

        _BaseQueue_Unlock; return datasize;
    }
}

typedef kernel::BaseQueue<int>  TIntQueue;

#endif
