
#ifndef ___HEADFILE_B3E81AC9_35E7_4ADF_A586_39A6E77844E7_
#define ___HEADFILE_B3E81AC9_35E7_4ADF_A586_39A6E77844E7_

#include <string>

#include "../nut_config.h"


namespace nut
{

/**
 * 打印调用栈
 *
 * NOTE:
 *    使用 backtrace_symbols() 函数要求编译时给 g++ 一个 -rdynamic 编译参数
 */
class NUT_API Backtrace
{
public:
    /**
     * 获取调用栈
     */
    static std::string backtrace(unsigned skip_top_frames = 0) noexcept;

    /**
     * 快捷打印调用栈
     */
    static void print_stack() noexcept;

private:
    Backtrace() = delete;
};

}

#endif
