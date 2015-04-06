
#ifndef ___HEADFILE_D7B6E0B3_59D4_458E_A8EB_0878F6F42145_
#define ___HEADFILE_D7B6E0B3_59D4_458E_A8EB_0878F6F42145_

#include <nut/platform/platform.h>

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
    SpinLock();

    ~SpinLock();

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

    void lock();

    bool trylock();

    void unlock();
};

}

#endif /* head file guarder */
