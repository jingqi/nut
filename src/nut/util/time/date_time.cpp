
#include <assert.h>
#include <time.h>
#include <string.h>     /* for strlen() */
#include <stdlib.h>     /* for malloc() and free() */

#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_CC_VC)
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
{
    ::time(&_seconds);
    _time_info = *localtime(&_seconds);
}

/**
 * 从1970年1月1日起算的时间(秒)
 * @param s
 */
DateTime::DateTime(time_t s)
    : _seconds(s)
{
    _time_info = *localtime(&_seconds);
}

/**
 * 使用具体时刻初始化
 */
DateTime::DateTime(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    ::memset(&_time_info, 0, sizeof(_time_info));
    _time_info.tm_year = year - 1900;
    _time_info.tm_mon = month - 1;
    _time_info.tm_mday = day;
    _time_info.tm_hour = hour;
    _time_info.tm_min = min;
    _time_info.tm_sec = sec;
    _seconds = mktime(&_time_info); /* '_time_info' is normalized also */

    /* 检查处理输入的结果 */
    assert(-1 != _seconds); /* 输入的数据有错误! */
}

/**
 * 获得并存储当前时刻
 */
void DateTime::update_to_current_time()
{
    ::time(&_seconds);
    _time_info = *localtime(&_seconds);
}

/// for example : "12:34:45"
std::string DateTime::get_clock_str() const
{
    return format_time("%H:%M:%S");
}

/// for example : "2007-3-12"
std::string DateTime::get_date_str() const
{
    return format_time("%Y-%m-%d");
}

/// for example : "2007-3-4 8:33:57"
std::string DateTime::get_time_str() const
{
    return get_date_str() + " " + get_clock_str();
}

/**
 * format time string
 * for example : "%m-%d %H:%M" -> "2-28 8:57"
 */
std::string DateTime::format_time(const char *format) const
{
    assert(NULL != format);
    size_t new_size = ::strlen(format) * 3;
    char * buf = (char*) ::malloc(new_size);
    ::strftime(buf, new_size, format, &_time_info);
    std::string ret(buf);
    ::free(buf);
    return ret;
}

}

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif
