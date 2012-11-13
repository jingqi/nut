/**
 * @file -
 * @author jingqi
 * @date 2012-11-12
 * @last-edit 2012-11-13 22:04:33 jingqi
 */

#ifndef ___HEADFILE_5CAFD4E0_0BB4_474D_A465_CD320415264E_
#define ___HEADFILE_5CAFD4E0_0BB4_474D_A465_CD320415264E_

#include <time.h>

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

namespace nut
{

class TimeVal
{
    enum
    {
        USECS_PER_SEC = 1000 * 1000,
        USECS_PER_MSEC = 1000,
        NSECS_PER_USEC = 1000
    };
public:
    time_t sec;
    long usec;

    TimeVal(time_t s = 0, long us = 0)
        : sec(s), usec(us)
    {
        normalize();
    }

    void set(time_t s = 0, long us = 0)
    {
        sec = s;
        usec = us;
        normalize();
    }

#if defined(NUT_PLATFORM_OS_WINDOWS)
    void set(const SYSTEMTIME& wtm)
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
    void set(const struct timeval& tv) { set(tv.tv_sec, tv.tv_usec); }
    void set(const struct timespec& tv) { set(tv.tv_sec, tv.tv_nsec / NSECS_PER_USEC); }
#endif

    static TimeVal now()
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
    void normalize()
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

public:
    inline bool operator==(const TimeVal& x) const { return sec == x.sec && usec == x.usec; }
    inline bool operator!=(const TimeVal& x) const { return !(*this == x); }

    inline bool operator<(const TimeVal& x) const
    {
        if (sec < x.sec)
            return true;
        else if (sec == x.sec && usec < x.usec)
            return true;
        return false;
    }

    inline bool operator>(const TimeVal& x) const { return x < *this; }
    inline bool operator<=(const TimeVal& x) const { return !(x < *this); }
    inline bool operator>=(const TimeVal& x) const { return !(*this < x); }

    TimeVal operator+(const TimeVal& x) const { return TimeVal(sec + x.sec, usec + x.usec); }
    TimeVal operator-(const TimeVal& x) const { return TimeVal(sec - x.sec, usec - x.usec); }
    TimeVal operator-() const { return TimeVal(-sec, -usec); }

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

}

#endif

