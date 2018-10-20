
#ifndef ___HEADFILE_B56CC2C0_68D4_44FD_AD11_BBA330875876_
#define ___HEADFILE_B56CC2C0_68D4_44FD_AD11_BBA330875876_

#include <time.h>
#include <string>

#include <nut/platform/platform.h>

#if !NUT_PLATFORM_OS_WINDOWS
#   include <sys/time.h>
#endif

#include "../../nut_config.h"


namespace nut
{

class NUT_API TimeDiff
{
public:
    TimeDiff();
    explicit TimeDiff(double s);

    /**
     * @param s 秒
     * @param ns 纳秒
     */
    explicit TimeDiff(time_t s, long ns);

    bool operator==(const TimeDiff& x) const;
    bool operator!=(const TimeDiff& x) const;
    bool operator<(const TimeDiff& x) const;
    bool operator>(const TimeDiff& x) const;
    bool operator<=(const TimeDiff& x) const;
    bool operator>=(const TimeDiff& x) const;

    TimeDiff operator+(const TimeDiff& x) const;
    TimeDiff operator-(const TimeDiff& x) const;
    TimeDiff operator*(double scale) const;
    TimeDiff operator/(double scale) const;
    TimeDiff operator-() const;

    TimeDiff& operator+=(const TimeDiff& x);
    TimeDiff& operator-=(const TimeDiff& x);
    TimeDiff& operator*=(double scale);
    TimeDiff& operator/=(double scale);

    void set(double s);
    void set(time_t s, long ns);

#if !NUT_PLATFORM_OS_WINDOWS
    void set(const struct timeval& tv);
    void set(const struct timespec& tv);
    void to_timeval(struct timeval *tv);
    void to_timespec(struct timespec *tv);
#endif

    time_t get_seconds() const;
    long get_nanoseconds() const;

    double to_double() const;

    // example: "12.2345"
    std::string to_string() const;

protected:
    /**
     * 规范化
     */
    void normalize();

public:
    static const TimeDiff ZERO;

protected:
    time_t _seconds;
    long _nanoseconds;
};

}

#endif
