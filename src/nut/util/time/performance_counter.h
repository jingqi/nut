
#ifndef ___HEADFILE_A61D5FAD_F8AF_40CE_A696_95AA55E4BFC1_
#define ___HEADFILE_A61D5FAD_F8AF_40CE_A696_95AA55E4BFC1_

#include <time.h>

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "../../nut_config.h"


namespace nut
{

/**
 * 性能计时
 */
class NUT_API PerformanceCounter
{
public:
    PerformanceCounter();

    double operator-(const PerformanceCounter& x) const;

    void set_to_now();

private:
#if NUT_PLATFORM_OS_WINDOWS
    static LARGE_INTEGER _frequency;
    static bool _frequency_initialized;
    LARGE_INTEGER _counter;
#else
    /* 自程序启动的计时 */
    clock_t _clock;
#endif
};

}

#endif
