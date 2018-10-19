
#include <assert.h>
#include <string.h>  /* for memset() */
#include <stdio.h>  /* for sprintf(), vsnprintf() and so on */
#include <stdarg.h> /* for va_start() */
#include <stdlib.h> /* for malloc() free() ltoa() */

#include <nut/platform/platform.h>

#include "to_string.h"
#include "string_utils.h"


#if NUT_PLATFORM_CC_VC
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

/// char
NUT_API std::string char_to_str(char i)
{
    return llong_to_str((long long) i);
}

/// unsigned char
NUT_API std::string uchar_to_str(unsigned char i)
{
    return llong_to_str((long long) i);
}

/// short
NUT_API std::string short_to_str(short i)
{
    return llong_to_str((long long) i);
}

/// unsigned short
NUT_API std::string ushort_to_str(unsigned short i)
{
    return llong_to_str((long long) i);
}

/// int
NUT_API std::string int_to_str(int i)
{
    return llong_to_str((long long) i);
}

/// unsigned int
NUT_API std::string uint_to_str(unsigned int i)
{
    return llong_to_str((long long) i);
}

/// long
NUT_API std::string long_to_str(long i)
{
    return llong_to_str((long long) i);
}

/// unsigned long
NUT_API std::string ulong_to_str(unsigned long i)
{
    return llong_to_str((long long) i);
}

/// long long
NUT_API std::string llong_to_str(long long i)
{
    const int BUF_SIZE = 60;
    char buf[BUF_SIZE];
#if defined(_MSC_VER) && _MSC_VER <= 1310
    // for VS2003 or ealier, "%lld" was not supported
    safe_snprintf(buf, BUF_SIZE, "%I64d", i);
    // ::ltoa(i, buf, 10);
#elif NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_MINGW
    assert(sizeof(long long) == 8);
    safe_snprintf(buf, BUF_SIZE, "%I64d", i);
#else
    safe_snprintf(buf, BUF_SIZE, "%lld", i);
#endif
    return buf;
}

/// unsigned long long
NUT_API std::string ullong_to_str(unsigned long long i)
{
    return llong_to_str((long long) i);
}

/// bool
NUT_API std::string bool_to_str(bool b)
{
    return (b ? "true" : "false");
}

/// double
NUT_API std::string double_to_str(double d)
{
    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    safe_snprintf(buf, BUF_SIZE, "%lf", d);
    return buf;
}

/// float
NUT_API std::string float_to_str(float f)
{
    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    safe_snprintf(buf, BUF_SIZE, "%f", f);
    return buf;
}

/// pointer
NUT_API std::string ptr_to_str(const void *p)
{
    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
#if NUT_PLATFORM_OS_WINDOWS
    safe_snprintf(buf, BUF_SIZE, "0x%p",p);    // windows: 0x002E459F
#else
    safe_snprintf(buf, BUF_SIZE, "%p",p);      // linux: 0x2e459f
#endif
    return buf;
}

/// memory block
NUT_API std::string mem_to_str(const void *p, size_t align, size_t count)
{
    assert(nullptr != p && align > 0 && count > 0);

    const int BUF_SIZE = 30;
    char buf[BUF_SIZE];
    unsigned char *current = (unsigned char*)p;

    std::string ret;
    for (size_t i = 0; i < count; ++i)
    {
        if (i % 0x08 == 0)
            ret += ptr_to_str((const void*) current);

        std::string single;
        for (size_t j = 0; j < align; ++j)
        {
            safe_snprintf(buf, BUF_SIZE, "%02X", *current);
            single = std::string(buf) + single;
            ++current;
        }
        ret.push_back(' ');
        ret += single;

        if ((i % 0x08 == 0x07) && (i != count - 1))
            ret.push_back('\n');
    }
    return ret;
}

/// n-size string
NUT_API std::string sub_cstr(const char *str, size_t len, const char *fill_nil)
{
    assert(nullptr != str && nullptr != fill_nil);

    std::string ret;
    for (size_t i = 0; i < len; ++i)
    {
        if (str[i] != '\0')
            ret.push_back(str[i]);
        else
            ret += fill_nil;
    }
    return ret;
}

/// string to long
NUT_API long str_to_long(const std::string& s)
{
    long ret = 0;
    ::sscanf(s.c_str(), "%ld", &ret);
    return ret;
}

/// string to double
NUT_API double str_to_double(const std::string& s)
{
    double ret = 0.0;
    ::sscanf(s.c_str(), "%lf", &ret);
    return ret;
}

}

#if NUT_PLATFORM_CC_VC
#   pragma warning(pop)
#endif
