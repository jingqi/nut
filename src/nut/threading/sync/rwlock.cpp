
#include <assert.h>
#include <nut/platform/platform.h>

#include "rwlock.h"

namespace nut
{

RwLock::RwLock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::InitializeSRWLock(&m_rwlock);
#else
    const int rs = ::pthread_rwlock_init(&m_rwlock, NULL);
    assert(0 == rs);
#endif
}

RwLock::~RwLock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    /** SRWLock 无需删除或销毁，系统自动执行清理工作 */
#else
    const int rs = ::pthread_rwlock_destroy(&m_rwlock);
    assert(0 == rs);
#endif
}

void RwLock::lock_read()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::AcquireSRWLockShared(&m_rwlock);
#else
    const int rs = ::pthread_rwlock_rdlock(&m_rwlock);
    assert(0 == rs);
#endif
}

bool RwLock::try_lock_read()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    return FALSE != ::TryAcquireSRWLockShared(&m_rwlock);
#else
    return 0 == ::pthread_rwlock_tryrdlock(&m_rwlock);
#endif
}

void RwLock::lock_write()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::AcquireSRWLockExclusive(&m_rwlock);
#else
    const int rs = pthread_rwlock_wrlock(&m_rwlock);
    assert(0 == rs);
#endif
}

bool RwLock::try_lock_write()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    return FALSE != ::TryAcquireSRWLockExclusive(&m_rwlock);
#else
    return 0 == pthread_rwlock_trywrlock(&m_rwlock);
#endif
}

void RwLock::unlock_read()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::ReleaseSRWLockShared(&m_rwlock);
#else
    const int rs = pthread_rwlock_unlock(&m_rwlock);
    assert(0 == rs);
#endif
}

void RwLock::unlock_write()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::ReleaseSRWLockExclusive(&m_rwlock);
#else
    const int rs = pthread_rwlock_unlock(&m_rwlock);
    assert(0 == rs);
#endif
}

}
