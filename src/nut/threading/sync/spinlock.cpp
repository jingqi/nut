
#include <assert.h>
#include <nut/platform/platform.h>

#include "spinlock.h"

namespace nut
{

SpinLock::SpinLock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::InitializeCriticalSection(&m_criticalSection);
#elif defined(NUT_PLATFORM_OS_MAC)
    ::pthread_mutexattr_t attr;
    ::pthread_mutexattr_init(&attr);
    ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); /* make the mutex recursive */
    const int rs = ::pthread_mutex_init(&m_spinlock, &attr);
    assert(0 == rs);
#else
    const int rs = ::pthread_spin_init(&m_spinlock, PTHREAD_PROCESS_PRIVATE);
    assert(0 == rs);
#endif
}

SpinLock::~SpinLock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::DeleteCriticalSection(&m_criticalSection);
#elif defined(NUT_PLATFORM_OS_MAC)
    const int rs = ::pthread_mutex_destroy(&m_spinlock);
    assert(0 == rs);
#else
    const int rs = ::pthread_spin_destroy(&m_spinlock);
    assert(0 == rs);
#endif
}

void SpinLock::lock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::EnterCriticalSection(&m_criticalSection);
#elif defined(NUT_PLATFORM_OS_MAC)
    const int rs = ::pthread_mutex_lock(&m_spinlock);
    assert(0 == rs);
#else
    const int rs = ::pthread_spin_lock(&m_spinlock);
    assert(0 == rs);
#endif
}

bool SpinLock::trylock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    return FALSE != ::TryEnterCriticalSection(&m_criticalSection);
#elif defined(NUT_PLATFORM_OS_MAC)
    return 0 == ::pthread_mutex_trylock(&m_spinlock);
#else
    return 0 == ::pthread_spin_trylock(&m_spinlock);
#endif
}

void SpinLock::unlock()
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    ::LeaveCriticalSection(&m_criticalSection);
#elif defined(NUT_PLATFORM_OS_MAC)
    const int rs = ::pthread_mutex_unlock(&m_spinlock);
    assert(0 == rs);
#else
    const int rs = ::pthread_spin_unlock(&m_spinlock);
    assert(0 == rs);
#endif
}

}
