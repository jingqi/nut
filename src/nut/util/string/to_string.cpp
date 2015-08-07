
#include <assert.h>
#include <string.h>  /* for memset() */
#include <stdio.h>  /* for sprintf(), vsnprintf() and so on */
#include <stdarg.h> /* for va_start() */
#include <stdlib.h> /* for malloc() free() ltoa() */

#include <nut/platform/platform.h>

#include "to_string.h"

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

/// char
void char_to_str(char i, std::string *appended)
{
    llong_to_str((long long) i, appended);
}

std::string char_to_str(char i)
{
    return llong_to_str((long long) i);
}

/// unsigned char
void uchar_to_str(unsigned char i, std::string *appended)
{
    llong_to_str((long long) i, appended);
}

std::string uchar_to_str(unsigned char i)
{
    return llong_to_str((long long) i);
}

/// short
void short_to_str(short i, std::string *appended)
{
    llong_to_str((long long) i, appended);
}

std::string short_to_str(short i)
{
    return llong_to_str((long long) i);
}

/// unsigned short
void ushort_to_str(unsigned short i, std::string *appended)
{
    llong_to_str((long long) i, appended);
}

std::string ushort_to_str(unsigned short i)
{
    return llong_to_str((long long) i);
}

/// int
void int_to_str(int i, std::string *appended)
{
    llong_to_str((long long) i, appended);
}

std::string int_to_str(int i)
{
    return llong_to_str((long long) i);
}

/// unsigned int
void uint_to_str(unsigned int i, std::string *appended)
{
    llong_to_str((long long) i, appended);
}

std::string uint_to_str(unsigned int i)
{
    return llong_to_str((long long) i);
}

/// long
void long_to_str(long i, std::string *appended)
{
    llong_to_str((long long) i, appended);
}

std::string long_to_str(long i)
{
    return llong_to_str((long long) i);
}

/// unsigned long
void ulong_to_str(unsigned long i, std::string *appended)
{
    llong_to_str((long long) i, appended);
}

std::string ulong_to_str(unsigned long i)
{
    return llong_to_str((long long) i);
}

/// long long
void llong_to_str(long long i, std::string *appended)
{
    assert(NULL != appended);

    const int BUF_SIZE = 60;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
#if defined(_MSC_VER) && _MSC_VER <= 1310
    // for VS2003 or ealier, "%lld" was not supported
    ::sprintf(buf, "%I64d", i);
    // ::ltoa(i, buf, 10);
#elif defined(NUT_PLATFORM_OS_WINDOWS) && defined(NUT_PLATFORM_CC_MINGW)
    assert(sizeof(long long) == 8);
    ::sprintf(buf, "%I64d", i);
#else
    ::sprintf(buf, "%lld", i);
#endif

    *appended += buf;
}

std::string llong_to_str(long long i)
{
    std::string ret;
    llong_to_str(i, &ret);
    return ret;
}

/// unsigned long long
void ullong_to_str(unsigned long long i, std::string *appended)
{
    llong_to_str((long long) i, appended);
}

std::string ullong_to_str(unsigned long long i)
{
    return llong_to_str((long long) i);
}

/// bool
void bool_to_str(bool b, std::string *appended)
{
    assert(NULL != appended);
    *appended += (b ? "true" : "false");
}

std::string bool_to_str(bool b)
{
    return (b ? "true" : "false");
}

/// double
void double_to_str(double d, std::string *appended)
{
    assert(NULL != appended);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf, "%lf", d);
    *appended += buf;
}

std::string double_to_str(double d)
{
    std::string ret;
    double_to_str(d, &ret);
    return ret;
}

/// float
void float_to_str(float f, std::string *appended)
{
    assert(NULL != appended);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf, "%f", f);
    *appended += buf;
}

std::string float_to_str(float f)
{
    std::string ret;
    float_to_str(f, &ret);
    return ret;
}

/// pointer
void ptr_to_str(const void *p, std::string *appended)
{
    assert(NULL != appended);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    ::sprintf(buf,"0x%p",p);    // windows: 0x002E459F
#else
    ::sprintf(buf,"%p",p);      // linux: 0x2e459f
#endif

    *appended += buf;
}

std::string ptr_to_str(const void *p)
{
    std::string ret;
    ptr_to_str(p, &ret);
    return ret;
}

/// memory block
void mem_to_str(const void *p, size_t align, size_t count, std::string *appended)
{
    assert(NULL != p && align > 0 && count > 0 && NULL != appended);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    unsigned char *current = (unsigned char*)p;

    for (size_t i = 0; i < count; ++i)
    {
        if (i % 0x08 == 0)
            ptr_to_str((const void*) current, appended);

        std::string single;
        for (size_t j = 0; j < align; ++j)
        {
            ::memset(buf, 0, BUF_SIZE);
            sprintf(buf, "%02X", *current);
            single = std::string(buf) + single;
            ++current;
        }
        appended->push_back(' ');
        *appended += single;

        if ((i % 0x08 == 0x07) && (i != count - 1))
            appended->push_back('\n');
    }
}

std::string mem_to_str(const void *p, size_t align, size_t count)
{
    std::string ret;
    mem_to_str(p, align, count, &ret);
    return ret;
}

/// n-size string
void sub_cstr(const char *str, size_t len, std::string *appended, const char *fill_nil)
{
    assert(NULL != str && NULL != appended && NULL != fill_nil);

    for (size_t i = 0; i < len; ++i)
    {
        if (str[i] != '\0')
            appended->push_back(str[i]);
        else
            *appended += fill_nil;
    }
}

std::string sub_cstr(const char *str, size_t len, const char *fill_nil)
{
    std::string ret;
    sub_cstr(str, len, &ret, fill_nil);
    return ret;
}

/// string to long
long str_to_long(const std::string& s)
{
    long ret = 0;
    ::sscanf(s.c_str(), "%ld", &ret);
    return ret;
}

}

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif
