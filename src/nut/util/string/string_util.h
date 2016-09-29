
#ifndef ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_
#define ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_

#include <assert.h>
#include <string>
#include <vector>

#include <nut/nut_config.h>
#include <nut/container/array.h>

#define __NUT_DEFAULT_BLANKS " \t\r\n"
#define __NUT_DEFAULT_BLANKSW L" \t\r\n"

namespace nut
{

/**
 * 用整个字符串来分割字符串
 */
NUT_API void str_split(const char *str, const char *sep_str,
                       std::vector<std::string> *result, bool ignore_empty = false);
NUT_API void str_split(const std::string& str, const std::string& sep_str,
                       std::vector<std::string> &result, bool ignore_empty = false);
NUT_API void str_split(const wchar_t *str, const wchar_t *sep_str,
                       std::vector<std::wstring> *result, bool ignore_empty = false);
NUT_API void str_split(const std::wstring& str, const std::wstring& sep_str,
                       std::vector<std::wstring> *result, bool ignore_empty = false);

/**
 * @param sep_chars
 *      该字符串中的每一个字符都是分割字符
 */
NUT_API void chr_split(const char *str, const char *sep_chars,
                       std::vector<std::string> *result, bool ignore_empty = false);
NUT_API void chr_split(const std::string& str, const std::string& sep_chars,
                       std::vector<std::string> *result, bool ignore_empty = false);
NUT_API void chr_split(const wchar_t *str, const wchar_t *sep_chars,
                       std::vector<std::wstring> *result, bool ignore_empty = false);
NUT_API void chr_split(const std::wstring& str, const std::wstring& sep_chars,
                       std::vector<std::wstring> *result, bool ignore_empty = false);

/**
 * Split the string
 */
NUT_API void chr_split(const char *str, char c, std::vector<std::string> *result,
                       bool ignore_empty = false);
NUT_API void chr_split(const std::string& str, char c, std::vector<std::string> *result,
                       bool ignore_empty = false);
NUT_API void chr_split(const wchar_t *str, wchar_t c, std::vector<std::wstring> *result,
                       bool ignore_empty = false);
NUT_API void chr_split(const std::wstring& str, wchar_t c, std::vector<std::wstring> *result,
                       bool ignore_empty = false);

NUT_API void format(std::string *result, const char *fmt, ...);
NUT_API void format(std::wstring *result, const wchar_t *fmt, ...);
NUT_API std::string format(const char *fmt, ...);
NUT_API std::wstring format(const wchar_t *fmt, ...);

/** 去除首尾空白 */
NUT_API void trim(const char *str, std::string *result,
                  const char *blanks = __NUT_DEFAULT_BLANKS);
NUT_API void trim(const std::string& str, std::string *result,
                  const std::string& blanks = __NUT_DEFAULT_BLANKS);
NUT_API void trim(const wchar_t *str, std::wstring *result,
                  const wchar_t *blanks = __NUT_DEFAULT_BLANKSW);
NUT_API void trim(const std::wstring& str, std::wstring *result,
                  const std::wstring& blanks = __NUT_DEFAULT_BLANKSW);
NUT_API std::string trim(const char *str, const char *blanks = __NUT_DEFAULT_BLANKS);
NUT_API std::string trim(const std::string& str, const std::string& blacks = __NUT_DEFAULT_BLANKS);
NUT_API std::wstring trim(const wchar_t *str, const wchar_t *blanks = __NUT_DEFAULT_BLANKSW);
NUT_API std::wstring trim(const std::wstring& str, const std::wstring& blacks = __NUT_DEFAULT_BLANKSW);

/** 去除左边空白 */
NUT_API void ltrim(const char *str, std::string *result,
                   const char *blanks = __NUT_DEFAULT_BLANKS);
NUT_API void ltrim(const std::string& str, std::string *result,
                   const std::string& blanks = __NUT_DEFAULT_BLANKS);
NUT_API void ltrim(const wchar_t *str, std::wstring *result,
                   const wchar_t *blanks = __NUT_DEFAULT_BLANKSW);
NUT_API void ltrim(const std::wstring& str, std::wstring *result,
                   const std::wstring& blanks = __NUT_DEFAULT_BLANKSW);
NUT_API std::string ltrim(const char *str, const char *blanks = __NUT_DEFAULT_BLANKS);
NUT_API std::string ltrim(const std::string& str, const std::string& blanks = __NUT_DEFAULT_BLANKS);
NUT_API std::wstring ltrim(const wchar_t *str, const wchar_t *blanks = __NUT_DEFAULT_BLANKSW);
NUT_API std::wstring ltrim(const std::wstring& str, const std::wstring& blanks = __NUT_DEFAULT_BLANKSW);

/** 去除右边空白 */
NUT_API void rtrim(const char *str, std::string *result,
                   const char *blanks = __NUT_DEFAULT_BLANKS);
NUT_API void rtrim(const std::string& str, std::string *result,
                   const std::string& blanks = __NUT_DEFAULT_BLANKS);
NUT_API void rtrim(const wchar_t *str, std::wstring *result,
                   const wchar_t *blanks = __NUT_DEFAULT_BLANKSW);
NUT_API void rtrim(const std::wstring& str, std::wstring *result,
                   const std::wstring& blanks = __NUT_DEFAULT_BLANKSW);
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
NUT_API bool ascii_to_wstr(const char *str, std::wstring *result);
NUT_API bool ascii_to_wstr(const std::string& str, std::wstring *result);
NUT_API bool wstr_to_ascii(const wchar_t *wstr, std::string *result);
NUT_API bool wstr_to_ascii(const std::wstring& wstr, std::string *result);

NUT_API bool utf8_to_wstr(const char *str, std::wstring *result);
NUT_API bool utf8_to_wstr(const std::string& str, std::wstring *result);
NUT_API bool wstr_to_utf8(const wchar_t *wstr, std::string *result);
NUT_API bool wstr_to_utf8(const std::wstring& wstr, std::string *result);

NUT_API bool ascii_to_utf8(const char *str, std::string *result);
NUT_API bool ascii_to_utf8(const std::string& str, std::string *result);
NUT_API bool utf8_to_ascii(const char *str, std::string *result);
NUT_API bool utf8_to_ascii(const std::string& str, std::string *result);

/**
 * @return '\0' if invalid
 */
NUT_API char int_to_hex_char(int i);

/**
 * @return -1 if invalid
 */
NUT_API int hex_char_to_int(char c);

NUT_API int xml_encode(const char *s, int len, std::string *result);
NUT_API int xml_decode(const char *s, int len, std::string *result);

NUT_API int url_encode(const char *s, int len, std::string *result);
NUT_API int url_decode(const char *s, int len, std::string *result);

NUT_API int hex_encode(const void *data, size_t cb, std::string *result);
NUT_API int hex_decode(const char *s, int len, Array<uint8_t> *result);

NUT_API int cstyle_encode(const char *s, int len, std::string *result);
NUT_API int cstyle_decode(const char *s, int len, std::string *result);

NUT_API int base64_encode(const void *data, size_t cb, std::string *result);
NUT_API int base64_decode(const char *s, int len, Array<uint8_t> *result);

}

#endif /* head file guarder */
