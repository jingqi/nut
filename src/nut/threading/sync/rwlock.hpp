/**
 * @file -
 * @author jingqi
 * @date 2012-03-06
 * @last-edit 2015-01-06 23:54:12 jingqi
 */

#ifndef ___HEADFILE_409C4713_58BB_467B_A1CB_A8BC6EDA6CFE_
#define ___HEADFILE_409C4713_58BB_467B_A1CB_A8BC6EDA6CFE_

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
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
    SRWLOCK m_rwlock;
#else
    pthread_rwlock_t m_rwlock;
#endif

public:
    RwLock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::InitializeSRWLock(&m_rwlock);
#else
        const int rs = ::pthread_rwlock_init(&m_rwlock, NULL);
        assert(0 == rs);
#endif
    }

    ~RwLock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        /** SRWLock 无需删除或销毁，系统自动执行清理工作 */
#else
        const int rs = ::pthread_rwlock_destroy(&m_rwlock);
        assert(0 == rs);
#endif
    }

    inline void lock_read()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::AcquireSRWLockShared(&m_rwlock);
#else
        const int rs = ::pthread_rwlock_rdlock(&m_rwlock);
        assert(0 == rs);
#endif
    }

    inline bool try_lock_read()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        return FALSE != ::TryAcquireSRWLockShared(&m_rwlock);
#else
        return 0 == ::pthread_rwlock_tryrdlock(&m_rwlock);
#endif
    }

    inline void lock_write()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::AcquireSRWLockExclusive(&m_rwlock);
#else
        const int rs = pthread_rwlock_wrlock(&m_rwlock);
        assert(0 == rs);
#endif
    }

    inline bool try_lock_write()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        return FALSE != ::TryAcquireSRWLockExclusive(&m_rwlock);
#else
        return 0 == pthread_rwlock_trywrlock(&m_rwlock);
#endif
    }

    inline void unlock_read()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::ReleaseSRWLockShared(&m_rwlock);
#else
        const int rs = pthread_rwlock_unlock(&m_rwlock);
        assert(0 == rs);
#endif
    }

    inline void unlock_write()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
        ::ReleaseSRWLockExclusive(&m_rwlock);
#else
        const int rs = pthread_rwlock_unlock(&m_rwlock);
        assert(0 == rs);
#endif
    }
};

}

#endif /* head file guarder */
