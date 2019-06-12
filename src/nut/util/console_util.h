
#ifndef ___HEADFILE___77946953_2090_4060_9651_10D87F3FB5B6_
#define ___HEADFILE___77946953_2090_4060_9651_10D87F3FB5B6_

#include <string>

#include "../nut_config.h"


namespace nut
{

enum class ConsoleColor
{
    Default     = 0,     /* 默认 */
    Black       = 1,     /* 黑 */
    Red         = 2,     /* 红 */
    Green       = 3,     /* 绿 */
    Blue        = 4,     /* 蓝 */
    Yellow      = 5,     /* 黄 = 红 + 绿 */
    Pink        = 6,     /* 紫 = 红 + 蓝 */
    Turquoise   = 7,     /* 青 = 蓝 + 绿 */
    White       = 8,     /* 白 = 红 + 绿 + 蓝*/
};

class NUT_API ConsoleUtil
{
public:
    // 判断是否运行在终端中
    static bool isatty() noexcept;

    // 设置终端文字前景色
    static void set_text_color(ConsoleColor forecolor = ConsoleColor::Default,
                               ConsoleColor backcolor = ConsoleColor::Default) noexcept;

    // 设置终端文字背景色
    static void set_back_ground_color(ConsoleColor forecolor = ConsoleColor::Default,
                                      ConsoleColor backcolor = ConsoleColor::Default) noexcept;

    // 暂停，等待用户按键
    static void pause() noexcept;

    // 从终端中读取密码
    static std::string get_password() noexcept;

private:
    ConsoleUtil() = delete;
};

}

#endif /* head file guarder */
