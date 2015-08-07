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

/// char
void char_to_str(char i, std::string *appended);
std::string char_to_str(char i);

/// unsigned char
void uchar_to_str(unsigned char i, std::string *appended);
std::string uchar_to_str(unsigned char i);

/// short
void short_to_str(short i, std::string *rs);
std::string short_to_str(short i);

/// unsigned short
void ushort_to_str(unsigned short i, std::string *appended);
std::string ushort_to_str(unsigned short i);

/// int
void int_to_str(int i, std::string *appended);
std::string int_to_str(int i);

/// unsigned int
void uint_to_str(unsigned int i, std::string *appended);
std::string uint_to_str(unsigned int i);

/// long
void long_to_str(long i, std::string *appended);
std::string long_to_str(long i);

/// unsigned long
void ulong_to_str(unsigned long i, std::string *appended);
std::string ulong_to_str(unsigned long i);

/// long long
void llong_to_str(long long i, std::string *appended);
std::string llong_to_str(long long i);

/// unsigned long long
void ullong_to_str(unsigned long long i, std::string *appended);
std::string ullong_to_str(unsigned long long i);

/// bool
void bool_to_str(bool b, std::string *appended);
std::string bool_to_str(bool b);

/// double
void double_to_str(double d, std::string *appended);
std::string double_to_str(double d);

/// float
void float_to_str(float f, std::string *appended);
std::string float_to_str(float f);

/// pointer
void ptr_to_str(const void *p, std::string *appended);
std::string ptr_to_str(const void *p);

/// memory block
void mem_to_str(const void *p, size_t align, size_t count, std::string *appended);
std::string mem_to_str(const void *p, size_t align, size_t count);

/// n-size string
void sub_cstr(const char *str, size_t len, std::string *appended, const char *fill_nil = "\\0");
std::string sub_cstr(const char *str, size_t len, const char *fill_nil = "\\0");

/// string to long
long str_to_long(const std::string& s);

}

#endif
