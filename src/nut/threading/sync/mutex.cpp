
#include <assert.h>

#include <nut/platform/platform.h>
#include <nut/util/time/time_val.h>

#if defined(NUT_PLATFORM_OS_MAC)
#   include <errno.h>
#endif

#include "mutex.h"

namespace nut
{

Mutex::Mutex()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    _hmutex = ::CreateMutex(NULL, FALSE, NULL);
    assert(NULL != _hmutex);
#elif defined(NUT_PLATFORM_OS_MAC)
    ::pthread_mutexattr_t attr;
    ::pthread_mutexattr_init(&attr);
    ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); /* make the mutex recursive */
    const int rs = ::pthread_mutex_init(&_mutex, &attr);
    assert(0 == rs);
    UNUSED(rs);
#else
    ::pthread_mutexattr_t attr;
    ::pthread_mutexattr_init(&attr);
    ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP); /* make the mutex recursive */
    const int rs = ::pthread_mutex_init(&_mutex, &attr);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

Mutex::~Mutex ()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    const BOOL rs = ::CloseHandle(_hmutex);
    assert(FALSE != rs);
    UNUSED(rs);
#else
    int rs = ::pthread_mutex_destroy(&_mutex);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

/**
 * lock the mutex, which may blocked the thread
 */
void Mutex::lock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    const DWORD rs = ::WaitForSingleObject(_hmutex, INFINITE);
    assert(WAIT_OBJECT_0 == rs);
    UNUSED(rs);
#else
    const int rs = ::pthread_mutex_lock(&_mutex);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

/**
 * unlock the mutex
 */
void Mutex::unlock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    const BOOL rs = ::ReleaseMutex(_hmutex);
    assert(FALSE != rs);
    UNUSED(rs);
#else
    const int rs = ::pthread_mutex_unlock(&_mutex);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

/**
 * try lock the mutex
 *
 * @return
 *      true, if lock successed
 */
bool Mutex::trylock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    return WAIT_OBJECT_0 == ::WaitForSingleObject(_hmutex, 0);
#else
    const int lock_result = ::pthread_mutex_trylock(&_mutex);
    /** returned values :
     *  0, lock ok
     *  EBUSY, The mutex is already locked.
     *  EINVAL, Mutex is not an initialized mutex.
     *  EFAULT, Mutex is an invalid pointer.
     */
    return 0 == lock_result;
#endif
}

#if defined(NUT_PLATFORM_OS_MAC)
/**
 * MAC 不支持 pthread_mutex_timedlock()
 * see http://lists.apple.com/archives/xcode-users/2007/Apr/msg00331.html
 */
static int _pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timeval *abs_timeout)
{
    assert(NULL != mutex && NULL != abs_timeout);

    while (true)
    {
        const int result = ::pthread_mutex_trylock(mutex);
        if (EBUSY != result)
            return result;

        // Sleep for 10,000,000 nanoseconds before trying again
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 10000000;
        while (-1 == ::nanosleep(&ts, &ts))
        {}

        if (0 != abs_timeout->tv_sec || 0 != abs_timeout->tv_usec) // 0 for infinitive
        {
            struct timeval now;
            ::gettimeofday(&now, NULL); // MAC 不支持 clock_gettime()
            const long long v = (((long long) now.tv_sec) - abs_timeout->tv_sec) * 1000 * 1000 +
                (now.tv_usec - abs_timeout->tv_usec);
            if (v >= 0) // timeout
                return result;
        }
    }
}
#endif

/**
 * try lock the mutex in given time
 *
 * @param s
 *      The timeout value in seconds
 * @param ms
 *      The timeout value in milliseconds
 * @return
 *      true, if lock successed
 */
bool Mutex::timedlock(unsigned s, unsigned ms)
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    const DWORD dw_milliseconds = s * 1000 + ms;
    return WAIT_OBJECT_0 == ::WaitForSingleObject(_hmutex, dw_milliseconds);
#elif defined(NUT_PLATFORM_OS_MAC)
    struct timeval abstime;
    ::gettimeofday(&abstime, NULL);
    abstime.tv_sec += s;
    abstime.tv_usec += ((long)ms) * 1000;
    return 0 == _pthread_mutex_timedlock(&_mutex, &abstime);
#else
    struct timespec abstime;
#   if defined(NUT_PLATFORM_OS_WINDOWS) && defined(NUT_PLATFORM_CC_MINGW)
    clock_getrealtime(&abstime);
#   else
    ::clock_gettime(CLOCK_REALTIME, &abstime);
#   endif
    abstime.tv_sec += s;
    abstime.tv_nsec += ((long)ms) * 1000 * 1000;
    const int lock_result = ::pthread_mutex_timedlock(&_mutex, &abstime);
    /** returned values :
     *  0, lock ok
     *  EAGAIN, The mutex couldn't be acquired because the maximum number of recursive locks for the mutex has been exceeded.
     *  EDEADLK, The current thread already owns the mutex.
     *  EINVAL, The mutex was created with the protocol attribute having the value PTHREAD_PRIO_PROTECT and the calling thread's priority is higher than the mutex' current priority ceiling; the process or thread would have blocked, and the abs_timeout parameter specified a nanoseconds field value less than zero or greater than or equal to 1000 million; or the value specified by mutex doesn't refer to an initialized mutex object.
     *  ETIMEDOUT, The mutex couldn't be locked before the specified timeout expired
     */
    return 0 == lock_result;
#endif
}

}
