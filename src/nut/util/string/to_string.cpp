
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

/// long long
void ll_to_str(long long i, std::string *appended)
{
    assert(NULL != appended);

    const int BUF_SIZE = 60;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
#if defined(_MSC_VER) && _MSC_VER <= 1310
    // for VS2003 or ealier, "%lld" was not supported
    ::sprintf(buf, "%I64d", i);
    // ::ltoa(i, buf, 10);
#else
    ::sprintf(buf, "%lld", i);
#endif

    *appended += buf;
}

std::string ll_to_str(long long i)
{
    std::string ret;
    ll_to_str(i, &ret);
    return ret;
}

/// char
void c_to_str(char i, std::string *rs)
{
    ll_to_str((long long) i, rs);
}

std::string c_to_str(char i)
{
    return ll_to_str((long long) i);
}

/// unsigned char
void uc_to_str(unsigned char i, std::string *rs)
{
    ll_to_str((long long) i, rs);
}

std::string uc_to_str(unsigned char i)
{
    return ll_to_str((long long) i);
}

/// short
void s_to_str(short i, std::string *rs)
{
    ll_to_str((long long) i, rs);
}

std::string s_to_str(short i)
{
    return ll_to_str((long long) i);
}

/// unsigned short
void us_to_str(unsigned short i, std::string *rs)
{
    ll_to_str((long long) i, rs);
}

std::string us_to_str(unsigned short i)
{
    return ll_to_str((long long) i);
}

/// int
void i_to_str(int i, std::string *rs)
{
    ll_to_str((long long) i, rs);
}

std::string i_to_str(int i)
{
    return ll_to_str((long long) i);
}

/// unsigned int
void ui_to_str(unsigned int i, std::string *rs)
{
    ll_to_str((long long) i, rs);
}

std::string ui_to_str(unsigned int i)
{
    return ll_to_str((long long) i);
}

/// long
void l_to_str(long i, std::string *rs)
{
    ll_to_str((long long) i, rs);
}

std::string l_to_str(long i)
{
    return ll_to_str((long long) i);
}

/// unsigned long
void ul_to_str(unsigned long i, std::string *rs)
{
    ll_to_str((long long) i, rs);
}

std::string ul_to_str(unsigned long i)
{
    return ll_to_str((long long) i);
}

/// unsigned long long
void ull_to_str(unsigned long long i, std::string *rs)
{
    ll_to_str((long long) i, rs);
}

std::string ull_to_str(unsigned long long i)
{
    return ll_to_str((long long) i);
}

/// bool
void b_to_str(bool b, std::string *rs)
{
    assert(NULL != rs);
    *rs += (b ? "true" : "false");
}

std::string b_to_str(bool b)
{
    return (b ? "true" : "false");
}

/// double
void d_to_str(double d, std::string *rs)
{
    assert(NULL != rs);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf, "%lf", d);
    *rs += buf;
}

std::string d_to_str(double d)
{
    std::string ret;
    d_to_str(d, &ret);
    return ret;
}

/// float
void f_to_str(float f, std::string *rs)
{
    assert(NULL != rs);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
    ::sprintf(buf, "%f", f);
    *rs += buf;
}

std::string f_to_str(float f)
{
    std::string ret;
    f_to_str(f, &ret);
    return ret;
}

/// pointer
void ptr_to_str(const void *p, std::string *rs)
{
    assert(NULL != rs);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    ::memset(buf, 0, BUF_SIZE);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    ::sprintf(buf,"0x%p",p);    // windows: 0x002E459F
#else
    ::sprintf(buf,"%p",p);      // linux: 0x2e459f
#endif

    *rs += buf;
}

std::string ptr_to_str(const void *p)
{
    std::string ret;
    ptr_to_str(p, &ret);
    return ret;
}

/// memory block
void mem_to_str(const void *p, size_t align, size_t count, std::string *rs)
{
    assert(NULL != p && align > 0 && count > 0 && NULL != rs);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    unsigned char *current = (unsigned char*)p;

    for (size_t i = 0; i < count; ++i)
    {
        if (i % 0x08 == 0)
            ptr_to_str((const void*) current, rs);

        std::string single;
        for (size_t j = 0; j < align; ++j)
        {
            ::memset(buf, 0, BUF_SIZE);
            sprintf(buf, "%02X", *current);
            single = std::string(buf) + single;
            ++current;
        }
        *rs += " ";
        *rs += single;

        if ((i % 0x08 == 0x07) && (i != count - 1))
            *rs += "\n";
    }
}

std::string mem_to_str(const void *p, size_t align, size_t count)
{
    std::string ret;
    mem_to_str(p, align, count, &ret);
    return ret;
}

/// n-size string
void ns_to_str(const char *str, size_t len, std::string *rs, const char *fill_nil)
{
    assert(NULL != str && NULL != rs && NULL != fill_nil);

    for (size_t i = 0; i < len; ++i)
    {
        if (str[i] != '\0')
            rs->push_back(str[i]);
        else
            *rs += fill_nil;
    }
}

std::string ns_to_str(const char *str, size_t len, const char *fill_nil)
{
    std::string ret;
    ns_to_str(str, len, &ret, fill_nil);
    return ret;
}

/// string to long
long str_to_l(const std::string& s)
{
    long ret = 0;
    ::sscanf(s.c_str(), "%ld", &ret);
    return ret;
}

}

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif
