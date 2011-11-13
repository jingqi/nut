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
#include <stdlib.h> /* for malloc() and free()*/
#include <string>
#include <vector>

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
    char buf[30];
    memset(buf,0,30);
    sprintf(buf,"%ld",i);
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
    char buf[30];
    memset(buf,0,30);
    sprintf(buf,"%lf",d);
    return buf;
}

inline std::string toString(float f)
{
    char buf[30];
    memset(buf,0,30);
    sprintf(buf,"%f",f);
    return buf;
}

inline std::string toString(const void *p)
{
    char buf[30];
    memset(buf,0,30);

#if defined(WINDOWS)  // windows
    sprintf(buf,"0x%p",p);  // e.g. 0x002E459F
#else  // linux
//    sprintf(buf,"%p",p);    // e.g. 0x2e459f
    sprintf(buf,"0x%08X",p);  // e.g. 0x002E459F
#endif

    return buf;
}

inline std::string toString(void *p)
{
    return toString((const void*)p);
}

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
        return "";
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

/** split the string */
inline std::vector<std::string> split(const std::string &str, const std::string &sstr, bool ignoreEmpty = false)
{
    std::vector<std::string> ret;
    std::string::size_type begin = 0, end = str.find_first_of(sstr);
    while (end != std::string::npos)
    {
        if (!ignoreEmpty || begin != end)
            ret.push_back(str.substr(begin,end-begin));
        begin = end + sstr.length();
        end = str.find(sstr,begin);
    }
    if (!ignoreEmpty || begin < str.length())
        ret.push_back(str.substr(begin));
    return ret;
}

/** split the string */
inline std::vector<std::string> split(const std::string &str, char c, bool ignoreEmpty = false)
{
    return split(str, toString(c), ignoreEmpty);
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
        va_start(ap,fmt);
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

inline std::string trim(const std::string &str, const std::string &blanks)
{
    std::string::size_type begin = str.find_first_not_of(blanks),
        end = str.find_last_not_of(blanks);
    if (begin == std::string::npos || end == std::string::npos)
        return std::string();
    else
        return str.substr(begin, end - begin + 1);
}

}

#endif /* head file guarder */

