
#ifndef ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_
#define ___HEADFILE___8BC3081E_4374_470D_9E66_CC7F414ED9B7_

#include <assert.h>
#include <string>
#include <vector>

namespace nut
{

/**
 * 用整个字符串来分割字符串
 */
void str_split(const char *str, const char *sep_str, std::vector<std::string> *appended, bool ignore_empty = false);
void str_split(const wchar_t *str, const wchar_t *sep_str, std::vector<std::wstring> *appended, bool ignore_empty = false);

/**
 * @param sep_chars
 *      该字符串中的每一个字符都是分割字符
 */
void chr_split(const char *str, const char *sep_chars, std::vector<std::string> *appended, bool ignore_empty = false);
void chr_split(const wchar_t *str, const wchar_t *sep_chars, std::vector<std::wstring> *appended, bool ignore_empty = false);

/**
 * split the string
 */
void chr_split(const char *str, char c, std::vector<std::string> *appended, bool ignore_empty = false);
void chr_split(const wchar_t *str, wchar_t c, std::vector<std::wstring> *appended, bool ignore_empty = false);

void format(std::string *appended, const char *fmt, ...);
void format(std::wstring *appended, const wchar_t *fmt, ...);

/* 去除首尾空白 */
void trim(const char *str, std::string *appended, const char *blanks = " \t\r\n");
void trim(const wchar_t *str, std::wstring *appended, const wchar_t *blanks = L" \t\r\n");

/** 去除左边空白 */
void ltrim(const char *str, std::string *appended, const char *blanks = " \t\r\n");
void ltrim(const wchar_t *str, std::wstring *appended, const wchar_t *blanks = L" \t\r\n");

/** 去除右边空白 */
void rtrim(const char *str, std::string *appended, const char *blanks = " \t\r\n");
void rtrim(const wchar_t *str, std::wstring *appended, const wchar_t *blanks = L" \t\r\n");

/** 忽略大小写的字符串比较 */
int stricmp(const char *str1, const char *str2);
int stricmp(const wchar_t *str1, const char *str2);

int strincmp(const char *str1, const char *str2, size_t n);
int strincmp(const wchar_t *str1, const wchar_t *str2, size_t n);

/**
 * 匹配字符串的开头
 */
bool starts_with(const char *s, const char *head);
bool starts_with(const wchar_t *s, const wchar_t *head);

/**
 * 匹配字符串的结尾
 */
bool ends_with(const char *s, const char *tail);
bool ends_with(const wchar_t *s, const wchar_t *tail);

/**
 * 字符编码转换
 */
bool ascii_to_wstr(const char *str, std::wstring *appended);
bool wstr_to_ascii(const wchar_t *wstr, std::string *appended);

bool utf8_to_wstr(const char *str, std::wstring *appended);
bool wstr_to_utf8(const wchar_t *wstr, std::string *appended);

bool ascii_to_utf8(const char *str, std::string *appended);
bool utf8_to_ascii(const char *str, std::string *appended);

/**
 * @return '\0' if invalid
 */
char int_to_hex_char(int i);

/**
 * @return -1 if invalid
 */
int hex_char_to_int(char c);

int xml_encode(const char *s, int len, std::string *appended);
int xml_decode(const char *s, int len, std::string *appended);

int url_encode(const char *s, int len, std::string *appended);
int url_decode(const char *s, int len, std::string *appended);

int hex_encode(const void *data, size_t cb, std::string *appended);
int hex_decode(const char *s, int len, std::vector<uint8_t> *appended);

int base64_encode(const void *data, size_t cb, std::string *appended);
int base64_decode(const char *s, int len, std::vector<uint8_t> *appended);

}

#endif /* head file guarder */
