
#ifndef ___HEADFILE_A61D5FAD_F8AF_40CE_A696_95AA55E4BFC1_
#define ___HEADFILE_A61D5FAD_F8AF_40CE_A696_95AA55E4BFC1_

#include <time.h>

#include "../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "../nut_config.h"


namespace nut
{

/**
 * 性能计时
 */
class NUT_API PerformanceCounter
{
public:
    PerformanceCounter() noexcept;

    double operator-(const PerformanceCounter& x) const noexcept;

    void set_to_now() noexcept;

    static PerformanceCounter now() noexcept;

    /**
     * 是否是有效状态
     */
    bool is_valid() const noexcept;

private:
#if NUT_PLATFORM_OS_WINDOWS
    static LARGE_INTEGER _frequency;
    static bool _frequency_initialized;
    LARGE_INTEGER _counter;
#else
    struct timespec _tv;
#endif
};

}

#endif
