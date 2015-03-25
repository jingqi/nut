/**
 * @file -
 * @author jingqi
 * @date
 * @last-edit 2015-01-06 23:47:28 jingqi
 * @brief
 *        扩展 std::to_string()
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

/// long long
inline void ll_to_str(long long i, std::string *out)
{
    assert(NULL != out);

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

    *out += buf;
}

inline std::string ll_to_str(long long i)
{
    std::string ret;
    ll_to_str(i, &ret);
    return ret;
}

/// char
inline void c_to_str(char i, std::string *out)
{
    ll_to_str((long long) i, out);
}

inline std::string c_to_str(char i)
{
    return ll_to_str((long long) i);
}

/// unsigned char
inline void uc_to_str(unsigned char i, std::string *out)
{
    ll_to_str((long long) i, out);
}

inline std::string uc_to_str(unsigned char i)
{
    return ll_to_str((long long) i);
}

/// short
inline void s_to_str(short i, std::string *out)
{
    ll_to_str((long long) i, out);
}

inline std::string s_to_str(short i)
{
    return ll_to_str((long long) i);
}

/// unsigned short
inline void us_to_str(unsigned short i, std::string *out)
{
    ll_to_str((long long) i, out);
}

inline std::string us_to_str(unsigned short i)
{
    return ll_to_str((long long) i);
}

/// int
inline void i_to_str(int i, std::string *out)
{
    ll_to_str((long long) i, out);
}

inline std::string i_to_str(int i)
{
    return ll_to_str((long long) i);
}

/// unsigned int
inline void ui_to_str(unsigned int i, std::string *out)
{
    ll_to_str((long long) i, out);
}

inline std::string ui_to_str(unsigned int i)
{
    return ll_to_str((long long) i);
}

/// long
inline void l_to_str(long i, std::string *out)
{
    ll_to_str((long long) i, out);
}

inline std::string l_to_str(long i)
{
    return ll_to_str((long long) i);
}

/// unsigned long
inline void ul_to_str(unsigned long i, std::string *out)
{
    ll_to_str((long long) i, out);
}

inline std::string ul_to_str(unsigned long i)
{
    return ll_to_str((long long) i);
}

/// unsigned long long
inline void ull_to_str(unsigned long long i, std::string *out)
{
    ll_to_str((long long) i, out);
}

inline std::string ull_to_str(unsigned long long i)
{
    return ll_to_str((long long) i);
}

/// bool
inline void b_to_str(bool b, std::string *out)
{
    assert(NULL != out);
    *out += (b ? "true" : "false");
}

inline std::string b_to_str(bool b)
{
    return (b ? "true" : "false");
}

/// double
inline void d_to_str(double d, std::string *out)
{
    assert(NULL != out);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf, "%lf", d);
    *out += buf;
}

inline std::string d_to_str(double d)
{
    std::string ret;
    d_to_str(d, &ret);
    return ret;
}

/// float
inline void f_to_str(float f, std::string *out)
{
    assert(NULL != out);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf, "%f", f);
    *out += buf;
}

inline std::string f_to_str(float f)
{
    std::string ret;
    f_to_str(f, &ret);
    return ret;
}

/// pointer
inline void ptr_to_str(const void *p, std::string *out)
{
    assert(NULL != out);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    ::sprintf(buf,"0x%p",p);    // windows: 0x002E459F
#else
    ::sprintf(buf,"%p",p);      // linux: 0x2e459f
#endif

    *out += buf;
}

inline std::string ptr_to_str(const void *p)
{
    std::string ret;
    ptr_to_str(p, &ret);
    return ret;
}

/// memory block
inline void mem_to_str(const void *p, size_t align, size_t count, std::string *out)
{
    assert(NULL != p && align > 0 && count > 0 && NULL != out);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    unsigned char *current = (unsigned char*)p;

    for (size_t i = 0; i < count; ++i)
    {
        if (i % 0x08 == 0)
            ptr_to_str((const void*) current, out);

        std::string single;
        for (size_t j = 0; j < align; ++j)
        {
            ::memset(buf, 0, BUF_SIZE);
            sprintf(buf, "%02X", *current);
            single = std::string(buf) + single;
            ++current;
        }
        *out += " ";
        *out += single;

        if ((i % 0x08 == 0x07) && (i != count - 1))
            *out += "\n";
    }
}

inline std::string mem_to_str(const void *p, size_t align, size_t count)
{
    std::string ret;
    mem_to_str(p, align, count, &ret);
    return ret;
}

/// n-size string
inline void ns_to_str(const char *str, size_t len, std::string *out, const char *fill_nil = "\\0")
{
    assert(NULL != str && NULL != out && NULL != fill_nil);

    for (size_t i = 0; i < len; ++i)
    {
        if (str[i] != '\0')
            out->push_back(str[i]);
        else
            *out += fill_nil;
    }
}

inline std::string ns_to_str(const char *str, size_t len, const char *fill_nil = "\\0")
{
    std::string ret;
    ns_to_str(str, len, &ret, fill_nil);
    return ret;
}

/// string to long
inline long str_to_l(const std::string& s)
{
    long ret = 0;
    ::sscanf(s.c_str(), "%ld", &ret);
    return ret;
}

}


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

#endif
