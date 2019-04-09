
#ifndef ___HEADFILE_49083D01_04DA_4385_A1BD_6D1F2902FA7A_
#define ___HEADFILE_49083D01_04DA_4385_A1BD_6D1F2902FA7A_

#include "../../platform/platform.h"

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
public:
    explicit Semaphore(int init_value);
    ~Semaphore();

    void wait();

    void post();

    bool trywait();

    bool timedwait(unsigned s, unsigned ms);

private:
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

private:
#if NUT_PLATFORM_OS_WINDOWS
    HANDLE _sem = nullptr;
#else
    sem_t _sem;
#endif
};

}

#endif
