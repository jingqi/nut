/**
 * @file -
 * @author jingqi
 * @date 2011-11-13
 * @brief
 *   see http://sourceforge.net/apps/mediawiki/predef/index.php?title=Main_Page
 *
 * @last-edit 2012-03-25 18:53:32 jingqi
 */

#ifndef ___HEADFILE___6689E00A_B262_4C15_A826_19665AF8EC32_
#define ___HEADFILE___6689E00A_B262_4C15_A826_19665AF8EC32_


/** 操作系统类型 */
#if defined(__WINDOWS__) || defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || \
    defined(__WIN32__) || defined(__TOS_WIN__)
#   define NUT_PLATFORM_OS_NAME "Windows"
#   define NUT_PLATFORM_OS_WINDOWS
#elif defined(__linux__) || defined(linux) || defined(__linux) || defined(__LINUX__) || defined(LINUX) || \
    defined(_LINUX)
#   define NUT_PLATFORM_OS_NAME "Linux"
#   define NUT_PLATFORM_OS_LINUX
#else
#   define NUT_PLATFORM_OS_NAME "Unknown"
#   error unknown os
#endif

/** 指针位数 */
#if defined(_WIN64) || defined(WIN64) || defined(__amd64__) || defined(__amd64) || defined(__LP64__) || \
    defined(_LP64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(__ia64__) || \
    defined(_IA64) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64)
#   define NUT_PLATFORM_BITS_NAME "64"
#   define NUT_PLATFORM_BITS_64
#elif defined(_WIN32) || defined(WIN32) || defined(__32BIT__) || defined(__ILP32__) || defined(_ILP32) || \
    defined(i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || \
    defined(__i386) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined(__I86__)
#   define NUT_PLATFORM_BITS_NAME "32"
#   define NUT_PLATFORM_BITS_32
#else
#   define NUT_PLATFORM_BITS_NAME "Unknown"
#   error unknown bits
#endif

/** 编译器 */
#if defined(_MSC_VER)
#	define NUT_PLATFORM_CC_NAME "VC"
#	define NUT_PLATFORM_CC_VC
#elif defined(__GNUG__) || defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
#	define NUT_PLATFORM_CC_NAME "GCC"
#	define NUT_PLATFORM_CC_GCC
#else
#	define NUT_PLATFORM_CC_NAME "Unknown"
#   error unknown compiler
#endif

/** 字符串 */
#define NUT_PLATFORM_STR "OS: " NUT_PLATFORM_OS_NAME ", Bits: " NUT_PLATFORM_BITS_NAME ", Compiler: " NUT_PLATFORM_CC_NAME


#endif /* head file guarder */
