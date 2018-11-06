
#include <assert.h>

#include "time_diff.h"
#include "../string/string_utils.h"


#define NSECS_PER_SEC  1000000000L
#define NSECS_PER_USEC 1000L

namespace nut
{

const TimeDiff TimeDiff::ZERO;

TimeDiff::TimeDiff()
    : _seconds(0), _nanoseconds(0)
{}

TimeDiff::TimeDiff(double s)
    : _seconds(s), _nanoseconds((s - (time_t) s) * NSECS_PER_SEC)
{}

TimeDiff::TimeDiff(time_t s, int64_t ns)
    : _seconds(s), _nanoseconds(ns)
{
    normalize();
}

void TimeDiff::normalize()
{
    if (_nanoseconds >= NSECS_PER_SEC)
    {
        _seconds += _nanoseconds / NSECS_PER_SEC;
        _nanoseconds %= NSECS_PER_SEC;
    }
    else if (_nanoseconds <= -NSECS_PER_SEC)
    {
        _seconds -= (-_nanoseconds) / NSECS_PER_SEC;
        _nanoseconds = -((-_nanoseconds) % NSECS_PER_SEC);
    }

    if (_seconds > 0 && _nanoseconds < 0)
    {
        --_seconds;
        _nanoseconds += NSECS_PER_SEC;
    }
    else if (_seconds < 0 && _nanoseconds > 0)
    {
        ++_seconds;
        _nanoseconds -= NSECS_PER_SEC;
    }
}

bool TimeDiff::operator==(const TimeDiff& x) const
{
    return _seconds == x._seconds && _nanoseconds == x._nanoseconds;
}

bool TimeDiff::operator!=(const TimeDiff& x) const
{
    return !(*this == x);
}

bool TimeDiff::operator<(const TimeDiff& x) const
{
    return ((_seconds < x._seconds) ||
            (_seconds == x._seconds && _nanoseconds < x._nanoseconds));
}

bool TimeDiff::operator>(const TimeDiff& x) const
{
    return x < *this;
}

bool TimeDiff::operator<=(const TimeDiff& x) const
{
    return !(x < *this);
}

bool TimeDiff::operator>=(const TimeDiff& x) const
{
    return !(*this < x);
}

TimeDiff TimeDiff::operator+(const TimeDiff& x) const
{
    return TimeDiff(_seconds + x._seconds, _nanoseconds + x._nanoseconds);
}

TimeDiff TimeDiff::operator+(double seconds) const
{
    const time_t int_seconds = (time_t) seconds;
    const double dec_seconds = seconds - int_seconds;
    return TimeDiff(_seconds + int_seconds,
                    _nanoseconds + dec_seconds * NSECS_PER_SEC);
}

TimeDiff TimeDiff::operator-(const TimeDiff& x) const
{
    return TimeDiff(_seconds - x._seconds, _nanoseconds - x._nanoseconds);
}

TimeDiff TimeDiff::operator-(double seconds) const
{
    const time_t int_seconds = (time_t) seconds;
    const double dec_seconds = seconds - int_seconds;
    return TimeDiff(_seconds - int_seconds,
                    _nanoseconds - dec_seconds * NSECS_PER_SEC);
}

TimeDiff TimeDiff::operator*(double scale) const
{
    return TimeDiff(_seconds * scale, _nanoseconds * scale);
}

TimeDiff TimeDiff::operator/(double scale) const
{
    return TimeDiff(_seconds / scale, _nanoseconds / scale);
}

TimeDiff TimeDiff::operator-() const
{
    return TimeDiff(-_seconds, -_nanoseconds);
}

TimeDiff& TimeDiff::operator+=(const TimeDiff& x)
{
    _seconds += x._seconds;
    _nanoseconds += x._nanoseconds;
    normalize();
    return *this;
}

TimeDiff& TimeDiff::operator-=(const TimeDiff& x)
{
    _seconds -= x._seconds;
    _nanoseconds -= x._nanoseconds;
    normalize();
    return *this;
}

TimeDiff& TimeDiff::operator*=(double scale)
{
    _seconds *= scale;
    _nanoseconds *= scale;
    normalize();
    return *this;
}

TimeDiff& TimeDiff::operator/=(double scale)
{
    _seconds /= scale;
    _nanoseconds /= scale;
    normalize();
    return *this;
}

void TimeDiff::set(double s)
{
    _seconds = (time_t) s;
    _nanoseconds = (int64_t) ((s - _seconds) * NSECS_PER_SEC);
}

void TimeDiff::set(time_t s, int64_t ns)
{
    _seconds = s;
    _nanoseconds = ns;
    normalize();
}

#if !NUT_PLATFORM_OS_WINDOWS
void TimeDiff::set(const struct timeval& tv)
{
    set(tv.tv_sec, tv.tv_usec * NSECS_PER_USEC);
}

void TimeDiff::set(const struct timespec& tv)
{
    set(tv.tv_sec, tv.tv_nsec);
}

void TimeDiff::to_timeval(struct timeval *tv)
{
    assert(nullptr != tv);
    tv->tv_sec = _seconds;
    tv->tv_usec = _nanoseconds / NSECS_PER_USEC;
}

void TimeDiff::to_timespec(struct timespec *tv)
{
    assert(nullptr != tv);
    tv->tv_sec = _seconds;
    tv->tv_nsec = _nanoseconds;
}
#endif

time_t TimeDiff::get_seconds() const
{
    return _seconds;
}

int64_t TimeDiff::get_nanoseconds() const
{
    return _nanoseconds;
}

double TimeDiff::to_double() const
{
    double ret = _seconds;
    ret += _nanoseconds / (double) NSECS_PER_SEC;
    return ret;
}

std::string TimeDiff::to_string() const
{
    const double f = to_double();
    char buf[128];
    safe_snprintf(buf, 128, "%lf", f);
    return buf;
}

}
