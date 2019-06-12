
#ifndef ___HEADFILE_D7B6E0B3_59D4_458E_A8EB_0878F6F42145_
#define ___HEADFILE_D7B6E0B3_59D4_458E_A8EB_0878F6F42145_

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
#   include <windows.h>
#else
#   include <pthread.h>
#endif

#include "../../nut_config.h"


namespace nut
{

/**
 * 自旋锁
 *
 * NOTE 自旋锁不支持重入
 */
class NUT_API SpinLock
{
public:
    SpinLock() noexcept;
    ~SpinLock() noexcept;

    void lock() noexcept;
    bool trylock() noexcept;
    void unlock() noexcept;

#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    CRITICAL_SECTION* inner_mutex() noexcept;
#elif NUT_PLATFORM_OS_MACOS
    pthread_mutex_t* inner_mutex() noexcept;
#else
    pthread_spinlock_t* inner_mutex() noexcept;
#endif

private:
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;

private:
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    CRITICAL_SECTION _critical_section;
#elif NUT_PLATFORM_OS_MACOS
    pthread_mutex_t _spinlock; // TODO mac 系统没有spinlock
#else
    pthread_spinlock_t _spinlock;
#endif
};

}

#endif /* head file guarder */
