/**
 * @file -
 * @author jingqi
 * @date 2012-10-18
 * @last-edit 2012-10-18 19:39:11 jingqi
 */

#ifndef ___HEADFILE_A04A7FB7_1516_4EF0_A8B9_44C5AABBF7EC_
#define ___HEADFILE_A04A7FB7_1516_4EF0_A8B9_44C5AABBF7EC_

#include <assert.h>
#include <string>

namespace nut
{

/**
 * 构建KMP自动状态机
 *
 * @param target 长度为 len 的字符串
 * @param next 长度为 len 的缓冲区，存放KMP自动状态机(特征码)
 */
inline void kmp_build_next(const char *target, int *next, size_t len)
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

inline void kmp_build_next(const wchar_t *target, int *next, size_t len)
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
 * 更新KMP状态
 */
inline int kmp_update(char c, int state, const char *target, const int *next)
{
    assert(state >= 0 && NULL != target && NULL != next);
    while (true)
    {
        if (c == target[state])
            return ++state;
        else if (0 == state)
            return 0;
        else
            state = next[state - 1];
    }
}

inline int kmp_update(wchar_t c, int state, const wchar_t *target, const int *next)
{
    assert(state >= 0 && NULL != target && NULL != next);
    while (true)
    {
        if (c == target[state])
            return ++state;
        else if (0 == state)
            return 0;
        else
            state = next[state - 1];
    }
}

/**
 * KMP搜索字符串
 *
 * @param start 开始搜索的位置
 */
inline int kmp_search(const char *src, size_t len_src, size_t start, const char *target, const int *next, size_t len_target)
{
    assert(NULL != src && NULL != target && NULL != next);
    size_t i = start;
    int state = 0; // 状态，其实代表着已经匹配的字符串长度
    while (i < len_src && ((size_t) state) < len_target)
        state = kmp_update(src[i++], state, target, next);
    if (state == len_target)
        return i - len_target;
    return -1;
}

inline int kmp_search(const wchar_t *src, size_t len_src, size_t start, const wchar_t *target, const int *next, size_t len_target)
{
    assert(NULL != src && NULL != target && NULL != next);
    size_t i = start;
    int state = 0; // 状态，其实代表着已经匹配的字符串长度
    while (i < len_src && ((size_t) state) < len_target)
        state = kmp_update(src[i++], state, target, next);
    if (state == len_target)
        return i - len_target;
    return -1;
}

/**
 * KMP 搜索
 * 这里对于要搜索字符串的特征码只做局部缓存
 */
inline int kmp_search(const std::string& src, size_t start, const std::string& target)
{
    int *next = new int[target.length()];
    kmp_build_next(target.c_str(), next, target.length());
    const int ret = kmp_search(src.c_str(), src.length(), start, target.c_str(), next, target.length());
    delete[] next;
    return ret;
}

inline int kmp_search(const std::wstring& src, size_t start, const std::wstring& target)
{
    int *next = new int[target.length()];
    kmp_build_next(target.c_str(), next, target.length());
    const int ret = kmp_search(src.c_str(), src.length(), start, target.c_str(), next, target.length());
    delete[] next;
    return ret;
}

}

#endif

