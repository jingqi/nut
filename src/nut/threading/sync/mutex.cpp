
#include <assert.h>

#include <nut/platform/platform.h>
#include <nut/util/time/time_val.h>


#include "mutex.h"

namespace nut
{

Mutex::Mutex()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    m_hmutex = ::CreateMutex(NULL, FALSE, NULL);
    assert(NULL != m_hmutex);
#elif defined(NUT_PLATFORM_OS_MAC)
    ::pthread_mutexattr_t attr;
    ::pthread_mutexattr_init(&attr);
    ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); /* make the mutex recursive */
    const int rs = ::pthread_mutex_init(&m_mutex, &attr);
    assert(0 == rs);
#else
    ::pthread_mutexattr_t attr;
    ::pthread_mutexattr_init(&attr);
    ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP); /* make the mutex recursive */
    const int rs = ::pthread_mutex_init(&m_mutex, &attr);
    assert(0 == rs);
#endif
}

Mutex::~Mutex ()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    const BOOL rs = ::CloseHandle(m_hmutex);
    assert(FALSE != rs);
#else
    int rs = ::pthread_mutex_destroy(&m_mutex);
    assert(0 == rs);
#endif
}

/**
 * lock the mutex, which may blocked the thread
 */
void Mutex::lock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    const DWORD rs = ::WaitForSingleObject(m_hmutex, INFINITE);
    assert(WAIT_OBJECT_0 == rs);
#else
    const int rs = ::pthread_mutex_lock(&m_mutex);
    assert(0 == rs);
#endif
}

/**
 * unlock the mutex
 */
void Mutex::unlock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    const BOOL rs = ::ReleaseMutex(m_hmutex);
    assert(FALSE != rs);
#else
    const int rs = ::pthread_mutex_unlock(&m_mutex);
    assert(0 == rs);
#endif
}

/**
 * try lock the mutex
 * @return
 *      true, if lock successed
 */
bool Mutex::trylock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hmutex, 0);
#else
    const int lock_result = ::pthread_mutex_trylock(&m_mutex);
    /** returned values :
     *  0, lock ok
     *  EBUSY, The mutex is already locked.
     *  EINVAL, Mutex is not an initialized mutex.
     *  EFAULT, Mutex is an invalid pointer.
     */
    return 0 == lock_result;
#endif
}

/**
 * try lock the mutex in given time
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
    return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hmutex, dw_milliseconds);
#elif defined(NUT_PLATFORM_OS_MAC)
#   warning FIXME MAC 不支持pthread_mutex_timedlock()
    return 0 == ::pthread_mutex_trylock(&m_mutex); // TODO MAC 不支持 pthread_mutex_timedlock()
#else
    struct timespec abstime;
#   if defined(NUT_PLATFORM_OS_WINDOWS) && defined(NUT_PLATFORM_CC_MINGW)
    clock_getrealtime(&abstime);
#   else
    ::clock_gettime(CLOCK_REALTIME, &abstime);
#   endif
    abstime.tv_sec += s;
    abstime.tv_nsec += ((long)ms) * 1000 * 1000;
    const int lock_result = ::pthread_mutex_timedlock(&m_mutex, &abstime);
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
