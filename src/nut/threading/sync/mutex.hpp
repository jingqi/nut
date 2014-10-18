/**
 * @file -
 * @author jingqi
 * @date 2010-7-9
 * @last-edit 2014-10-19 00:48:56 jingqi
 */

#ifndef ___HEADFILE___814FCD2E_2F65_4787_93E5_ECDE19588938_
#define ___HEADFILE___814FCD2E_2F65_4787_93E5_ECDE19588938_

#include <assert.h>
#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#  include <windows.h>
#endif

#if !defined(NUT_PLATFORM_OS_WINDOWS) || defined(NUT_PLATFORM_CC_MINGW)
#  include <pthread.h>
#endif

namespace nut
{

class Mutex
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    HANDLE m_hmutex;
#else
    pthread_mutex_t m_mutex;
#endif

public :
    Mutex ()
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

    ~Mutex ()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        const BOOL rs = ::CloseHandle(m_hmutex);
        assert(FALSE != rs);
#else
        int rs = ::pthread_mutex_destroy(&m_mutex);
        assert(0 == rs);
#endif
    }

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    inline HANDLE innerMutex()
    {
        return m_hmutex;
    }
#else
    inline pthread_mutex_t* innerMutex()
    {
        return &m_mutex;
    }
#endif

    /**
     * lock the mutex, which may blocked the thread
     */
    inline void lock()
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
    inline void unlock()
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
    inline bool trylock()
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

#if defined(NUT_PLATFORM_OS_WINDOWS) && defined(NUT_PLATFORM_CC_MINGW)
    /** time between jan 1, 1601 and jan 1, 1970 in units of 100 nanoseconds */
#   define PTW32_TIMESPEC_TO_FILETIME_OFFSET (LONGLONG)((((LONGLONG) 27111902LL << 32)+(LONGLONG) 3577643008LL ))
    /**
     * mingw 没有定义clock_gettime(), 这里参考其pthread_mutex_timedlock.c ptw32_relmillisecs.c 的实现
     */
    static void clock_getrealtime(struct timespec *ts)
    {
        assert(NULL != ts);

        SYSTEMTIME st;
        ::GetSystemTime(&st);
        FILETIME ft;
        ::SystemTimeToFileTime(&st, &ft);
        ts->tv_sec = (int)((*(LONGLONG *)&ft - PTW32_TIMESPEC_TO_FILETIME_OFFSET) / 10000000LL);
        ts->tv_nsec = (int)((*(LONGLONG *)&ft - PTW32_TIMESPEC_TO_FILETIME_OFFSET - ((LONGLONG)ts->tv_sec * (LONGLONG)10000000LL)) * 100);
    }
#   undef PTW32_TIMESPEC_TO_FILETIME_OFFSET
#endif

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
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        const DWORD dwMilliseconds = s * 1000 + ms;
        return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hmutex, dwMilliseconds);
#elif defined(NUT_PLATFORM_OS_MAC)
#   warning "MAC 不支持pthread_mutex_timedlock()"
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
};

}

#endif /* head file guarder */
