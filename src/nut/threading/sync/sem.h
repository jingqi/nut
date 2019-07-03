﻿
#ifndef ___HEADFILE_49083D01_04DA_4385_A1BD_6D1F2902FA7A_
#define ___HEADFILE_49083D01_04DA_4385_A1BD_6D1F2902FA7A_

#include <stdint.h>
#include <chrono>

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#elif NUT_PLATFORM_OS_MACOS
#   include <mutex>
#   include <condition_variable>
#else
#   include <semaphore.h>
#endif

#include "../../nut_config.h"


namespace nut
{

class NUT_API Semaphore
{
public:
    explicit Semaphore(unsigned init_value) noexcept;
    ~Semaphore() noexcept;

    void post() noexcept;

    void wait() noexcept;
    bool try_wait() noexcept;

    template <class Rep, class Period>
    bool try_wait_for(const std::chrono::duration<Rep,Period>& timeout)
    {
        return try_wait_for_ms(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
    }

private:
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    bool try_wait_for_ms(uint64_t ms) noexcept;

private:
#if NUT_PLATFORM_OS_WINDOWS
    HANDLE _sem = nullptr;
#elif NUT_PLATFORM_OS_MACOS
    // XXX macOS 上匿名 posix sem 被废弃了
    std::mutex _lock;
    std::condition_variable _cond;
    unsigned _count;
#else
    sem_t _sem;
#endif
};

}

#endif
