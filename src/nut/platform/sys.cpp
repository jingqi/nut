
#include "platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
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
#else
    return (unsigned) ::get_nprocs();
#endif
}

}
