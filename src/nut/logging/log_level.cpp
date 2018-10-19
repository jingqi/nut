
#include <string.h>

#include <nut/util/string/string_utils.h>

#include "log_level.h"

namespace nut
{

const char* log_level_to_str(LogLevel level)
{
    // 为了对齐，保持返回字符串长度一致
    switch (level)
    {
    case LogLevel::Debug:
        return "DEBUG";

    case LogLevel::Info:
        return "INFO ";

    case LogLevel::Warn:
        return "WARN ";

    case LogLevel::Error:
        return "ERROR";

    case LogLevel::Fatal:
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
    case LogLevel::Debug:
        return 'D';

    case LogLevel::Info:
        return 'I';

    case LogLevel::Warn:
        return 'W';

    case LogLevel::Error:
        return 'E';

    case LogLevel::Fatal:
        return 'F';

    default:
        return 'U';
    }
}

LogLevel str_to_log_level(const char *str)
{
    if (nullptr == str || 0 == strincmp(str, "DEBUG", 5))
        return LogLevel::Debug;
    else if (0 == strincmp(str, "INFO", 4))
        return LogLevel::Info;
    else if (0 == strincmp(str, "WARN", 4))
        return LogLevel::Warn;
    else if (0 == strincmp(str, "ERROR", 5))
        return LogLevel::Error;
    else if (0 == strincmp(str, "FATAL", 5))
        return LogLevel::Fatal;

    return LogLevel::Debug;
}

}
