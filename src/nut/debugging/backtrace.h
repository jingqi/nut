
#ifndef ___HEADFILE_B3E81AC9_35E7_4ADF_A586_39A6E77844E7_
#define ___HEADFILE_B3E81AC9_35E7_4ADF_A586_39A6E77844E7_

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_LINUX

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
public:
    /**
     * @return >=0， 调用栈层数
     *         <0， 出错
     */
    static int backtrace(std::string *appended);
};

}

#endif

#endif
