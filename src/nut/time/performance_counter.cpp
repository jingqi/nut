﻿
#include "performance_counter.h"


#define NSECS_PER_SEC (1e9)

namespace nut
{

#if NUT_PLATFORM_OS_WINDOWS
LARGE_INTEGER PerformanceCounter::_frequency;
bool PerformanceCounter::_frequency_initialized = false;
#endif

PerformanceCounter::PerformanceCounter() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    if (!_frequency_initialized)
    {
        ::QueryPerformanceFrequency(&_frequency);
        _frequency_initialized = true;
    }
    _counter.QuadPart = 0;
#else
    _tv.tv_sec = 0;
    _tv.tv_nsec = 0;
#endif
}

double PerformanceCounter::operator-(const PerformanceCounter& x) const noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    return (_counter.QuadPart - x._counter.QuadPart) / (double) _frequency.QuadPart;
#else
    return (_tv.tv_sec + _tv.tv_nsec / (double) NSECS_PER_SEC) -
        (x._tv.tv_sec + x._tv.tv_nsec / (double) NSECS_PER_SEC);
#endif
}

void PerformanceCounter::set_to_now() noexcept
{
    // NOTE 各个可计时函数信息:
    // - time(), POSIX, 数值精度 1s
    // - clock(), POSIX, 数值精度 10ms
    // - times(), POSIX(obsolete), 数值精度 10ms
    // - timeGetTime(), Windows, 数值精度 1ms
    // - GetTickCount(), Windows, 数值精度 1ms
    // - GetLocalTime(), Windows, 数值精度 1ms
    // - gettimeofday(), POSIX(obsolete), 数值精度 1us
    // - QueryPerformanceCounter(), Windows, 数值精度 1us
    // - clock_gettime(CLOCK_MONOTONIC), POSIX, 数值精度 1ns

#if NUT_PLATFORM_OS_WINDOWS
    ::QueryPerformanceCounter(&_counter);
#elif NUT_PLATFORM_OS_LINUX
    ::clock_gettime(CLOCK_MONOTONIC_RAW, &_tv);
#else
    ::clock_gettime(CLOCK_MONOTONIC, &_tv);
#endif
}

PerformanceCounter PerformanceCounter::now() noexcept
{
    PerformanceCounter ret;
    ret.set_to_now();
    return ret;
}

bool PerformanceCounter::is_valid() const noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    return 0 != _counter.QuadPart;
#else
    return 0 != _tv.tv_sec || 0 != _tv.tv_nsec;
#endif
}

}
