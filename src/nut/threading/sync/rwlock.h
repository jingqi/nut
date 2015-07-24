
#ifndef ___HEADFILE_409C4713_58BB_467B_A1CB_A8BC6EDA6CFE_
#define ___HEADFILE_409C4713_58BB_467B_A1CB_A8BC6EDA6CFE_

#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
#   include <windows.h>
#else
#   include <pthread.h>
#endif

namespace nut
{

class RwLock
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    /** windows下的共享锁是在Windows Vista/Server 2008极其以后的版本中提供的 */
    SRWLOCK _rwlock;
#else
    pthread_rwlock_t _rwlock;
#endif

public:
    RwLock();

    ~RwLock();

    void lock_read();

    bool try_lock_read();

    void lock_write();

    bool try_lock_write();

    void unlock_read();

    void unlock_write();
};

}

#endif /* head file guarder */
