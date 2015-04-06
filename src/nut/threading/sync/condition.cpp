
#include <assert.h>
#include <time.h>

#include <nut/platform/platform.h>

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

#include "condition.h"

namespace nut
{

Condition::Condition()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::InitializeConditionVariable(&m_cond);
#else
    const int rs = ::pthread_cond_init(&m_cond, NULL);
    assert(0 == rs);
#endif
}

Condition::~Condition()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    /* no need to destroy in windows */
#else
    const int rs = ::pthread_cond_destroy(&m_cond);
    assert(0 == rs);
#endif
}

bool Condition::signal()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::WakeConditionVariable(&m_cond);
    return true;
#else
    return 0 == ::pthread_cond_signal(&m_cond);
#endif
}

bool Condition::broadcast()
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
bool Condition::wait(condition_lock_type *mutex)
{
    assert(NULL != mutex);

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    return FALSE != ::SleepConditionVariableCS(&m_cond, mutex->inner_mutex(), INFINITE);
#else
    return 0 == ::pthread_cond_wait(&m_cond, mutex->inner_mutex());
#endif
}

/**
 * work the same as above
 */
bool Condition::timedwait(condition_lock_type *mutex, unsigned s, unsigned ms)
{
    assert(NULL != mutex);

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    const DWORD milli_seconds = s * 1000 + ms;
    return FALSE != ::SleepConditionVariableCS(&m_cond, mutex->inner_mutex(), milli_seconds);
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
    return 0 == ::pthread_cond_timedwait(&m_cond, mutex->inner_mutex(), &abstime);
#endif
}

}
