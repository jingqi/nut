
#ifndef ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_
#define ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_

#include <assert.h>
#include <string>
#include <vector>

#include "../../nut_config.h"
#include "../../platform/int_type.h" // for ssize_t in Windows VC


#define __NUT_DEFAULT_BLANKS " \t\r\n"
#define __NUT_DEFAULT_BLANKSW L" \t\r\n"

namespace nut
{

/**
 * 用整个字符串来分割字符串
 */
NUT_API std::vector<std::string> str_split(
    const char *str, const char *sep_str, bool ignore_empty = false);

NUT_API std::vector<std::string> str_split(
    const std::string& str, const std::string& sep_str,
    bool ignore_empty = false);

NUT_API std::vector<std::wstring> str_split(
    const wchar_t *str, const wchar_t *sep_str, bool ignore_empty = false);

NUT_API std::vector<std::wstring> str_split(
    const std::wstring& str, const std::wstring& sep_str,
    bool ignore_empty = false);

/**
 * @param sep_chars
 *      该字符串中的每一个字符都是分割字符
 */
NUT_API std::vector<std::string> chr_split(
    const char *str, const char *sep_chars, bool ignore_empty = false);

NUT_API std::vector<std::string> chr_split(
    const std::string& str, const std::string& sep_chars,
    bool ignore_empty = false);

NUT_API std::vector<std::wstring> chr_split(
    const wchar_t *str, const wchar_t *sep_chars,
    bool ignore_empty = false);

NUT_API std::vector<std::wstring> chr_split(
    const std::wstring& str, const std::wstring& sep_chars,
    bool ignore_empty = false);

/**
 * Split the string
 */
NUT_API std::vector<std::string> chr_split(
    const char *str, char c, bool ignore_empty = false);

NUT_API std::vector<std::string> chr_split(
    const std::string& str, char c, bool ignore_empty = false);

NUT_API std::vector<std::wstring> chr_split(
    const wchar_t *str, wchar_t c, bool ignore_empty = false);

NUT_API std::vector<std::wstring> chr_split(
    const std::wstring& str, wchar_t c, bool ignore_empty = false);

/**
 * 安全格式化
 *
 * @return >=0, 格式化后的字符串长度(可能被截断格式化)
 */
NUT_API ssize_t safe_snprintf(char *buf, size_t buf_size, const char *fmt, ...);
NUT_API ssize_t safe_snprintf(wchar_t *buf, size_t buf_size, const wchar_t *fmt, ...);

/** 格式化 */
NUT_API std::string format(const char *fmt, ...);
NUT_API std::wstring format(const wchar_t *fmt, ...);

/** 去除首尾空白 */
NUT_API std::string trim(const char *str, const char *blanks = __NUT_DEFAULT_BLANKS);
NUT_API std::string trim(const std::string& str, const std::string& blacks = __NUT_DEFAULT_BLANKS);
NUT_API std::wstring trim(const wchar_t *str, const wchar_t *blanks = __NUT_DEFAULT_BLANKSW);
NUT_API std::wstring trim(const std::wstring& str, const std::wstring& blacks = __NUT_DEFAULT_BLANKSW);

/** 去除左边空白 */
NUT_API std::string ltrim(const char *str, const char *blanks = __NUT_DEFAULT_BLANKS);
NUT_API std::string ltrim(const std::string& str, const std::string& blanks = __NUT_DEFAULT_BLANKS);
NUT_API std::wstring ltrim(const wchar_t *str, const wchar_t *blanks = __NUT_DEFAULT_BLANKSW);
NUT_API std::wstring ltrim(const std::wstring& str, const std::wstring& blanks = __NUT_DEFAULT_BLANKSW);

/** 去除右边空白 */
NUT_API std::string rtrim(const char *str, const char *blanks = __NUT_DEFAULT_BLANKS);
NUT_API std::string rtrim(const std::string& str, const std::string& blanks = __NUT_DEFAULT_BLANKS);
NUT_API std::wstring rtrim(const wchar_t *str, const wchar_t *blanks = __NUT_DEFAULT_BLANKSW);
NUT_API std::wstring rtrim(const std::wstring& str, const std::wstring& blanks = __NUT_DEFAULT_BLANKSW);

/** 忽略大小写的字符串比较 */
NUT_API int chricmp(int c1, int c2);
NUT_API int stricmp(const char *str1, const char *str2);
NUT_API int stricmp(const std::string& str1, const std::string& str2);
NUT_API int stricmp(const wchar_t *str1, const wchar_t *str2);
NUT_API int stricmp(const std::wstring& str1, const std::wstring& str2);

NUT_API int strincmp(const char *str1, const char *str2, size_t n);
NUT_API int strincmp(const std::string& str1, const std::string& str2, size_t n);
NUT_API int strincmp(const wchar_t *str1, const wchar_t *str2, size_t n);
NUT_API int strincmp(const std::wstring& str1, const std::wstring& str2, size_t n);

/**
 * 匹配字符串的开头
 */
NUT_API bool starts_with(const char *s, const char *head);
NUT_API bool starts_with(const std::string& s, const std::string& head);
NUT_API bool starts_with(const wchar_t *s, const wchar_t *head);
NUT_API bool starts_with(const std::wstring& s, const std::wstring& head);

/**
 * 匹配字符串的结尾
 */
NUT_API bool ends_with(const char *s, const char *tail);
NUT_API bool ends_with(const std::string& s, const std::string& tail);
NUT_API bool ends_with(const wchar_t *s, const wchar_t *tail);
NUT_API bool ends_with(const std::wstring& s, const std::wstring& tail);

/**
 * 字符编码转换
 */
NUT_API std::wstring ascii_to_wstr(const char *str);
NUT_API std::wstring ascii_to_wstr(const std::string& str);
NUT_API std::string wstr_to_ascii(const wchar_t *wstr);
NUT_API std::string wstr_to_ascii(const std::wstring& wstr);

NUT_API std::wstring utf8_to_wstr(const char *str);
NUT_API std::wstring utf8_to_wstr(const std::string& str);
NUT_API std::string wstr_to_utf8(const wchar_t *wstr);
NUT_API std::string wstr_to_utf8(const std::wstring& wstr);

NUT_API std::string ascii_to_utf8(const char *str);
NUT_API std::string ascii_to_utf8(const std::string& str);
NUT_API std::string utf8_to_ascii(const char *str);
NUT_API std::string utf8_to_ascii(const std::string& str);

/**
 * Convert 1 to '1', 15 to 'f' or 'F'
 *
 * @return '\0' if invalid, or return [0-9a-zA-Z]
 */
constexpr char int_to_char(int i, bool upper_case = true)
{
    return (0 <= i && i < 10) ? ('0' + i) :
        ((10 <= i && i < 36) ? ((upper_case ? 'A' : 'a') + (i - 10)) :
         '\0');
}

constexpr char int_to_wchar(int i, bool upper_case = true)
{
    return (0 <= i && i < 10) ? (L'0' + i) :
        ((10 <= i && i < 36) ? ((upper_case ? L'A' : L'a') + (i - 10)) :
         L'\0');
}

/**
 * Convert '1' to 1, 'f' or 'F' to 15
 *
 * @param c should be [0-9a-zA-Z]
 * @return -1 if invalid
 */
constexpr int char_to_int(char c)
{
    return ('0' <= c && c <= '9') ? (c - '0') :
        (('a' <= c && c <= 'z') ? (c - 'a' + 10) :
         (('A' <= c && c <= 'Z') ? (c - 'A' + 10) : -1));
}

constexpr int char_to_int(wchar_t c)
{
    return (L'0' <= c && c <= L'9') ? (c - L'0') :
        ((L'a' <= c && c <= L'z') ? (c - L'a' + 10) :
         ((L'A' <= c && c <= L'Z') ? (c - L'A' + 10) : -1));
}

/**
 * 将 & " < > 转换为 &amp; &quot; &lt; &gt;
 */
NUT_API std::string xml_encode(const char *s, ssize_t len = -1);
NUT_API std::string xml_encode(const std::string& s);
NUT_API std::string xml_decode(const char *s, ssize_t len = -1);
NUT_API std::string xml_decode(const std::string& s);

/**
 * url 转义，例如 "http://search?ab%20c%20"
 */
NUT_API std::string url_encode(const char *s, ssize_t len = -1);
NUT_API std::string url_encode(const std::string& s);
NUT_API std::string url_decode(const char *s, ssize_t len = -1);
NUT_API std::string url_decode(const std::string& s);

/**
 * 16进制编码转换，例如 "4FC012B" 等
 */
NUT_API std::string hex_encode(const void *data, size_t cb, bool upper_case = true);
NUT_API std::vector<uint8_t> hex_decode(const char *s, ssize_t len = -1);
NUT_API std::vector<uint8_t> hex_decode(const std::string& s);

/**
 * C 常量风格编码转换
 */
NUT_API std::string cstyle_encode(const char *s, ssize_t len = -1);
NUT_API std::string cstyle_encode(const std::string& s);
NUT_API std::string cstyle_decode(const char *s, ssize_t len = -1);
NUT_API std::string cstyle_decode(const std::string& s);

/**
 * base64 编码
 */
NUT_API std::string base64_encode(const void *data, size_t cb);
NUT_API std::vector<uint8_t> base64_decode(const char *s, ssize_t len = -1);
NUT_API std::vector<uint8_t> base64_decode(const std::string& s);

}

#endif /* head file guarder */
