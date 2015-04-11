
#ifndef ___HEADFILE___93387CEA_B68D_450E_8F86_5BCC3961AE9A_
#define ___HEADFILE___93387CEA_B68D_450E_8F86_5BCC3961AE9A_

#include <time.h>
#include <string>
#include <stdint.h>

namespace nut
{

/**
 * 时刻
 */
class Time
{
protected:
    time_t seconds;      /* 从1970年1月1日起算的时间(秒) */
    struct tm time_info;   /* 时刻的细节 */

public:
    /**
     * 获取当前时间
     */
    Time();

    /**
     * 从1970年1月1日起算的时间(秒)
     * @param s
     */
    Time(time_t s);

    /**
     * 使用具体时刻初始化
     */
    Time(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

    /**
     * 时刻比较
     */
    bool operator==(const Time &another) const
    {
        return seconds == another.seconds;
    }

    /**
     * 时刻比较
     */
    bool operator!=(const Time &another) const
    {
        return seconds != another.seconds;
    }

    /**
     * 时刻比较
     */
    bool operator>(const Time &another) const
    {
        return seconds > another.seconds;
    }

    /**
     * 时刻比较
     */
    bool operator<(const Time &another) const
    {
        return seconds < another.seconds;
    }

    /**
     * 时刻比较
     */
    bool operator>=(const Time &another) const
    {
        return seconds >= another.seconds;
    }

    /**
     * 时刻比较
     */
    bool operator<=(const Time &another) const
    {
        return seconds <= another.seconds;
    }

    Time operator+(time_t period) const
    {
        return Time(seconds + period);
    }

    Time operator-(time_t period) const
    {
        return Time(seconds - period);
    }

    Time& operator+=(time_t period)
    {
        seconds += period;
        return *this;
    }

    Time& operator-=(time_t period)
    {
        seconds -= period;
        return *this;
    }

    time_t operator-(const Time &another) const
    {
        return seconds - another.seconds;
    }

    /**
     * 获得并存储当前时刻
     */
    void update_to_current_time();

    time_t get_ori_seconds() const
    {
        return seconds;
    }

    /**
     * 获得秒数
     * @return 范围 [0,59]
     */
    uint8_t get_second_of_minute() const
    {
        return static_cast<uint8_t>(time_info.tm_sec);
    }

    /**
     * 获得分钟数
     * @return 范围 [0,59]
     */
    uint8_t get_minute_of_hour() const
    {
        return static_cast<uint8_t>(time_info.tm_min);
    }

    /**
     * 获得小时数
     * @return 范围 [0,23]
     */
    uint8_t get_hour_of_day() const
    {
        return static_cast<uint8_t>(time_info.tm_hour);
    }

    /**
     * 获得月之中的日子数
     * @return 范围 [1,31];  1 for the first day in a month
     */
    uint8_t get_day_of_month() const
    {
        return static_cast<uint8_t>(time_info.tm_mday);
    }

    /**
     * 获得月数
     * @return 范围 [1,12];  1 for Junuary
     */
    uint8_t get_month_of_year() const
    {
        return static_cast<uint8_t>(time_info.tm_mon + 1);
    }

    /**
     * 获得年份
     * @return 范围 year number; 2009 for the year of 2009
     */
    uint32_t get_year() const
    {
        return static_cast<uint32_t>(time_info.tm_year + 1900);
    }

    /**
     * @brief 获得星期中的天数
     * @return 范围 [0,6];   0 for Sunday
     */
    uint8_t get_day_of_week() const
    {
        return static_cast<uint8_t>(time_info.tm_wday);
    }

    /**
     * 获得一年中的天数
     * @return 范围 [0,365]; 0 for the first day in a year
     */
    uint16_t  get_day_of_year() const
    {
        return static_cast<uint16_t>(time_info.tm_yday);
    }

    /// for example : "12:34:45"
    std::string get_clock_str() const;

    /// for example : "2007-3-12"
    std::string get_date_str() const;

    /// for example : "2007-3-4 8:33:57"
    std::string get_time_str() const;

    std::string to_string() const
    {
        return get_time_str();
    }

    /**
     * format time string
     * for example : "%m-%d %H:%M" -> "2-28 8:57"
     */
    std::string format_time(const char *format) const;
};

}

#endif /* head file guarder */
