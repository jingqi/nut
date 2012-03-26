/**
 * @file -
 * @author jingqi
 * @date 2010-7-9
 */

#ifndef ___HEADFILE___814FCD2E_2F65_4787_93E5_ECDE19588938_
#define ___HEADFILE___814FCD2E_2F65_4787_93E5_ECDE19588938_

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#  include <windows.h>
#else
#  include <pthread.h>
#endif

#define USE_CRITICAL_SECTION

namespace nut
{

class Mutex
{
#if defined(NUT_PLATFORM_OS_WINDOWS)
    /** windows下:
        临界区只能线程间同步，不能进程间同步； 互斥量能够线程间、进程间(需要命名)同步。
        临界区可以喝信号量配合；互斥量不行。 */
#   if defined(USE_CRITICAL_SECTION)
    CRITICAL_SECTION m_criticalSection; // 使用临界区
#   else
    HANDLE m_hmutex; // 使用互斥量
#   endif
#else
    pthread_mutex_t m_mutex;
#endif

public :
    Mutex ()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(USE_CRITICAL_SECTION)
        ::InitializeCriticalSection(&m_criticalSection);
#   else
        m_hmutex = ::CreateMutex(NULL, FALSE, NULL);
#   endif
#else
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP); /* make the mutex recursive */
        pthread_mutex_init(&m_mutex, &attr);
#endif
    }

    ~Mutex ()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(USE_CRITICAL_SECTION)
        ::DeleteCriticalSection(&m_criticalSection);
#   else
        ::ReleaseMutex(m_hmutex);
#   endif
#else
        pthread_mutex_destroy(&m_mutex);
#endif
    }

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(USE_CRITICAL_SECTION)
    inline CRITICAL_SECTION innerMutex() { return m_criticalSection; }
#   else
    inline HANDLE innerMutex() { return m_hmutex; }
#   endif
#else
    inline pthread_mutex_t* innerMutex() { return &m_mutex; }
#endif

    /**
     * lock the mutex, which may blocked the thread
     */
    inline void lock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(USE_CRITICAL_SECTION)
        ::EnterCriticalSection(&m_criticalSection);
#   else
        ::WaitForSingleObject(m_hmutex, INFINITE);
#   endif
#else
        pthread_mutex_lock(&m_mutex);
#endif
    }

    /**
     * unlock the mutex
     */
    inline void unlock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(USE_CRITICAL_SECTION)
        ::LeaveCriticalSection(&m_criticalSection);
#   else
        ::ReleaseMutex(m_hmutex);
#   endif
#else
        pthread_mutex_unlock(&m_mutex);
#endif
    }

    /**
     * try lock the mutex
     * @return
     *      true, if lock successed
     */
    inline bool trylock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(USE_CRITICAL_SECTION)
        return TRUE == ::TryEnterCriticalSection(&m_criticalSection);
#   else
        return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hmutex, 0);
#   endif
#else
        int lock_result = pthread_mutex_trylock(&m_mutex);
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
    inline bool timedlock(unsigned s, unsigned ms = 0)
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(USE_CRITICAL_SECTION)
        return trylock();
#   else
        return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hmutex, ms);
#   endif
#else
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += s;
        abstime.tv_nsec += ((long)ms) * 1000 * 1000;
        int lock_result = pthread_mutex_timedlock(&m_mutex, &abstime);
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
};

}

#endif /* head file guarder */

