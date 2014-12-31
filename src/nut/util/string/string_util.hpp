/**
 * @file -
 * @author jingqi
 * @date 2010-8-18
 * @last-edit 2014-07-25 20:49:29 jingqi
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

#include "to_string.hpp"


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
    char *buf = (char*) ::malloc(size * sizeof(char));
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

inline void format(std::wstring *out, const wchar_t *fmt, ...)
{
    assert(NULL != out && NULL != fmt);
    size_t size = 100;
    wchar_t *buf = (wchar_t*) ::malloc(size * sizeof(wchar_t));
    assert(NULL != buf);

    va_list ap;
    while (NULL != buf)
    {
        va_start(ap, fmt);
#if defined(NUT_PLATFORM_CC_VC)
        int n = ::_vsnwprintf(buf, size, fmt, ap);
#elif defined(NUT_PLATFORM_OS_MAC)
        int n = ::vswprintf(buf, size, fmt, ap);
#else
        int n = ::vsnwprintf(buf, size, fmt, ap);
#endif
        va_end(ap);
        if (n > -1 && n < (int)size)
            break;

        if (n > -1)
            size = n + 1; /* glibc 2.1 */
        else
            size *= 2;  /* glib 2.0 */

        wchar_t *np = (wchar_t*) ::realloc(buf, size);
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
    char *buf = (char*) ::malloc(size * sizeof(char));
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

inline std::wstring format(const wchar_t *fmt, ...)
{
    assert(NULL != fmt);
    size_t size = 100;
    wchar_t *buf = (wchar_t*) ::malloc(size * sizeof(wchar_t));
    assert(NULL != buf);

    va_list ap;
    while (NULL != buf)
    {
        va_start(ap, fmt);
#if defined(NUT_PLATFORM_CC_VC)
        int n = ::_vsnwprintf(buf, size, fmt, ap);
#elif defined(NUT_PLATFORM_OS_MAC)
        int n = ::vswprintf(buf, size, fmt, ap);
#else
        int n = ::vsnwprintf(buf, size, fmt, ap);
#endif
        va_end(ap);
        if (n > -1 && n < (int)size)
            break;

        if (n > -1)
            size = n + 1; /* glibc 2.1 */
        else
            size *= 2;  /* glib 2.0 */

        wchar_t *np = (wchar_t*) ::realloc(buf, size);
        assert(NULL != np);
        if (NULL != np)
            buf = np;
    }
    std::wstring ret = (NULL == buf ? L"" : buf);
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
    for (size_t i = 0, len = str1.length(); i < len; ++i)
        if ((str1.at(i) | 0x20) != (str2.at(i) | 0x20))
            return false;
    return true;
}

inline bool strieq(const std::wstring& str1, const std::wstring& str2)
{
    if (str1.length() != str2.length())
        return false;
    for (size_t i = 0, len = str1.length(); i < len; ++i)
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
    for (size_t i = 0, len = head.length(); i < len; ++i)
        if (s.at(i) != head.at(i))
            return false;
    return true;
}

inline bool starts_with(const std::wstring& s, const std::wstring& head)
{
    if (s.length() < head.length())
        return false;
    for (size_t i = 0, len = head.length(); i < len; ++i)
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
    for (size_t i = 1, len = tail.length(); i <= len; ++i)
        if (s.at(s.length() - i) != tail.at(tail.length() - i))
            return false;
    return true;
}

inline bool ends_with(const std::wstring& s, const std::wstring& tail)
{
    if (s.length() < tail.length())
        return false;
    for (size_t i = 1, len = tail.length(); i <= len; ++i)
        if (s.at(s.length() - i) != tail.at(tail.length() - i))
            return false;
    return true;
}

inline bool ascii_to_wstr(const char *str, std::wstring *out)
{
	assert(NULL != str && NULL != out);
	out->clear();
	
#if defined(NUT_PLATFORM_OS_WINDOWS)
	const int n = ::MultiByteToWideChar(CP_ACP, 0 /* flags */, str, -1 /* 字符串以'\0'结束 */, NULL, 0); // 返回值包含了 '\0'
	if (n <= 0)
		return false;
	out->resize(n - 1);
	const int rs = ::MultiByteToWideChar(CP_ACP, 0, str, -1, &(*out)[0], n - 1);
	assert(out->length() == n - 1);
	return rs > 0;
#else
	const int n = (int) ::mbstowcs(NULL, str, 0); // 返回值未包含 '\0'
	if (n <= 0)
		return 0 == n;
	out->resize(n);
	const int rs = ::mbstowcs(&(*out)[0], str, n); // 未包含 '\0'
	assert(out->length() == n);
	return rs > 0;
#endif
}

inline bool ascii_to_wstr(const std::string& str, std::wstring *out)
{
	assert(NULL != out);
	return ascii_to_wstr(str.c_str(), out);
}

inline std::wstring ascii_to_wstr(const char *str)
{
	assert(NULL != str);
	std::wstring ret;
	ascii_to_wstr(str, &ret);
	return ret;
}

inline std::wstring ascii_to_wstr(const std::string& str)
{
	return ascii_to_wstr(str.c_str());
}

inline bool wstr_to_ascii(const wchar_t *wstr, std::string *out)
{
	assert(NULL != wstr && NULL != out);
	out->clear();
	
#if defined(NUT_PLATFORM_OS_WINDOWS)
	const int n = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (n <= 0)
		return false;
	out->resize(n - 1);
	const int rs = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, &(*out)[0], n - 1, NULL, NULL);
	assert(out->length() == n - 1);
	return rs > 0;
#else
	const int n = (int) ::wcstombs(NULL, wstr, 0);
	if (n <= 0)
		return 0 == n;
	out->resize(n);
	const int rs = ::wcstombs(&(*out)[0], wstr, n);
	assert(out->length() == n);
	return rs > 0;
#endif
}

inline bool wstr_to_ascii(const std::wstring& wstr, std::string *out)
{
	assert(NULL != out);
	return wstr_to_ascii(wstr.c_str(), out);
}

inline std::string wstr_to_ascii(const wchar_t *wstr)
{
	assert(NULL != wstr);
	std::string ret;
	wstr_to_ascii(wstr, &ret);
	return ret;
}

inline std::string wstr_to_ascii(const std::wstring& wstr)
{
	return wstr_to_ascii(wstr.c_str());
}

inline bool utf8_to_wstr(const char *str, std::wstring *out)
{
	assert(NULL != str && NULL != out);

#if defined(NUT_PLATFORM_OS_WINDOWS)
	out->clear();
	const int n = ::MultiByteToWideChar(CP_UTF8, 0 /* flags */, str, -1 /* 字符串以'\0'结束 */, NULL, 0);
	if (n <= 0)
		return false;
	out->resize(n - 1);
	const int rs = ::MultiByteToWideChar(CP_UTF8, 0, str, -1, &(*out)[0], n - 1);
	assert(out->length() == n - 1);
	return rs > 0;
#else
	return ascii_to_wstr(str, out);
#endif
}

inline bool utf8_to_wstr(const std::string& str, std::wstring *out)
{
	assert(NULL != out);
	return utf8_to_wstr(str.c_str(), out);
}

inline std::wstring utf8_to_wstr(const char *str)
{
	assert(NULL != str);
	std::wstring ret;
	utf8_to_wstr(str, &ret);
	return ret;
}

inline std::wstring utf8_to_wstr(const std::string& str)
{
	return utf8_to_wstr(str.c_str());
}

inline bool wstr_to_utf8(const wchar_t *wstr, std::string *out)
{
	assert(NULL != wstr && NULL != out);

#if defined(NUT_PLATFORM_OS_WINDOWS)
	out->clear();
	const int n = ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (n <= 0)
		return false;
	out->resize(n - 1);
	const int rs = ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &(*out)[0], n - 1, NULL, NULL);
	assert(out->length() == n - 1);
	return rs > 0;
#else
	return wstr_to_utf8(wstr, out);
#endif
}

inline bool wstr_to_utf8(const std::wstring& wstr, std::string *out)
{
	assert(NULL != out);
	return wstr_to_utf8(wstr.c_str(), out);
}

inline std::string wstr_to_utf8(const wchar_t *wstr)
{
	assert(NULL != wstr);
	std::string ret;
	wstr_to_utf8(wstr, &ret);
	return ret;
}

inline std::string wstr_to_utf8(const std::wstring& wstr)
{
	return wstr_to_utf8(wstr.c_str());
}

}


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

#endif /* head file guarder */
