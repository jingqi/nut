
#ifndef ___HEADFILE_409C4713_58BB_467B_A1CB_A8BC6EDA6CFE_
#define ___HEADFILE_409C4713_58BB_467B_A1CB_A8BC6EDA6CFE_

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
#   include <windows.h>
#else
#   include <pthread.h>
#endif

#include "../../nut_config.h"


namespace nut
{

class NUT_API RwLock
{
public:
    RwLock() noexcept;
    ~RwLock() noexcept;

    void lock_read() noexcept;
    bool try_lock_read() noexcept;
    void unlock_read() noexcept;

    void lock_write() noexcept;
    bool try_lock_write() noexcept;
    void unlock_write() noexcept;

private:
    RwLock(const RwLock&) = delete;
    RwLock& operator=(const RwLock&) = delete;

private:
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    /** NOTE windows 下的共享锁是在 Windows Vista/Server 2008 及其以后的版本中提供的 */
    SRWLOCK _rwlock;
#else
    pthread_rwlock_t _rwlock;
#endif
};

}

#endif /* head file guarder */
