/**
 * @file -
 * @author jingqi
 * @date 2012-10-18
 * @last-edit 2012-10-18 19:39:11 jingqi
 */

#ifndef ___HEADFILE_A04A7FB7_1516_4EF0_A8B9_44C5AABBF7EC_
#define ___HEADFILE_A04A7FB7_1516_4EF0_A8B9_44C5AABBF7EC_

#include <assert.h>

namespace nut
{

/**
 * 构建KMP自动状态机
 *
 * @param target 长度为 len 的字符串
 * @param next 长度为 len 的缓冲区，存放KMP自动状态机
 */
inline void buildKmpNext(const char *target, int *next, size_t len)
{
    assert(NULL != target && NULL != next && len > 0);
    if (len > 0)
        next[0] = 0;
    size_t i = 1;
    int state = 0;
    while (i < len)
    {
        if (target[i] == target[state])
            next[i++] = ++state;
        else if (0 == state)
            next[i++] = 0;
        else
            state = next[state - 1];
    }
}

/**
 * KMP搜索字符串
 *
 * @param start 开始搜索的位置
 */
inline int indexOf(const char *src, size_t len_src, size_t start, const char *target, const int *next, size_t len_target)
{
    size_t i = start;
    int state = 0;
    while (i < len_src && state < len_target)
    {
        if (src[i] == target[state])
        {
            ++i;
            ++state;
        }
        else if (0 == state)
        {
            ++i;
        }
        else
        {
            state = next[state - 1];
        }
    }
    if (state == len_target)
        return i - len_target;
    return -1;
}

inline int indexOf(const char *src, size_t len_src, const char *target, const int *next, size_t len_target)
{
    return indexOf(src, len_src, 0, target, next, len_target);
}

}

#endif

