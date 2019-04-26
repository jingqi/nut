﻿
#include <assert.h>

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <malloc.h> // for ::alloca()
#else
#   include <alloca.h>
#endif

#include "kmp.h"


namespace nut
{

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
NUT_API int kmp_search(const std::string& src, size_t start, const std::string& target)
{
    int *next = (int*) ::alloca(sizeof(int) * target.length());
    assert(nullptr != next);
    kmp_build_next(target.c_str(), next, target.length());
    const int ret = kmp_search(src.c_str(), src.length(), start, target.c_str(),
                               next, target.length());
    return ret;
}

NUT_API int kmp_search(const std::wstring& src, size_t start, const std::wstring& target)
{
    int *next = (int*) ::alloca(sizeof(int) * target.length());
    assert(nullptr != next);
    kmp_build_next(target.c_str(), next, target.length());
    const int ret = kmp_search(src.c_str(), src.length(), start, target.c_str(),
                               next, target.length());
    return ret;
}

}
