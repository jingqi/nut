
#include <string.h>

#include "log_level.h"

namespace nut
{

const char* log_level_to_str(LogLevel level)
{
    switch (level)
    {
#define _H(l) case l: return #l
    _H(LL_DEBUG);
    _H(LL_INFO);
    _H(LL_WARN);
    _H(LL_ERROR);
    _H(LL_FATAL);
#undef _H
    default:
        return "LL_FATAL";
    }
}

LogLevel str_to_log_level(const char *str)
{
#define _H(l) else if (0 == ::strcmp(str, #l)) return l

    if (NULL == str)
        return LL_FATAL;
    _H(LL_DEBUG);
    _H(LL_INFO);
    _H(LL_WARN);
    _H(LL_ERROR);
    _H(LL_FATAL);
    else
        return LL_FATAL;

#undef _H
}

}

