/**
 * @file -
 * @author jingqi
 * @date 2010-8-18
 */

#ifndef ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_
#define ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_

#include <assert.h>
#include <string.h>  /* for memset() */
#include <stdio.h>  /* for sprintf(), vsnprintf() and so on */
#include <stdarg.h> /* for va_start() */
#include <stdlib.h> /* for malloc() free() ltoa() */
#include <string>
#include <vector>

#include <nut/platform/platform.hpp>

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

/** 打印内存块 */
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

/**
 * 用整个字符串来分割字符串
 */
inline std::vector<std::string> str_split(const std::string &str, const std::string &sstr, bool ignoreEmpty = false)
{
    assert(sstr.length() > 0);

    std::vector<std::string> ret;
    std::string::size_type begin = 0, end = str.find(sstr);
    while (std::string::npos != end)
    {
        if (!ignoreEmpty || begin != end)
            ret.push_back(str.substr(begin, end - begin));
        begin = end + sstr.length();
        end = str.find(sstr, begin);
    }
    if (!ignoreEmpty || begin < str.length())
        ret.push_back(str.substr(begin));
    return ret;
}

/**
 * @param sstr
 *      该字符串中的每一个字符都是分割字符
 */
inline std::vector<std::string> chr_split(const std::string &str, const std::string &sstr, bool ignoreEmpty = false)
{
    assert(sstr.length() > 0);

    std::vector<std::string> ret;
    std::string::size_type begin = 0, end = str.find_first_of(sstr);
    while (std::string::npos != end)
    {
        if (!ignoreEmpty || begin != end)
            ret.push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sstr, begin);
    }
    if (!ignoreEmpty || begin < str.length())
        ret.push_back(str.substr(begin));
    return ret;
}

/** split the string */
inline std::vector<std::string> chr_split(const std::string &str, char c, bool ignoreEmpty = false)
{
    return chr_split(str, toString(c), ignoreEmpty);
}

inline std::string format(const char *fmt, ...)
{
    assert(NULL != fmt);
    size_t size = 100;
    char *buf = (char*)malloc(size);
    assert(NULL != buf);

    va_list ap;
    while (NULL != buf)
    {
        va_start(ap, fmt);
        int n = vsnprintf(buf, size, fmt, ap);
        va_end(ap);
        if (n > -1 && n < (int)size)
            break;

        if (n > -1)
            size = n + 1; /* glibc 2.1 */
        else
            size *= 2;  /* glib 2.0 */

        char *np = (char*)realloc(buf, size);
        assert(NULL != np);
        if (NULL != np)
            buf = np;
    }
    std::string ret = (NULL == buf ? "" : buf);
    if (NULL != buf)
        free(buf); /* include the case of success of realloc() and failure of realloc() */
    return ret;
}

/* 去除首尾空白 */
inline std::string trim(const std::string& str, const std::string& blanks = " \t\r\n")
{
    const std::string::size_type begin = str.find_first_not_of(blanks),
        end = str.find_last_not_of(blanks);
    if (std::string::npos == begin || std::string::npos == end)
        return std::string();
    else
        return str.substr(begin, end - begin + 1);
}

/** 去除左边空白 */
inline std::string ltrim(const std::string& str, const std::string& blanks = " \t\r\n")
{
    const std::string::size_type begin = str.find_first_not_of(blanks);
    return str.substr(begin);
}

/** 去除右边空白 */
inline std::string rtrim(const std::string& str, const std::string& blanks = " \t\r\n")
{
    const std::string::size_type end = str.find_last_not_of(blanks);
    if (std::string::npos == end)
        return std::string();
    return str.substr(0, end + 1);
}

/** 忽略大小写的字符串比较 */
inline bool strieq(const std::string& str1, const std::string& str2)
{
    if (str1.length() != str2.length())
        return false;
    const int len = str1.length();
    for (register int i = 0; i < len; ++i)
        if ((str1.at(i) | 0x20) != (str2.at(i) | 0x20))
            return false;
    return true;
}

/**
 * 匹配字符串的开头
 */
inline bool starts_with(const std::string& s, const std::string& head)
{
    if (s.length() < head.length())
        return false;
    for (register int i = 0, len = head.length(); i < len; ++i)
        if (s.at(i) != head.at(i))
            return false;
    return true;
}

/**
 * 匹配字符串的结尾
 */
inline bool ends_with(const std::string& s, const std::string& tail)
{
    if (s.length() < tail.length())
        return false;
    for (register int i = 1, len = tail.length(); i <= len; ++i)
        if (s.at(s.length() - i) != tail.at(tail.length() - i))
            return false;
    return true;
}

}

#endif /* head file guarder */

