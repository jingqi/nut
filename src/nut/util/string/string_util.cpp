
#include <assert.h>
#include <string.h>  /* for memset() */
#include <stdio.h>  /* for sprintf(), vsnprintf() and so on */
#include <stdarg.h> /* for va_start() */
#include <stdlib.h> /* for malloc() free() ltoa() wcstombs() and so on */
#include <wchar.h>

#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#endif

#include "to_string.h"

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

/**
 * 用整个字符串来分割字符串
 */
void str_split(const std::string &str, const std::string &sstr, std::vector<std::string> *appended, bool ignore_empty = false)
{
    assert(sstr.length() > 0 && NULL != appended);

    std::string::size_type begin = 0, end = str.find(sstr);
    while (std::string::npos != end)
    {
        if (!ignore_empty || begin != end)
            appended->push_back(str.substr(begin, end - begin));
        begin = end + sstr.length();
        end = str.find(sstr, begin);
    }
    if (!ignore_empty || begin < str.length())
        appended->push_back(str.substr(begin));
}

void str_split(const std::wstring &str, const std::wstring &sstr, std::vector<std::wstring> *appended, bool ignore_empty = false)
{
    assert(sstr.length() > 0 && NULL != appended);

    std::wstring::size_type begin = 0, end = str.find(sstr);
    while (std::wstring::npos != end)
    {
        if (!ignore_empty || begin != end)
            appended->push_back(str.substr(begin, end - begin));
        begin = end + sstr.length();
        end = str.find(sstr, begin);
    }
    if (!ignore_empty || begin < str.length())
        appended->push_back(str.substr(begin));
}

/**
 * @param sstr
 *      该字符串中的每一个字符都是分割字符
 */
void chr_split(const std::string &str, const std::string &sstr, std::vector<std::string> *appended, bool ignore_empty = false)
{
    assert(sstr.length() > 0 && NULL != appended);

    std::string::size_type begin = 0, end = str.find_first_of(sstr);
    while (std::string::npos != end)
    {
        if (!ignore_empty || begin != end)
            appended->push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sstr, begin);
    }
    if (!ignore_empty || begin < str.length())
        appended->push_back(str.substr(begin));
}

void chr_split(const std::wstring &str, const std::wstring &sstr, std::vector<std::wstring> *appended, bool ignore_empty = false)
{
    assert(sstr.length() > 0 && NULL != appended);

    std::wstring::size_type begin = 0, end = str.find_first_of(sstr);
    while (std::wstring::npos != end)
    {
        if (!ignore_empty || begin != end)
            appended->push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sstr, begin);
    }
    if (!ignore_empty || begin < str.length())
        appended->push_back(str.substr(begin));
}

/**
 * split the string
 */
void chr_split(const std::string &str, char c, std::vector<std::string> *appended, bool ignore_empty = false)
{
    assert(NULL != appended);
    chr_split(str, std::string(1, c), appended, ignore_empty);
}

void chr_split(const std::wstring &str, wchar_t c, std::vector<std::wstring> *appended, bool ignore_empty = false)
{
    assert(NULL != appended);
    chr_split(str, std::wstring(1, c), appended, ignore_empty);
}

void format(std::string *appended, const char *fmt, ...)
{
    assert(NULL != appended && NULL != fmt);

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
    if (NULL != buf)
    {
        *appended += buf;
        ::free(buf); /* include the case of success of realloc() and failure of realloc() */
    }
}

void format(std::wstring *appended, const wchar_t *fmt, ...)
{
    assert(NULL != appended && NULL != fmt);

    size_t size = 100;
    wchar_t *buf = (wchar_t*) ::malloc(size * sizeof(wchar_t));
    assert(NULL != buf);

    va_list ap;
    while (NULL != buf)
    {
        va_start(ap, fmt);
#if defined(NUT_PLATFORM_CC_VC)
        int n = ::_vsnwprintf(buf, size, fmt, ap);
#elif defined(NUT_PLATFORM_OS_MAC) || defined(NUT_PLATFORM_OS_LINUX)
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
    if (NULL != buf)
    {
        *appended += buf;
        ::free(buf); /* include the case of success of realloc() and failure of realloc() */
    }
}

std::string format(const char *fmt, ...)
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

std::wstring format(const wchar_t *fmt, ...)
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
#elif defined(NUT_PLATFORM_OS_MAC) || defined(NUT_PLATFORM_OS_LINUX)
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
void trim(const std::string& str, std::string *appended, const std::string& blanks = " \t\r\n")
{
    assert(NULL != appended);
    const std::string::size_type begin = str.find_first_not_of(blanks),
        end = str.find_last_not_of(blanks);
    if (std::string::npos != begin && std::string::npos != end)
        *appended += str.substr(begin, end - begin + 1);
}

void trim(const std::wstring& str, std::wstring *appended, const std::wstring& blanks = L" \t\r\n")
{
    assert(NULL != appended);
    const std::wstring::size_type begin = str.find_first_not_of(blanks),
        end = str.find_last_not_of(blanks);
    if (std::wstring::npos != begin && std::wstring::npos == end)
        *appended += str.substr(begin, end - begin + 1);
}

std::string trim(const std::string& str, const std::string& blanks = " \t\r\n")
{
    std::string ret;
    trim(str, &ret, blanks);
    return ret;
}

std::wstring trim(const std::wstring& str, const std::wstring& blanks = L" \t\r\n")
{
    std::wstring ret;
    trim(str, &ret, blanks);
    return ret;
}

/** 去除左边空白 */
void ltrim(const std::string& str, std::string *appended, const std::string& blanks = " \t\r\n")
{
    assert(NULL != appended);
    const std::string::size_type begin = str.find_first_not_of(blanks);
    *appended += str.substr(begin);
}

void ltrim(const std::wstring& str, std::wstring *appended, const std::wstring& blanks = L" \t\r\n")
{
    assert(NULL != appended);
    const std::wstring::size_type begin = str.find_first_not_of(blanks);
    *appended += str.substr(begin);
}

std::string ltrim(const std::string& str, const std::string& blanks = " \t\r\n")
{
    std::string ret;
    ltrim(str, &ret, blanks);
    return ret;
}

std::wstring ltrim(const std::wstring& str, const std::wstring& blanks = L" \t\r\n")
{
    std::wstring ret;
    ltrim(str, &ret, blanks);
    return ret;
}

/** 去除右边空白 */
void rtrim(const std::string& str, std::string *appended, const std::string& blanks = " \t\r\n")
{
    assert(NULL != appended);
    const std::string::size_type end = str.find_last_not_of(blanks);
    if (std::string::npos != end)
        *appended += str.substr(0, end + 1);
}

void rtrim(const std::wstring& str, std::wstring *appended, const std::wstring& blanks = L" \t\r\n")
{
    assert(NULL != appended);
    const std::wstring::size_type end = str.find_last_not_of(blanks);
    if (std::wstring::npos != end)
        *appended += str.substr(0, end + 1);
}

std::string rtrim(const std::string& str, const std::string& blanks = " \t\r\n")
{
    std::string ret;
    rtrim(str, &ret, blanks);
    return ret;
}

std::wstring rtrim(const std::wstring& str, const std::wstring& blanks = L" \t\r\n")
{
    std::wstring ret;
    rtrim(str, &ret, blanks);
    return ret;
}

/** 忽略大小写的字符串比较 */
bool strieq(const std::string& str1, const std::string& str2)
{
    if (str1.length() != str2.length())
        return false;
    for (size_t i = 0, len = str1.length(); i < len; ++i)
        if ((str1.at(i) | 0x20) != (str2.at(i) | 0x20))
            return false;
    return true;
}

bool strieq(const std::wstring& str1, const std::wstring& str2)
{
    if (str1.length() != str2.length())
        return false;
    for (size_t i = 0, len = str1.length(); i < len; ++i)
        if ((str1.at(i) | 0x20) != (str2.at(i) | 0x20))
            return false;
    return true;
}

int stricmp(const char *str1, const char *str2)
{
    assert(NULL != str1 && NULL != str2);
    size_t i = 0;
    while ((str1[i] | 0x20) == (str2[i] | 0x20) && 0 != str1[i])
        ++i;
    return str1[i] - str2[i];
}

int stricmp(const wchar_t *str1, const char *str2)
{
    assert(NULL != str1 && NULL != str2);
    size_t i = 0;
    while ((str1[i] | 0x20) == (str2[i] | 0x20) && 0 != str1[i])
        ++i;
    return (str1[i] | 0x20) - (str2[i] | 0x20);
}

int strincmp(const char *str1, const char *str2, size_t n)
{
    assert(NULL != str1 && NULL != str2);
    size_t i = 0;
    while (i < n && (str1[i] | 0x20) == (str2[i] | 0x20))
        ++i;
    if (i >= n)
        return 0;
    return (str1[i] | 0x20) - (str2[i] | 0x20);
}

int strincmp(const wchar_t *str1, const wchar_t *str2, size_t n)
{
    assert(NULL != str1 && NULL != str2);
    size_t i = 0;
    while (i < n && (str1[i] | 0x20) == (str2[i] | 0x20))
        ++i;
    if (i >= n)
        return 0;
    return (str1[i] | 0x20) - (str2[i] | 0x20);
}

/**
 * 匹配字符串的开头
 */
bool starts_with(const std::string& s, const std::string& head)
{
    if (s.length() < head.length())
        return false;
    for (size_t i = 0, len = head.length(); i < len; ++i)
        if (s.at(i) != head.at(i))
            return false;
    return true;
}

bool starts_with(const std::wstring& s, const std::wstring& head)
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
bool ends_with(const std::string& s, const std::string& tail)
{
    if (s.length() < tail.length())
        return false;
    for (size_t i = 1, len = tail.length(); i <= len; ++i)
        if (s.at(s.length() - i) != tail.at(tail.length() - i))
            return false;
    return true;
}

bool ends_with(const std::wstring& s, const std::wstring& tail)
{
    if (s.length() < tail.length())
        return false;
    for (size_t i = 1, len = tail.length(); i <= len; ++i)
        if (s.at(s.length() - i) != tail.at(tail.length() - i))
            return false;
    return true;
}

bool ascii_to_wstr(const char *str, std::wstring *appended)
{
    assert(NULL != str && NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    const int n = ::MultiByteToWideChar(CP_ACP, 0 /* flags */, str, -1 /* 字符串以'\0'结束 */, NULL, 0); // 返回值包含了 '\0'
    if (n <= 0)
        return false;
    const int old_len = (int) appended->length();
    appended->resize(old_len + n - 1);
    const int rs = ::MultiByteToWideChar(CP_ACP, 0, str, -1, &(*appended)[old_len], n - 1);
    assert(((int) appended->length()) == old_len + n - 1);
    return rs > 0;
#else
    const size_t n = ::mbstowcs(NULL, str, 0); // 返回值未包含 '\0'
    if (n <= 0)
        return false;
    const size_t old_len = appended->length();
    appended->resize(old_len + n);
    const size_t rs = ::mbstowcs(&(*appended)[old_len], str, n); // 未包含 '\0'
    assert(appended->length() == old_len + n);
    return rs > 0;
#endif
}

bool ascii_to_wstr(const std::string& str, std::wstring *appended)
{
    assert(NULL != appended);
    return ascii_to_wstr(str.c_str(), appended);
}

std::wstring ascii_to_wstr(const char *str)
{
    assert(NULL != str);
    std::wstring ret;
    ascii_to_wstr(str, &ret);
    return ret;
}

std::wstring ascii_to_wstr(const std::string& str)
{
    return ascii_to_wstr(str.c_str());
}

bool wstr_to_ascii(const wchar_t *wstr, std::string *appended)
{
    assert(NULL != wstr && NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    const int n = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (n <= 0)
        return false;
    const int old_len = (int) appended->length();
    appended->resize(old_len + n - 1);
    const int rs = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, &(*appended)[old_len], n - 1, NULL, NULL);
    assert(((int) appended->length()) == old_len + n - 1);
    return rs > 0;
#else
    const size_t n = ::wcstombs(NULL, wstr, 0);
    if (n <= 0)
        return false;
    const size_t old_len = appended->length();
    appended->resize(old_len + n);
    const size_t rs = ::wcstombs(&(*appended)[old_len], wstr, n);
    assert(appended->length() == old_len + n);
    return rs > 0;
#endif
}

bool wstr_to_ascii(const std::wstring& wstr, std::string *appended)
{
    assert(NULL != appended);
    return wstr_to_ascii(wstr.c_str(), appended);
}

std::string wstr_to_ascii(const wchar_t *wstr)
{
    assert(NULL != wstr);
    std::string ret;
    wstr_to_ascii(wstr, &ret);
    return ret;
}

std::string wstr_to_ascii(const std::wstring& wstr)
{
    return wstr_to_ascii(wstr.c_str());
}

bool utf8_to_wstr(const char *str, std::wstring *appended)
{
    assert(NULL != str && NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    const int n = ::MultiByteToWideChar(CP_UTF8, 0 /* flags */, str, -1 /* 字符串以'\0'结束 */, NULL, 0);
    if (n <= 0)
        return false;
    const int old_len = (int) appended->length();
    appended->resize(old_len + n - 1);
    const int rs = ::MultiByteToWideChar(CP_UTF8, 0, str, -1, &(*appended)[old_len], n - 1);
    assert(((int) appended->length()) == old_len + n - 1);
    return rs > 0;
#else
    return ascii_to_wstr(str, appended);
#endif
}

bool utf8_to_wstr(const std::string& str, std::wstring *appended)
{
    assert(NULL != appended);
    return utf8_to_wstr(str.c_str(), appended);
}

std::wstring utf8_to_wstr(const char *str)
{
    assert(NULL != str);
    std::wstring ret;
    utf8_to_wstr(str, &ret);
    return ret;
}

std::wstring utf8_to_wstr(const std::string& str)
{
    return utf8_to_wstr(str.c_str());
}

bool wstr_to_utf8(const wchar_t *wstr, std::string *appended)
{
    assert(NULL != wstr && NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    const int n = ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (n <= 0)
        return false;
    const int old_len = (int) appended->length();
    appended->resize(old_len + n - 1);
    const int rs = ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &(*appended)[old_len], n - 1, NULL, NULL);
    assert(((int) appended->length()) == old_len + n - 1);
    return rs > 0;
#else
    return wstr_to_ascii(wstr, appended);
#endif
}

bool wstr_to_utf8(const std::wstring& wstr, std::string *appended)
{
    assert(NULL != appended);
    return wstr_to_utf8(wstr.c_str(), appended);
}

std::string wstr_to_utf8(const wchar_t *wstr)
{
    assert(NULL != wstr);
    std::string ret;
    wstr_to_utf8(wstr, &ret);
    return ret;
}

std::string wstr_to_utf8(const std::wstring& wstr)
{
    return wstr_to_utf8(wstr.c_str());
}

char int_to_hex_char(int i)
{
    const char *hex_chars = "0123456789ABCDEF";
    if (0 <= i && i < 16)
        return hex_chars[i];
    return '\0';
}

int hex_char_to_int(char c)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    else if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

int xml_encode(const char *s, int len, std::string *appended)
{
    assert(NULL != s && NULL != appended);

    int ret = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        switch (s[i])
        {
        case '&':
            *appended += "&amp;";
            ret += 5;
            break;

        case '"':
            *appended += "&quot;";
            ret += 6;
            break;

        case '<':
            *appended += "&lt;";
            ret += 4;
            break;

        case '>':
            *appended += "&gt;";
            ret += 4;
            break;

        default:
            appended->push_back(s[i]);
            ++ret;
            break;
        }
    }
    return ret;
}

int xml_decode(const char *s, int len, std::string *appended)
{
    assert(NULL != s && NULL != appended);

    int ret = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        if ('&' == s[i])
        {
            if (0 == ::strncmp(s + i + 1, "amp;", 4))
            {
                appended->push_back('&');
                i += 4;
                ++ret;
                continue;
            }
            else if (0 == ::strncmp(s + i + 1, "quot;", 5))
            {
                appended->push_back('"');
                i += 5;
                ++ret;
                continue;
            }
            else if (0 == ::strncmp(s + i + 1, "lt;", 3))
            {
                appended->push_back('<');
                i += 3;
                ++ret;
                continue;
            }
            else if (0 == ::strncmp(s + i + 1, "gt;", 3))
            {
                appended->push_back('>');
                i += 3;
                ++ret;
                continue;
            }
        }

        appended->push_back(s[i]);
        ++ret;
    }
    return ret;
}

int url_encode(const char *s, int len, std::string *appended)
{
    assert(NULL != s && NULL != appended);

    int ret = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        const char c = s[i];
        if (('0' <= c && c <= '9') || ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z'))
        {
            appended->push_back(c);
            ++ret;
            continue;
        }

        appended->push_back('%');
        appended->push_back(int_to_hex_char((c >> 4) & 0x0F));
        appended->push_back(int_to_hex_char(c & 0x0F));
        ret += 3;
    }
    return ret;
}

int url_decode(const char *s, int len, std::string *appended)
{
    assert(NULL != s && NULL != appended);

    int ret = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        if ('%' == s[i] && (len < 0 || i + 2 < len))
        {
            const int high = hex_char_to_int(s[i + 1]), low = hex_char_to_int(s[i + 2]);
            if (high >= 0 && low >= 0)
            {
                appended->push_back((char) ((high << 4) | low));
                i += 2;
                ret += 2;
                continue;
            }
        }

        appended->push_back(s[i]);
        ++ret;
    }
    return ret;
}

int hex_encode(const void *data, size_t cb, std::string *appended)
{
    assert(NULL != data && NULL != appended);

    for (size_t i = 0; i < cb; ++i)
    {
        const uint8_t b = ((const uint8_t*) data)[i];
        appended->push_back(int_to_hex_char((b >> 4) & 0x0F));
        appended->push_back(int_to_hex_char(b & 0x0F));
    }
    return (int) cb * 2;
}

int hex_decode(const char *s, int len, std::vector<uint8_t> *appended)
{
    assert(NULL != s && NULL != appended);

    int ret = 0;
    uint8_t v = 0;
    bool one_byte = true;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        const int vv = hex_char_to_int(s[i]);
        if (vv >= 0)
        {
            v <<= 4;
            v |= vv;
            one_byte = !one_byte;
            if (one_byte)
            {
                appended->push_back(v);
                ++ret;
            }
        }
    }
    return ret;
}

static char int_to_base64_char(int i)
{
    const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (0 <= i && i < 64)
        return base64_chars[i];
    return '=';
}

static int base64_char_to_int(char c)
{
    if ('A' <= c && c <= 'Z')
        return c - 'A';
    else if ('a' <= c && c <= 'z')
        return c - 'a' + 26;
    else if ('0' <= c && c <= '9')
        return c - '0' + 26 + 26;
    else if ('+' == c)
        return 62;
    else if ('/' == c)
        return 63;
    else if ('=' == c)
        return 64;
    return -1;
}

int base64_encode(const void *data, size_t cb, std::string *appended)
{
    assert(NULL != data && NULL != appended);

    const uint8_t *bytes = (const uint8_t*) data;
    for (size_t i = 0; i < cb; i += 3)
    {
        // every 3 bytes convert to 4 bytes, 6bit per byte

        // first 6 bit
        appended->push_back(int_to_base64_char(bytes[i] >> 2));

        // second 6 bit
        if (i + 1 < cb)
        {
            appended->push_back(int_to_base64_char(((bytes[i] & 0x03) << 4) ^ (bytes[i + 1] >> 4)));
        }
        else
        {
            appended->push_back(int_to_base64_char(((bytes[i] & 0x03) << 4)));
            appended->push_back('=');
            appended->push_back('=');
            break;
        }

        // third 6 bit
        if (i + 2 < cb)
        {
            appended->push_back(int_to_base64_char(((bytes[i + 1] & 0x0f) << 2) ^ (bytes[i + 2] >> 6)));
        }
        else
        {
            appended->push_back(int_to_base64_char(((bytes[i + 1] & 0x0f) << 2)));
            appended->push_back('=');
            break;
        }

        // last 6 bit
        appended->push_back(int_to_base64_char(bytes[i + 2] & 0x3f));
    }
    return (int) ((cb + 2) / 3) * 4;
}

int base64_decode(const char *s, int len, std::vector<uint8_t> *appended)
{
    assert(NULL != s && NULL != appended);

    int ret = 0;
    int buff[4], state = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        const int v = base64_char_to_int(s[i]);
        if (0 <= v and v <= 64)
        {
            buff[state++] = v;
            if (4 == state)
            {
                // first byte
                appended->push_back((uint8_t) ((buff[0] << 2) ^ ((buff[1] & 0x30) >> 4)));
                ++ret;

                // second byte
                if (64 == buff[2])
                    break;
                appended->push_back((uint8_t) ((buff[1] << 4) ^ ((buff[2] & 0x3c) >> 2)));
                ++ret;

                // third byte
                if (64 == buff[3])
                    break;
                appended->push_back((uint8_t) ((buff[2] << 6) ^ buff[3]));
                ++ret;

                // reset state
                state = 0;
            }
        }
    }
    return ret;
}

}

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif
