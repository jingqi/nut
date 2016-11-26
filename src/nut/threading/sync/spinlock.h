
#ifndef ___HEADFILE_D7B6E0B3_59D4_458E_A8EB_0878F6F42145_
#define ___HEADFILE_D7B6E0B3_59D4_458E_A8EB_0878F6F42145_

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
#   include <windows.h>
#else
#   include <pthread.h>
#endif

#include "../../nut_config.h"


namespace nut
{

class NUT_API SpinLock
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    CRITICAL_SECTION _critical_section;
#elif NUT_PLATFORM_OS_MAC
    pthread_mutex_t _spinlock; // TODO mac 系统没有spinlock
#else
    pthread_spinlock_t _spinlock;
#endif

public:
    SpinLock();

    ~SpinLock();

#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    CRITICAL_SECTION* inner_mutex()
    {
        return &_critical_section;
    }
#elif NUT_PLATFORM_OS_MAC
    pthread_mutex_t* inner_mutex()
    {
        return &_spinlock;
    }
#else
    pthread_spinlock_t* inner_mutex()
    {
        return &_spinlock;
    }
#endif

    void lock();

    bool trylock();

    void unlock();
};

}

#endif /* head file guarder */
