
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
void str_split(const std::string &str, const std::string &sstr, std::vector<std::string> *appended, bool ignore_empty = false);
void str_split(const std::wstring &str, const std::wstring &sstr, std::vector<std::wstring> *appended, bool ignore_empty = false);

/**
 * @param sstr
 *      该字符串中的每一个字符都是分割字符
 */
void chr_split(const std::string &str, const std::string &sstr, std::vector<std::string> *appended, bool ignore_empty = false);
void chr_split(const std::wstring &str, const std::wstring &sstr, std::vector<std::wstring> *appended, bool ignore_empty = false);

/**
 * split the string
 */
void chr_split(const std::string &str, char c, std::vector<std::string> *appended, bool ignore_empty = false);
void chr_split(const std::wstring &str, wchar_t c, std::vector<std::wstring> *appended, bool ignore_empty = false);

void format(std::string *appended, const char *fmt, ...);
void format(std::wstring *appended, const wchar_t *fmt, ...);
std::string format(const char *fmt, ...);
std::wstring format(const wchar_t *fmt, ...);

/* 去除首尾空白 */
void trim(const std::string& str, std::string *appended, const std::string& blanks = " \t\r\n");
void trim(const std::wstring& str, std::wstring *appended, const std::wstring& blanks = L" \t\r\n");
std::string trim(const std::string& str, const std::string& blanks = " \t\r\n");
std::wstring trim(const std::wstring& str, const std::wstring& blanks = L" \t\r\n");

/** 去除左边空白 */
void ltrim(const std::string& str, std::string *appended, const std::string& blanks = " \t\r\n");
void ltrim(const std::wstring& str, std::wstring *appended, const std::wstring& blanks = L" \t\r\n");
std::string ltrim(const std::string& str, const std::string& blanks = " \t\r\n");
std::wstring ltrim(const std::wstring& str, const std::wstring& blanks = L" \t\r\n");

/** 去除右边空白 */
void rtrim(const std::string& str, std::string *appended, const std::string& blanks = " \t\r\n");
void rtrim(const std::wstring& str, std::wstring *appended, const std::wstring& blanks = L" \t\r\n");
std::string rtrim(const std::string& str, const std::string& blanks = " \t\r\n");
std::wstring rtrim(const std::wstring& str, const std::wstring& blanks = L" \t\r\n");

/** 忽略大小写的字符串比较 */
bool strieq(const std::string& str1, const std::string& str2);
bool strieq(const std::wstring& str1, const std::wstring& str2);

int stricmp(const char *str1, const char *str2);
int stricmp(const wchar_t *str1, const char *str2);
int strincmp(const char *str1, const char *str2, size_t n);
int strincmp(const wchar_t *str1, const wchar_t *str2, size_t n);

/**
 * 匹配字符串的开头
 */
bool starts_with(const std::string& s, const std::string& head);
bool starts_with(const std::wstring& s, const std::wstring& head);

/**
 * 匹配字符串的结尾
 */
bool ends_with(const std::string& s, const std::string& tail);
bool ends_with(const std::wstring& s, const std::wstring& tail);

bool ascii_to_wstr(const char *str, std::wstring *appended);
bool ascii_to_wstr(const std::string& str, std::wstring *appended);
std::wstring ascii_to_wstr(const char *str);
std::wstring ascii_to_wstr(const std::string& str);

bool wstr_to_ascii(const wchar_t *wstr, std::string *appended);
bool wstr_to_ascii(const std::wstring& wstr, std::string *appended);
std::string wstr_to_ascii(const wchar_t *wstr);
std::string wstr_to_ascii(const std::wstring& wstr);

bool utf8_to_wstr(const char *str, std::wstring *appended);
bool utf8_to_wstr(const std::string& str, std::wstring *appended);
std::wstring utf8_to_wstr(const char *str);
std::wstring utf8_to_wstr(const std::string& str);

bool wstr_to_utf8(const wchar_t *wstr, std::string *appended);
bool wstr_to_utf8(const std::wstring& wstr, std::string *appended);
std::string wstr_to_utf8(const wchar_t *wstr);
std::string wstr_to_utf8(const std::wstring& wstr);

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
