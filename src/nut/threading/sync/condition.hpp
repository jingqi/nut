/**
 * @file -
 * @author jingqi
 * @date 2010-07-09
 * @last-edit 2014-10-19 00:43:45 jingqi
 */

#ifndef ___HEADFILE___B926495D_967A_45A2_8F56_4FFB10F2E34B_
#define ___HEADFILE___B926495D_967A_45A2_8F56_4FFB10F2E34B_

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
#   include <windows.h>
#   include "spinlock.hpp"
#elif defined(NUT_PLATFORM_OS_MAC)
#   include <time.h>
#   include <sys/time.h>
#   include <mach/clock.h>
#   include <mach/mach.h>
#else
#   include <pthread.h>
#endif

#include "mutex.hpp"


namespace nut
{

class Condition
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    CONDITION_VARIABLE m_cond;
#else
    pthread_cond_t m_cond;
#endif

public:
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    typedef SpinLock condition_lock_type; // windows 下condition只能配合临界区
#else
    typedef Mutex condition_lock_type;
#endif

    Condition()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::InitializeConditionVariable(&m_cond);
#else
        const int rs = ::pthread_cond_init(&m_cond, NULL);
        assert(0 == rs);
#endif
    }

    ~Condition()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        /* no need to destroy in windows */
#else
        const int rs = ::pthread_cond_destroy(&m_cond);
        assert(0 == rs);
#endif
    }

    bool signal()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::WakeConditionVariable(&m_cond);
        return true;
#else
        return 0 == ::pthread_cond_signal(&m_cond);
#endif
    }

    bool broadcast()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::WakeAllConditionVariable(&m_cond);
        return true;
#else
        return 0 == ::pthread_cond_broadcast(&m_cond);
#endif
    }

    /**
     * release lock, wait for signal or interrupt, lock and wake
     */
    bool wait(condition_lock_type *mutex)
    {
        assert(NULL != mutex);

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        return FALSE != ::SleepConditionVariableCS(&m_cond, mutex->innerMutex(), INFINITE);
#else
        return 0 == ::pthread_cond_wait(&m_cond, mutex->innerMutex());
#endif
    }

    /**
     * work the same as above
     */
    bool timedwait(condition_lock_type *mutex, unsigned s, unsigned ms = 0)
    {
        assert(NULL != mutex);

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        const DWORD dwMilliseconds = s * 1000 + ms;
        return FALSE != ::SleepConditionVariableCS(&m_cond, mutex->innerMutex(), dwMilliseconds);
#else
        struct timespec abstime;
#   if defined(NUT_PLATFORM_OS_WINDOWS) && defined(NUT_PLATFORM_CC_MINGW)
        Mutex::clock_getrealtime(&abstime);
#   elif defined(NUT_PLATFORM_OS_MAC)
        // OS X does not have clock_gettime(), use clock_get_time()
        clock_serv_t cclock;
        mach_timespec_t mts;
        ::host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        ::clock_get_time(cclock, &mts);
        ::mach_port_deallocate(mach_task_self(), cclock);
        abstime.tv_sec = mts.tv_sec;
        abstime.tv_nsec = mts.tv_nsec;
#   else
        ::clock_gettime(CLOCK_REALTIME, &abstime);
#   endif

        abstime.tv_sec += s;
        abstime.tv_nsec += ((long)ms) * 1000 * 1000;
        return 0 == ::pthread_cond_timedwait(&m_cond, mutex->innerMutex(), &abstime);
#endif
    }
};

}

#endif /* head file guarder */
