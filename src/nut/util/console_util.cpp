
#include <assert.h>
#include <stdio.h>

#include "../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <conio.h> // for ::getch()
#   include <io.h> // for ::_isatty()
#   include <windows.h>
#else
#   include <unistd.h>
#endif

#include "console_util.h"


#if NUT_PLATFORM_CC_VC
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

using color_rep_type = std::underlying_type<ConsoleColor>::type;

bool ConsoleUtil::isatty()
{
#if NUT_PLATFORM_OS_WINDOWS
    return 0 != ::_isatty(::_fileno(stdout));
#else
    return 1 == ::isatty(STDOUT_FILENO);
#endif
}

void ConsoleUtil::set_text_color(ConsoleColor forecolor, ConsoleColor backcolor)
{
#if NUT_PLATFORM_OS_WINDOWS
    const unsigned fgtable[9] =
    {
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,                         /* 默认 */
        0,                                                                           /* 黑 */
        FOREGROUND_RED | FOREGROUND_INTENSITY,                                       /* 红 */
        FOREGROUND_GREEN | FOREGROUND_INTENSITY,                                     /* 绿 */
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,                                      /* 蓝 */
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,                    /* 黄 = 红 + 绿 */
        FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,                     /* 紫 = 红 + 蓝 */
        FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,                   /* 青 = 蓝 + 绿 */
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,  /* 白 = 红 + 绿 + 蓝*/
    };
    const unsigned bgtable[9] =
    {
        0,                                                    /* 默认 */
        0,                                                    /* 黑 */
        BACKGROUND_RED,                                       /* 红 */
        BACKGROUND_GREEN,                                     /* 绿 */
        BACKGROUND_BLUE,                                      /* 蓝 */
        BACKGROUND_RED | BACKGROUND_GREEN,                    /* 黄 = 红 + 绿 */
        BACKGROUND_RED | BACKGROUND_BLUE,                     /* 紫 = 红 + 蓝 */
        BACKGROUND_BLUE | BACKGROUND_GREEN,                   /* 青 = 蓝 + 绿 */
        BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,  /* 白 = 红 + 绿 + 蓝*/
    };
    unsigned color = fgtable[static_cast<color_rep_type>(forecolor)] |
        bgtable[static_cast<color_rep_type>(backcolor)];
    ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), color);
#else
    const unsigned fgtable[9] =
    {
        39, /* 默认 */
        30, /* 黑 */
        31, /* 红 */
        32, /* 绿 */
        34, /* 蓝 */
        33, /* 黄 = 红 + 绿 */
        35, /* 紫 = 红 + 蓝 */
        36, /* 青 = 蓝 + 绿 */
        37  /* 白 = 红 + 绿 + 蓝*/
    };
    const unsigned bgtable[9] =
    {
        49,  /* 默认 */
        40,  /* 黑 */
        41,  /* 红 */
        42,  /* 绿 */
        44,  /* 蓝 */
        43,  /* 黄 = 红 + 绿 */
        45,  /* 紫 = 红 + 蓝 */
        46,  /* 青 = 蓝 + 绿 */
        47   /* 白 = 红 + 绿 + 蓝*/
    };
    ::printf("\33[%dm\33[%dm", fgtable[static_cast<color_rep_type>(forecolor)],
             bgtable[static_cast<color_rep_type>(backcolor)]);
#endif
}

void ConsoleUtil::set_back_ground_color(ConsoleColor forecolor, ConsoleColor backcolor)
{
#if NUT_PLATFORM_OS_WINDOWS
    const char fgtable[9] =
    {
        '7', '0', '4', '2', '1', '6', 'D', 'B', 'F'
    };
    const char bgtable[9] =
    {
        '0', '0', '4', '2', '1', '6', 'D', 'B', 'F'
    };
    char buf[9] = {'c', 'o', 'l', 'o', 'r', ' ', 0, 0, 0};
    buf[6] = bgtable[static_cast<color_rep_type>(backcolor)];
    buf[7] = fgtable[static_cast<color_rep_type>(forecolor)];
    ::system(buf);
#else
    UNUSED(forecolor);
    UNUSED(backcolor);
    /* not supported now */
#endif
}

void ConsoleUtil::pause()
{
#if NUT_PLATFORM_OS_WINDOWS
    ::getch();
#else
    ::fflush(stdin);
    ::getchar();  /* performing not well */
    ::fflush(stdin);
#endif
}

std::string ConsoleUtil::get_password()
{
#if NUT_PLATFORM_OS_WINDOWS
    std::string ret;
    int c = 0;
    do
    {
        c = ::getch();
        switch(c)
        {
        case '\r':
        case '\n':
            break;

        case '\b':      /* back space, ASCII is 0x08 */
            if (ret.size() > 0)
            {
                ret.erase(ret.end() - 1);
                ::printf("\b \b");
            }
            break;

        default:
            ret.push_back((char) c);
            ::printf("*");
        }
    } while ('\r' != c && '\n' != c);
    return ret;
#else
    return ::getpass("");
#endif
}

}

#if NUT_PLATFORM_CC_VC
#   pragma warning(pop)
#endif
