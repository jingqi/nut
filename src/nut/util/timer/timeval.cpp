
#include <time.h>

#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

#include "timeval.h"


#define USECS_PER_SEC (1000 * 1000)
#define USECS_PER_MSEC 1000
#define NSECS_PER_USEC 1000

namespace nut
{

TimeVal::TimeVal(time_t s, long us)
    : sec(s), usec(us)
{
    normalize();
}

void TimeVal::set(time_t s, long us)
{
    sec = s;
    usec = us;
    normalize();
}

#if defined(NUT_PLATFORM_OS_WINDOWS)
void TimeVal::set(const SYSTEMTIME& wtm)
{
    struct tm tm;
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock_t clock = mktime(&tm);
    set(clock, wtm.wMilliseconds * USECS_PER_MSEC);
}
#else
void TimeVal::set(const struct timeval& tv)
{
    set(tv.tv_sec, tv.tv_usec);
}

void TimeVal::set(const struct timespec& tv)
{
    set(tv.tv_sec, tv.tv_nsec / NSECS_PER_USEC);
}
#endif

TimeVal TimeVal::now()
{
    TimeVal ret;
#if defined(NUT_PLATFORM_OS_WINDOWS)
    SYSTEMTIME wtm;
    ::GetLocalTime(&wtm);
    ret.set(wtm);
#else
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    ret.set(tv);
#endif
    return ret;
}

/**
 * 变换特殊为一般表达方式
 */
void TimeVal::normalize()
{
    if (usec >= USECS_PER_SEC)
    {
        sec += usec / USECS_PER_SEC;
        usec %= USECS_PER_SEC;
    }
    else if (usec <= -USECS_PER_SEC)
    {
        sec -= (-usec) / USECS_PER_SEC;
        usec = -((-usec) % USECS_PER_SEC);
    }

    if (sec > 0 && usec < 0)
    {
        --sec;
        usec += USECS_PER_SEC;
    }
    else if (sec < 0 && usec > 0)
    {
        ++sec;
        usec -= USECS_PER_SEC;
    }
}

}
