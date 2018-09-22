
#include "time_diff.h"


#define USECS_PER_SEC (1000 * 1000)
#define USECS_PER_MSEC 1000
#define NSECS_PER_USEC 1000

namespace nut
{

const TimeDiff TimeDiff::ZERO;

TimeDiff::TimeDiff(time_t s, long us)
    : _seconds(s), _useconds(us)
{
    normalize();
}

void TimeDiff::normalize()
{
    if (_useconds >= USECS_PER_SEC)
    {
        _seconds += _useconds / USECS_PER_SEC;
        _useconds %= USECS_PER_SEC;
    }
    else if (_useconds <= -USECS_PER_SEC)
    {
        _seconds -= (-_useconds) / USECS_PER_SEC;
        _useconds = -((-_useconds) % USECS_PER_SEC);
    }

    if (_seconds > 0 && _useconds < 0)
    {
        --_seconds;
        _useconds += USECS_PER_SEC;
    }
    else if (_seconds < 0 && _useconds > 0)
    {
        ++_seconds;
        _useconds -= USECS_PER_SEC;
    }
}

bool TimeDiff::operator==(const TimeDiff& x) const
{
    return _seconds == x._seconds && _useconds == x._useconds;
}

bool TimeDiff::operator!=(const TimeDiff& x) const
{
    return !(*this == x);
}

bool TimeDiff::operator<(const TimeDiff& x) const
{
    return ((_seconds < x._seconds) ||
            (_seconds == x._seconds && _useconds < x._useconds));
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
    return TimeDiff(_seconds + x._seconds, _useconds + x._useconds);
}

TimeDiff TimeDiff::operator-(const TimeDiff& x) const
{
    return TimeDiff(_seconds - x._seconds, _useconds - x._useconds);
}

TimeDiff TimeDiff::operator*(double scale) const
{
    return TimeDiff(_seconds * scale, _useconds * scale);
}

TimeDiff TimeDiff::operator/(double scale) const
{
    return TimeDiff(_seconds / scale, _useconds / scale);
}

TimeDiff TimeDiff::operator-() const
{
    return TimeDiff(-_seconds, -_useconds);
}

TimeDiff& TimeDiff::operator+=(const TimeDiff& x)
{
    _seconds += x._seconds;
    _useconds += x._useconds;
    normalize();
    return *this;
}

TimeDiff& TimeDiff::operator-=(const TimeDiff& x)
{
    _seconds -= x._seconds;
    _useconds -= x._useconds;
    normalize();
    return *this;
}

TimeDiff& TimeDiff::operator*=(double scale)
{
    _seconds *= scale;
    _useconds *= scale;
    normalize();
    return *this;
}

TimeDiff& TimeDiff::operator/=(double scale)
{
    _seconds /= scale;
    _useconds /= scale;
    normalize();
    return *this;
}

void TimeDiff::set(time_t s, long us)
{
    _seconds = s;
    _useconds = us;
    normalize();
}

#if !NUT_PLATFORM_OS_WINDOWS
void TimeDiff::set(const struct timeval& tv)
{
    set(tv.tv_sec, tv.tv_usec);
}

void TimeDiff::set(const struct timespec& tv)
{
    set(tv.tv_sec, tv.tv_nsec / NSECS_PER_USEC);
}

void TimeDiff::to_timeval(struct timeval *tv)
{
    assert(nullptr != tv);
    tv->tv_sec = _seconds;
    tv->tv_usec = _useconds;
}

void TimeDiff::to_timespec(struct timespec *tv)
{
    assert(nullptr != tv);
    tv->tv_sec = _seconds;
    tv->tv_nsec = _useconds * NSECS_PER_USEC;
}
#endif

time_t TimeDiff::get_seconds() const
{
    return _seconds;
}

long TimeDiff::get_useconds() const
{
    return _useconds;
}

std::string TimeDiff::to_string() const
{
    double sec = _seconds * USECS_PER_SEC + _useconds;
    sec /= USECS_PER_SEC;
    char buf[128];
    ::sprintf(buf, "%lf", sec);
    return buf;
}

}
