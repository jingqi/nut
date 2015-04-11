/**
 * 扩展 std::to_string()
 */

#ifndef ___HEADFILE_791BEBBF_7FE0_40BA_AC96_703F0C8F38C1_
#define ___HEADFILE_791BEBBF_7FE0_40BA_AC96_703F0C8F38C1_

#include <assert.h>
#include <string>
#include <vector>

namespace nut
{

/// long long
void ll_to_str(long long i, std::string *appended);
std::string ll_to_str(long long i);

/// char
void c_to_str(char i, std::string *rs);
std::string c_to_str(char i);

/// unsigned char
void uc_to_str(unsigned char i, std::string *rs);
std::string uc_to_str(unsigned char i);

/// short
void s_to_str(short i, std::string *rs);
std::string s_to_str(short i);

/// unsigned short
void us_to_str(unsigned short i, std::string *rs);
std::string us_to_str(unsigned short i);

/// int
void i_to_str(int i, std::string *rs);
std::string i_to_str(int i);

/// unsigned int
void ui_to_str(unsigned int i, std::string *rs);
std::string ui_to_str(unsigned int i);

/// long
void l_to_str(long i, std::string *rs);
std::string l_to_str(long i);

/// unsigned long
void ul_to_str(unsigned long i, std::string *rs);
std::string ul_to_str(unsigned long i);

/// unsigned long long
void ull_to_str(unsigned long long i, std::string *rs);
std::string ull_to_str(unsigned long long i);

/// bool
void b_to_str(bool b, std::string *rs);
std::string b_to_str(bool b);

/// double
void d_to_str(double d, std::string *rs);
std::string d_to_str(double d);

/// float
void f_to_str(float f, std::string *rs);
std::string f_to_str(float f);

/// pointer
void ptr_to_str(const void *p, std::string *rs);
std::string ptr_to_str(const void *p);

/// memory block
void mem_to_str(const void *p, size_t align, size_t count, std::string *rs);
std::string mem_to_str(const void *p, size_t align, size_t count);

/// n-size string
void ns_to_str(const char *str, size_t len, std::string *rs, const char *fill_nil = "\\0");
std::string ns_to_str(const char *str, size_t len, const char *fill_nil = "\\0");

/// string to long
long str_to_l(const std::string& s);

}

#endif
