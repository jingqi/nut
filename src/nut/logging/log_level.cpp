
#include <string.h>

#include <nut/util/string/string_util.h>

#include "log_level.h"

namespace nut
{

const char* log_level_to_str(LogLevel level)
{
    // 为了对齐，保持返回字符串长度一致
    switch (level)
    {
    case LogLevel::LL_DEBUG:
        return "DEBUG";

    case LogLevel::LL_INFO:
        return "INFO ";

    case LogLevel::LL_WARN:
        return "WARN ";

    case LogLevel::LL_ERROR:
        return "ERROR";

    case LogLevel::LL_FATAL:
        return "FATAL";

    default:
        return "UNKWN"; // UNKOWN
    }
}

char log_level_to_char(LogLevel level)
{
    // 为了对齐，保持返回字符串长度一致
    switch (level)
    {
    case LogLevel::LL_DEBUG:
        return 'D';

    case LogLevel::LL_INFO:
        return 'I';

    case LogLevel::LL_WARN:
        return 'W';

    case LogLevel::LL_ERROR:
        return 'E';

    case LogLevel::LL_FATAL:
        return 'F';

    default:
        return 'U';
    }
}

LogLevel str_to_log_level(const char *str)
{
    if (nullptr == str || 0 == strincmp(str, "DEBUG", 5))
        return LogLevel::LL_DEBUG;
    else if (0 == strincmp(str, "INFO", 4))
        return LogLevel::LL_INFO;
    else if (0 == strincmp(str, "WARN", 4))
        return LogLevel::LL_WARN;
    else if (0 == strincmp(str, "ERROR", 5))
        return LogLevel::LL_ERROR;
    else if (0 == strincmp(str, "FATAL", 5))
        return LogLevel::LL_FATAL;

    return LogLevel::LL_DEBUG;
}

}

