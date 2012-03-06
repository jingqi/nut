/**
 * @file -
 * @author jingqi
 * @date 2012-03-06
 * @last-edit 2012-03-06 12:51:37 jingqi
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
#if defined(NUT_PLATFORM_OS_WINDOWS)
#else
    pthread_rwlock_t m_rwlock;
#endif

public:
    RwLock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#else
        int rs = pthread_rwlock_init(&m_rwlock, NULL);
        assert(0 == rs);
#endif
    }

    ~RwLock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#else
        int rs = pthread_rwlock_destroy(&m_rwlock);
#endif
    }

    inline void lockRead()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#else
        int rs = pthread_rwlock_rdlock(&m_rwlock);
        assert(0 == rs);
#endif
    }

    inline bool trylockRead()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#else
        return 0 == pthread_rwlock_tryrdlock(&m_rwlock);
#endif
    }

    inline void lockWrite()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#else
        int rs = pthread_rwlock_wrlock(&m_rwlock);
        assert(0 == rs);
#endif
    }

    inline bool trylockWrite()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#else
        return 0 == pthread_rwlock_trywrlock(&m_rwlock);
#endif
    }

    inline void unlock()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
#else
        int rs = pthread_rwlock_unlock(&m_rwlock);
        assert(0 == rs);
#endif
    }
};

}

#endif /* head file guarder */


