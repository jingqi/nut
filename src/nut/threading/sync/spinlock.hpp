/**
 * @file -
 * @author jingqi
 * @date 2012-03-06
 * @last-edit 2015-01-06 23:54:37 jingqi
 */

#ifndef ___HEADFILE_D7B6E0B3_59D4_458E_A8EB_0878F6F42145_
#define ___HEADFILE_D7B6E0B3_59D4_458E_A8EB_0878F6F42145_

#include <assert.h>
#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
#   include <windows.h>
#else
#   include <pthread.h>
#endif

namespace nut
{

class SpinLock
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    CRITICAL_SECTION m_criticalSection;
#elif defined(NUT_PLATFORM_OS_MAC)
    pthread_mutex_t m_spinlock; // TODO mac 系统没有spinlock
#else
    pthread_spinlock_t m_spinlock;
#endif

public:
    SpinLock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::InitializeCriticalSection(&m_criticalSection);
#elif defined(NUT_PLATFORM_OS_MAC)
        ::pthread_mutexattr_t attr;
        ::pthread_mutexattr_init(&attr);
        ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); /* make the mutex recursive */
        const int rs = ::pthread_mutex_init(&m_spinlock, &attr);
        assert(0 == rs);
#else
        const int rs = ::pthread_spin_init(&m_spinlock, PTHREAD_PROCESS_PRIVATE);
        assert(0 == rs);
#endif
    }

    ~SpinLock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::DeleteCriticalSection(&m_criticalSection);
#elif defined(NUT_PLATFORM_OS_MAC)
        const int rs = ::pthread_mutex_destroy(&m_spinlock);
        assert(0 == rs);
#else
        const int rs = ::pthread_spin_destroy(&m_spinlock);
        assert(0 == rs);
#endif
    }

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    CRITICAL_SECTION* inner_mutex()
    {
        return &m_criticalSection;
    }
#elif defined(NUT_PLATFORM_OS_MAC)
    pthread_mutex_t* inner_mutex()
    {
        return &m_spinlock;
    }
#else
    pthread_spinlock_t* inner_mutex()
    {
        return &m_spinlock;
    }
#endif

    void lock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::EnterCriticalSection(&m_criticalSection);
#elif defined(NUT_PLATFORM_OS_MAC)
        const int rs = ::pthread_mutex_lock(&m_spinlock);
        assert(0 == rs);
#else
        const int rs = ::pthread_spin_lock(&m_spinlock);
        assert(0 == rs);
#endif
    }

    bool trylock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        return FALSE != ::TryEnterCriticalSection(&m_criticalSection);
#elif defined(NUT_PLATFORM_OS_MAC)
        return 0 == ::pthread_mutex_trylock(&m_spinlock);
#else
        return 0 == ::pthread_spin_trylock(&m_spinlock);
#endif
    }

    void unlock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::LeaveCriticalSection(&m_criticalSection);
#elif defined(NUT_PLATFORM_OS_MAC)
        const int rs = ::pthread_mutex_unlock(&m_spinlock);
        assert(0 == rs);
#else
        const int rs = ::pthread_spin_unlock(&m_spinlock);
        assert(0 == rs);
#endif
    }
};

}

#endif /* head file guarder */
