/**
 * @file -
 * @author jingqi
 * @date 2011-11-13 14:45
 */

#ifndef ___HEADFILE___6689E00A_B262_4C15_A826_19665AF8EC32_
#define ___HEADFILE___6689E00A_B262_4C15_A826_19665AF8EC32_

/** 操作系统类型 */
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__CYGWIN__) || defined(__WINDOWS__)
#   define NUT_PLATFORM_OS_NAME "Windows"
#   define NUT_PLATFORM_OS_WINDOWS
#elif defined(__linux__) || defined(__LINUX__) || defined(LINUX) || defined(_LINUX)
#   define NUT_PLATFORM_OS_NAME "Linux"
#   define NUT_PLATFORM_OS_LINUX
#else
#   define NUT_PLATFORM_OS_NAME "Unknown"
#   error unknown os
#endif

/** 系统位数 */
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__32BIT__)
#   define NUT_PLATFORM_BITS_NAME "32"
#   define NUT_PLATFORM_BITS_32
#elif defined(_WIN64) || defined(WIN64)
#   define NUT_PLATFORM_BITS_NAME "64"
#   define NUT_PLATFORM_BITS_64
#else
#   define NUT_PLATFORM_BITS_NAME "Unknown"
#   error unknown bits
#endif

/** 编译器 */
#if defined(_MSC_VER)
#	define NUT_PLATFORM_CC_NAME "VC"
#	define NUT_PLATFORM_CC_VC
#elif defined(__GNUG__) || defined(__GNUC__)
#	define NUT_PLATFORM_CC_NAME "GCC"
#	define NUT_PLATFORM_CC_GCC
#else
#	define NUT_PLATFORM_CC_NAME "Unknown"
#   error unknown compiler
#endif

/** 字符串 */
#define NUT_PLATFORM_STR "OS: " NUT_PLATFORM_OS_NAME ", Bits: " NUT_PLATFORM_BITS_NAME ", Compiler: " NUT_PLATFORM_CC_NAME


#endif /* head file guarder */
