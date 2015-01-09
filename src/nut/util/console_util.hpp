﻿/**
 * @file -
 * @author jingqi
 * @date 2010-4-15
 * @last-edit 2015-01-06 23:31:35 jingqi
 */

#ifndef ___HEADFILE___77946953_2090_4060_9651_10D87F3FB5B6_
#define ___HEADFILE___77946953_2090_4060_9651_10D87F3FB5B6_

#include <nut/platform/platform.hpp>

#include <assert.h>
#include <stdio.h>
#include <string>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <conio.h>
#   include <windows.h>
#else
#   include <unistd.h>
#endif

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

class ConsoleUtil
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

    static void set_text_color(ConsoleColor forecolor = DEFAULT, ConsoleColor backcolor = DEFAULT)
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
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
        unsigned color = fgtable[forecolor] | bgtable[backcolor];
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
        ::printf("\33[%dm\33[%dm", fgtable[forecolor], bgtable[backcolor]);
#endif
    }

    static void set_back_ground_color(ConsoleColor forecolor = DEFAULT, ConsoleColor bkcolor = DEFAULT)
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        const char fgtable[9] =
        {
            '7', '0', '4', '2', '1', '6', 'D', 'B', 'F'
        };
        const char bgtable[9] =
        {
            '0', '0', '4', '2', '1', '6', 'D', 'B', 'F'
        };
        char buf[9] = {'c', 'o', 'l', 'o', 'r', ' ', 0, 0, 0};
        buf[6] = bgtable[bkcolor];
        buf[7] = fgtable[forecolor];
        ::system(buf);
#else
        /* not supported now */
        (void) forecolor;
        (void) bkcolor;
#endif
    }

    static void pause()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::getch();
#else
        ::fflush(stdin);
        ::getchar();  /* performing not well */
        ::fflush(stdin);
#endif
    }

    static std::string get_password ()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
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
};

}

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

#endif /* head file guarder */