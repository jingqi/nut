/**
 * @file -
 * @author jingqi
 * @date 2010-07-09
 */

#ifndef ___HEADFILE___B926495D_967A_45A2_8F56_4FFB10F2E34B_
#define ___HEADFILE___B926495D_967A_45A2_8F56_4FFB10F2E34B_

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <pthread.h>
#endif

#include "mutex.hpp"

namespace nut
{

class Condition
{
#if defined(NUT_PLATFORM_OS_WINDOWS)
    CONDITION_VARIABLE m_cond;
#else
    pthread_cond_t m_cond;
#endif

public :
    Condition()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::InitializeConditionVariable(&m_cond);
#else
        pthread_cond_init(&m_cond, NULL);
#endif
    }

    ~Condition()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        /* no need to destroy in windows */
#else
        pthread_cond_destroy(&m_cond);
#endif
    }

    bool signal()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::WakeConditionVariable(&m_cond);
        return true;
#else
        return 0 == pthread_cond_signal(&m_cond);
#endif
    }

    bool broadcast()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
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
#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(USE_CRITICAL_SECTION)
        return TRUE == ::SleepConditionVariableCS(&m_cond,mutex.innerMutex(), INFINITE);
#   else
#        error not supported!
#   endif
#else
        return 0 == pthread_cond_wait(&m_cond, mutex.innerMutex());
#endif
    }

    /**
     * work the same as above
     */
    bool timedwait(Mutex &mutex, unsigned s, unsigned ms = 0)
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(USE_CRITICAL_SECTION)
        DWORD dwMilliseconds = s * 1000 + ms;
        return TRUE == ::SleepConditionVariableCS(&m_cond, mutex.innerMutex(), dwMilliseconds);
#   else
#        error not supported!
#   endif
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

