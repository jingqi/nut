
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

namespace nut
{

/**
 * 获取当前时间
 */
DateTime::DateTime()
    : _seconds(0)
{
    _time_info = *::localtime(&_seconds);
}

/**
 * 从1970年1月1日起算的时间(秒)
 * @param s
 */
DateTime::DateTime(time_t s)
    : _seconds(s)
{
    _time_info = *::localtime(&_seconds);
}

/**
 * 使用具体时刻初始化
 */
DateTime::DateTime(uint32_t year, uint8_t month, uint8_t day,
                   uint8_t hour, uint8_t min, uint8_t sec)
{
    ::memset(&_time_info, 0, sizeof(_time_info));
    _time_info.tm_year = year - 1900;
    _time_info.tm_mon = month - 1;
    _time_info.tm_mday = day;
    _time_info.tm_hour = hour;
    _time_info.tm_min = min;
    _time_info.tm_sec = sec;
    _seconds = ::mktime(&_time_info); /* '_time_info' is normalized also */

    /* 检查处理输入的结果 */
    assert(-1 != _seconds); /* 输入的数据有错误! */
}

bool DateTime::operator==(const DateTime &another) const
{
    return _seconds == another._seconds;
}

bool DateTime::operator!=(const DateTime &another) const
{
    return _seconds != another._seconds;
}

bool DateTime::operator>(const DateTime &another) const
{
    return _seconds > another._seconds;
}

bool DateTime::operator<(const DateTime &another) const
{
    return _seconds < another._seconds;
}

bool DateTime::operator>=(const DateTime &another) const
{
    return _seconds >= another._seconds;
}

bool DateTime::operator<=(const DateTime &another) const
{
    return _seconds <= another._seconds;
}

DateTime DateTime::operator+(time_t period) const
{
    return DateTime(_seconds + period);
}

DateTime DateTime::operator-(time_t period) const
{
    return DateTime(_seconds - period);
}

DateTime& DateTime::operator+=(time_t period)
{
    _seconds += period;
    return *this;
}

DateTime& DateTime::operator-=(time_t period)
{
    _seconds -= period;
    return *this;
}

time_t DateTime::operator-(const DateTime &another) const
{
    return _seconds - another._seconds;
}

/**
 * 获得并存储当前时刻
 */
void DateTime::set_to_now()
{
    ::time(&_seconds);
    _time_info = *::localtime(&_seconds);
}

DateTime DateTime::now()
{
    time_t seconds = 0;
    ::time(&seconds);
    return DateTime(seconds);
}

time_t DateTime::get_raw_seconds() const
{
    return _seconds;
}

uint8_t DateTime::get_second_of_minute() const
{
    return static_cast<uint8_t>(_time_info.tm_sec);
}

/**
 * 获得分钟数
 *
 * @return 范围 [0,59]
 */
uint8_t DateTime::get_minute_of_hour() const
{
    return static_cast<uint8_t>(_time_info.tm_min);
}

/**
 * 获得小时数
 *
 * @return 范围 [0,23]
 */
uint8_t DateTime::get_hour_of_day() const
{
    return static_cast<uint8_t>(_time_info.tm_hour);
}

/**
 * 获得月之中的日子数
 *
 * @return 范围 [1,31];  1 for the first day in a month
 */
uint8_t DateTime::get_day_of_month() const
{
    return static_cast<uint8_t>(_time_info.tm_mday);
}

/**
 * 获得月数
 *
 * @return 范围 [1,12];  1 for Junuary
 */
uint8_t DateTime::get_month_of_year() const
{
    return static_cast<uint8_t>(_time_info.tm_mon + 1);
}

/**
 * 获得年份
 *
 * @return 范围 year number; 2009 for the year of 2009
 */
uint32_t DateTime::get_year() const
{
    return static_cast<uint32_t>(_time_info.tm_year + 1900);
}

/**
 * 获得星期中的天数
 *
 * @return 范围 [0,6];   0 for Sunday
 */
uint8_t DateTime::get_day_of_week() const
{
    return static_cast<uint8_t>(_time_info.tm_wday);
}

/**
 * 获得一年中的天数
 *
 * @return 范围 [0,365]; 0 for the first day in a year
 */
uint16_t DateTime::get_day_of_year() const
{
    return static_cast<uint16_t>(_time_info.tm_yday);
}

// for example : "2007-3-12"
std::string DateTime::get_date_str() const
{
    return format_time("%Y-%m-%d");
}

// for example : "12:34:45"
std::string DateTime::get_clock_str() const
{
    return format_time("%H:%M:%S");
}

// for example : "2007-3-4 8:33:57"
std::string DateTime::get_datetime_str() const
{
    return get_date_str() + " " + get_clock_str();
}

std::string DateTime::to_string() const
{
    return get_datetime_str();
}

/**
 * format time string
 * for example : "%m-%d %H:%M" -> "2-28 8:57"
 */
std::string DateTime::format_time(const char *format) const
{
    assert(nullptr != format);
    size_t new_size = ::strlen(format) * 3;
    char * buf = (char*) ::malloc(new_size);
    ::strftime(buf, new_size, format, &_time_info);
    std::string ret(buf);
    ::free(buf);
    return ret;
}

}

#if NUT_PLATFORM_CC_VC
#   pragma warning(pop)
#endif
