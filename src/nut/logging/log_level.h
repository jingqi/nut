
#ifndef ___HEADFILE___B1ADE08B_473E_40C6_A7C4_29E3F07E8EA1_
#define ___HEADFILE___B1ADE08B_473E_40C6_A7C4_29E3F07E8EA1_

#include <stdint.h>
#include <type_traits> // for std::underlying_type


namespace nut
{

/**
 * Log 等级
 */
enum LogLevel
{
    LL_DEBUG = 0x01,
    LL_INFO = 0x02,
    LL_WARN = 0x04,
    LL_ERROR = 0x08,
    LL_FATAL = 0x10,

    // 全部 LogLevel 掩码
    LL_ALL_LEVELS = 0x1F
};

// LogLevel 掩码类型
typedef uint8_t loglevel_mask_type;

const char* log_level_to_str(enum LogLevel level) noexcept;
char log_level_to_char(enum LogLevel level) noexcept;
enum LogLevel str_to_log_level(const char *str) noexcept;

}

#endif // head file guarder
