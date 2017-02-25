
#ifndef ___HEADFILE_A04A7FB7_1516_4EF0_A8B9_44C5AABBF7EC_
#define ___HEADFILE_A04A7FB7_1516_4EF0_A8B9_44C5AABBF7EC_

#include <assert.h>
#include <string>

#include "../../nut_config.h"


namespace nut
{

/**
 * 构建KMP自动状态机(特征码)
 *
 * @param target 长度为 len 的字符串，即要搜索的字符串子串
 * @param next 长度为 len 的缓冲区，用于存放生成的KMP自动状态机(特征码)
 */
template <typename T>
void kmp_build_next(const T *target, int *next, size_t len)
{
    assert(nullptr != target && nullptr != next && len > 0);
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
 *
 * @param c 输入的单个字符
 * @param state 当前状态
 * @param target 要搜索的字符串子串
 * @param next KMP特征码
 * @return 新的状态
 */
template <typename T>
int kmp_update(T c, int state, const T *target, const int *next)
{
    assert(state >= 0 && nullptr != target && nullptr != next);
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
 * KMP字符串搜索
 *
 * @param src 被搜索的字符串
 * @param start 开始搜索的位置
 * @param target 要搜索的字符串子串
 * @param next KMP特征码(长度与target相同)
 * @returns >=0 搜索到的子串位置
 *          -1 未搜索到子串
 */
template <typename T>
int kmp_search(const T *src, size_t len_src, size_t start, const T *target,
               const int *next, size_t len_target)
{
    assert(nullptr != src && nullptr != target && nullptr != next);
    size_t i = start;
    int state = 0; // 状态，其实代表着已经匹配的字符串长度
    while (i < len_src && ((size_t) state) < len_target)
        state = kmp_update(src[i++], state, target, next);
    if (state == (int) len_target)
        return (int) (i - len_target);
    return -1;
}

/**
 * KMP字符串搜索
 * 
 *      这里对于要搜索字符串的KMP特征码只做局部缓存，如果有特殊需求，例如被搜索的字符串不连续
 * 的存放在不同位置，则可使用其他函数组合使用(参见本函数的实现)
 *
 * @param src 被搜索的字符串
 * @param start 搜索开始的位置
 * @param target 要搜索的字符串子串
 * @returns >=0 搜索到的子串位置
 *          -1 未搜索到子串
 */
NUT_API int kmp_search(const std::string& src, size_t start, const std::string& target);
NUT_API int kmp_search(const std::wstring& src, size_t start, const std::wstring& target);

}

#endif

