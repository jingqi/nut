
#ifndef ___HEADFILE___93387CEA_B68D_450E_8F86_5BCC3961AE9A_
#define ___HEADFILE___93387CEA_B68D_450E_8F86_5BCC3961AE9A_

#include <time.h>
#include <string>
#include <stdint.h>

#include "../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "../nut_config.h"
#include "time_diff.h"


namespace nut
{

/**
 * 日期时间
 */
class NUT_API DateTime
{
public:
    DateTime() noexcept;

    /**
     * @param s 秒，从 1970/1/1 00:00:00 起算的时间
     */
    explicit DateTime(double s) noexcept;

    /**
     * @param s 秒，从 1970/1/1 00:00:00 起算的时间
     * @param ns 纳秒
     */
    explicit DateTime(time_t s, long ns) noexcept;

    /**
     * 使用具体时刻初始化
     *
     * @param year 年份，如 2018
     * @param month 月份，取值范围 [1,12]
     * @param day 取值范围 [1,31]
     * @param hour 取值范围 [0,23]
     * @param min 取值范围 [0,59]
     * @param sec 取值范围 [0,59]
     * @param nsec 取值范围 [0,999999999]
     * @param utc 是本地时间还是 UTC 时间
     */
    DateTime(uint32_t year, uint8_t month, uint8_t day, uint8_t hour = 0,
             uint8_t min = 0, uint8_t sec = 0, uint32_t nsec = 0, bool utc = false) noexcept;

    bool operator==(const DateTime &x) const noexcept;
    bool operator!=(const DateTime &x) const noexcept;
    bool operator<(const DateTime &x) const noexcept;
    bool operator>(const DateTime &x) const noexcept;
    bool operator>=(const DateTime &x) const noexcept;
    bool operator<=(const DateTime &x) const noexcept;

    DateTime operator+(const TimeDiff& diff) const noexcept;
    DateTime operator+(double seconds) const noexcept;

    DateTime operator-(const TimeDiff& diff) const noexcept;
    DateTime operator-(double seconds) const noexcept;
    TimeDiff operator-(const DateTime& x) const noexcept;

    DateTime& operator+=(const TimeDiff& diff) noexcept;
    DateTime& operator+=(double seconds) noexcept;

    DateTime& operator-=(const TimeDiff& diff) noexcept;
    DateTime& operator-=(double seconds) noexcept;

    void set(double s) noexcept;
    void set(time_t s, long ns) noexcept;
    void set(uint32_t year, uint8_t month, uint8_t day, uint8_t hour = 0,
             uint8_t min = 0, uint8_t sec = 0, uint32_t nsec = 0, bool utc = false) noexcept;

#if NUT_PLATFORM_OS_WINDOWS
    /**
     * NOTE SYSTEMTIME 表示的是永远是本地时间，不会是 UTC 时间
     */
    void set(const SYSTEMTIME& wtm, bool utc = false) noexcept;
    void to_wtm(SYSTEMTIME *wtm, bool utc = false) const noexcept;
#else
    void set(const struct timeval& tv) noexcept;
    void set(const struct timespec& tv) noexcept;
    void to_timeval(struct timeval *tv) const noexcept;
    void to_timespec(struct timespec *tv) const noexcept;
#endif

    /**
     * 获得并存储当前时刻
     */
    void set_to_now() noexcept;

    static DateTime now() noexcept;

    /**
     * 获得年份
     *
     * @return 范围 year number; 2009 for the year of 2009
     */
    uint32_t get_year(bool utc = false) const noexcept;

    /**
     * 获得月数
     *
     * @return 范围 [1,12];  1 for Junuary
     */
    uint8_t get_month(bool utc = false) const noexcept;

    /**
     * 获得一年中的天数
     *
     * @return 范围 [0,365]; 0 for the first day in a year
     */
    uint16_t get_yday(bool utc = false) const noexcept;

    /**
     * 获得月之中的日子数
     *
     * @return 范围 [1,31];  1 for the first day in a month
     */
    uint8_t get_mday(bool utc = false) const noexcept;

    /**
     * 获得星期中的天数
     *
     * @return 范围 [0,6];   0 for Sunday
     */
    uint8_t get_wday(bool utc = false) const noexcept;

    /**
     * 获得小时数
     *
     * @return 范围 [0,23]
     */
    uint8_t get_hour(bool utc = false) const noexcept;

    /**
     * 获得分钟数
     *
     * @return 范围 [0,59]
     */
    uint8_t get_minute(bool utc = false) const noexcept;

    /**
     * 获得秒数
     *
     * @return 范围 [0,59]
     */
    uint8_t get_second(bool utc = false) const noexcept;

    /**
     * 获得微秒数
     *
     * @return 范围 [0,999999999]
     */
    uint32_t get_nanosecond() const noexcept;

    /**
     * 是否是有效状态
     */
    bool is_valid() const noexcept;

    /**
     * @return 从1970年1月1日0时0分0秒开始计算的秒数
     */
    time_t to_integer() const noexcept;
    double to_double() const noexcept;

    /* for example : "2007-3-12" */
    std::string get_date_str(bool utc = false) const noexcept;

    /* for example : "12:34:45.572936192" */
    std::string get_clock_str(bool utc = false) const noexcept;

    /* for example : "2007-3-4 8:33:57.762917263" */
    std::string get_datetime_str(bool utc = false) const noexcept;

    std::string to_string() const noexcept;

    /**
     * 格式化时间
     *
     * %Y 年份; 4字符
     * %y 年份; 2字符
     * %m 月份; 2字符, 不足2字符时前面补0
     * %d 月份中的天数; 2字符, 不足2字符时前面补0
     * %j 年中的天数
     * %H 24小时制小时数; 2字符, 不足2字符时前面补0
     * %I 12小时制小时数; 2字符, 不足2字符时前面补0
     * %M 分钟数; 24小时制; 2字符, 不足2字符时前面补0
     * %S 秒数; 24小时制; 2字符, 不足2字符时前面补0
     * %9f 秒数的浮点数部分; 需要指定保留的字符数, 例如 "%3f", 0.1234 -> 123
     */
    std::string format_time(const char *format, bool utc = false) const noexcept;

protected:
    void normalize() noexcept;
    void ensure_time_info(bool utc) const noexcept;

protected:
    /* 从 1970年1月1日 0时0分0秒 起算的 UTC 时间 */
    time_t _seconds;
    long _nanoseconds;

    /* 结构化本地时间 */
    mutable struct tm _time_info;
    mutable bool _time_info_utc = false, _time_info_dirty = true;
};

/**
 * time between jan 1, 1601 and jan 1, 1970 in units of 100 nanoseconds
 *
 * mingw 没有定义clock_gettime(), 这里参考其pthread_mutex_timedlock.c ptw32_relmillisecs.c 的实现
 * 相当于 ::clock_gettime(CLOCK_REALTIME, &ts);
 */
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_MINGW
void clock_getrealtime(struct timespec *ts) noexcept;
#endif

}

#endif /* head file guarder */
