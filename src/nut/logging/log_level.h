
#ifndef ___HEADFILE___B1ADE08B_473E_40C6_A7C4_29E3F07E8EA1_
#define ___HEADFILE___B1ADE08B_473E_40C6_A7C4_29E3F07E8EA1_

namespace nut
{

/// Log 的级别
enum LogLevel
{
    LL_DEBUG = 0,
    LL_INFO,
    LL_WARN,
    LL_ERROR,
    LL_FATAL,

    /// LogLevel 的种类
    COUNT_OF_LOG_LEVEL
};

const char * log_level_to_str(LogLevel level);
LogLevel str_to_log_level(const char *str);

}

#endif // head file guarder
