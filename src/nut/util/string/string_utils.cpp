
#include <assert.h>
#include <string.h>  /* for memset() */
#include <stdio.h>  /* for sprintf(), vsnprintf() and so on */
#include <stdarg.h> /* for va_start() */
#include <stdlib.h> /* for malloc() free() ltoa() wcstombs() and so on */
#include <wchar.h>

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "../../platform/int_type.h" // for ssize_t
#include "kmp.h"
#include "to_string.h"
#include "string_utils.h"


#if NUT_PLATFORM_CC_VC
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

/**
 * 用整个字符串来分割字符串
 */
NUT_API std::vector<std::string> str_split(const char *str, const char *sep_str,
                                           bool ignore_empty)
{
    assert(nullptr != str && nullptr != sep_str && 0 != sep_str[0]);

    const size_t sep_len = ::strlen(sep_str);
    int *next = (int*) ::malloc(sizeof(int) * sep_len);
    kmp_build_next(sep_str, next, sep_len);

    const size_t str_len = ::strlen(str);
    size_t begin = 0;
    ssize_t end = kmp_search(str, str_len, 0, sep_str, next, sep_len);
    std::vector<std::string> result;
    while (end >= 0)
    {
        if (!ignore_empty || begin != (size_t) end)
            result.emplace_back(str + begin, end - begin);
        begin = end + sep_len;
        end = kmp_search(str, str_len, begin, sep_str, next, sep_len);
    }
    if (!ignore_empty || begin < str_len)
        result.push_back(str + begin);
    ::free(next);
    return result;
}

NUT_API std::vector<std::string> str_split(
    const std::string& str, const std::string& sep_str, bool ignore_empty)
{
    return str_split(str.c_str(), sep_str.c_str(), ignore_empty);
}

NUT_API std::vector<std::wstring> str_split(
    const wchar_t *str, const wchar_t *sep_str, bool ignore_empty)
{
    assert(nullptr != str && nullptr != sep_str && 0 != sep_str[0]);

    const size_t sep_len = ::wcslen(sep_str);
    int *next = (int*) ::malloc(sizeof(int) * sep_len);
    kmp_build_next(sep_str, next, sep_len);

    const size_t str_len = ::wcslen(str);
    size_t begin = 0;
    ssize_t end = kmp_search(str, str_len, 0, sep_str, next, sep_len);
    std::vector<std::wstring> result;
    while (end >= 0)
    {
        if (!ignore_empty || begin != (size_t) end)
            result.emplace_back(str + begin, end - begin);
        begin = end + sep_len;
        end = kmp_search(str, str_len, begin, sep_str, next, sep_len);
    }
    if (!ignore_empty || begin < str_len)
        result.push_back(str + begin);
    ::free(next);
    return result;
}

NUT_API std::vector<std::wstring> str_split(
    const std::wstring& str, const std::wstring& sep_str, bool ignore_empty)
{
    return str_split(str.c_str(), sep_str.c_str(), ignore_empty);
}

/**
 * @param sep_chars
 *      该字符串中的每一个字符都是分割字符
 */ 
NUT_API std::vector<std::string> chr_split(
    const char *str_, const char *sep_chars, bool ignore_empty)
{
    assert(nullptr != str_ && nullptr != sep_chars && 0 != sep_chars[0]);

    const std::string str(str_);
    std::string::size_type begin = 0, end = str.find_first_of(sep_chars);
    std::vector<std::string> result;
    while (std::string::npos != end)
    {
        if (!ignore_empty || begin != end)
            result.push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sep_chars, begin);
    }
    if (!ignore_empty || begin < str.length())
        result.push_back(str.substr(begin));
    return result;
}

NUT_API std::vector<std::string> chr_split(
    const std::string& str, const std::string& sep_chars, bool ignore_empty)
{
    return chr_split(str.c_str(), sep_chars.c_str(), ignore_empty);
}

NUT_API std::vector<std::wstring> chr_split(
    const wchar_t *str_, const wchar_t *sep_chars, bool ignore_empty)
{
    assert(nullptr != str_ && nullptr != sep_chars && 0 != sep_chars[0]);

    const std::wstring str(str_);
    std::wstring::size_type begin = 0, end = str.find_first_of(sep_chars);
    std::vector<std::wstring> result;
    while (std::wstring::npos != end)
    {
        if (!ignore_empty || begin != end)
            result.push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sep_chars, begin);
    }
    if (!ignore_empty || begin < str.length())
        result.push_back(str.substr(begin));
    return result;
}

NUT_API std::vector<std::wstring> chr_split(
    const std::wstring& str, const std::wstring& sep_chars, bool ignore_empty)
{
    return chr_split(str.c_str(), sep_chars.c_str(), ignore_empty);
}

/**
 * split the string
 */
NUT_API std::vector<std::string> chr_split(
    const char *str, char c, bool ignore_empty)
{
    assert(nullptr != str);
    char seps[2] = {c, 0};
    return chr_split(str, seps, ignore_empty);
}

NUT_API std::vector<std::string> chr_split(
    const std::string& str, char c, bool ignore_empty)
{
    return chr_split(str.c_str(), c, ignore_empty);
}

NUT_API std::vector<std::wstring> chr_split(
    const wchar_t *str, wchar_t c, bool ignore_empty)
{
    wchar_t seps[2] = {c, 0};
    return chr_split(str, seps, ignore_empty);
}

NUT_API std::vector<std::wstring> chr_split(
    const std::wstring& str, wchar_t c, bool ignore_empty)
{
    return chr_split(str.c_str(), c, ignore_empty);
}

NUT_API ssize_t safe_snprintf(char *buf, size_t buf_size, const char *fmt, ...)
{
    assert(nullptr != fmt);
    if (nullptr == buf || buf_size <= 0)
        return 0;

    va_list ap;
    va_start(ap, fmt);
    ssize_t n = ::vsnprintf(buf, buf_size, fmt, ap);
    va_end(ap);

    if (n < 0)
        n = 0; /* glibc 2.0 */
    else if (n >= (ssize_t) buf_size)
        n = buf_size - 1; /* glibc 2.1 */

    buf[n] = 0; /* linux版的vsnprintf能保证'\0'结尾，windows版的vsnprintf不能保证'\0'结尾 */
    return n;
}

NUT_API ssize_t safe_snprintf(wchar_t *buf, size_t buf_size, const wchar_t *fmt, ...)
{
    assert(nullptr != fmt);
    if (nullptr == buf || buf_size <= 0)
        return 0;

    va_list ap;
    va_start(ap, fmt);
#if NUT_PLATFORM_CC_VC
    ssize_t n = ::_vsnwprintf(buf, buf_size, fmt, ap);
#elif NUT_PLATFORM_OS_MACOS || NUT_PLATFORM_OS_LINUX
    ssize_t n = ::vswprintf(buf, buf_size, fmt, ap);
#else
    ssize_t n = ::vsnwprintf(buf, buf_size, fmt, ap);
#endif
    va_end(ap);

    if (n < 0)
        n = 0; /* glibc 2.0 */
    else if (n >= (ssize_t) buf_size)
        n = buf_size - 1; /* glibc 2.1 */

    buf[n] = 0; /* linux版的vsnprintf能保证'\0'结尾，windows版的vsnprintf不能保证'\0'结尾 */
    return n;
}

NUT_API std::string format(const char *fmt, ...)
{
    assert(nullptr != fmt);

    std::string ret;
    size_t size = ::strlen(fmt) * 3 / 2 + 8;
    ret.resize(size);

    va_list ap;
    while (true)
    {
        va_start(ap, fmt);
        const int n = ::vsnprintf((char*) ret.data(), size, fmt, ap);
        va_end(ap);

        if (0 <= n && n < (int) size)
        {
            ret.resize(n);
            break;
        }

        if (n < 0)
            size *= 2; /* glibc 2.0 */
        else
            size = n + 1; /* glibc 2.1 */
        ret.resize(size);
    }

    return ret;
}

NUT_API std::wstring format(const wchar_t *fmt, ...)
{
    assert(nullptr != fmt);

    std::wstring ret;
    size_t size = ::wcslen(fmt) * 3 / 2 + 8;
    ret.resize(size);

    va_list ap;
    while (true)
    {
        va_start(ap, fmt);
#if NUT_PLATFORM_CC_VC
        const int n = ::_vsnwprintf((wchar_t*) ret.data(), size, fmt, ap);
#elif NUT_PLATFORM_OS_MACOS || NUT_PLATFORM_OS_LINUX
        const int n = ::vswprintf((wchar_t*) ret.data(), size, fmt, ap);
#else
        const int n = ::vsnwprintf((wchar_t*) ret.data(), size, fmt, ap);
#endif
        va_end(ap);

        if (0 <= n && n < (int) size)
        {
            ret.resize(n);
            break;
        }
        
        if (n < 0)
            size *= 2; /* glibc 2.0 */
        else
            size = n + 1; /* glibc 2.1 */
        ret.resize(size);
    }

    return ret;
}

/* 去除首尾空白 */
NUT_API std::string trim(const char *str_, const char *blanks)
{
    assert(nullptr != str_ && nullptr != blanks);

    const std::string str(str_);
    const std::string::size_type begin = str.find_first_not_of(blanks),
        end = str.find_last_not_of(blanks);
    if (std::string::npos != begin && std::string::npos != end)
        return str.substr(begin, end - begin + 1);
    return std::string();
}

NUT_API std::string trim(const std::string& str, const std::string& blanks)
{
    return trim(str.c_str(), blanks.c_str());
}

NUT_API std::wstring trim(const wchar_t *str_, const wchar_t *blanks)
{
    assert(nullptr != str_ && nullptr != blanks);

    const std::wstring str(str_);
    const std::wstring::size_type begin = str.find_first_not_of(blanks),
        end = str.find_last_not_of(blanks);
    if (std::wstring::npos != begin && std::wstring::npos == end)
        return str.substr(begin, end - begin + 1);
    return std::wstring();
}

NUT_API std::wstring trim(const std::wstring& str, const std::wstring& blanks)
{
    return trim(str.c_str(), blanks.c_str());
}

/** 去除左边空白 */
NUT_API std::string ltrim(const char *str_, const char *blanks)
{
    assert(nullptr != str_ && nullptr != blanks);
    const std::string str(str_);
    const std::string::size_type begin = str.find_first_not_of(blanks);
    return str.substr(begin);
}

NUT_API std::string ltrim(const std::string& str, const std::string& blanks)
{
    return ltrim(str.c_str(), blanks.c_str());
}

NUT_API std::wstring ltrim(const wchar_t *str_, const wchar_t *blanks)
{
    assert(nullptr != str_ && nullptr != blanks);
    const std::wstring str(str_);
    const std::wstring::size_type begin = str.find_first_not_of(blanks);
    return str.substr(begin);
}

NUT_API std::wstring ltrim(const std::wstring str, const std::wstring& blanks)
{
    return ltrim(str.c_str(), blanks.c_str());
}

/** 去除右边空白 */
NUT_API std::string rtrim(const char *str_, const char *blanks)
{
    assert(nullptr != str_ && nullptr != blanks);
    const std::string str(str_);
    const std::string::size_type end = str.find_last_not_of(blanks);
    if (std::string::npos != end)
        return str.substr(0, end + 1);
    return std::string();
}

NUT_API std::string rtrim(const std::string& str, const std::string& blanks)
{
    return rtrim(str.c_str(), blanks.c_str());
}

NUT_API std::wstring rtrim(const wchar_t *str_, const wchar_t *blanks)
{
    assert(nullptr != str_ && nullptr != blanks);
    const std::wstring str(str_);
    const std::wstring::size_type end = str.find_last_not_of(blanks);
    if (std::wstring::npos != end)
        return str.substr(0, end + 1);
    return std::wstring();
}

NUT_API std::wstring rtrim(const std::wstring& str, const std::wstring& blanks)
{
    return rtrim(str.c_str(), blanks.c_str());
}

NUT_API int chricmp(int c1, int c2)
{
    static_assert(((int) 'A') == ((int) L'A') && ((int) 'Z') == ((int) L'Z') &&
                  ((int) 'a') == ((int) L'a') && ((int) 'z') == ((int) L'z'),
                  "Unexpected 'wchar_t' ASCII encoding");
    if ('A' <= c1 && c1 <= 'Z')
        c1 |= 0x20;
    if ('A' <= c2 && c2 <= 'Z')
        c2 |= 0x20;
    return c1 - c2;
}

/** 忽略大小写的字符串比较 */
NUT_API int stricmp(const char *str1, const char *str2)
{
    assert(nullptr != str1 && nullptr != str2);
    size_t i = 0;
    while (0 != str1[i] && 0 == chricmp(str1[i], str2[i]))
        ++i;
    return chricmp(str1[i], str2[i]);
}

NUT_API int stricmp(const std::string& str1, const std::string& str2)
{
    return stricmp(str1.c_str(), str2.c_str());
}

NUT_API int stricmp(const wchar_t *str1, const wchar_t *str2)
{
    assert(nullptr != str1 && nullptr != str2);
    size_t i = 0;
    while (0 != str1[i] && 0 == chricmp(str1[i], str2[i]))
        ++i;
    return chricmp(str1[i], str2[i]);
}

NUT_API int stricmp(const std::wstring& str1, const std::wstring& str2)
{
    return stricmp(str1.c_str(), str2.c_str());
}

NUT_API int strincmp(const char *str1, const char *str2, size_t n)
{
    assert(nullptr != str1 && nullptr != str2);
    size_t i = 0;
    while (i < n && 0 == chricmp(str1[i], str2[i]))
        ++i;
    if (i >= n)
        return 0;
    return chricmp(str1[i], str2[i]);
}

NUT_API int strincmp(const std::string& str1, const std::string& str2, size_t n)
{
    return strincmp(str1.c_str(), str2.c_str(), n);
}

NUT_API int strincmp(const wchar_t *str1, const wchar_t *str2, size_t n)
{
    assert(nullptr != str1 && nullptr != str2);
    size_t i = 0;
    while (i < n && 0 == chricmp(str1[i], str2[i]))
        ++i;
    if (i >= n)
        return 0;
    return chricmp(str1[i], str2[i]);
}

NUT_API int strincmp(const std::wstring& str1, const std::wstring& str2, size_t n)
{
    return strincmp(str1.c_str(), str2.c_str(), n);
}

/**
 * 匹配字符串的开头
 */
NUT_API bool starts_with(const char *s, const char *head)
{
    assert(nullptr != s && nullptr != head);
    for (size_t i = 0; 0 != head[i]; ++i)
    {
        if (s[i] != head[i])
            return false;
    }
    return true;
}

NUT_API bool starts_with(const std::string& s, const std::string& head)
{
    return starts_with(s.c_str(), head.c_str());
}

NUT_API bool starts_with(const wchar_t *s, const wchar_t *head)
{
    assert(nullptr != s && nullptr != head);
    for (size_t i = 0; 0 != head[i]; ++i)
    {
        if (s[i] != head[i])
            return false;
    }
    return true;
}

NUT_API bool starts_with(const std::wstring& s, const std::wstring& head)
{
    return starts_with(s.c_str(), head.c_str());
}

/**
 * 匹配字符串的结尾
 */
NUT_API bool ends_with(const char *s, const char *tail)
{
    assert(nullptr != s && nullptr != tail);
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

NUT_API bool ends_with(const std::string& s, const std::string& tail)
{
    return ends_with(s.c_str(), tail.c_str());
}

NUT_API bool ends_with(const wchar_t *s, const wchar_t *tail)
{
    assert(nullptr != s && nullptr != tail);
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

NUT_API bool ends_with(const std::wstring& s, const std::wstring& tail)
{
    return ends_with(s.c_str(), tail.c_str());
}

NUT_API std::wstring ascii_to_wstr(const char *str)
{
    assert(nullptr != str);

    std::wstring result;

#if NUT_PLATFORM_OS_WINDOWS
    const int n = ::MultiByteToWideChar(CP_ACP,
                                        0,       // flags
                                        str,
                                        -1,      // 字符串以'\0'结束
                                        nullptr,
                                        0);      // 返回值 n 包含了 '\0'
    if (n <= 0)
        return result; // failed
    result.resize(n - 1);
    const int rs = ::MultiByteToWideChar(CP_ACP,
                                         0,
                                         str,
                                         -1,
                                         (wchar_t*) result.data(),
                                         n);
    assert(rs > 0); // success
    assert(((int) result.length()) == n - 1);
    return result;
#else
    const size_t n = ::mbstowcs(nullptr, str, 0); // 返回值 n 未包含 '\0'
    if (n == (size_t) -1)
        return result; // failed
    result.resize(n);
    const size_t rs = ::mbstowcs((wchar_t*) result.data(), str, n); // n 未包含 '\0'
    assert(rs != (size_t) -1); // success
    UNUSED(rs);
    assert(result.length() == n);
    return result;
#endif
}

NUT_API std::wstring ascii_to_wstr(const std::string& str)
{
    return ascii_to_wstr(str.c_str());
}

NUT_API std::string wstr_to_ascii(const wchar_t *wstr)
{
    assert(nullptr != wstr);

    std::string result;

#if NUT_PLATFORM_OS_WINDOWS
    const int n = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    if (n <= 0)
        return result; // failed
    result.resize(n - 1);
    const int rs = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, (char*) result.data(),
                                         n, nullptr, nullptr);
    assert(rs > 0); // success
    assert(((int) result.length()) == n - 1);
    return result;
#else
    const size_t n = ::wcstombs(nullptr, wstr, 0);
    if (n == (size_t) -1)
        return result; // failed
    result.resize(n);
    const size_t rs = ::wcstombs((char*) result.data(), wstr, n);
    assert(rs != (size_t) -1); // success
    UNUSED(rs);
    assert(result.length() == n);
    return result;
#endif
}

NUT_API std::string wstr_to_ascii(const std::wstring& wstr)
{
    return wstr_to_ascii(wstr.c_str());
}

NUT_API std::wstring utf8_to_wstr(const char *str)
{
    assert(nullptr != str);

    std::wstring result;

#if NUT_PLATFORM_OS_WINDOWS
    const int n = ::MultiByteToWideChar(CP_UTF8,
                                        0,       // flags
                                        str,
                                        -1,      // 字符串以'\0'结束
                                        nullptr,
                                        0);
    if (n <= 0)
        return result; // failed
    result.resize(n - 1);
    const int rs = ::MultiByteToWideChar(CP_UTF8, 0, str, -1,
                                         (wchar_t*) result.data(), n);
    assert(rs > 0); // success
    assert(((int) result.length()) == n - 1);
    return result;
#else
    return ascii_to_wstr(str);
#endif
}

NUT_API std::wstring utf8_to_wstr(const std::string& str)
{
    return utf8_to_wstr(str.c_str());
}

NUT_API std::string wstr_to_utf8(const wchar_t *wstr)
{
    assert(nullptr != wstr);

    std::string result;

#if NUT_PLATFORM_OS_WINDOWS
    const int n = ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0,
                                        nullptr, nullptr);
    if (n <= 0)
        return result; // failed
    result.resize(n - 1);
    const int rs = ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1, (char*) result.data(),
                                         n, nullptr, nullptr);
    assert(rs > 0); // success
    assert(((int) result.length()) == n - 1);
    return result;
#else
    return wstr_to_ascii(wstr);
#endif
}

NUT_API std::string wstr_to_utf8(const std::wstring& wstr)
{
    return wstr_to_utf8(wstr.c_str());
}

NUT_API std::string ascii_to_utf8(const char *str)
{
    assert(nullptr != str);

#if NUT_PLATFORM_OS_WINDOWS
    return wstr_to_utf8(ascii_to_wstr(str));
#else
    return str;
#endif
}

NUT_API std::string ascii_to_utf8(const std::string& str)
{
    return ascii_to_utf8(str.c_str());
}

NUT_API std::string utf8_to_ascii(const char *str)
{
    assert(nullptr != str);

#if NUT_PLATFORM_OS_WINDOWS
    return wstr_to_ascii(utf8_to_wstr(str));
#else
    return str;
#endif
}

NUT_API std::string utf8_to_ascii(const std::string& str)
{
    return utf8_to_ascii(str.c_str());
}

NUT_API std::string xml_encode(const char *s, ssize_t len)
{
    assert(nullptr != s);

    std::string result;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        switch (s[i])
        {
        case '&':
            result += "&amp;";
            break;

        case '"':
            result += "&quot;";
            break;

        case '<':
            result += "&lt;";
            break;

        case '>':
            result += "&gt;";
            break;

        default:
            result.push_back(s[i]);
            break;
        }
    }
    return result;
}

NUT_API std::string xml_decode(const char *s, ssize_t len)
{
    assert(nullptr != s);

    std::string result;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        if ('&' == s[i])
        {
            if (0 == ::strncmp(s + i + 1, "amp;", 4))
            {
                result.push_back('&');
                i += 4;
                continue;
            }
            else if (0 == ::strncmp(s + i + 1, "quot;", 5))
            {
                result.push_back('"');
                i += 5;
                continue;
            }
            else if (0 == ::strncmp(s + i + 1, "lt;", 3))
            {
                result.push_back('<');
                i += 3;
                continue;
            }
            else if (0 == ::strncmp(s + i + 1, "gt;", 3))
            {
                result.push_back('>');
                i += 3;
                continue;
            }
        }

        result.push_back(s[i]);
    }
    return result;
}

NUT_API std::string url_encode(const char *s, ssize_t len)
{
    assert(nullptr != s);

    std::string result;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        const char c = s[i];
        if (('0' <= c && c <= '9') || ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z'))
        {
            result.push_back(c);
            continue;
        }

        result.push_back('%');
        result.push_back(int_to_char((c >> 4) & 0x0f));
        result.push_back(int_to_char(c & 0x0f));
    }
    return result;
}

NUT_API std::string url_decode(const char *s, ssize_t len)
{
    assert(nullptr != s);

    std::string result;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        if ('%' == s[i] && (len < 0 || i + 2 < len))
        {
            const int high = char_to_int(s[i + 1]), low = char_to_int(s[i + 2]);
            if (0 <= high && high < 16 && 0 <= low && low < 16)
            {
                result.push_back((char) ((high << 4) | low));
                i += 2;
                continue;
            }
        }

        result.push_back(s[i]);
    }
    return result;
}

NUT_API std::string hex_encode(const void *data, size_t cb, bool upper_case)
{
    assert(nullptr != data);

    std::string result;
    for (size_t i = 0; i < cb; ++i)
    {
        const uint8_t b = ((const uint8_t*) data)[i];
        result.push_back(int_to_char((b >> 4) & 0x0f, upper_case));
        result.push_back(int_to_char(b & 0x0f, upper_case));
    }
    return result;
}

NUT_API std::vector<uint8_t> hex_decode(const char *s, ssize_t len)
{
    assert(nullptr != s);

    std::vector<uint8_t> result;
    uint8_t v = 0;
    bool one_byte = true;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        const int vv = char_to_int(s[i]);
        if (0 <= vv && vv < 16)
        {
            v <<= 4;
            v |= vv;
            one_byte = !one_byte;
            if (one_byte)
                result.push_back(v);
        }
    }
    return result;
}

NUT_API std::string cstyle_encode(const char *s, ssize_t len)
{
    assert(nullptr != s);

    std::string result;
    for (int i = 0; i < len || (len < 0 && 0 != s[i]); ++i)
    {
        const char c = s[i];
        switch (c)
        {
        case '\a':
            result += "\\a";
            break;

        case '\b':
            result += "\\b";
            break;

        case '\f':
            result += "\\f";
            break;

        case '\n':
            result += "\\n";
            break;

        case '\r':
            result += "\\r";
            break;

        case '\t':
            result += "\\t";
            break;

        case '\v':
            result += "\\v";
            break;

        case '\\':
            result += "\\\\";
            break;

        case '\"':
            result += "\\\"";
            break;

        case '\'':
            result += "\\\'";
            break;

        default:
            if (' ' <= c && c <= '~')
            {
                // 32 ~ 126 是可见字符
                result.push_back(c);
            }
            else
            {
                result += "\\x";
                result.push_back(int_to_char((c >> 4) & 0x0f));
                result.push_back(int_to_char(c & 0x0f));
            }
        }
    }
    return result;
}

NUT_API std::string cstyle_decode(const char *s, ssize_t len)
{
    assert(nullptr != s);

    std::string result;
    for (int i = 0; i < len || (len < 0 && 0 != s[i]); ++i)
    {
        char c = s[i];
        if ('\\' != c || i + 1 == len || 0 == s[i + 1])
        {
            result.push_back(c);
            continue;
        }

        ++i;
        c = s[i];
        switch (c)
        {
        case 'a':
            result.push_back('\a');
            break;

        case 'b':
            result.push_back('\b');
            break;

        case 'f':
            result.push_back('\f');
            break;

        case 'n':
            result.push_back('\n');
            break;

        case 'r':
            result.push_back('\r');
            break;

        case 't':
            result.push_back('\t');
            break;

        case 'v':
            result.push_back('\v');
            break;

        case '\\':
            result.push_back('\\');
            break;

        case '\"':
            result.push_back('\"');
            break;

        case '\'':
            result.push_back('\'');
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
            result.push_back(c);
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
            result.push_back(c);
            break;

        default:
            result.push_back('\\');
            result.push_back(c);
            break;
        }
    }
    return result;
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

NUT_API std::string base64_encode(const void *data, size_t cb)
{
    assert(nullptr != data);

    std::string result;
    const uint8_t *bytes = (const uint8_t*) data;
    for (size_t i = 0; i < cb; i += 3)
    {
        // every 3 bytes convert to 4 bytes, 6bit per byte

        // first 6 bit
        result.push_back(int_to_base64_char(bytes[i] >> 2));

        // second 6 bit
        if (i + 1 < cb)
        {
            result.push_back(int_to_base64_char(((bytes[i] & 0x03) << 4) ^ (bytes[i + 1] >> 4)));
        }
        else
        {
            result.push_back(int_to_base64_char(((bytes[i] & 0x03) << 4)));
            result.push_back('=');
            result.push_back('=');
            break;
        }

        // third 6 bit
        if (i + 2 < cb)
        {
            result.push_back(int_to_base64_char(((bytes[i + 1] & 0x0f) << 2) ^ (bytes[i + 2] >> 6)));
        }
        else
        {
            result.push_back(int_to_base64_char(((bytes[i + 1] & 0x0f) << 2)));
            result.push_back('=');
            break;
        }

        // last 6 bit
        result.push_back(int_to_base64_char(bytes[i + 2] & 0x3f));
    }
    return result;
}

NUT_API std::vector<uint8_t> base64_decode(const char *s, ssize_t len)
{
    assert(nullptr != s);

    std::vector<uint8_t> result;
    int buff[4], state = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        const int v = base64_char_to_int(s[i]);
        if (v < 0 || v > 64)
            continue;

        buff[state++] = v;
        if (4 == state)
        {
            // first byte
            result.push_back((uint8_t) ((buff[0] << 2) ^ ((buff[1] & 0x30) >> 4)));

            // second byte
            if (64 == buff[2])
                break;
            result.push_back((uint8_t) ((buff[1] << 4) ^ ((buff[2] & 0x3c) >> 2)));

            // third byte
            if (64 == buff[3])
                break;
            result.push_back((uint8_t) ((buff[2] << 6) ^ buff[3]));

            // reset state
            state = 0;
        }
    }
    return result;
}

}

#if NUT_PLATFORM_CC_VC
#   pragma warning(pop)
#endif
