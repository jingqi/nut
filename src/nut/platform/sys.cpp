
#include "platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#elif NUT_PLATFORM_OS_MAC
#   include <unistd.h> // for ::sysconf()
#else
#   include <sys/sysinfo.h> // for ::get_nprocs()
#endif

#include "sys.h"

namespace nut
{

unsigned Sys::get_processor_num()
{
#if NUT_PLATFORM_OS_WINDOWS
    SYSTEM_INFO info;
    ::GetSystemInfo(&info);
    return (unsigned) info.dwNumberOfProcessors;
#elif NUT_PLATFORM_OS_MAC
    return (unsigned) ::sysconf(_SC_NPROCESSORS_ONLN);
#else
    return (unsigned) ::get_nprocs();
#endif
}

}
