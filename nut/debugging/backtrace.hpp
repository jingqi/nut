/**
 * @file -
 * @author jingqi
 * @date 2013-08-30
 * @last-edit 2013-08-30 22:29:53 jingqi
 * @brief
 */

#ifndef ___HEADFILE_B3E81AC9_35E7_4ADF_A586_39A6E77844E7_
#define ___HEADFILE_B3E81AC9_35E7_4ADF_A586_39A6E77844E7_

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_LINUX)

#include <execinfo.h>
#include <stdlib.h>
#include <string>

namespace nut
{

/**
 * 打印调用栈
 *
 * NOTE:
 *    使用 backtrace_symbols() 函数要求编译时给 g++ 一个 -rdynamic 编译参数
 */
class Backtrace
{
    // 最大回溯层数
    enum { MAX_BACKTRACE = 100 };

public:
    static std::string backtrace()
    {
        void *buffer[MAX_BACKTRACE];
        int nptrs = ::backtrace(buffer, MAX_BACKTRACE);
        char **strs = ::backtrace_symbols(buffer, nptrs);
        std::string ret;
        if (NULL == strs)
            return ret; // error
        for (int i = 0; i < nptrs; ++i)
        {
            ret += strs[i];
            ret += "\n";
        }
        ::free(strs);
        return ret;
    }
};

}

#endif

#endif
