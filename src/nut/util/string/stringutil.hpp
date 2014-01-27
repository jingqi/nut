/**
 * @file -
 * @author jingqi
 * @date 2010-8-18
 * @last-edit 2013-12-11 17:34:22 jingqi
 */

#ifndef ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_
#define ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_

#include <assert.h>
#include <string.h>  /* for memset() */
#include <stdio.h>  /* for sprintf(), vsnprintf() and so on */
#include <stdarg.h> /* for va_start() */
#include <stdlib.h> /* for malloc() free() ltoa() wcstombs() and so on */
#include <string>
#include <vector>
#include <wchar.h>

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#endif

#include "tostring.hpp"


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

/**
 * 用整个字符串来分割字符串
 */
inline void str_split(const std::string &str, const std::string &sstr, std::vector<std::string> *out, bool ignoreEmpty = false)
{
    assert(sstr.length() > 0 && NULL != out);

    std::string::size_type begin = 0, end = str.find(sstr);
    while (std::string::npos != end)
    {
        if (!ignoreEmpty || begin != end)
            out->push_back(str.substr(begin, end - begin));
        begin = end + sstr.length();
        end = str.find(sstr, begin);
    }
    if (!ignoreEmpty || begin < str.length())
        out->push_back(str.substr(begin));
}

inline void str_split(const std::wstring &str, const std::wstring &sstr, std::vector<std::wstring> *out, bool ignoreEmpty = false)
{
    assert(sstr.length() > 0 && NULL != out);

    std::wstring::size_type begin = 0, end = str.find(sstr);
    while (std::wstring::npos != end)
    {
        if (!ignoreEmpty || begin != end)
            out->push_back(str.substr(begin, end - begin));
        begin = end + sstr.length();
        end = str.find(sstr, begin);
    }
    if (!ignoreEmpty || begin < str.length())
        out->push_back(str.substr(begin));
}

inline std::vector<std::string> str_split(const std::string &str, const std::string &sstr, bool ignoreEmpty = false)
{
    std::vector<std::string> ret;
    str_split(str, sstr, &ret, ignoreEmpty);
    return ret;
}

inline std::vector<std::wstring> str_split(const std::wstring &str, const std::wstring &sstr, bool ignoreEmpty = false)
{
    std::vector<std::wstring> ret;
    str_split(str, sstr, &ret, ignoreEmpty);
    return ret;
}

/**
 * @param sstr
 *      该字符串中的每一个字符都是分割字符
 */
inline void chr_split(const std::string &str, const std::string &sstr, std::vector<std::string> *out, bool ignoreEmpty = false)
{
    assert(sstr.length() > 0 && NULL != out);

    std::string::size_type begin = 0, end = str.find_first_of(sstr);
    while (std::string::npos != end)
    {
        if (!ignoreEmpty || begin != end)
            out->push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sstr, begin);
    }
    if (!ignoreEmpty || begin < str.length())
        out->push_back(str.substr(begin));
}

inline void chr_split(const std::wstring &str, const std::wstring &sstr, std::vector<std::wstring> *out, bool ignoreEmpty = false)
{
    assert(sstr.length() > 0 && NULL != out);

    std::wstring::size_type begin = 0, end = str.find_first_of(sstr);
    while (std::wstring::npos != end)
    {
        if (!ignoreEmpty || begin != end)
            out->push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sstr, begin);
    }
    if (!ignoreEmpty || begin < str.length())
        out->push_back(str.substr(begin));
}

inline std::vector<std::string> chr_split(const std::string &str, const std::string &sstr, bool ignoreEmpty = false)
{
    std::vector<std::string> ret;
    chr_split(str, sstr, &ret, ignoreEmpty);
    return ret;
}

inline std::vector<std::wstring> chr_split(const std::wstring &str, const std::wstring &sstr, bool ignoreEmpty = false)
{
    std::vector<std::wstring> ret;
    chr_split(str, sstr, &ret, ignoreEmpty);
    return ret;
}

/** split the string */
inline void chr_split(const std::string &str, char c, std::vector<std::string> *out, bool ignoreEmpty = false)
{
    assert(NULL != out);
    chr_split(str, std::string(1, c), out, ignoreEmpty);
}

inline void chr_split(const std::wstring &str, wchar_t c, std::vector<std::wstring> *out, bool ignoreEmpty = false)
{
    assert(NULL != out);
    chr_split(str, std::wstring(1, c), out, ignoreEmpty);
}

inline std::vector<std::string> chr_split(const std::string &str, char c, bool ignoreEmpty = false)
{
    std::vector<std::string> ret;
    chr_split(str, std::string(1, c), &ret, ignoreEmpty);
    return ret;
}

inline std::vector<std::wstring> chr_split(const std::wstring &str, wchar_t c, bool ignoreEmpty = false)
{
    std::vector<std::wstring> ret;
    chr_split(str, std::wstring(1, c), &ret, ignoreEmpty);
    return ret;
}

inline void format(std::string *out, const char *fmt, ...)
{
    assert(NULL != out && NULL != fmt);
    size_t size = 100;
    char *buf = (char*) ::malloc(size);
    assert(NULL != buf);

    va_list ap;
    while (NULL != buf)
    {
        va_start(ap, fmt);
        int n = ::vsnprintf(buf, size, fmt, ap);
        va_end(ap);
        if (n > -1 && n < (int)size)
            break;

        if (n > -1)
            size = n + 1; /* glibc 2.1 */
        else
            size *= 2;  /* glib 2.0 */

        char *np = (char*) ::realloc(buf, size);
        assert(NULL != np);
        if (NULL != np)
            buf = np;
    }
    if (NULL == buf)
    {
        out->clear();
    }
    else
    {
        *out = buf;
        ::free(buf); /* include the case of success of realloc() and failure of realloc() */
    }
}

inline std::string format(const char *fmt, ...)
{
    assert(NULL != fmt);
    size_t size = 100;
    char *buf = (char*) ::malloc(size);
    assert(NULL != buf);

    va_list ap;
    while (NULL != buf)
    {
        va_start(ap, fmt);
        int n = ::vsnprintf(buf, size, fmt, ap);
        va_end(ap);
        if (n > -1 && n < (int)size)
            break;

        if (n > -1)
            size = n + 1; /* glibc 2.1 */
        else
            size *= 2;  /* glib 2.0 */

        char *np = (char*) ::realloc(buf, size);
        assert(NULL != np);
        if (NULL != np)
            buf = np;
    }
    std::string ret = (NULL == buf ? "" : buf);
    if (NULL != buf)
        ::free(buf); /* include the case of success of realloc() and failure of realloc() */
    return ret;
}

/* 去除首尾空白 */
inline void trim(const std::string& str, std::string *out, const std::string& blanks = " \t\r\n")
{
    assert(NULL != out);
    const std::string::size_type begin = str.find_first_not_of(blanks),
        end = str.find_last_not_of(blanks);
    if (std::string::npos == begin || std::string::npos == end)
        out->clear();
    else
        *out = str.substr(begin, end - begin + 1);
}

inline void trim(const std::wstring& str, std::wstring *out, const std::wstring& blanks = L" \t\r\n")
{
    assert(NULL != out);
    const std::wstring::size_type begin = str.find_first_not_of(blanks),
        end = str.find_last_not_of(blanks);
    if (std::wstring::npos == begin || std::wstring::npos == end)
        out->clear();
    else
        *out = str.substr(begin, end - begin + 1);
}

inline std::string trim(const std::string& str, const std::string& blanks = " \t\r\n")
{
    std::string ret;
    trim(str, &ret, blanks);
    return ret;
}

inline std::wstring trim(const std::wstring& str, const std::wstring& blanks = L" \t\r\n")
{
    std::wstring ret;
    trim(str, &ret, blanks);
    return ret;
}

/** 去除左边空白 */
inline void ltrim(const std::string& str, std::string *out, const std::string& blanks = " \t\r\n")
{
    assert(NULL != out);
    const std::string::size_type begin = str.find_first_not_of(blanks);
    *out = str.substr(begin);
}

inline void ltrim(const std::wstring& str, std::wstring *out, const std::wstring& blanks = L" \t\r\n")
{
    assert(NULL != out);
    const std::wstring::size_type begin = str.find_first_not_of(blanks);
    *out = str.substr(begin);
}

inline std::string ltrim(const std::string& str, const std::string& blanks = " \t\r\n")
{
    std::string ret;
    ltrim(str, &ret, blanks);
    return ret;
}

inline std::wstring ltrim(const std::wstring& str, const std::wstring& blanks = L" \t\r\n")
{
    std::wstring ret;
    ltrim(str, &ret, blanks);
    return ret;
}

/** 去除右边空白 */
inline void rtrim(const std::string& str, std::string *out, const std::string& blanks = " \t\r\n")
{
    assert(NULL != out);
    const std::string::size_type end = str.find_last_not_of(blanks);
    if (std::string::npos == end)
        out->clear();
    else
        *out = str.substr(0, end + 1);
}

inline void rtrim(const std::wstring& str, std::wstring *out, const std::wstring& blanks = L" \t\r\n")
{
    assert(NULL != out);
    const std::wstring::size_type end = str.find_last_not_of(blanks);
    if (std::wstring::npos == end)
        out->clear();
    else
        *out = str.substr(0, end + 1);
}

inline std::string rtrim(const std::string& str, const std::string& blanks = " \t\r\n")
{
    std::string ret;
    rtrim(str, &ret, blanks);
    return ret;
}

inline std::wstring rtrim(const std::wstring& str, const std::wstring& blanks = L" \t\r\n")
{
    std::wstring ret;
    rtrim(str, &ret, blanks);
    return ret;
}

/** 忽略大小写的字符串比较 */
inline bool strieq(const std::string& str1, const std::string& str2)
{
    if (str1.length() != str2.length())
        return false;
    for (register size_t i = 0, len = str1.length(); i < len; ++i)
        if ((str1.at(i) | 0x20) != (str2.at(i) | 0x20))
            return false;
    return true;
}

inline bool strieq(const std::wstring& str1, const std::wstring& str2)
{
    if (str1.length() != str2.length())
        return false;
    for (register size_t i = 0, len = str1.length(); i < len; ++i)
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
    for (register size_t i = 0, len = head.length(); i < len; ++i)
        if (s.at(i) != head.at(i))
            return false;
    return true;
}

inline bool starts_with(const std::wstring& s, const std::wstring& head)
{
    if (s.length() < head.length())
        return false;
    for (register size_t i = 0, len = head.length(); i < len; ++i)
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
    for (register size_t i = 1, len = tail.length(); i <= len; ++i)
        if (s.at(s.length() - i) != tail.at(tail.length() - i))
            return false;
    return true;
}

inline bool ends_with(const std::wstring& s, const std::wstring& tail)
{
    if (s.length() < tail.length())
        return false;
    for (register size_t i = 1, len = tail.length(); i <= len; ++i)
        if (s.at(s.length() - i) != tail.at(tail.length() - i))
            return false;
    return true;
}

inline void wstr2str(const wchar_t* wstr, std::string* out)
{
    assert(NULL != wstr && NULL != out);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    const int n = ::WideCharToMultiByte(CP_ACP /* code page */, 0 /* flags */,
        wstr, -1 /* 字符串以\0结束 */, NULL, 0, NULL, NULL) + 1;
    char *p = new char[n];
    ::memset(p, 0, n * sizeof(char));
    ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, p, n, NULL, NULL);
    *out = p;
    delete[] p;
#else
    const int n = ::wcstombs(NULL, wstr, 0) + 1; // '\0' is added
    if (n <= 0)
    {
        out->clear();
        return;
    }
    char *p = new char[n];
    ::memset(p, 0, n * sizeof(char));
    ::wcstombs(p, wstr, n);
    *out = p;
    delete[] p;
#endif
}

inline std::string wstr2str(const wchar_t* wstr)
{
    std::string ret;
    wstr2str(wstr, &ret);
    return ret;
}

inline std::string wstr2str(const std::wstring& wstr)
{
    std::string ret;
    wstr2str(wstr.c_str(), &ret);
    return ret;
}

inline void str2wstr(const char* str, std::wstring* out)
{
    assert(NULL != str && NULL != out);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    const int n = ::MultiByteToWideChar(CP_ACP, 0, str, -1/* 字符串以\0结束 */, NULL, 0) + 1;
    wchar_t *p = new wchar_t[n];
    ::memset(p, 0, n * sizeof(wchar_t));
    ::MultiByteToWideChar(CP_ACP, 0, str, -1, p, n);
    *out = p;
    delete[] p;
#else
    const int n = ::mbstowcs(NULL, str, 0) + 1;
    if (n <= 0)
    {
        out->clear();
        return;
    }
    wchar_t *p = new wchar_t[n];
    ::memset(p, 0, n * sizeof(wchar_t));
    ::mbstowcs(p, str, n);
    *out = p;
    delete[] p;
#endif
}

inline std::wstring str2wstr(const char* str)
{
    std::wstring ret;
    str2wstr(str, &ret);
    return ret;
}

inline std::wstring str2wstr(const std::string& str)
{
    std::wstring ret;
    str2wstr(str.c_str(), &ret);
    return ret;
}

}


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

#endif /* head file guarder */

