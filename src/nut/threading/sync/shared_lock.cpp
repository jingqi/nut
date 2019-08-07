
#include <assert.h>
#include <thread>

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h> // for QueryPerformanceCounter()
#else
#   include <time.h> // for clock_gettime()
#endif

#include "shared_lock.h"


#if __cplusplus < 201703L

namespace nut
{

namespace
{

/**
 * 毫秒计数
 */
uint64_t get_ms_counter() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    LARGE_INTEGER counter, frequency;
    ::QueryPerformanceCounter(&counter);
    ::QueryPerformanceFrequency(&frequency);
    return counter.QuadPart * 1000 / frequency.QuadPart;
#elif NUT_PLATFORM_OS_LINUX
    struct timespec tv;
    ::clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
    uint64_t ret = tv.tv_sec;
    ret *= 1000;
    ret += tv.tv_nsec / 1000000;
    return ret;
#else
    struct timespec tv;
    ::clock_gettime(CLOCK_MONOTONIC, &tv);
    uint64_t ret = tv.tv_sec;
    ret *= 1000;
    ret += tv.tv_nsec / 1000000;
    return ret;
#endif
}

}

SharedLock::SharedLock() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::InitializeSRWLock(&_rwlock);
#else
    const int rs = ::pthread_rwlock_init(&_rwlock, nullptr);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

SharedLock::~SharedLock() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    /** SRWLock 无需删除或销毁，系统自动执行清理工作 */
#else
    const int rs = ::pthread_rwlock_destroy(&_rwlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

void SharedLock::lock() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::AcquireSRWLockExclusive(&_rwlock);
#else
    const int rs = pthread_rwlock_wrlock(&_rwlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

bool SharedLock::try_lock() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    return FALSE != ::TryAcquireSRWLockExclusive(&_rwlock);
#else
    return 0 == pthread_rwlock_trywrlock(&_rwlock);
#endif
}

bool SharedLock::try_lock_for_ms(uint64_t ms) noexcept
{
    const uint64_t until = get_ms_counter() + ms;
    while (!try_lock())
    {
        if (get_ms_counter() >= until)
            return false;
        std::this_thread::yield();
    }
    return true;
}

void SharedLock::unlock() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::ReleaseSRWLockExclusive(&_rwlock);
#else
    const int rs = pthread_rwlock_unlock(&_rwlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

void SharedLock::lock_shared() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::AcquireSRWLockShared(&_rwlock);
#else
    const int rs = ::pthread_rwlock_rdlock(&_rwlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

bool SharedLock::try_lock_shared() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    return FALSE != ::TryAcquireSRWLockShared(&_rwlock);
#else
    return 0 == ::pthread_rwlock_tryrdlock(&_rwlock);
#endif
}

bool SharedLock::try_lock_shared_for_ms(uint64_t ms) noexcept
{
    const uint64_t until = get_ms_counter() + ms;
    while (!try_lock_shared())
    {
        if (get_ms_counter() >= until)
            return false;
        std::this_thread::yield();
    }
    return true;
}

void SharedLock::unlock_shared() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    ::ReleaseSRWLockShared(&_rwlock);
#else
    const int rs = pthread_rwlock_unlock(&_rwlock);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

}

#endif // __cplusplus
