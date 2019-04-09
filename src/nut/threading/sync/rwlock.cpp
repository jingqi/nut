
#include <assert.h>

#include "../../platform/platform.h"
#include "rwlock.h"


namespace nut
{

RwLock::RwLock()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::InitializeSRWLock(&_rwlock);
#else
    const int rs = ::pthread_rwlock_init(&_rwlock, nullptr);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

RwLock::~RwLock()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    /** SRWLock 无需删除或销毁，系统自动执行清理工作 */
#else
    const int rs = ::pthread_rwlock_destroy(&_rwlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

void RwLock::lock_read()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::AcquireSRWLockShared(&_rwlock);
#else
    const int rs = ::pthread_rwlock_rdlock(&_rwlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

bool RwLock::try_lock_read()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    return FALSE != ::TryAcquireSRWLockShared(&_rwlock);
#else
    return 0 == ::pthread_rwlock_tryrdlock(&_rwlock);
#endif
}

void RwLock::unlock_read()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::ReleaseSRWLockShared(&_rwlock);
#else
    const int rs = pthread_rwlock_unlock(&_rwlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

void RwLock::lock_write()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::AcquireSRWLockExclusive(&_rwlock);
#else
    const int rs = pthread_rwlock_wrlock(&_rwlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

bool RwLock::try_lock_write()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    return FALSE != ::TryAcquireSRWLockExclusive(&_rwlock);
#else
    return 0 == pthread_rwlock_trywrlock(&_rwlock);
#endif
}

void RwLock::unlock_write()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::ReleaseSRWLockExclusive(&_rwlock);
#else
    const int rs = pthread_rwlock_unlock(&_rwlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

}
