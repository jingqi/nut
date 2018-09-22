
#include "performance_counter.h"

namespace nut
{

#if NUT_PLATFORM_OS_WINDOWS
static LARGE_INTEGER PerformanceCounter::_frequency;
static bool PerformanceCounter::_frequency_initialized = false;
#endif

PerformanceCounter::PerformanceCounter()
{
#if NUT_PLATFORM_OS_WINDOWS
    if (!_frequency_initialized)
    {
        ::QueryPerformanceFrequency(&_frequency);
        _frequency_initialized = true;
    }
#endif

    set_to_now();
}

double PerformanceCounter::operator-(const PerformanceCounter& x) const
{
#if NUT_PLATFORM_OS_WINDOWS
    return (_counter.QuadPart - x._counter.QuadPart) / (double) _frequency.QuadPart;
#else
    return (_clock - x._clock) / (double) CLOCKS_PER_SEC;
#endif
}

void PerformanceCounter::set_to_now()
{
#if NUT_PLATFORM_OS_WINDOWS
    ::QueryPerformanceCounter(&_counter);
#else
    _clock = ::clock();
#endif
}

}
