/**
 * @file -
 * @author jingqi
 * @date 2010-07-09
 */

#ifndef ___HEADFILE___B926495D_967A_45A2_8F56_4FFB10F2E34B_
#define ___HEADFILE___B926495D_967A_45A2_8F56_4FFB10F2E34B_

#if defined(WIN32)
#   include <windows.h>
#else
#   include <pthread.h>
#endif

#include "mutex.h"

namespace nut
{

class Condition
{
#if defined(WIN32)
    CONDITION_VARIABLE m_cond;
#else
    pthread_cond_t m_cond;
#endif

public :
    Condition()
    {
#if defined(WIN32)
        ::InitializeConditionVariable(&m_cond);
#else
        pthread_cond_init(&m_cond, NULL);
#endif
    }

    ~Condition()
    {
#if defined(WIN32)
        /* no need to destroy in windows */
#else
        pthread_cond_destroy(&m_cond);
#endif
    }

    bool signal()
    {
#if defined(WIN32)
        ::WakeConditionVariable(&m_cond);
        return true;
#else
        return 0 == pthread_cond_signal(&m_cond);
#endif
    }

    bool broadcast()
    {
#if defined(WIN32)
        ::WakeAllConditionVariable(&m_cond);
        return true;
#else
        return 0 == pthread_cond_broadcast(&m_cond);
#endif
    }

    /**
     * release lock, wait for signal or interrupt, lock and wake
     */
    bool wait(Mutex &mutex)
    {
#if defined(WIN32)
        return TRUE == ::SleepConditionVariableCS(&m_cond,mutex.innerMutex(), INFINITE);
#else
        return 0 == pthread_cond_wait(&m_cond, mutex.innerMutex());
#endif
    }

    /**
     * work the same as above
     */
    bool timedwait(Mutex &mutex, unsigned s, unsigned ms = 0)
    {
#if defined(WIN32)
        DWORD dwMilliseconds = s * 1000 + ms;
        return TRUE == ::SleepConditionVariableCS(&m_cond,mutex.innerMutex(), dwMilliseconds);
#else
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += s;
        abstime.tv_nsec += ((long)ms) * 1000 * 1000;
        return 0 == pthread_cond_timedwait(&m_cond, mutex.innerMutex(), &abstime);
#endif
    }
};

}

#endif /* head file guarder */

