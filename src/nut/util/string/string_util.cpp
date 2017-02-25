
#include <assert.h>
#include <string.h>  /* for memset() */
#include <stdio.h>  /* for sprintf(), vsnprintf() and so on */
#include <stdarg.h> /* for va_start() */
#include <stdlib.h> /* for malloc() free() ltoa() wcstombs() and so on */
#include <wchar.h>

#include <nut/platform/platform.h>
#include <nut/platform/stdint_traits.h> // for ssize_t

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "kmp.h"
#include "to_string.h"
#include "string_util.h"

#if NUT_PLATFORM_CC_VC
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

/**
 * 用整个字符串来分割字符串
 */
NUT_API void str_split(const char *str, const char *sep_str,
                       std::vector<std::string> *result, bool ignore_empty)
{
    assert(nullptr != str && nullptr != sep_str && 0 != sep_str[0] && nullptr != result);

    const size_t sep_len = ::strlen(sep_str);
    int *next = (int*) ::malloc(sizeof(int) * sep_len);
    kmp_build_next(sep_str, next, sep_len);

    const size_t str_len = ::strlen(str);
    size_t begin = 0;
    ssize_t end = kmp_search(str, str_len, 0, sep_str, next, sep_len);
    while (end >= 0)
    {
        if (!ignore_empty || begin != (size_t) end)
            result->push_back(std::string(str + begin, end - begin));
        begin = end + sep_len;
        end = kmp_search(str, str_len, begin, sep_str, next, sep_len);
    }
    if (!ignore_empty || begin < str_len)
        result->push_back(str + begin);
    ::free(next);
}

NUT_API void str_split(const std::string& str, const std::string& sep_str,
                       std::vector<std::string> *result, bool ignore_empty)
{
    assert(nullptr != result);
    str_split(str.c_str(), sep_str.c_str(), result, ignore_empty);
}

NUT_API void str_split(const wchar_t *str, const wchar_t *sep_str,
                       std::vector<std::wstring> *result, bool ignore_empty)
{
    assert(nullptr != str && nullptr != sep_str && 0 != sep_str[0] && nullptr != result);

    const size_t sep_len = ::wcslen(sep_str);
    int *next = (int*) ::malloc(sizeof(int) * sep_len);
    kmp_build_next(sep_str, next, sep_len);

    const size_t str_len = ::wcslen(str);
    size_t begin = 0;
    ssize_t end = kmp_search(str, str_len, 0, sep_str, next, sep_len);
    while (end >= 0)
    {
        if (!ignore_empty || begin != (size_t) end)
            result->push_back(std::wstring(str + begin, end - begin));
        begin = end + sep_len;
        end = kmp_search(str, str_len, begin, sep_str, next, sep_len);
    }
    if (!ignore_empty || begin < str_len)
        result->push_back(str + begin);
    ::free(next);
}

NUT_API void str_split(const std::wstring& str, const std::wstring& sep_str,
                       std::vector<std::wstring> *result, bool ignore_empty)
{
    assert(nullptr != result);
    str_split(str.c_str(), sep_str.c_str(), result, ignore_empty);
}

/**
 * @param sep_chars
 *      该字符串中的每一个字符都是分割字符
 */
NUT_API void chr_split(const char *str_, const char *sep_chars,
                       std::vector<std::string> *result, bool ignore_empty)
{
    assert(nullptr != str_ && nullptr != sep_chars && 0 != sep_chars[0] && nullptr != result);

    const std::string str(str_);
    std::string::size_type begin = 0, end = str.find_first_of(sep_chars);
    while (std::string::npos != end)
    {
        if (!ignore_empty || begin != end)
            result->push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sep_chars, begin);
    }
    if (!ignore_empty || begin < str.length())
        result->push_back(str.substr(begin));
}

NUT_API void chr_split(const std::string& str, const std::string& sep_chars,
                       std::vector<std::string> *result, bool ignore_empty)
{
    assert(nullptr != result);
    chr_split(str.c_str(), sep_chars.c_str(), result, ignore_empty);
}

NUT_API void chr_split(const wchar_t *str_, const wchar_t *sep_chars,
                       std::vector<std::wstring> *result, bool ignore_empty)
{
    assert(nullptr != str_ && nullptr != sep_chars && 0 != sep_chars[0] && nullptr != result);

    const std::wstring str(str_);
    std::wstring::size_type begin = 0, end = str.find_first_of(sep_chars);
    while (std::wstring::npos != end)
    {
        if (!ignore_empty || begin != end)
            result->push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find_first_of(sep_chars, begin);
    }
    if (!ignore_empty || begin < str.length())
        result->push_back(str.substr(begin));
}

NUT_API void chr_split(const std::wstring& str, const std::wstring& sep_chars,
                       std::vector<std::wstring> *result, bool ignore_empty)
{
    assert(nullptr != result);
    chr_split(str.c_str(), sep_chars.c_str(), result, ignore_empty);
}

/**
 * split the string
 */
NUT_API void chr_split(const char *str, char c, std::vector<std::string> *result,
                       bool ignore_empty)
{
    assert(nullptr != str && nullptr != result);
    char seps[2] = {c, 0};
    chr_split(str, seps, result, ignore_empty);
}

NUT_API void chr_split(const std::string& str, char c, std::vector<std::string> *result,
                       bool ignore_empty)
{
    assert(nullptr != result);
    chr_split(str.c_str(), c, result, ignore_empty);
}

NUT_API void chr_split(const wchar_t *str, wchar_t c, std::vector<std::wstring> *result,
                       bool ignore_empty)
{
    assert(nullptr != result);
    wchar_t seps[2] = {c, 0};
    chr_split(str, seps, result, ignore_empty);
}

NUT_API void chr_split(const std::wstring& str, wchar_t c, std::vector<std::wstring> *result,
                       bool ignore_empty)
{
    assert(nullptr != result);
    chr_split(str.c_str(), c, result, ignore_empty);
}

NUT_API std::string format(const char *fmt, ...)
{
    assert(nullptr != fmt);

    std::string ret;
    size_t size = ::strlen(fmt) * 2 + 16;
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

        if (n > -1)
            size = n + 1; /* glibc 2.1 */
        else
            size *= 2;    /* glib 2.0 */
        ret.resize(size);
    }

    return ret;
}

NUT_API std::wstring format(const wchar_t *fmt, ...)
{
    assert(nullptr != fmt);

    std::wstring ret;
    size_t size = ::wcslen(fmt) * 2 + 16;
    ret.resize(size);

    va_list ap;
    while (true)
    {
        va_start(ap, fmt);
#if NUT_PLATFORM_CC_VC
        const int n = ::_vsnwprintf((wchar_t*) ret.data(), size, fmt, ap);
#elif NUT_PLATFORM_OS_MAC || NUT_PLATFORM_OS_LINUX
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

        if (n > -1)
            size = n + 1; /* glibc 2.1 */
        else
            size *= 2;    /* glib 2.0 */
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

NUT_API bool ascii_to_wstr(const char *str, std::wstring *result)
{
    assert(nullptr != str && nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
    const int n = ::MultiByteToWideChar(CP_ACP,
                                        0,       // flags
                                        str,
                                        -1,      // 字符串以'\0'结束
                                        nullptr,
                                        0);      // 返回值包含了 '\0'
    if (n <= 0)
        return false;
    const int old_len = (int) result->length();
    result->resize(old_len + n - 1);
    const int rs = ::MultiByteToWideChar(CP_ACP,
                                         0,
                                         str,
                                         -1,
                                         (wchar_t*) (result->data() + old_len),
                                         n);
    assert(((int) result->length()) == old_len + n - 1);
    return rs > 0;
#else
    const size_t n = ::mbstowcs(nullptr, str, 0); // 返回值未包含 '\0'
    if (n == (size_t) -1)
        return false;
    const size_t old_len = result->length();
    result->resize(old_len + n);
    const size_t rs = ::mbstowcs((wchar_t*) (result->data() + old_len), str, n); // 未包含 '\0'
    assert(result->length() == old_len + n);
    return rs != (size_t) -1;
#endif
}

NUT_API bool ascii_to_wstr(const std::string& str, std::wstring *result)
{
    assert(nullptr != result);
    return ascii_to_wstr(str.c_str(), result);
}

NUT_API bool wstr_to_ascii(const wchar_t *wstr, std::string *result)
{
    assert(nullptr != wstr && nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
    const int n = ::WideCharToMultiByte(CP_ACP,
                                        0,
                                        wstr,
                                        -1,
                                        nullptr,
                                        0,
                                        nullptr,
                                        nullptr);
    if (n <= 0)
        return false;
    const int old_len = (int) result->length();
    result->resize(old_len + n - 1);
    const int rs = ::WideCharToMultiByte(CP_ACP,
                                         0,
                                         wstr,
                                         -1,
                                         (char*) (result->data() + old_len),
                                         n,
                                         nullptr,
                                         nullptr);
    assert(((int) result->length()) == old_len + n - 1);
    return rs > 0;
#else
    const size_t n = ::wcstombs(nullptr, wstr, 0);
    if (n == (size_t) -1)
        return false;
    const size_t old_len = result->length();
    result->resize(old_len + n);
    const size_t rs = ::wcstombs((char*) (result->data() + old_len), wstr, n);
    assert(result->length() == old_len + n);
    return rs != (size_t) -1;
#endif
}

NUT_API bool wstr_to_ascii(const std::wstring& wstr, std::string *result)
{
    assert(nullptr != result);
    return wstr_to_ascii(wstr.c_str(), result);
}

NUT_API bool utf8_to_wstr(const char *str, std::wstring *result)
{
    assert(nullptr != str && nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
    const int n = ::MultiByteToWideChar(CP_UTF8,
                                        0,       // flags
                                        str,
                                        -1,      // 字符串以'\0'结束
                                        nullptr,
                                        0);
    if (n <= 0)
        return false;
    const int old_len = (int) result->length();
    result->resize(old_len + n - 1);
    const int rs = ::MultiByteToWideChar(CP_UTF8,
                                         0,
                                         str,
                                         -1,
                                         (wchar_t*) (result->data() + old_len),
                                         n);
    assert(((int) result->length()) == old_len + n - 1);
    return rs > 0;
#else
    return ascii_to_wstr(str, result);
#endif
}

NUT_API bool utf8_to_wstr(const std::string& str, std::wstring *result)
{
    assert(nullptr != result);
    return utf8_to_wstr(str.c_str(), result);
}

NUT_API bool wstr_to_utf8(const wchar_t *wstr, std::string *result)
{
    assert(nullptr != wstr && nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
    const int n = ::WideCharToMultiByte(CP_UTF8,
                                        0,
                                        wstr,
                                        -1,
                                        nullptr,
                                        0,
                                        nullptr,
                                        nullptr);
    if (n <= 0)
        return false;
    const int old_len = (int) result->length();
    result->resize(old_len + n - 1);
    const int rs = ::WideCharToMultiByte(CP_UTF8,
                                         0,
                                         wstr,
                                         -1,
                                         (char*) (result->data() + old_len),
                                         n,
                                         nullptr,
                                         nullptr);
    assert(((int) result->length()) == old_len + n - 1);
    return rs > 0;
#else
    return wstr_to_ascii(wstr, result);
#endif
}

NUT_API bool wstr_to_utf8(const std::wstring& wstr, std::string *result)
{
    assert(nullptr != result);
    return wstr_to_utf8(wstr.c_str(), result);
}

NUT_API bool ascii_to_utf8(const char *str, std::string *result)
{
    assert(nullptr != str && nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
    std::wstring tmp;
    if (!ascii_to_wstr(str, &tmp))
        return false;
    return wstr_to_utf8(tmp.c_str(), result);
#else
    *result += str;
    return true;
#endif
}

NUT_API bool ascii_to_utf8(const std::string& str, std::string *result)
{
    assert(nullptr != result);
    return ascii_to_utf8(str.c_str(), result);
}

NUT_API bool utf8_to_ascii(const char *str, std::string *result)
{
    assert(nullptr != str && nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
    std::wstring tmp;
    if (!utf8_to_wstr(str, &tmp))
        return false;
    return wstr_to_ascii(tmp.c_str(), result);
#else
    *result += str;
    return true;
#endif
}

NUT_API bool utf8_to_ascii(const std::string& str, std::string *result)
{
    assert(nullptr != result);
    return utf8_to_ascii(str.c_str(), result);
}

NUT_API char int_to_hex_char(int i)
{
    const char *hex_chars = "0123456789ABCDEF";
    if (0 <= i && i < 16)
        return hex_chars[i];
    return '\0';
}

NUT_API int hex_char_to_int(char c)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    else if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

NUT_API int xml_encode(const char *s, int len, std::string *result)
{
    assert(nullptr != s && nullptr != result);

    int ret = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        switch (s[i])
        {
        case '&':
            *result += "&amp;";
            ret += 5;
            break;

        case '"':
            *result += "&quot;";
            ret += 6;
            break;

        case '<':
            *result += "&lt;";
            ret += 4;
            break;

        case '>':
            *result += "&gt;";
            ret += 4;
            break;

        default:
            result->push_back(s[i]);
            ++ret;
            break;
        }
    }
    return ret;
}

NUT_API int xml_decode(const char *s, int len, std::string *result)
{
    assert(nullptr != s && nullptr != result);

    int ret = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        if ('&' == s[i])
        {
            if (0 == ::strncmp(s + i + 1, "amp;", 4))
            {
                result->push_back('&');
                i += 4;
                ++ret;
                continue;
            }
            else if (0 == ::strncmp(s + i + 1, "quot;", 5))
            {
                result->push_back('"');
                i += 5;
                ++ret;
                continue;
            }
            else if (0 == ::strncmp(s + i + 1, "lt;", 3))
            {
                result->push_back('<');
                i += 3;
                ++ret;
                continue;
            }
            else if (0 == ::strncmp(s + i + 1, "gt;", 3))
            {
                result->push_back('>');
                i += 3;
                ++ret;
                continue;
            }
        }

        result->push_back(s[i]);
        ++ret;
    }
    return ret;
}

NUT_API int url_encode(const char *s, int len, std::string *result)
{
    assert(nullptr != s && nullptr != result);

    int ret = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        const char c = s[i];
        if (('0' <= c && c <= '9') || ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z'))
        {
            result->push_back(c);
            ++ret;
            continue;
        }

        result->push_back('%');
        result->push_back(int_to_hex_char((c >> 4) & 0x0F));
        result->push_back(int_to_hex_char(c & 0x0F));
        ret += 3;
    }
    return ret;
}

NUT_API int url_decode(const char *s, int len, std::string *result)
{
    assert(nullptr != s && nullptr != result);

    int ret = 0;
    for (int i = 0; 0 != s[i] && (len < 0 || i < len); ++i)
    {
        if ('%' == s[i] && (len < 0 || i + 2 < len))
        {
            const int high = hex_char_to_int(s[i + 1]), low = hex_char_to_int(s[i + 2]);
            if (high >= 0 && low >= 0)
            {
                result->push_back((char) ((high << 4) | low));
                i += 2;
                ret += 2;
                continue;
            }
        }

        result->push_back(s[i]);
        ++ret;
    }
    return ret;
}

NUT_API int hex_encode(const void *data, size_t cb, std::string *result)
{
    assert(nullptr != data && nullptr != result);

    for (size_t i = 0; i < cb; ++i)
    {
        const uint8_t b = ((const uint8_t*) data)[i];
        result->push_back(int_to_hex_char((b >> 4) & 0x0F));
        result->push_back(int_to_hex_char(b & 0x0F));
    }
    return (int) cb * 2;
}

NUT_API int hex_decode(const char *s, int len, Array<uint8_t> *result)
{
    assert(nullptr != s && nullptr != result);

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
                result->push_back(v);
                ++ret;
            }
        }
    }
    return ret;
}

NUT_API int cstyle_encode(const char *s, int len, std::string *result)
{
    assert(nullptr != s && nullptr != result);

    int ret = 0;
    for (int i = 0; i < len || (len < 0 && 0 != s[i]); ++i)
    {
        const char c = s[i];
        switch (c)
        {
        case '\a':
            *result += "\\a";
            ret += 2;
            break;

        case '\b':
            *result += "\\b";
            ret += 2;
            break;

        case '\f':
            *result += "\\f";
            ret += 2;
            break;

        case '\n':
            *result += "\\n";
            ret += 2;
            break;

        case '\r':
            *result += "\\r";
            ret += 2;
            break;

        case '\t':
            *result += "\\t";
            ret += 2;
            break;

        case '\v':
            *result += "\\v";
            ret += 2;
            break;

        case '\\':
            *result += "\\\\";
            ret += 2;
            break;

        case '\"':
            *result += "\\\"";
            ret += 2;
            break;

        case '\'':
            *result += "\\\'";
            ret += 2;
            break;

        default:
            if (' ' <= c && c <= '~')
            {
                // 32 ~ 126 是可见字符
                result->push_back(c);
                ++ret;
            }
            else
            {
                *result += "\\x";
                result->push_back(int_to_hex_char((c >> 4) & 0x0f));
                result->push_back(int_to_hex_char(c & 0x0f));
                ret += 4;
            }
        }
    }
    return ret;
}

NUT_API int cstyle_decode(const char *s, int len, std::string *result)
{
    assert(nullptr != s && nullptr != result);

    int ret = 0;
    for (int i = 0; i < len || (len < 0 && 0 != s[i]); ++i)
    {
        char c = s[i];
        if ('\\' != c || i + 1 == len || 0 == s[i + 1])
        {
            result->push_back(c);
            ++ret;
            continue;
        }

        ++i;
        c = s[i];
        switch (c)
        {
        case 'a':
            result->push_back('\a');
            ++ret;
            break;

        case 'b':
            result->push_back('\b');
            ++ret;
            break;

        case 'f':
            result->push_back('\f');
            ++ret;
            break;

        case 'n':
            result->push_back('\n');
            ++ret;
            break;

        case 'r':
            result->push_back('\r');
            ++ret;
            break;

        case 't':
            result->push_back('\t');
            ++ret;
            break;

        case 'v':
            result->push_back('\v');
            ++ret;
            break;

        case '\\':
            result->push_back('\\');
            ++ret;
            break;

        case '\"':
            result->push_back('\"');
            ++ret;
            break;

        case '\'':
            result->push_back('\'');
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
            result->push_back(c);
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
            result->push_back(c);
            ++ret;
            break;

        default:
            result->push_back('\\');
            result->push_back(c);
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

NUT_API int base64_encode(const void *data, size_t cb, std::string *result)
{
    assert(nullptr != data && nullptr != result);

    const uint8_t *bytes = (const uint8_t*) data;
    for (size_t i = 0; i < cb; i += 3)
    {
        // every 3 bytes convert to 4 bytes, 6bit per byte

        // first 6 bit
        result->push_back(int_to_base64_char(bytes[i] >> 2));

        // second 6 bit
        if (i + 1 < cb)
        {
            result->push_back(int_to_base64_char(((bytes[i] & 0x03) << 4) ^ (bytes[i + 1] >> 4)));
        }
        else
        {
            result->push_back(int_to_base64_char(((bytes[i] & 0x03) << 4)));
            result->push_back('=');
            result->push_back('=');
            break;
        }

        // third 6 bit
        if (i + 2 < cb)
        {
            result->push_back(int_to_base64_char(((bytes[i + 1] & 0x0f) << 2) ^ (bytes[i + 2] >> 6)));
        }
        else
        {
            result->push_back(int_to_base64_char(((bytes[i + 1] & 0x0f) << 2)));
            result->push_back('=');
            break;
        }

        // last 6 bit
        result->push_back(int_to_base64_char(bytes[i + 2] & 0x3f));
    }
    return (int) ((cb + 2) / 3) * 4;
}

NUT_API int base64_decode(const char *s, int len, Array<uint8_t> *result)
{
    assert(nullptr != s && nullptr != result);

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
                result->push_back((uint8_t) ((buff[0] << 2) ^ ((buff[1] & 0x30) >> 4)));
                ++ret;

                // second byte
                if (64 == buff[2])
                    break;
                result->push_back((uint8_t) ((buff[1] << 4) ^ ((buff[2] & 0x3c) >> 2)));
                ++ret;

                // third byte
                if (64 == buff[3])
                    break;
                result->push_back((uint8_t) ((buff[2] << 6) ^ buff[3]));
                ++ret;

                // reset state
                state = 0;
            }
        }
    }
    return ret;
}

}

#if NUT_PLATFORM_CC_VC
#   pragma warning(pop)
#endif
