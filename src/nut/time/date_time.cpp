
#include <assert.h>
#include <time.h>
#include <string.h>     /* for strlen() */

#include "../platform/platform.h"
#include "../util/string/string_utils.h"
#include "date_time.h"


#if NUT_PLATFORM_CC_VC
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

#define NSECS_PER_SEC  1000000000L
#define NSECS_PER_MSEC 1000000L
#define NSECS_PER_USEC 1000L

/* time between 1601/1/1 00:00:00 and 1970/1/1 00:00:00 in units of 100 nanoseconds */
#define TIMESPEC_TO_FILETIME_OFFSET 116444736000000000LL

namespace nut
{

DateTime::DateTime() noexcept
    : _seconds(0), _nanoseconds(0), _time_info_dirty(true)
{}

DateTime::DateTime(double s) noexcept
    : _seconds(s), _nanoseconds((s - (time_t) s) * NSECS_PER_SEC),
      _time_info_dirty(true)
{}

DateTime::DateTime(time_t s, long ns) noexcept
    : _seconds(s), _nanoseconds(ns), _time_info_dirty(true)
{
    normalize();
}

DateTime::DateTime(uint32_t year, uint8_t month, uint8_t day,
                   uint8_t hour, uint8_t min, uint8_t sec, uint32_t nsec, bool utc) noexcept
{
    set(year, month, day, hour, min, sec, nsec, utc);
}

void DateTime::normalize() noexcept
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

void DateTime::ensure_time_info(bool utc) const noexcept
{
    if (!_time_info_dirty && _time_info_utc == utc)
        return;

#if NUT_PLATFORM_OS_WINDOWS
    if (utc)
        ::gmtime_s(&_time_info, &_seconds);
    else
        ::localtime_s(&_time_info, &_seconds);
#else
    if (utc)
        ::gmtime_r(&_seconds, &_time_info);
    else
        ::localtime_r(&_seconds, &_time_info);
#endif

    _time_info_utc = utc;
    _time_info_dirty = false;
}

bool DateTime::operator==(const DateTime &x) const noexcept
{
    return _seconds == x._seconds && _nanoseconds == x._nanoseconds;
}

bool DateTime::operator!=(const DateTime &x) const noexcept
{
    return !(*this == x);
}

bool DateTime::operator<(const DateTime &x) const noexcept
{
    return (_seconds < x._seconds ||
            (_seconds == x._seconds && _nanoseconds < x._nanoseconds));
}

bool DateTime::operator>(const DateTime &x) const noexcept
{
    return x < *this;
}

bool DateTime::operator>=(const DateTime &x) const noexcept
{
    return !(*this < x);
}

bool DateTime::operator<=(const DateTime &x) const noexcept
{
    return !(x < *this);
}

DateTime DateTime::operator+(const TimeDiff& diff) const noexcept
{
    return DateTime(_seconds + diff.get_seconds(),
                    _nanoseconds + diff.get_nanoseconds());
}

DateTime DateTime::operator+(double seconds) const noexcept
{
    const time_t int_seconds = (time_t) seconds;
    const double dec_seconds = seconds - int_seconds;
    return DateTime(_seconds + int_seconds,
                    _nanoseconds + dec_seconds * NSECS_PER_SEC);
}

DateTime DateTime::operator-(const TimeDiff& diff) const noexcept
{
    return DateTime(_seconds - diff.get_seconds(),
                    _nanoseconds - diff.get_nanoseconds());
}

DateTime DateTime::operator-(double seconds) const noexcept
{
    const time_t int_seconds = (time_t) seconds;
    const double dec_seconds = seconds - int_seconds;
    return DateTime(_seconds - int_seconds,
                    _nanoseconds - dec_seconds * NSECS_PER_SEC);
}

TimeDiff DateTime::operator-(const DateTime& x) const noexcept
{
    return TimeDiff(_seconds - x._seconds, _nanoseconds - x._nanoseconds);
}

DateTime& DateTime::operator+=(const TimeDiff& diff) noexcept
{
    _seconds += diff.get_seconds();
    _nanoseconds += diff.get_nanoseconds();
    normalize();
    _time_info_dirty = true;
    return *this;
}

DateTime& DateTime::operator+=(double seconds) noexcept
{
    const time_t int_seconds = (time_t) seconds;
    const double dec_seconds = seconds - int_seconds;
    _seconds += int_seconds;
    _nanoseconds += dec_seconds * NSECS_PER_SEC;
    normalize();
    _time_info_dirty = true;
    return *this;
}

DateTime& DateTime::operator-=(const TimeDiff& diff) noexcept
{
    _seconds -= diff.get_seconds();
    _nanoseconds -= diff.get_nanoseconds();
    _time_info_dirty = true;
    normalize();
    return *this;
}

DateTime& DateTime::operator-=(double seconds) noexcept
{
    const time_t int_seconds = (time_t) seconds;
    const double dec_seconds = seconds - int_seconds;
    _seconds -= int_seconds;
    _nanoseconds -= dec_seconds;
    normalize();
    _time_info_dirty = true;
    return *this;
}

void DateTime::set(double s) noexcept
{
    _seconds = (time_t) s;
    _nanoseconds = (long) ((s - _seconds) * NSECS_PER_SEC);
    _time_info_dirty = true;
}

void DateTime::set(time_t s, long ns) noexcept
{
    _seconds = s;
    _nanoseconds = ns;
    _time_info_dirty = true;
    normalize();
}

void DateTime::set(uint32_t year, uint8_t month, uint8_t day, uint8_t hour,
                   uint8_t min, uint8_t sec, uint32_t nsec, bool utc) noexcept
{
    ::memset(&_time_info, 0, sizeof(_time_info));
    _time_info.tm_year = year - 1900;
    _time_info.tm_mon = month - 1;
    _time_info.tm_mday = day;
    _time_info.tm_hour = hour;
    _time_info.tm_min = min;
    _time_info.tm_sec = sec + nsec / NSECS_PER_SEC;
    _time_info.tm_isdst = -1;
    _time_info_utc = utc;
    _time_info_dirty = false;

    if (utc)
#if NUT_PLATFORM_OS_WINDOWS
        _seconds = ::_mkgmtime(&_time_info);
#else
        _seconds = ::timegm(&_time_info); /* '_time_info' is normalized also */
#endif
    else
        _seconds = ::mktime(&_time_info);

    _nanoseconds = nsec % NSECS_PER_SEC;

    /* 检查处理输入的结果 */
    assert(-1 != _seconds); /* 输入的数据有错误! */
}

#if NUT_PLATFORM_OS_WINDOWS
void DateTime::set(const FILETIME& ft) noexcept
{
    uint64_t val = (uint64_t(ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
    assert(val >= TIMESPEC_TO_FILETIME_OFFSET);
    val -= TIMESPEC_TO_FILETIME_OFFSET;
    _seconds = (time_t) (val / 10000000LL);
    _nanoseconds = (long) ((val % 10000000LL) * 100);
}

void DateTime::to_filetime(FILETIME *ft) const noexcept
{
    assert(nullptr != ft);
    uint64_t val = uint64_t(_seconds) * 10000000LL + _nanoseconds / 100 + TIMESPEC_TO_FILETIME_OFFSET;
    ft->dwLowDateTime = (DWORD) val;
    ft->dwHighDateTime = (DWORD) (val >> 32);
}

void DateTime::set(const SYSTEMTIME& wtm, bool utc) noexcept
{
    set(wtm.wYear, wtm.wMonth, wtm.wDay, wtm.wHour, wtm.wMinute, wtm.wSecond,
        wtm.wMilliseconds * NSECS_PER_MSEC, utc);
}

void DateTime::to_wtm(SYSTEMTIME *wtm, bool utc) const noexcept
{
    assert(nullptr != wtm);
    ensure_time_info(utc);
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
void DateTime::set(const struct timeval &tv) noexcept
{
    set(tv.tv_sec, tv.tv_usec * NSECS_PER_USEC);
}

void DateTime::set(const struct timespec &tv) noexcept
{
    set(tv.tv_sec, tv.tv_nsec);
}

void DateTime::to_timeval(struct timeval *tv) const noexcept
{
    assert(nullptr != tv);
    tv->tv_sec = _seconds;
    tv->tv_usec = _nanoseconds / NSECS_PER_USEC;
}

void DateTime::to_timespec(struct timespec *tv) const noexcept
{
    assert(nullptr != tv);
    tv->tv_sec = _seconds;
    tv->tv_nsec = _nanoseconds;
}
#endif

void DateTime::set_to_now() noexcept
{
    // NOTE 各个墙上时间函数信息:
    // - time(), POSIX, UTC, 数值精度 1s
    // - GetLocalTime(), Windows, local tz, 数值精度 1ms
    // - gettimeofday(), POSIX(obsolete), local tz, 数值精度 1us,
    // - clock_gettime(CLOCK_REALTIME), POSIX, UTC, 数值精度 1ns

#if NUT_PLATFORM_OS_WINDOWS
    SYSTEMTIME wtm;
    ::GetLocalTime(&wtm);
    set(wtm, false);
#else
    struct timespec tv;
    ::clock_gettime(CLOCK_REALTIME, &tv);
    set(tv);
#endif
}

DateTime DateTime::now() noexcept
{
    DateTime ret;
    ret.set_to_now();
    return ret;
}

uint32_t DateTime::get_year(bool utc) const noexcept
{
    ensure_time_info(utc);
    return static_cast<uint32_t>(_time_info.tm_year + 1900);
}

uint8_t DateTime::get_month(bool utc) const noexcept
{
    ensure_time_info(utc);
    return static_cast<uint8_t>(_time_info.tm_mon + 1);
}

uint16_t DateTime::get_yday(bool utc) const noexcept
{
    ensure_time_info(utc);
    return static_cast<uint16_t>(_time_info.tm_yday);
}

uint8_t DateTime::get_mday(bool utc) const noexcept
{
    ensure_time_info(utc);
    return static_cast<uint8_t>(_time_info.tm_mday);
}

uint8_t DateTime::get_wday(bool utc) const noexcept
{
    ensure_time_info(utc);
    return static_cast<uint8_t>(_time_info.tm_wday);
}

uint8_t DateTime::get_hour(bool utc) const noexcept
{
    ensure_time_info(utc);
    return static_cast<uint8_t>(_time_info.tm_hour);
}

uint8_t DateTime::get_minute(bool utc) const noexcept
{
    ensure_time_info(utc);
    return static_cast<uint8_t>(_time_info.tm_min);
}

uint8_t DateTime::get_second(bool utc) const noexcept
{
    ensure_time_info(utc);
    return static_cast<uint8_t>(_time_info.tm_sec);
}

uint32_t DateTime::get_nanosecond() const noexcept
{
    return _nanoseconds;
}

bool DateTime::is_valid() const noexcept
{
    return 0 != _seconds || 0 != _nanoseconds;
}

time_t DateTime::to_integer() const noexcept
{
    return _seconds;
}

double DateTime::to_double() const noexcept
{
    return _seconds + _nanoseconds / (double) NSECS_PER_SEC;
}

// for example : "2007-3-12"
std::string DateTime::get_date_str(bool utc) const noexcept
{
    return format_time("%Y-%m-%d", utc);
}

// for example : "12:34:45.572936192"
std::string DateTime::get_clock_str(bool utc) const noexcept
{
    return format_time("%H:%M:%S.%9f", utc);
}

// for example : "2007-3-4 8:33:57.762917263"
std::string DateTime::get_datetime_str(bool utc) const noexcept
{
    return get_date_str(utc) + " " + get_clock_str(utc);
}

std::string DateTime::to_string() const noexcept
{
    return get_datetime_str(false);
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
static std::string format_decimal_tails(const char *format, double decimal) noexcept
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

std::string DateTime::format_time(const char *format, bool utc) const noexcept
{
    assert(nullptr != format);
    if (0 == format[0])
        return "";

    ensure_time_info(utc);

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
void clock_getrealtime(struct timespec *ts) noexcept
{
    assert(nullptr != ts);

    SYSTEMTIME st;
    ::GetSystemTime(&st);

    FILETIME ft;
    ::SystemTimeToFileTime(&st, &ft);

    uint64_t val = (uint64_t(ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
    val -= TIMESPEC_TO_FILETIME_OFFSET;
    ts->tv_sec = (time_t) (val / 10000000LL);
    ts->tv_nsec = (long) ((val % 10000000LL) * 100);
}
#endif

}

#if NUT_PLATFORM_CC_VC
#   pragma warning(pop)
#endif
