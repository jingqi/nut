
#include <assert.h>
#include <time.h>
#include <string.h>     /* for strlen() */
#include <stdlib.h>     /* for malloc() and free() */

#include <nut/platform/platform.h>

#if NUT_PLATFORM_CC_VC
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

#include "date_time.h"
#include "../string/string_utils.h"


#define NSECS_PER_SEC  1000000000L
#define NSECS_PER_MSEC 1000000L
#define NSECS_PER_USEC 1000L

namespace nut
{

DateTime::DateTime()
    : _seconds(0), _nanoseconds(0), _time_info_dirty(true)
{}

DateTime::DateTime(double s)
    : _seconds(s), _nanoseconds((s - (time_t) s) * NSECS_PER_SEC),
      _time_info_dirty(true)
{}

DateTime::DateTime(time_t s, long ns)
    : _seconds(s), _nanoseconds(ns), _time_info_dirty(true)
{
    normalize();
}

DateTime::DateTime(uint32_t year, uint8_t month, uint8_t day,
                   uint8_t hour, uint8_t min, uint8_t sec, uint32_t nsec)
{
    set(year, month, day, hour, min, sec, nsec);
}

void DateTime::normalize()
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

void DateTime::check_time_info() const
{
    if (!_time_info_dirty)
        return;
    _time_info = *::localtime(&_seconds);
    _time_info_dirty = false;
}

bool DateTime::operator==(const DateTime &x) const
{
    return _seconds == x._seconds && _nanoseconds == x._nanoseconds;
}

bool DateTime::operator!=(const DateTime &x) const
{
    return !(*this == x);
}

bool DateTime::operator<(const DateTime &x) const
{
    return (_seconds < x._seconds ||
            (_seconds == x._seconds && _nanoseconds < x._nanoseconds));
}

bool DateTime::operator>(const DateTime &x) const
{
    return x < *this;
}

bool DateTime::operator>=(const DateTime &x) const
{
    return !(*this < x);
}

bool DateTime::operator<=(const DateTime &x) const
{
    return !(x < *this);
}

DateTime DateTime::operator+(double seconds) const
{
    const time_t int_seconds = (time_t) seconds;
    const double dec_seconds = seconds - int_seconds;
    return DateTime(_seconds + int_seconds,
                    _nanoseconds + dec_seconds * NSECS_PER_SEC);
}

DateTime DateTime::operator-(double seconds) const
{
    const time_t int_seconds = (time_t) seconds;
    const double dec_seconds = seconds - int_seconds;
    return DateTime(_seconds - int_seconds,
                    _nanoseconds - dec_seconds * NSECS_PER_SEC);
}

DateTime& DateTime::operator+=(double seconds)
{
    const time_t int_seconds = (time_t) seconds;
    const double dec_seconds = seconds - int_seconds;
    _seconds += int_seconds;
    _nanoseconds += dec_seconds * NSECS_PER_SEC;
    normalize();
    _time_info_dirty = true;
    return *this;
}

DateTime& DateTime::operator-=(double seconds)
{
    const time_t int_seconds = (time_t) seconds;
    const double dec_seconds = seconds - int_seconds;
    _seconds -= int_seconds;
    _nanoseconds -= dec_seconds;
    normalize();
    _time_info_dirty = true;
    return *this;
}

DateTime DateTime::operator+(const TimeDiff& diff) const
{
    return DateTime(_seconds + diff.get_seconds(),
                    _nanoseconds + diff.get_nanoseconds());
}

DateTime DateTime::operator-(const TimeDiff& diff) const
{
    return DateTime(_seconds - diff.get_seconds(),
                    _nanoseconds - diff.get_nanoseconds());
}

TimeDiff DateTime::operator-(const DateTime& x) const
{
    return TimeDiff(_seconds - x._seconds, _nanoseconds - x._nanoseconds);
}

DateTime& DateTime::operator+=(const TimeDiff& diff)
{
    _seconds += diff.get_seconds();
    _nanoseconds += diff.get_nanoseconds();
    normalize();
    _time_info_dirty = true;
    return *this;
}

DateTime& DateTime::operator-=(const TimeDiff& diff)
{
    _seconds -= diff.get_seconds();
    _nanoseconds -= diff.get_nanoseconds();
    _time_info_dirty = true;
    normalize();
    return *this;
}

void DateTime::set(double s)
{
    _seconds = (time_t) s;
    _nanoseconds = (long) ((s - _seconds) * NSECS_PER_SEC);
    _time_info_dirty = true;
}

void DateTime::set(time_t s, long ns)
{
    _seconds = s;
    _nanoseconds = ns;
    _time_info_dirty = true;
    normalize();
}

void DateTime::set(uint32_t year, uint8_t month, uint8_t day, uint8_t hour,
                   uint8_t min, uint8_t sec, uint32_t nsec)
{
    ::memset(&_time_info, 0, sizeof(_time_info));
    _time_info.tm_year = year - 1900;
    _time_info.tm_mon = month - 1;
    _time_info.tm_mday = day;
    _time_info.tm_hour = hour;
    _time_info.tm_min = min;
    _time_info.tm_sec = sec + nsec / NSECS_PER_SEC;
    _time_info.tm_isdst = -1;
    _time_info_dirty = false;

    _seconds = ::mktime(&_time_info); /* '_time_info' is normalized also */
    _nanoseconds = nsec % NSECS_PER_SEC;

    /* 检查处理输入的结果 */
    assert(-1 != _seconds); /* 输入的数据有错误! */
}

#if NUT_PLATFORM_OS_WINDOWS
void DateTime::set(const SYSTEMTIME& wtm)
{
    set(wtm.wYear, wtm.wMonth, wtm.wDay, wtm.wHour, wtm.wMinute, wtm.wSecond,
        wtm.wMilliseconds * NSECS_PER_MSEC);
}

void DateTime::to_wtm(SYSTEMTIME *wtm) const
{
    assert(nullptr != wtm);
    check_time_info();
    ::memset(wtm, 0, sizeof(SYSTEMTIME));
    wtm->wYear = _time_info.tm_year + 1900;
    wtm->wMonth = _time_info.tm_mon + 1;
    wtm->wDay = _time_info.tm_mday;
    wtm->wDayOfWeek = _time_info.tm_wday;
    wtm->wHour = _time_info.tm_hour;
    wtm->wMinute = _time_info.tm_min;
    wtm->wSecond = _time_info.tm_sec;
    wtm->wMilliseconds = _nanoseconds / NSECS_PER_MSEC;
}
#else
void DateTime::set(const struct timeval &tv)
{
    set(tv.tv_sec, tv.tv_usec * NSECS_PER_USEC);
}

void DateTime::set(const struct timespec &tv)
{
    set(tv.tv_sec, tv.tv_nsec);
}

void DateTime::to_timeval(struct timeval *tv) const
{
    assert(nullptr != tv);
    tv->tv_sec = _seconds;
    tv->tv_usec = _nanoseconds / NSECS_PER_USEC;
}

void DateTime::to_timespec(struct timespec *tv) const
{
    assert(nullptr != tv);
    tv->tv_sec = _seconds;
    tv->tv_nsec = _nanoseconds;
}
#endif

/**
 * 获得并存储当前时刻
 */
void DateTime::set_to_now()
{
    // NOTE 各个墙上时间函数信息:
    // - time(), POSIX, UTC, 实际精度 1s
    // - GetLocalTime(), Windows, local tz, 实际精度 1ms
    // - gettimeofday(), POSIX(obsolete), local tz, 实际精度 1us,
    // - clock_gettime(CLOCK_REALTIME), POSIX, UTC, 实际精度 1ns

#if NUT_PLATFORM_OS_WINDOWS
    SYSTEMTIME wtm;
    ::GetLocalTime(&wtm);
    set(wtm);
#else
    struct timespec tv;
    ::clock_gettime(CLOCK_REALTIME, &tv);
    set(tv);
#endif
}

DateTime DateTime::now()
{
    DateTime ret;
    ret.set_to_now();
    return ret;
}

/**
 * 获得年份
 *
 * @return 范围 year number; 2009 for the year of 2009
 */
uint32_t DateTime::get_year() const
{
    check_time_info();
    return static_cast<uint32_t>(_time_info.tm_year + 1900);
}

/**
 * 获得月数
 *
 * @return 范围 [1,12];  1 for Junuary
 */
uint8_t DateTime::get_month() const
{
    check_time_info();
    return static_cast<uint8_t>(_time_info.tm_mon + 1);
}

/**
 * 获得一年中的天数
 *
 * @return 范围 [0,365]; 0 for the first day in a year
 */
uint16_t DateTime::get_yday() const
{
    check_time_info();
    return static_cast<uint16_t>(_time_info.tm_yday);
}

/**
 * 获得月之中的日子数
 *
 * @return 范围 [1,31];  1 for the first day in a month
 */
uint8_t DateTime::get_mday() const
{
    check_time_info();
    return static_cast<uint8_t>(_time_info.tm_mday);
}

/**
 * 获得星期中的天数
 *
 * @return 范围 [0,6];   0 for Sunday
 */
uint8_t DateTime::get_wday() const
{
    check_time_info();
    return static_cast<uint8_t>(_time_info.tm_wday);
}

/**
 * 获得小时数
 *
 * @return 范围 [0,23]
 */
uint8_t DateTime::get_hour() const
{
    check_time_info();
    return static_cast<uint8_t>(_time_info.tm_hour);
}

/**
 * 获得分钟数
 *
 * @return 范围 [0,59]
 */
uint8_t DateTime::get_minute() const
{
    check_time_info();
    return static_cast<uint8_t>(_time_info.tm_min);
}

uint8_t DateTime::get_second() const
{
    check_time_info();
    return static_cast<uint8_t>(_time_info.tm_sec);
}

uint32_t DateTime::get_nanosecond() const
{
    return _nanoseconds;
}

bool DateTime::is_valid() const
{
    return 0 != _seconds || 0 != _nanoseconds;
}

time_t DateTime::to_integer() const
{
    return _seconds;
}

double DateTime::to_double() const
{
    return _seconds + _nanoseconds / (double) NSECS_PER_SEC;
}

// for example : "2007-3-12"
std::string DateTime::get_date_str() const
{
    return format_time("%Y-%m-%d");
}

// for example : "12:34:45.572936192"
std::string DateTime::get_clock_str() const
{
    return format_time("%H:%M:%S.%9f");
}

// for example : "2007-3-4 8:33:57.762917263"
std::string DateTime::get_datetime_str() const
{
    return get_date_str() + " " + get_clock_str();
}

std::string DateTime::to_string() const
{
    return get_datetime_str();
}

// 获取浮点数小数部分，例如 "%.3f", 0.1234 -> 123
static std::string format_decimal_tail(const char *format, double decimal)
{
    assert(nullptr != format);
    char buf[128];
    safe_snprintf(buf, 128, format, decimal);
    const char *tail = buf;
    while ('.' != *tail)
        ++tail;
    ++tail;
    return tail;
}

// 格式化浮点，例如 "%3f", 0.1234 -> 123
static std::string format_decimal_tails(const char *format, double decimal)
{
    assert(nullptr != format);

    std::string ret;
    std::string fmt;
    for (const char *p = format; 0 != *p; ++p)
    {
        const char c = *p;
        if (fmt.empty())
        {
            if ('%' == c)
                fmt.push_back(c);
            else
                ret.push_back(c);
        }
        else if ('0' <= c && c <= '9')
        {
            fmt.push_back(c);
        }
        else if ('f' == c)
        {
            fmt.push_back(c);
            if (fmt.size() > 2)
                fmt.insert(1, ".");
            ret += format_decimal_tail(fmt.c_str(), decimal);
            fmt.clear();
        }
        else
        {
            ret += fmt;
            ret.push_back(c);
            fmt.clear();
        }
    }
    if (!fmt.empty())
        ret += fmt;
    return ret;
}

/**
 * format time string
 * for example : "%m-%d %H:%M" -> "2-28 8:57"
 */
std::string DateTime::format_time(const char *format) const
{
    assert(nullptr != format);
    if (0 == format[0])
        return "";

    check_time_info();

    // 先格式化浮点数部分，因为 mac 下的 strftime() 会错误处理 %f
    const double f = _nanoseconds / (double) NSECS_PER_SEC;
    const std::string fmt = format_decimal_tails(format, f);

    // 格式化其他部分
    std::string ret;
    size_t size = fmt.length() * 3;
    ret.resize(size);
    while (true)
    {
        const int n = ::strftime((char*) ret.data(), size, fmt.c_str(), &_time_info);
        if (n > 0)
        {
            ret.resize(n);
            break;
        }
        size *= 2;
        ret.resize(size);
    }

    return ret;
}

#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_MINGW
#   define PTW32_TIMESPEC_TO_FILETIME_OFFSET (LONGLONG)((((LONGLONG) 27111902LL << 32)+(LONGLONG) 3577643008LL ))
void clock_getrealtime(struct timespec *ts)
{
    assert(nullptr != ts);

    SYSTEMTIME st;
    ::GetSystemTime(&st);
    FILETIME ft;
    ::SystemTimeToFileTime(&st, &ft);
    ts->tv_sec = (int)((*(LONGLONG *)&ft - PTW32_TIMESPEC_TO_FILETIME_OFFSET) / 10000000LL);
    ts->tv_nsec = (int)((*(LONGLONG *)&ft - PTW32_TIMESPEC_TO_FILETIME_OFFSET - ((LONGLONG)ts->tv_sec * (LONGLONG)10000000LL)) * 100);
}
#endif

}

#if NUT_PLATFORM_CC_VC
#   pragma warning(pop)
#endif
