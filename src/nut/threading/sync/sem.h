
#ifndef ___HEADFILE_49083D01_04DA_4385_A1BD_6D1F2902FA7A_
#define ___HEADFILE_49083D01_04DA_4385_A1BD_6D1F2902FA7A_

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#else
#   include <semaphore.h>
#endif

#include "../../nut_config.h"


namespace nut
{

class NUT_API Semaphore
{
#if NUT_PLATFORM_OS_WINDOWS
    HANDLE _sem = NULL;
#else
    sem_t _sem;
#endif

public:
    Semaphore(int init_value);
    ~Semaphore();

    void wait();

    void post();

    bool trywait();

    bool timedwait(unsigned s, unsigned ms);
};

}

#endif
