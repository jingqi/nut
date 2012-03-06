/**
 * @file -
 * @author jingqi
 * @date 2012-03-06
 * @last-edit 2012-03-06 13:03:12 jingqi
 */

#ifndef ___HEADFILE_D7B6E0B3_59D4_458E_A8EB_0878F6F42145_
#define ___HEADFILE_D7B6E0B3_59D4_458E_A8EB_0878F6F42145_

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <pthread.h>
#endif

class SpinLock
{
#if defined(NUT_PLATFORM_OS_WINDOWS)
    /** windows下没有单独的用户态自旋锁，而临界区是自旋锁和互斥量的结合体，
     加锁时先自旋一段时间，之后就进入了互斥量，这里聊以代替自旋锁 */
    CRITICAL_SECTION m_criticalSection;
#else
    pthread_spinlock_t m_spinlock;
#endif

public:
    SpinLock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::InitializeCriticalSection(&m_criticalSection);
#else
        pthread_spin_init(&m_spinlock, NULL);
#endif
    }

    ~SpinLock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::DeleteCriticalSection(&m_criticalSection);
#else
        pthread_spin_destroy(&m_spinlock);
#endif
}

    inline void lock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::EnterCriticalSection(&m_criticalSection);
#else
        int rs = pthread_spin_lock(&m_spinlock);
        assert(0 == rs);
#endif
    }

    inline bool trylock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return TRUE == ::TryEnterCriticalSection(&m_criticalSection);
#else
        return 0 == pthread_spin_trylock(&m_spinlock);
#endif
    }

    inline void unlock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::LeaveCriticalSection(&m_criticalSection);
#else
        int rs = pthread_spin_unlock(&m_spinlock);
        assert(0 == rs);
#endif
}

};

#endif /* head file guarder */

