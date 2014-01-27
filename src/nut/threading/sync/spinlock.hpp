/**
 * @file -
 * @author jingqi
 * @date 2012-03-06
 * @last-edit 2012-08-19 18:44:41 jingqi
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

class SpinLock
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    CRITICAL_SECTION m_criticalSection;
#else
    pthread_spinlock_t m_spinlock;
#endif

public:
    SpinLock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::InitializeCriticalSection(&m_criticalSection);
#else
        int rs = ::pthread_spin_init(&m_spinlock, PTHREAD_PROCESS_PRIVATE);
        assert(0 == rs);
#endif
    }

    ~SpinLock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::DeleteCriticalSection(&m_criticalSection);
#else
        int rs = ::pthread_spin_destroy(&m_spinlock);
        assert(0 == rs);
#endif
    }

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    inline CRITICAL_SECTION* innerMutex() { return &m_criticalSection; }
#else
    inline pthread_spinlock_t* innerMutex() { return &m_spinlock; }
#endif

    inline void lock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::EnterCriticalSection(&m_criticalSection);
#else
        int rs = ::pthread_spin_lock(&m_spinlock);
        assert(0 == rs);
#endif
    }

    inline bool trylock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        return TRUE == ::TryEnterCriticalSection(&m_criticalSection);
#else
        return 0 == ::pthread_spin_trylock(&m_spinlock);
#endif
    }

    inline void unlock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::LeaveCriticalSection(&m_criticalSection);
#else
        int rs = ::pthread_spin_unlock(&m_spinlock);
        assert(0 == rs);
#endif
}

};

#endif /* head file guarder */

