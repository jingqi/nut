
#ifndef ___HEADFILE_409C4713_58BB_467B_A1CB_A8BC6EDA6CFE_
#define ___HEADFILE_409C4713_58BB_467B_A1CB_A8BC6EDA6CFE_

#include <assert.h>
#include <stdint.h>
#include <mutex> // for std::adopt_lock_t
#include <chrono>

#include "../../platform/platform.h"

#if __cplusplus < 201703L
#   if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
#       include <windows.h>
#   else
#       include <pthread.h>
#   endif
#else
#   include <shared_lock>
#endif

#include "../../nut_config.h"


namespace nut
{

#if __cplusplus < 201703L
/**
 * std::shared_lock 需要 C++17, 这里提供一个替代选择
 */
class NUT_API SharedLock
{
public:
    SharedLock() noexcept;
    ~SharedLock() noexcept;

    // 排他性锁定
    void lock() noexcept;
    bool try_lock() noexcept;
    void unlock() noexcept;

    template <class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep,Period>& timeout)
    {
        return try_lock_for_ms(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
    }

    // 共享锁定
    void lock_shared() noexcept;
    bool try_lock_shared() noexcept;
    void unlock_shared() noexcept;

    template <class Rep, class Period>
    bool try_lock_shared_for(const std::chrono::duration<Rep,Period>& timeout)
    {
        return try_lock_shared_for_ms(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
    }

private:
    SharedLock(const SharedLock&) = delete;
    SharedLock& operator=(const SharedLock&) = delete;

    bool try_lock_for_ms(uint64_t ms) noexcept;

    bool try_lock_shared_for_ms(uint64_t ms) noexcept;

private:
#   if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    /** NOTE windows 下的共享锁是在 Windows Vista/Server 2008 及其以后的版本中提供的 */
    SRWLOCK _rwlock;
#   else
    pthread_rwlock_t _rwlock;
#   endif
};
#endif // __cplusplus


template <typename LOCK>
class SharedLockGuard
{
public:
    explicit SharedLockGuard(LOCK &lock)
        : _lock(&lock), _own_lock(true)
    {
        lock.lock();
    }

    SharedLockGuard(LOCK &lock, std::adopt_lock_t)
        : _lock(&lock), _own_lock(true)
    {}

    SharedLockGuard(LOCK &lock, std::defer_lock_t)
        : _lock(&lock), _own_lock(false)
    {}

    SharedLockGuard(LOCK &lock, std::try_to_lock_t)
        : _lock(&lock)
    {
        _own_lock = lock.try_lock();
    }

    ~SharedLockGuard()
    {
        if (_own_lock)
            _lock->unlock_shared();
    }

    void lock_shared()
    {
        assert(!_own_lock);
        _lock->lock_shared();
        _own_lock = true;
    }

    bool try_lock_shared()
    {
        assert(!_own_lock);
        _own_lock = _lock->try_lock_shared();
        return _own_lock;
    }

    void unlock_shared()
    {
        assert(_own_lock);
        _lock->unlock_shared();
        _own_lock = false;
    }

    bool owns_lock() const noexcept
    {
        return _own_lock;
    }

    explicit operator bool() const noexcept
    {
        return _own_lock;
    }

private:
    SharedLockGuard(const SharedLockGuard&) = delete;
    SharedLockGuard& operator=(const SharedLockGuard&) = delete;

private:
    LOCK *_lock;
    bool _own_lock;
};

}

#endif /* head file guarder */
