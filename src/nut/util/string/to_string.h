/**
 * 扩展 std::to_string()
 */

#ifndef ___HEADFILE_791BEBBF_7FE0_40BA_AC96_703F0C8F38C1_
#define ___HEADFILE_791BEBBF_7FE0_40BA_AC96_703F0C8F38C1_

#include <assert.h>
#include <string>
#include <vector>

#include <nut/nut_config.h>

namespace nut
{

// char
NUT_API void char_to_str(char i, std::string *appended);
NUT_API std::string char_to_str(char i);

// unsigned char
NUT_API void uchar_to_str(unsigned char i, std::string *appended);
NUT_API std::string uchar_to_str(unsigned char i);

// short
NUT_API void short_to_str(short i, std::string *appended);
NUT_API std::string short_to_str(short i);

// unsigned short
NUT_API void ushort_to_str(unsigned short i, std::string *appended);
NUT_API std::string ushort_to_str(unsigned short i);

// int
NUT_API void int_to_str(int i, std::string *appended);
NUT_API std::string int_to_str(int i);

// unsigned int
NUT_API void uint_to_str(unsigned int i, std::string *appended);
NUT_API std::string uint_to_str(unsigned int i);

// long
NUT_API void long_to_str(long i, std::string *appended);
NUT_API std::string long_to_str(long i);

// unsigned long
NUT_API void ulong_to_str(unsigned long i, std::string *appended);
NUT_API std::string ulong_to_str(unsigned long i);

// long long
NUT_API void llong_to_str(long long i, std::string *appended);
NUT_API std::string llong_to_str(long long i);

// unsigned long long
NUT_API void ullong_to_str(unsigned long long i, std::string *appended);
NUT_API std::string ullong_to_str(unsigned long long i);

// bool
NUT_API void bool_to_str(bool b, std::string *appended);
NUT_API std::string bool_to_str(bool b);

// double
NUT_API void double_to_str(double d, std::string *appended);
NUT_API std::string double_to_str(double d);

// float
NUT_API void float_to_str(float f, std::string *appended);
NUT_API std::string float_to_str(float f);

// pointer
NUT_API void ptr_to_str(const void *p, std::string *appended);
NUT_API std::string ptr_to_str(const void *p);

// memory block
NUT_API void mem_to_str(const void *p, size_t align, size_t count, std::string *appended);
NUT_API std::string mem_to_str(const void *p, size_t align, size_t count);

// n-size string
NUT_API void sub_cstr(const char *str, size_t len, std::string *appended, const char *fill_nil = "\\0");
NUT_API std::string sub_cstr(const char *str, size_t len, const char *fill_nil = "\\0");

// string to long
NUT_API long str_to_long(const std::string& s);
NUT_API double str_to_double(const std::string& s);

}

#endif
