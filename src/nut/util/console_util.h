
#ifndef ___HEADFILE___77946953_2090_4060_9651_10D87F3FB5B6_
#define ___HEADFILE___77946953_2090_4060_9651_10D87F3FB5B6_

#include <string>

#include "../nut_config.h"


namespace nut
{

enum class ConsoleColor
{
    DEFAULT     = 0,     /* 默认 */
    BLACK       = 1,     /* 黑 */
    RED         = 2,     /* 红 */
    GREEN       = 3,     /* 绿 */
    BLUE        = 4,     /* 蓝 */
    YELLOW      = 5,     /* 黄 = 红 + 绿 */
    PINK        = 6,     /* 紫 = 红 + 蓝 */
    TURQUOISE   = 7,     /* 青 = 蓝 + 绿 */
    WHITE       = 8,     /* 白 = 红 + 绿 + 蓝*/
};

class NUT_API ConsoleUtil
{
private:
    ConsoleUtil() = delete;
    
public:
    // 判断是否运行在终端中
    static bool isatty();

    // 设置终端文字前景色
    static void set_text_color(ConsoleColor forecolor = ConsoleColor::DEFAULT,
                               ConsoleColor backcolor = ConsoleColor::DEFAULT);

    // 设置终端文字背景色
    static void set_back_ground_color(ConsoleColor forecolor = ConsoleColor::DEFAULT,
                                      ConsoleColor backcolor = ConsoleColor::DEFAULT);

    // 暂停，等待用户按键
    static void pause();

    // 从终端中读取密码
    static std::string get_password();
};

}

#endif /* head file guarder */
