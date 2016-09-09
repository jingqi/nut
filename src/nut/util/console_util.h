
#ifndef ___HEADFILE___77946953_2090_4060_9651_10D87F3FB5B6_
#define ___HEADFILE___77946953_2090_4060_9651_10D87F3FB5B6_

#include <string>

#include <nut/nut_config.h>

namespace nut
{

class NUT_API ConsoleUtil
{
public :
    enum ConsoleColor
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

    static void set_text_color(ConsoleColor forecolor = DEFAULT, ConsoleColor backcolor = DEFAULT);

    static void set_back_ground_color(ConsoleColor forecolor = DEFAULT, ConsoleColor backcolor = DEFAULT);

    static void pause();

    static std::string get_password();
};

}

#endif /* head file guarder */
