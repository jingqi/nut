
#include <string.h>

#include "../util/string/string_utils.h"
#include "log_level.h"


namespace nut
{

const char* log_level_to_str(enum LogLevel level) noexcept
{
    // 为了对齐，保持返回字符串长度一致
    switch (level)
    {
    case LL_DEBUG:
        return "DEBUG";

    case LL_INFO:
        return "INFO ";

    case LL_WARN:
        return "WARN ";

    case LL_ERROR:
        return "ERROR";

    case LL_FATAL:
        return "FATAL";

    default:
        return "UNKWN"; // UNKOWN
    }
}

char log_level_to_char(enum LogLevel level) noexcept
{
    // 为了对齐，保持返回字符串长度一致
    switch (level)
    {
    case LL_DEBUG:
        return 'D';

    case LL_INFO:
        return 'I';

    case LL_WARN:
        return 'W';

    case LL_ERROR:
        return 'E';

    case LL_FATAL:
        return 'F';

    default:
        return 'U';
    }
}

enum LogLevel str_to_log_level(const char *str) noexcept
{
    if (nullptr == str || 0 == strincmp(str, "DEBUG", 5))
        return LL_DEBUG;
    else if (0 == strincmp(str, "INFO", 4))
        return LL_INFO;
    else if (0 == strincmp(str, "WARN", 4))
        return LL_WARN;
    else if (0 == strincmp(str, "ERROR", 5))
        return LL_ERROR;
    else if (0 == strincmp(str, "FATAL", 5))
        return LL_FATAL;

    return LL_DEBUG;
}

}
