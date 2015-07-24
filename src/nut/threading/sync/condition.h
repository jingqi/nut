
#ifndef ___HEADFILE___B926495D_967A_45A2_8F56_4FFB10F2E34B_
#define ___HEADFILE___B926495D_967A_45A2_8F56_4FFB10F2E34B_

#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
#   include <windows.h>
#   include "spinlock.h"
#elif defined(NUT_PLATFORM_OS_MAC)
#   include <time.h>
#   include <sys/time.h>
#   include <mach/clock.h>
#   include <mach/mach.h>
#else
#   include <pthread.h>
#endif

#include "mutex.h"


namespace nut
{

class Condition
{
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    CONDITION_VARIABLE _cond;
#else
    pthread_cond_t _cond;
#endif

public:
#if defined(NUT_PLATFORM_OS_WINDOWS) && !defined(NUT_PLATFORM_CC_MINGW)
    typedef SpinLock condition_lock_type; // windows 下condition只能配合临界区
#else
    typedef Mutex condition_lock_type;
#endif

    Condition();
    ~Condition();

    bool signal();

    bool broadcast();

    /**
     * release lock, wait for signal or interrupt, lock and wake
     */
    bool wait(condition_lock_type *mutex);

    /**
     * work the same as above
     */
    bool timedwait(condition_lock_type *mutex, unsigned s, unsigned ms = 0);
};

}

#endif /* head file guarder */
