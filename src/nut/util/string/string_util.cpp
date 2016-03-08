
#include <assert.h>
#include <string.h>  /* for memset() */
#include <stdio.h>  /* for sprintf(), vsnprintf() and so on */
#include <stdarg.h> /* for va_start() */
#include <stdlib.h> /* for malloc() free() ltoa() wcstombs() and so on */
#include <wchar.h>

#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
typedef SSIZE_T ssize_t;
#endif

#include "kmp.h"
#include "to_string.h"
#include "string_util.h"

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

/**
 * 用整个字符串来分割字符串
 */
void str_split(const char *str, const char *sep_str, std::vector<std::string> *appended, bool ignore_empty)
{
    assert(NULL != str && NULL != sep_str && 0 != sep_str[0] && NULL != appended);

    const size_t sep_len = ::strlen(sep_str);
    int *next = (int*) ::malloc(sizeof(int) * sep_len);
    kmp_build_next(sep_str, next, sep_len);

    const size_t str_len = ::strlen(str);
    size_t begin = 0;
    ssize_t end = kmp_search(str, str_len, 0, sep_str, next, sep_len);
    while (end >= 0)
    {
        if (!ignore_empty || begin != (size_t) end)
            appended->push_back(std::string(str + begin, end - begin));
        begin = end + sep_len;
        end = kmp_search(str, str_len, begin, sep_str, next, sep_len);
    }
    if (!ignore_empty || begin < str_len)
        appended->push_back(str + begin);
    ::free(next);
}

void str_split(const wchar_t *str, const wchar_t *sep_str, std::vector<std::wstring> *appended, bool ignore_empty)
{
    assert(NULL != str && NULL != sep_str && 0 != sep_str[0] && NULL != appended);

    const size_t sep_len = ::wcslen(sep_str);
    int *next = (int*) ::malloc(sizeof(int) * sep_len);
    kmp_build_next(sep_str, next, sep_len);

    const size_t str_len = ::wcslen(str);
    size_t begin = 0;
    ssize_t end = kmp_search(str, str_len, 0, sep_str, next, sep_len);
    while (end >= 0)
    {
        if (!ignore_empty || begin != (size_t) end)
            appended->push_back(std::wstring(str + begin, end - begin));
        begin = end + sep_len;
        end = kmp_search(str, str_len, begin, sep_str, next, sep_len);
    }
    if (!ignore_empty || begin < str_len)
        appended->push_back(str + begin);
    ::free(next);
}

/**
 * @param sep_chars
 *      该字符串中的每一个字符都是分割字符
 */
void chr_split(const char *str_, const char *sep_chars, std::vector<std::string> *appended, bool ignore_empty)
{
    assert(NULL != str_ && NULL != sep_chars && 0 != sep_chars[0] && NULL != appended);

    const std::string str(str_);
    std::string::size_type begin = 0, end = str.find_first_of(sep_chars);
    while (std::string::npos != end)
    {
        if (!ignore_empty || begin != end)
            appended->push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sep_chars, begin);
    }
    if (!ignore_empty || begin < str.length())
        appended->push_back(str.substr(begin));
}

void chr_split(const wchar_t *str_, const wchar_t *sep_chars, std::vector<std::wstring> *appended, bool ignore_empty)
{
    assert(NULL != str_ && NULL != sep_chars && 0 != sep_chars[0] && NULL != appended);

    const std::wstring str(str_);
    std::wstring::size_type begin = 0, end = str.find_first_of(sep_chars);
    while (std::wstring::npos != end)
    {
        if (!ignore_empty || begin != end)
            appended->push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sep_chars, begin);
    }
    if (!ignore_empty || begin < str.length())
        appended->push_back(str.substr(begin));
}

/**
 * split the string
 */
void chr_split(const char *str, char c, std::vector<std::string> *appended, bool ignore_empty)
{
    assert(NULL != str && NULL != appended);
    char seps[2] = {c, 0};
    chr_split(str, seps, appended, ignore_empty);
}

void chr_split(const wchar_t *str, wchar_t c, std::vector<std::wstring> *appended, bool ignore_empty)
{
    assert(NULL != appended);
    wchar_t seps[2] = {c, 0};
    chr_split(str, seps, appended, ignore_empty);
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
    std::string ret;
    if (NULL != buf)
    {
        ret = buf;
        ::free(buf); /* include the case of success of realloc() and failure of realloc() */
    }
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
    std::wstring ret;
    if (NULL != buf)
    {
        ret = buf;
        ::free(buf); /* include the case of success of realloc() and failure of realloc() */
    }
    return ret;
}

/* 去除首尾空白 */
void trim(const char *str_, std::string *appended, const char *blanks)
{
    assert(NULL != str_ && NULL != appended && NULL != blanks);

    const std::string str(str_);
    const std::string::size_type begin = str.find_first_not_of(blanks),
        end = str.find_last_not_of(blanks);
    if (std::string::npos != begin && std::string::npos != end)
        *appended += str.substr(begin, end - begin + 1);
}

void trim(const wchar_t *str_, std::wstring *appended, const wchar_t *blanks)
{
    assert(NULL != str_ && NULL != appended && NULL != blanks);

    const std::wstring str(str_);
    const std::wstring::size_type begin = str.find_first_not_of(blanks),
        end = str.find_last_not_of(blanks);
    if (std::wstring::npos != begin && std::wstring::npos == end)
        *appended += str.substr(begin, end - begin + 1);
}

/** 去除左边空白 */
void ltrim(const char *str_, std::string *appended, const char *blanks)
{
    assert(NULL != str_ && NULL != appended && NULL != blanks);
    const std::string str(str_);
    const std::string::size_type begin = str.find_first_not_of(blanks);
    *appended += str.substr(begin);
}

void ltrim(const wchar_t *str_, std::wstring *appended, const wchar_t *blanks)
{
    assert(NULL != str_ && NULL != appended && NULL != blanks);
    const std::wstring str(str_);
    const std::wstring::size_type begin = str.find_first_not_of(blanks);
    *appended += str.substr(begin);
}

/** 去除右边空白 */
void rtrim(const char *str_, std::string *appended, const char *blanks)
{
    assert(NULL != str_ && NULL != appended && NULL != blanks);
    const std::string str(str_);
    const std::string::size_type end = str.find_last_not_of(blanks);
    if (std::string::npos != end)
        *appended += str.substr(0, end + 1);
}

void rtrim(const wchar_t *str_, std::wstring *appended, const wchar_t *blanks)
{
    assert(NULL != str_ && NULL != appended && NULL != blanks);
    const std::wstring str(str_);
    const std::wstring::size_type end = str.find_last_not_of(blanks);
    if (std::wstring::npos != end)
        *appended += str.substr(0, end + 1);
}

static int charicmp(int c1, int c2)
{
    if ('A' <= c1 && c1 <= 'Z')
        c1 |= 0x20;
    if ('A' <= c2 && c2 <= 'Z')
        c2 |= 0x20;
    return c1 - c2;
}

/** 忽略大小写的字符串比较 */
int stricmp(const char *str1, const char *str2)
{
    assert(NULL != str1 && NULL != str2);
    size_t i = 0;
    while (0 != str1[i] && 0 == charicmp(str1[i], str2[i]))
        ++i;
    return charicmp(str1[i], str2[i]);
}

int stricmp(const wchar_t *str1, const wchar_t *str2)
{
    assert(NULL != str1 && NULL != str2);
    size_t i = 0;
    while (0 != str1[i] && 0 == charicmp(str1[i], str2[i]))
        ++i;
    return charicmp(str1[i], str2[i]);
}

int strincmp(const char *str1, const char *str2, size_t n)
{
    assert(NULL != str1 && NULL != str2);
    size_t i = 0;
    while (i < n && 0 == charicmp(str1[i], str2[i]))
        ++i;
    if (i >= n)
        return 0;
    return charicmp(str1[i], str2[i]);
}

int strincmp(const wchar_t *str1, const wchar_t *str2, size_t n)
{
    assert(NULL != str1 && NULL != str2);
    size_t i = 0;
    while (i < n && 0 == charicmp(str1[i], str2[i]))
        ++i;
    if (i >= n)
        return 0;
    return charicmp(str1[i], str2[i]);
}

/**
 * 匹配字符串的开头
 */
bool starts_with(const char *s, const char *head)
{
    assert(NULL != s && NULL != head);
    for (size_t i = 0; 0 != head[i]; ++i)
    {
        if (s[i] != head[i])
            return false;
    }
    return true;
}

bool starts_with(const wchar_t *s, const wchar_t *head)
{
    assert(NULL != s && NULL != head);
    for (size_t i = 0; 0 != head[i]; ++i)
    {
        if (s[i] != head[i])
            return false;
    }
    return true;
}

/**
 * 匹配字符串的结尾
 */
bool ends_with(const char *s, const char *tail)
{
    assert(NULL != s && NULL != tail);
    const size_t s_len = ::strlen(s), tail_len = ::strlen(tail);
    if (s_len < tail_len)
        return false;
    for (size_t i = 0; i < tail_len; ++i)
    {
        if (s[s_len - i] != tail[tail_len - i])
            return false;
    }
    return true;
}

bool ends_with(const wchar_t *s, const wchar_t *tail)
{
    assert(NULL != s && NULL != tail);
    const size_t s_len = ::wcslen(s), tail_len = ::wcslen(tail);
    if (s_len < tail_len)
        return false;
    for (size_t i = 0; i < tail_len; ++i)
    {
        if (s[s_len - i] != tail[tail_len - i])
            return false;
    }
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
    const int rs = ::MultiByteToWideChar(CP_ACP, 0, str, -1, &(*appended)[old_len], n);
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

bool wstr_to_ascii(const wchar_t *wstr, std::string *appended)
{
    assert(NULL != wstr && NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    const int n = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (n <= 0)
        return false;
    const int old_len = (int) appended->length();
    appended->resize(old_len + n - 1);
    const int rs = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, &(*appended)[old_len], n, NULL, NULL);
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

bool utf8_to_wstr(const char *str, std::wstring *appended)
{
    assert(NULL != str && NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    const int n = ::MultiByteToWideChar(CP_UTF8, 0 /* flags */, str, -1 /* 字符串以'\0'结束 */, NULL, 0);
    if (n <= 0)
        return false;
    const int old_len = (int) appended->length();
    appended->resize(old_len + n - 1);
    const int rs = ::MultiByteToWideChar(CP_UTF8, 0, str, -1, &(*appended)[old_len], n);
    assert(((int) appended->length()) == old_len + n - 1);
    return rs > 0;
#else
    return ascii_to_wstr(str, appended);
#endif
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
    const int rs = ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &(*appended)[old_len], n, NULL, NULL);
    assert(((int) appended->length()) == old_len + n - 1);
    return rs > 0;
#else
    return wstr_to_ascii(wstr, appended);
#endif
}

bool ascii_to_utf8(const char *str, std::string *appended)
{
    assert(NULL != str && NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    std::wstring tmp;
    if (!ascii_to_wstr(str, &tmp))
        return false;
    return wstr_to_utf8(tmp.c_str(), appended);
#else
    *appended += str;
    return true;
#endif
}

bool utf8_to_ascii(const char *str, std::string *appended)
{
    assert(NULL != str && NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    std::wstring tmp;
    if (!utf8_to_wstr(str, &tmp))
        return false;
    return wstr_to_ascii(tmp.c_str(), appended);
#else
    *appended += str;
    return true;
#endif
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

int hex_decode(const char *s, int len, Array<uint8_t> *appended)
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

int cstyle_encode(const char *s, int len, std::string *appended)
{
    assert(NULL != s && NULL != appended);

    int ret = 0;
    for (int i = 0; i < len || (len < 0 && 0 != s[i]); ++i)
    {
        const char c = s[i];
        switch (c)
        {
        case '\a':
            *appended += "\\a";
            ret += 2;
            break;

        case '\b':
            *appended += "\\b";
            ret += 2;
            break;

        case '\f':
            *appended += "\\f";
            ret += 2;
            break;

        case '\n':
            *appended += "\\n";
            ret += 2;
            break;

        case '\r':
            *appended += "\\r";
            ret += 2;
            break;

        case '\t':
            *appended += "\\t";
            ret += 2;
            break;

        case '\v':
            *appended += "\\v";
            ret += 2;
            break;

        case '\\':
            *appended += "\\\\";
            ret += 2;
            break;

        case '\"':
            *appended += "\\\"";
            ret += 2;
            break;

        case '\'':
            *appended += "\\\'";
            ret += 2;
            break;

        default:
            if (' ' <= c && c <= '~')
            {
                // 32 ~ 126 是可见字符
                appended->push_back(c);
                ++ret;
            }
            else
            {
                *appended += "\\x";
                appended->push_back(int_to_hex_char((c >> 4) & 0x0f));
                appended->push_back(int_to_hex_char(c & 0x0f));
                ret += 4;
            }
        }
    }
    return ret;
}

int cstyle_decode(const char *s, int len, std::string *appended)
{
    assert(NULL != s && NULL != appended);

    int ret = 0;
    for (int i = 0; i < len || (len < 0 && 0 != s[i]); ++i)
    {
        char c = s[i];
        if ('\\' != c || i + 1 == len || 0 == s[i + 1])
        {
            appended->push_back(c);
            ++ret;
            continue;
        }

        ++i;
        c = s[i];
        switch (c)
        {
        case 'a':
            appended->push_back('\a');
            ++ret;
            break;

        case 'b':
            appended->push_back('\b');
            ++ret;
            break;

        case 'f':
            appended->push_back('\f');
            ++ret;
            break;

        case 'n':
            appended->push_back('\n');
            ++ret;
            break;

        case 'r':
            appended->push_back('\r');
            ++ret;
            break;

        case 't':
            appended->push_back('\t');
            ++ret;
            break;

        case 'v':
            appended->push_back('\v');
            ++ret;
            break;

        case '\\':
            appended->push_back('\\');
            ++ret;
            break;

        case '\"':
            appended->push_back('\"');
            ++ret;
            break;

        case '\'':
            appended->push_back('\'');
            ++ret;
            break;

        case 'x': // 16进制转义
            c = 0;
            for (int j = 0; j < 2 && (i + 1 < len || (len < 0 && 0 != s[i + 1])); ++j)
            {
                const char cc = s[++i];
                if ('0' <= cc && cc <= '9')
                {
                    c = (char) (c * 16 + cc - '0');
                }
                else if ('a' <= cc && cc <= 'f')
                {
                    c = (char) (c * 16 + cc - 'a' + 10);
                }
                else if ('A' <= cc && cc <= 'F')
                {
                    c = (char) (c * 16 + cc - 'A' + 10);
                }
                else
                {
                    --i;
                    break;
                }
            }
            appended->push_back(c);
            ++ret;
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            c -= '0';
            for (size_t j = 0; j < 2 && (i + 1 < len || (len < 0 && 0 != s[i + 1])); ++j)
            {
                const char cc = s[++i];
                if ('0' <= cc && cc <= '7')
                {
                    c = (char) (c * 8 + cc - '0');
                }
                else
                {
                    --i;
                    break;
                }
            }
            appended->push_back(c);
            ++ret;
            break;

        default:
            appended->push_back('\\');
            appended->push_back(c);
            ret += 2;
            break;
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

int base64_decode(const char *s, int len, Array<uint8_t> *appended)
{
    assert(NULL != s && NULL != appended);

    int ret = 0;
    int buff[4], state = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        const int v = base64_char_to_int(s[i]);
        if (0 <= v && v <= 64)
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
