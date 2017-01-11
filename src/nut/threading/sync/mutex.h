
#ifndef ___HEADFILE___814FCD2E_2F65_4787_93E5_ECDE19588938_
#define ___HEADFILE___814FCD2E_2F65_4787_93E5_ECDE19588938_

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#  include <windows.h>
#endif

#if !NUT_PLATFORM_OS_WINDOWS || NUT_PLATFORM_CC_MINGW
#  include <pthread.h>
#endif

#include "../../nut_config.h"


namespace nut
{

class NUT_API Mutex
{
#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    HANDLE _hmutex = nullptr;
#else
    pthread_mutex_t _mutex;
#endif
    
private:
    // Non-copyable
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

public :
    Mutex();

    ~Mutex();

#if NUT_PLATFORM_OS_WINDOWS && !NUT_PLATFORM_CC_MINGW
    HANDLE inner_mutex()
    {
        return _hmutex;
    }
#else
    pthread_mutex_t* inner_mutex()
    {
        return &_mutex;
    }
#endif

    /**
     * lock the mutex, which may blocked the thread
     */
    void lock();

    /**
     * unlock the mutex
     */
    void unlock();

    /**
     * try lock the mutex
     *
     * @return
     *      true, if lock successed
     */
    bool trylock();

    /**
     * try lock the mutex in given time
     *
     * @param s
     *      The timeout value in seconds
     * @param ms
     *      The timeout value in milliseconds
     * @return
     *      true, if lock successed
     */
    bool timedlock(unsigned s, unsigned ms = 0);
};

}

#endif /* head file guarder */
