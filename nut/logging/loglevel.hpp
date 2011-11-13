/**
 * @file -
 * @author jingqi
 * @date 2010-8-14
 */

#ifndef ___HEADFILE___B1ADE08B_473E_40C6_A7C4_29E3F07E8EA1_
#define ___HEADFILE___B1ADE08B_473E_40C6_A7C4_29E3F07E8EA1_

#include <assert.h>

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
};

/// LogLevel 的种类
enum { COUNT_OF_LOG_LEVEL = LL_FATAL + 1 };

inline const char * logLevelToStr(LogLevel level)
{
    struct LevelInfo { LogLevel l; const char * s; };
#define _H(l) {l,#l}
    static LevelInfo info[] = { _H(LL_DEBUG),_H(LL_INFO),_H(LL_WARN),_H(LL_ERROR),_H(LL_FATAL) };
#undef _H
    assert(sizeof(info) / sizeof(info[0]) == COUNT_OF_LOG_LEVEL);

    for (int i = 0; i < COUNT_OF_LOG_LEVEL; ++i)
        if (info[i].l == level)
            return info[i].s;
    return "";
}

inline LogLevel strToLogLevel(const char *str)
{
    struct LevelInfo { LogLevel l; const char * s; };
#define _H(l) {l,#l}
    static LevelInfo info[] = { _H(LL_DEBUG),_H(LL_INFO),_H(LL_WARN),_H(LL_ERROR),_H(LL_FATAL) };
#undef _H
    assert(sizeof(info) / sizeof(info[0]) == COUNT_OF_LOG_LEVEL);

    if (str == NULL)
        return LL_FATAL;
    for (int i = 0; i < COUNT_OF_LOG_LEVEL; ++i)
        if (0 == strcmp(info[i].s, str))
            return info[i].l;
    return LL_FATAL;
}

}

#endif // head file guarder

