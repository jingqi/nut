﻿
#ifndef ___HEADFILE___B1ADE08B_473E_40C6_A7C4_29E3F07E8EA1_
#define ___HEADFILE___B1ADE08B_473E_40C6_A7C4_29E3F07E8EA1_

#include <stdint.h>
#include <type_traits> // for std::underlying_type

namespace nut
{

/**
 * Log 等级
 */
enum class LogLevel : uint8_t
{
    Debug = 0x01,
    Info = 0x02,
    Warn = 0x04,
    Error = 0x08,
    Fatal = 0x10,

    // 全部 LogLevel 掩码
    AllLevels = 0x1F
};

// LogLevel 掩码类型
typedef std::underlying_type<LogLevel>::type loglevel_mask_type;

const char* log_level_to_str(LogLevel level);
char log_level_to_char(LogLevel level);
LogLevel str_to_log_level(const char *str);

}

#endif // head file guarder
