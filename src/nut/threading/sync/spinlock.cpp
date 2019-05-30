
#include <assert.h>

#include "../../platform/platform.h"
#include "spinlock.h"


namespace nut
{

SpinLock::SpinLock()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::InitializeCriticalSection(&_critical_section);
#elif NUT_PLATFORM_OS_MACOS
    ::pthread_mutexattr_t attr;
    ::pthread_mutexattr_init(&attr);
    // ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); // 重入
    const int rs = ::pthread_mutex_init(&_spinlock, &attr);
    assert(0 == rs);
    UNUSED(rs);
#else
    // pthread_spinlock_t 不支持重入
    const int rs = ::pthread_spin_init(&_spinlock, PTHREAD_PROCESS_PRIVATE);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

SpinLock::~SpinLock()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::DeleteCriticalSection(&_critical_section);
#elif NUT_PLATFORM_OS_MACOS
    const int rs = ::pthread_mutex_destroy(&_spinlock);
    assert(0 == rs);
    UNUSED(rs);
#else
    const int rs = ::pthread_spin_destroy(&_spinlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
CRITICAL_SECTION* SpinLock::inner_mutex()
{
    return &_critical_section;
}
#elif NUT_PLATFORM_OS_MACOS
pthread_mutex_t* SpinLock::inner_mutex()
{
    return &_spinlock;
}
#else
pthread_spinlock_t* SpinLock::inner_mutex()
{
    return &_spinlock;
}
#endif

void SpinLock::lock()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::EnterCriticalSection(&_critical_section);
#elif NUT_PLATFORM_OS_MACOS
    const int rs = ::pthread_mutex_lock(&_spinlock);
    assert(0 == rs);
    UNUSED(rs);
#else
    const int rs = ::pthread_spin_lock(&_spinlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

bool SpinLock::trylock()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    return FALSE != ::TryEnterCriticalSection(&_critical_section);
#elif NUT_PLATFORM_OS_MACOS
    return 0 == ::pthread_mutex_trylock(&_spinlock);
#else
    return 0 == ::pthread_spin_trylock(&_spinlock);
#endif
}

void SpinLock::unlock()
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::LeaveCriticalSection(&_critical_section);
#elif NUT_PLATFORM_OS_MACOS
    const int rs = ::pthread_mutex_unlock(&_spinlock);
    assert(0 == rs);
    UNUSED(rs);
#else
    const int rs = ::pthread_spin_unlock(&_spinlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

}
