
#include <assert.h>
#include <nut/platform/platform.h>

#include "spinlock.h"

namespace nut
{

SpinLock::SpinLock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::InitializeCriticalSection(&_critical_section);
#elif defined(NUT_PLATFORM_OS_MAC)
    ::pthread_mutexattr_t attr;
    ::pthread_mutexattr_init(&attr);
    ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); /* make the mutex recursive */
    const int rs = ::pthread_mutex_init(&_spinlock, &attr);
    assert(0 == rs);
    UNUSED(rs);
#else
    const int rs = ::pthread_spin_init(&_spinlock, PTHREAD_PROCESS_PRIVATE);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

SpinLock::~SpinLock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::DeleteCriticalSection(&_critical_section);
#elif defined(NUT_PLATFORM_OS_MAC)
    const int rs = ::pthread_mutex_destroy(&_spinlock);
    assert(0 == rs);
    UNUSED(rs);
#else
    const int rs = ::pthread_spin_destroy(&_spinlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

void SpinLock::lock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::EnterCriticalSection(&_critical_section);
#elif defined(NUT_PLATFORM_OS_MAC)
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
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    return FALSE != ::TryEnterCriticalSection(&_critical_section);
#elif defined(NUT_PLATFORM_OS_MAC)
    return 0 == ::pthread_mutex_trylock(&_spinlock);
#else
    return 0 == ::pthread_spin_trylock(&_spinlock);
#endif
}

void SpinLock::unlock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::LeaveCriticalSection(&_critical_section);
#elif defined(NUT_PLATFORM_OS_MAC)
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
