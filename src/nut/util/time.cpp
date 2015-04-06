
#include <assert.h>
#include <time.h>
#include <string.h>     /* for strlen() */
#include <stdlib.h>     /* for malloc() and free() */

#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

#include "time.h"

namespace nut
{

/**
 * 获取当前时间
 */
Time::Time()
{
    ::time(&seconds);
    time_info = *localtime(&seconds);
}

/**
 * 从1970年1月1日起算的时间(秒)
 * @param s
 */
Time::Time(time_t s)
    : seconds(s)
{
    time_info = *localtime(&seconds);
}

/**
 * 使用具体时刻初始化
 */
Time::Time(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    time_info.tm_year = year - 1900;
    time_info.tm_mon = month - 1;
    time_info.tm_mday = day;
    time_info.tm_hour = hour;
    time_info.tm_min = min;
    time_info.tm_sec = sec;
    seconds = mktime(&time_info);

    /* 检查处理输入的结果 */
    assert(-1 != seconds); /* 输入的数据有错误! */

    time_info = *localtime(&seconds);
}

/**
 * 获得并存储当前时刻
 */
void Time::update_to_current_time()
{
    ::time(&seconds);
    time_info = *localtime(&seconds);
}

/// for example : "12:34:45"
std::string Time::get_clock_str() const
{
    return format_time("%H:%M:%S");
}

/// for example : "2007-3-12"
std::string Time::get_date_str() const
{
    return format_time("%Y-%m-%d");
}

/// for example : "2007-3-4 8:33:57"
std::string Time::get_time_str() const
{
    return get_date_str() + " " + get_clock_str();
}

/**
 * format time string
 * for example : "%m-%d %H:%M" -> "2-28 8:57"
 */
std::string Time::format_time(const char *format) const
{
    assert(NULL != format);
    size_t new_size = ::strlen(format) * 3;
    char * buf = (char*) ::malloc(new_size);
    ::strftime(buf, new_size, format, &time_info);
    std::string ret(buf);
    ::free(buf);
    return ret;
}

}

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif
