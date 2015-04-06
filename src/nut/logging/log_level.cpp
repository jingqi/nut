
#include <assert.h>
#include <string.h>

#include "log_level.h"

namespace nut
{

const char * log_level_to_str(LogLevel level)
{
    struct LevelInfo
    {
        LogLevel l;
        const char * s;
    };

#define _H(l) {l,#l}
    static LevelInfo info[] = {
        _H(LL_DEBUG),
        _H(LL_INFO),
        _H(LL_WARN),
        _H(LL_ERROR),
        _H(LL_FATAL)
    };
#undef _H
    assert(sizeof(info) / sizeof(info[0]) == COUNT_OF_LOG_LEVEL);

    for (int i = 0; i < COUNT_OF_LOG_LEVEL; ++i)
        if (info[i].l == level)
            return info[i].s;
    return "";
}

LogLevel str_to_log_level(const char *str)
{
    struct LevelInfo
    {
        LogLevel l;
        const char * s;
    };

#define _H(l) {l,#l}
    static LevelInfo info[] = {
        _H(LL_DEBUG),
        _H(LL_INFO),
        _H(LL_WARN),
        _H(LL_ERROR),
        _H(LL_FATAL)
    };
#undef _H
    assert(sizeof(info) / sizeof(info[0]) == COUNT_OF_LOG_LEVEL);

    if (str == NULL)
        return LL_FATAL;
    for (int i = 0; i < COUNT_OF_LOG_LEVEL; ++i)
        if (0 == ::strcmp(info[i].s, str))
            return info[i].l;
    return LL_FATAL;
}

}
