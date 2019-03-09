
#ifndef ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_
#define ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_

#include <assert.h>
#include <string>
#include <vector>

#include <nut/container/array.h>

#include "../../nut_config.h"


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
 * @return '\0' if invalid
 */
NUT_API char int_to_hex_char(int i, bool upper_case = true);

/**
 * @return -1 if invalid
 */
NUT_API int hex_char_to_int(char c);

/**
 * 将 & " < > 转换为 &amp; &quot; &lt; &gt;
 */
NUT_API std::string xml_encode(const char *s, ssize_t len);
NUT_API std::string xml_decode(const char *s, ssize_t len);

/**
 * url 转义，例如 "http://search?ab%20c%20"
 */
NUT_API std::string url_encode(const char *s, ssize_t len);
NUT_API std::string url_decode(const char *s, ssize_t len);

/**
 * 16进制编码转换，例如 "4FC012B" 等
 */
NUT_API std::string hex_encode(const void *data, size_t cb, bool upper_case = true);
NUT_API Array<uint8_t> hex_decode(const char *s, ssize_t len);

/**
 * C 常量风格编码转换
 */
NUT_API std::string cstyle_encode(const char *s, ssize_t len);
NUT_API std::string cstyle_decode(const char *s, ssize_t len);

/**
 * base64 编码
 */
NUT_API std::string base64_encode(const void *data, size_t cb);
NUT_API Array<uint8_t> base64_decode(const char *s, ssize_t len);

}

#endif /* head file guarder */
