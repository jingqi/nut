/**
 * 扩展 std::to_string()
 */

#ifndef ___HEADFILE_791BEBBF_7FE0_40BA_AC96_703F0C8F38C1_
#define ___HEADFILE_791BEBBF_7FE0_40BA_AC96_703F0C8F38C1_

#include <assert.h>
#include <string>
#include <vector>

#include "../../nut_config.h"


namespace nut
{

// char
NUT_API std::string char_to_str(char i);

// unsigned char
NUT_API std::string uchar_to_str(unsigned char i);

// short
NUT_API std::string short_to_str(short i);

// unsigned short
NUT_API std::string ushort_to_str(unsigned short i);

// int
NUT_API std::string int_to_str(int i);

// unsigned int
NUT_API std::string uint_to_str(unsigned int i);

// long
NUT_API std::string long_to_str(long i);

// unsigned long
NUT_API std::string ulong_to_str(unsigned long i);

// long long
NUT_API std::string llong_to_str(long long i);

// unsigned long long
NUT_API std::string ullong_to_str(unsigned long long i);

// bool
NUT_API std::string bool_to_str(bool b);

// double
NUT_API std::string double_to_str(double d);

// float
NUT_API std::string float_to_str(float f);

// pointer
NUT_API std::string ptr_to_str(const void *p);

// memory block
NUT_API std::string mem_to_str(const void *p, size_t align, size_t count);

// n-size string
NUT_API std::string sub_cstr(const char *str, size_t len, const char *fill_nil = "\\0");

// string to long
NUT_API long str_to_long(const std::string& s);
NUT_API double str_to_double(const std::string& s);

}

#endif
