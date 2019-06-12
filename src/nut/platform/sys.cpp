
#include "platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#   include <chrono> // for std::chrono
#elif NUT_PLATFORM_OS_MACOS
#   include <unistd.h> // for ::sysconf()
#else
#   include <sys/sysinfo.h> // for ::get_nprocs()
#endif

#include "../threading/threading.h" // for NUT_THREAD_LOCAL
#include "sys.h"


namespace nut
{

unsigned Sys::get_processor_num() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    SYSTEM_INFO info;
    ::GetSystemInfo(&info);
    return (unsigned) info.dwNumberOfProcessors;
#elif NUT_PLATFORM_OS_MACOS
    return (unsigned) ::sysconf(_SC_NPROCESSORS_ONLN);
#else
    return (unsigned) ::get_nprocs();
#endif
}

std::mt19937_64& Sys::random_engine() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_MINGW
    // FIXME GCC hasn't implemented std::random_device nondeterministically on Windows
    //       see https://stackoverflow.com/questions/18880654/why-do-i-get-the-same-sequence-for-every-run-with-stdrandom-device-with-mingw
    static NUT_THREAD_LOCAL std::mt19937_64 gen(
        std::chrono::system_clock::now().time_since_epoch().count());
#else
    static NUT_THREAD_LOCAL std::random_device rd;
    static NUT_THREAD_LOCAL std::mt19937_64 gen(rd());
#endif
    return gen;
}

}
