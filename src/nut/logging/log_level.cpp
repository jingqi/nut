
#include <string.h>

#include "log_level.h"

namespace nut
{

const char* log_level_to_str(LogLevel level)
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
    default:
        return "FATAL";
    }
}

LogLevel str_to_log_level(const char *str)
{
    if (NULL == str)
        return LL_FATAL;
    else if (0 == ::strncmp(str, "DEBUG", 5))
        return LL_DEBUG;
    else if (0 == ::strncmp(str, "INFO", 4))
        return LL_INFO;
    else if (0 == ::strncmp(str, "WARN", 4))
        return LL_WARN;
    else if (0 == ::strncmp(str, "ERROR", 5))
        return LL_ERROR;
    else if (0 == ::strncmp(str, "FATAL", 5))
        return LL_FATAL;

    return LL_FATAL;
}

}

