
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
class DateTime
{
protected:
    time_t _seconds;      /* 从1970年1月1日起算的时间(秒) */
    struct tm _time_info;   /* 时刻的细节 */

public:
    /**
     * 获取当前时间
     */
    DateTime();

    /**
     * 从1970年1月1日起算的时间(秒)
     *
     * @param s
     */
    DateTime(time_t s);

    /**
     * 使用具体时刻初始化
     */
    DateTime(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

    /**
     * 时刻比较
     */
    bool operator==(const DateTime &another) const;

    /**
     * 时刻比较
     */
    bool operator!=(const DateTime &another) const;

    /**
     * 时刻比较
     */
    bool operator>(const DateTime &another) const;

    /**
     * 时刻比较
     */
    bool operator<(const DateTime &another) const;

    /**
     * 时刻比较
     */
    bool operator>=(const DateTime &another) const;

    /**
     * 时刻比较
     */
    bool operator<=(const DateTime &another) const;

    DateTime operator+(time_t period) const;

    DateTime operator-(time_t period) const;

    DateTime& operator+=(time_t period);

    DateTime& operator-=(time_t period);

    time_t operator-(const DateTime &another) const;

    /**
     * 获得并存储当前时刻
     */
    void update_to_current_time();

    time_t get_ori_seconds() const;

    /**
     * 获得秒数
     *
     * @return 范围 [0,59]
     */
    uint8_t get_second_of_minute() const;

    /**
     * 获得分钟数
     *
     * @return 范围 [0,59]
     */
    uint8_t get_minute_of_hour() const;

    /**
     * 获得小时数
     *
     * @return 范围 [0,23]
     */
    uint8_t get_hour_of_day() const;

    /**
     * 获得月之中的日子数
     *
     * @return 范围 [1,31];  1 for the first day in a month
     */
    uint8_t get_day_of_month() const;

    /**
     * 获得月数
     *
     * @return 范围 [1,12];  1 for Junuary
     */
    uint8_t get_month_of_year() const;

    /**
     * 获得年份
     *
     * @return 范围 year number; 2009 for the year of 2009
     */
    uint32_t get_year() const;

    /**
     * 获得星期中的天数
     *
     * @return 范围 [0,6];   0 for Sunday
     */
    uint8_t get_day_of_week() const;

    /**
     * 获得一年中的天数
     *
     * @return 范围 [0,365]; 0 for the first day in a year
     */
    uint16_t get_day_of_year() const;

    // for example : "12:34:45"
    std::string get_clock_str() const;

    // for example : "2007-3-12"
    std::string get_date_str() const;

    // for example : "2007-3-4 8:33:57"
    std::string get_time_str() const;

    std::string to_string() const;

    /**
     * format time string
     * for example : "%m-%d %H:%M" -> "2-28 8:57"
     */
    std::string format_time(const char *format) const;
};

}

#endif /* head file guarder */
