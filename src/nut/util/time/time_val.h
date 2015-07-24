
#ifndef ___HEADFILE_5CAFD4E0_0BB4_474D_A465_CD320415264E_
#define ___HEADFILE_5CAFD4E0_0BB4_474D_A465_CD320415264E_

#include <time.h>

#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

namespace nut
{

/**
 * 秒、微妙
 */
class TimeVal
{
public:
    time_t sec = 0;
    long usec = 0;

public:
    TimeVal(time_t s = 0, long us = 0);

    void set(time_t s = 0, long us = 0);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    void set(const SYSTEMTIME& wtm);
#else
    void set(const struct timeval& tv);
    void set(const struct timespec& tv);
#endif

    static TimeVal now();

    /**
     * 变换特殊为一般表达方式
     */
    void normalize();

public:
    bool operator==(const TimeVal& x) const
    {
        return sec == x.sec && usec == x.usec;
    }

    bool operator!=(const TimeVal& x) const
    {
        return !(*this == x);
    }

    bool operator<(const TimeVal& x) const
    {
        if (sec < x.sec)
            return true;
        else if (sec == x.sec && usec < x.usec)
            return true;
        return false;
    }

    bool operator>(const TimeVal& x) const
    {
        return x < *this;
    }

    bool operator<=(const TimeVal& x) const
    {
        return !(x < *this);
    }

    bool operator>=(const TimeVal& x) const
    {
        return !(*this < x);
    }

    TimeVal operator+(const TimeVal& x) const
    {
        return TimeVal(sec + x.sec, usec + x.usec);
    }

    TimeVal operator-(const TimeVal& x) const
    {
        return TimeVal(sec - x.sec, usec - x.usec);
    }

    TimeVal operator-() const
    {
        return TimeVal(-sec, -usec);
    }

    TimeVal& operator+=(const TimeVal& x)
    {
        sec += x.sec;
        usec += x.usec;
        normalize();
        return *this;
    }

    TimeVal& operator-=(const TimeVal& x)
    {
        sec -= x.sec;
        usec -= x.usec;
        normalize();
        return *this;
    }
};

/**
 * time between jan 1, 1601 and jan 1, 1970 in units of 100 nanoseconds
 *
 * mingw 没有定义clock_gettime(), 这里参考其pthread_mutex_timedlock.c ptw32_relmillisecs.c 的实现
 * 相当于 ::clock_gettime(CLOCK_REALTIME, &ts);
 */
#if defined(NUT_PLATFORM_OS_WINDOWS) && defined(NUT_PLATFORM_CC_MINGW)
void clock_getrealtime(struct timespec *ts);
#endif

}

#endif
