/**
 * @file -
 * @author jingqi
 * @date 
 * @last-edit 2012-08-04 13:27:56 jingqi
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

template <typename T>
inline std::string toString(const T &v)
{
    return v.toString();
}

inline std::string toString(char c)
{
    return std::string(1,c);
}

inline std::string toString(long i)
{
    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    ::ltoa(i, buf, 10);
#else
    ::sprintf(buf, "%ld", i);
#endif
    return buf;
}

inline std::string toString(unsigned char i) { return toString((long)i); }
inline std::string toString(short i) { return toString((long)i); }
inline std::string toString(unsigned short i) { return toString((long)i); }
inline std::string toString(int i) { return toString((long)i); }
inline std::string toString(unsigned int i) { return toString((long)i); }
inline std::string toString(unsigned long i) { return toString((long)i); }

inline std::string toString(bool b)
{
    return (b ? "true" : "false");
}

inline std::string toString(double d)
{
    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf, "%lf", d);
    return buf;
}

inline std::string toString(float f)
{
    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf, "%f", f);
    return buf;
}

inline std::string toString(const void *p)
{
    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf,"0x%p",p);    // windows: 0x002E459F, linux: 0x2e459f

    return buf;
}

inline std::string toString(void *p)
{
    return toString((const void*)p);
}

/** ´òÓ¡ÄÚ´æ¿é */
inline std::string toString(const void *p, size_t align, size_t count)
{
    assert(NULL != p && 0 != align && 0 != count);
    std::string ret;
    const int bufsize = 30;
    char buf[bufsize];
    unsigned char *current = (unsigned char*)p;

    for (size_t i = 0; i < count; ++i)
    {
        if (i % 0x08 == 0)
            ret += toString((const void*)current);

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

inline std::string toString(void *p, size_t align, size_t count)
{
    return toString((const void*)p, align, count);
}

inline std::string toString(const char *str)
{
    assert(NULL != str);
    if (NULL == str)
        return "(null str)";
    return str;
}

inline std::string toString(const char *str, size_t size, const char *fillNul = "\\0")
{
    assert(NULL != str && NULL != fillNul);
    if (NULL == str)
        return "";

    std::string ret;
    for (size_t i = 0; i < size; ++i)
    {
        if (str[i] != '\0')
            ret += str[i];
        else
            ret += fillNul;
    }
    return ret;
}

inline std::string toString(const std::string &s)
{
    return s;
}

template <typename T>
inline std::string toString(const std::vector<T> &v, const std::string &split = ", ")
{
    std::string ret("[");
    if (v.size() >= 1)
        ret += toString(v[0]);
    for (size_t i = 1; i < v.size(); ++i)
    {
        ret += split;
        ret += toString(v[i]);
    }
    ret += "]";
    return ret;
}

}


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

#endif

