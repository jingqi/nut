/**
 * @file -
 * @author jingqi
 * @date 2009-11-17
 */

#ifndef ___HEADFILE___93387CEA_B68D_450E_8F86_5BCC3961AE9A_
#define ___HEADFILE___93387CEA_B68D_450E_8F86_5BCC3961AE9A_

#include <time.h>
#include <string.h>     /* for strlen() */
#include <stdlib.h>     /* for malloc() and free() */
#include <string>
#include <assert.h>
#include <stdint.h>     /* for uint8_t and so on */

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

/**
 * 时刻
 */
class Time
{
protected :
    time_t seconds;      /* 从1970年1月1日起算的时间(秒) */
    struct tm timeInfo;   /* 时刻的细节 */

public :
    /**
     * 获取当前时间
     */
    Time()
    {
        time(&seconds);
        timeInfo = *localtime(&seconds);
    }

    /**
     * 从1970年1月1日起算的时间(秒)
     * @param s
     */
    Time(time_t s)
        : seconds(s)
    {
        timeInfo = *localtime(&seconds);
    }

    /**
     * 使用具体时刻初始化
     */
    Time(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec){
        timeInfo.tm_year = year - 1900;
        timeInfo.tm_mon = month - 1;
        timeInfo.tm_mday = day;
        timeInfo.tm_hour = hour;
        timeInfo.tm_min = min;
        timeInfo.tm_sec = sec;
        seconds = mktime(&timeInfo);

        /* 检查处理输入的结果 */
        assert(-1 != seconds); /* 输入的数据有错误! */

        timeInfo = *localtime(&seconds);
    }

    /**
     * 时刻比较
     */
    bool operator ==(const Time &another) const
    {
        return seconds == another.seconds;
    }

    /**
     * 时刻比较
     */
    bool operator !=(const Time &another) const
    {
        return seconds != another.seconds;
    }

    /**
     * 时刻比较
     */
    bool operator >(const Time &another) const
    {
        return seconds > another.seconds;
    }

    /**
     * 时刻比较
     */
    bool operator <(const Time &another) const
    {
        return seconds < another.seconds;
    }

    /**
     * 时刻比较
     */
    bool operator >=(const Time &another) const
    {
        return seconds >= another.seconds;
    }

    /**
     * 时刻比较
     */
    bool operator <=(const Time &another) const
    {
        return seconds <= another.seconds;
    }

    Time operator +(time_t period) const
    {
        return Time(seconds + period);
    }

    Time operator -(time_t period) const
    {
        return Time(seconds - period);
    }

    Time& operator +=(time_t period)
    {
        seconds += period;
        return *this;
    }

    Time& operator -=(time_t period)
    {
        seconds -= period;
        return *this;
    }

    time_t operator -(const Time &another) const
    {
        return seconds - another.seconds;
    }

    /**
     * 获得并存储当前时刻
     */
    void updateToCurrentTime()
    {
        time(&seconds);
        timeInfo = *localtime(&seconds);
    }

    time_t getOriSeconds() const
    {
        return seconds;
    }

    /**
     * 获得秒数
     * @return 范围 [0,59]
     */
    uint8_t getSecondOfMinute() const
    {
        return static_cast<uint8_t>(timeInfo.tm_sec);
    }

    /**
     * 获得分钟数
     * @return 范围 [0,59]
     */
    uint8_t getMinuteOfHour() const
    {
        return static_cast<uint8_t>(timeInfo.tm_min);
    }

    /**
     * 获得小时数
     * @return 范围 [0,23]
     */
    uint8_t getHourOfDay() const
    {
        return static_cast<uint8_t>(timeInfo.tm_hour);
    }

    /**
     * 获得月之中的日子数
     * @return 范围 [1,31];  1 for the first day in a month
     */
    uint8_t getDayOfMonth() const
    {
        return static_cast<uint8_t>(timeInfo.tm_mday);
    }

    /**
     * 获得月数
     * @return 范围 [1,12];  1 for Junuary
     */
    uint8_t getMonthOfYear() const
    {
        return static_cast<uint8_t>(timeInfo.tm_mon + 1);
    }

    /**
     * 获得年份
     * @return 范围 year number; 2009 for the year of 2009
     */
    uint32_t getYear() const
    {
        return static_cast<uint32_t>(timeInfo.tm_year + 1900);
    }

    /**
     * @brief 获得星期中的天数
     * @return 范围 [0,6];   0 for Sunday
     */
    uint8_t getDayOfWeek() const
    {
        return static_cast<uint8_t>(timeInfo.tm_wday);
    }

    /**
     * 获得一年中的天数
     * @return 范围 [0,365]; 0 for the first day in a year
     */
    uint16_t  getDayOfYear() const
    {
        return static_cast<uint16_t>(timeInfo.tm_yday);
    }

    /// for example : "12:34:45"
    std::string getClockStr() const
    {
        return formatTime("%H:%M:%S");
    }

    /// for example : "2007-3-12"
    std::string getDateStr() const
    {
        return formatTime("%Y-%m-%d");
    }

    /// for example : "2007-3-4 8:33:57"
    std::string getTimeStr() const
    {
        return getDateStr() + " " + getClockStr();
    }

    std::string toString() const
    {
        return getTimeStr();
    }

    /**
     * format time string
     * for example : "%m-%d %H:%M" -> "2-28 8:57"
     */
    std::string formatTime (const char *format) const
    {
        assert(NULL != format);
        size_t newSize = strlen(format) * 3;
        char * buf = (char*)malloc(newSize);
        strftime(buf, newSize, format, &timeInfo);
        std::string ret(buf);
        free(buf);
        return ret;
    }
};

}


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

#endif /* head file guarder */
