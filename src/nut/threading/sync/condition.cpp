
#include <assert.h>
#include <time.h>

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
#   include <windows.h>
#   include "spinlock.h"
#elif NUT_PLATFORM_OS_MAC
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
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::InitializeConditionVariable(&_cond);
#else
    const int rs = ::pthread_cond_init(&_cond, nullptr);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

Condition::~Condition()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    /* no need to destroy in windows */
#else
    const int rs = ::pthread_cond_destroy(&_cond);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

bool Condition::signal()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::WakeConditionVariable(&_cond);
    return true;
#else
    return 0 == ::pthread_cond_signal(&_cond);
#endif
}

bool Condition::broadcast()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::WakeAllConditionVariable(&_cond);
    return true;
#else
    return 0 == ::pthread_cond_broadcast(&_cond);
#endif
}

/**
 * release lock, wait for signal or interrupt, lock and wake
 */
bool Condition::wait(condition_lock_type *mutex)
{
    assert(nullptr != mutex);

#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    return FALSE != ::SleepConditionVariableCS(&_cond, mutex->inner_mutex(), INFINITE);
#else
    return 0 == ::pthread_cond_wait(&_cond, mutex->inner_mutex());
#endif
}

/**
 * work the same as above
 */
bool Condition::timedwait(condition_lock_type *mutex, unsigned s, unsigned ms)
{
    assert(nullptr != mutex);

#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    const DWORD milli_seconds = s * 1000 + ms;
    return FALSE != ::SleepConditionVariableCS(&_cond, mutex->inner_mutex(), milli_seconds);
#else
    struct timespec abstime;
#   if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_MINGW
    clock_getrealtime(&abstime);
#   elif NUT_PLATFORM_OS_MAC
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
    return 0 == ::pthread_cond_timedwait(&_cond, mutex->inner_mutex(), &abstime);
#endif
}

}
