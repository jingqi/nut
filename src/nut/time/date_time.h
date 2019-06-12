﻿
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
     * @param year, 年份，如 2018
     * @param month, 月份，取值范围 [1,12]
     * @param day, 取值范围 [1,31]
     * @param hour, 取值范围 [0,23]
     * @param min, 取值范围 [0,59]
     * @param sec, 取值范围 [0,59]
     * @param nsec, 取值范围 [0,999999999]
     */
    DateTime(uint32_t year, uint8_t month, uint8_t day, uint8_t hour = 0,
             uint8_t min = 0, uint8_t sec = 0, uint32_t nsec = 0) noexcept;

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
             uint8_t min = 0, uint8_t sec = 0, uint32_t nsec = 0) noexcept;

#if NUT_PLATFORM_OS_WINDOWS
    void set(const SYSTEMTIME& wtm) noexcept;
    void to_wtm(SYSTEMTIME *wtm) const noexcept;
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
    uint32_t get_year() const noexcept;

    /**
     * 获得月数
     *
     * @return 范围 [1,12];  1 for Junuary
     */
    uint8_t get_month() const noexcept;

    /**
     * 获得一年中的天数
     *
     * @return 范围 [0,365]; 0 for the first day in a year
     */
    uint16_t get_yday() const noexcept;

    /**
     * 获得月之中的日子数
     *
     * @return 范围 [1,31];  1 for the first day in a month
     */
    uint8_t get_mday() const noexcept;

    /**
     * 获得星期中的天数
     *
     * @return 范围 [0,6];   0 for Sunday
     */
    uint8_t get_wday() const noexcept;

    /**
     * 获得小时数
     *
     * @return 范围 [0,23]
     */
    uint8_t get_hour() const noexcept;

    /**
     * 获得分钟数
     *
     * @return 范围 [0,59]
     */
    uint8_t get_minute() const noexcept;

    /**
     * 获得秒数
     *
     * @return 范围 [0,59]
     */
    uint8_t get_second() const noexcept;

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
    std::string get_date_str() const noexcept;

    /* for example : "12:34:45.572936192" */
    std::string get_clock_str() const noexcept;

    /* for example : "2007-3-4 8:33:57.762917263" */
    std::string get_datetime_str() const noexcept;

    std::string to_string() const noexcept;

    /**
     * format time string
     * for example : "%m-%d %H:%M" -> "2-28 8:57"
     */
    std::string format_time(const char *format) const noexcept;

protected:
    void normalize() noexcept;
    void check_time_info() const noexcept;

protected:
    /* 从 1970年1月1日 0时0分0秒 起算的 UTC 时间 */
    time_t _seconds;
    long _nanoseconds;

    /* 结构化本地时间 */
    mutable struct tm _time_info;
    mutable bool _time_info_dirty = true;
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
