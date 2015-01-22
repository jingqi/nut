/**
 * @file -
 * @author jingqi
 * @date
 * @last-edit 2015-01-06 23:47:28 jingqi
 * @brief
 *		扩展 std::to_string()
 */

#ifndef ___HEADFILE_791BEBBF_7FE0_40BA_AC96_703F0C8F38C1_
#define ___HEADFILE_791BEBBF_7FE0_40BA_AC96_703F0C8F38C1_

#include <assert.h>
#include <string.h>  /* for memset() */
#include <stdio.h>  /* for sprintf(), vsnprintf() and so on */
#include <stdarg.h> /* for va_start() */
#include <stdlib.h> /* for malloc() free() ltoa() */
#include <string>
#include <vector>

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

inline std::string c_to_str(char c)
{
    return std::string(1,c);
}

inline std::string ll_to_str(long long i)
{
    const int BUF_SIZE = 60;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
#if defined(_MSC_VER) && _MSC_VER <= 1310
	// for VS2003 or ealier, "%lld" was not supported
    ::sprintf(buf, "%I64d", i);
    // ::ltoa(i, buf, 10);
#else
    ::sprintf(buf, "%lld", i);
#endif
    return buf;
}

inline std::string uc_to_str(unsigned char i) { return ll_to_str((long long)i); }
inline std::string s_to_str(short i) { return ll_to_str((long long)i); }
inline std::string us_to_str(unsigned short i) { return ll_to_str((long long)i); }
inline std::string i_to_str(int i) { return ll_to_str((long long)i); }
inline std::string ui_to_str(unsigned int i) { return ll_to_str((long long) i); }
inline std::string l_to_str(long i) { return ll_to_str((long long) i); }
inline std::string ul_to_str(unsigned long i) { return ll_to_str((long long) i); }
inline std::string ull_to_str(unsigned long long i) { return ll_to_str((long long) i); }

inline std::string b_to_str(bool b)
{
    return (b ? "true" : "false");
}

inline std::string d_to_str(double d)
{
    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf, "%lf", d);
    return buf;
}

inline std::string f_to_str(float f)
{
    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf, "%f", f);
    return buf;
}

inline std::string ptr_to_str(const void *p)
{
    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    ::sprintf(buf,"0x%p",p);    // windows: 0x002E459F
#else
    ::sprintf(buf,"%p",p);      // linux: 0x2e459f
#endif

    return buf;
}

/** 打印内存块 */
inline std::string ptr_to_str(const void *p, size_t align, size_t count)
{
    assert(NULL != p && 0 != align && 0 != count);
    std::string ret;
    const int bufsize = 30;
    char buf[bufsize];
    unsigned char *current = (unsigned char*)p;

    for (size_t i = 0; i < count; ++i)
    {
        if (i % 0x08 == 0)
            ret += ptr_to_str((const void*)current);

        std::string single;
        for (size_t j = 0; j < align; ++j)
        {
            memset(buf,0,bufsize);
            sprintf(buf, "%02X", *current);
            single = std::string(buf) +single;
            ++current;
        }
        ret += std::string(" ") + single;

        if ((i % 0x08 == 0x07) && (i != count - 1))
            ret += "\n";
    }
    return ret;
}

inline std::string s_to_str(const char *str)
{
    assert(NULL != str);
    if (NULL == str)
        return "(null str)";
    return str;
}

inline std::string s_to_str(const char *str, size_t size, const char *fill_nul = "\\0")
{
    assert(NULL != str && NULL != fill_nul);
    if (NULL == str)
        return "";

    std::string ret;
    for (size_t i = 0; i < size; ++i)
    {
        if (str[i] != '\0')
            ret += str[i];
        else
            ret += fill_nul;
    }
    return ret;
}

inline long str_to_l(const std::string& s)
{
    long ret = 0;
    sscanf(s.c_str(), "%ld", &ret);
    return ret;
}

}


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

#endif
