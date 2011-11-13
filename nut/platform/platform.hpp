/**
 * @file -
 * @author jingqi
 * @date 2011-11-13 14:45
 */

#ifndef ___HEADFILE___6689E00A_B262_4C15_A826_19665AF8EC32_
#define ___HEADFILE___6689E00A_B262_4C15_A826_19665AF8EC32_

/** 操作系统类型 */
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__WINDOWS__)
#   define NUT_PLATFORM_OS_WINDOWS
#elif defined(__linux__) || defined(__LINUX__) || defined(LINUX) || defined(_LINUX)
#   define NUT_PLATFORM_OS_LINUX
#else
#   error unknown os
#endif

/** 系统位数 */
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__32BIT__)
#   define NUT_PLATFORM_BITS_32
#else
#   error unknown bits
#endif

/** 编译器 */
#if defined(_MSC_VER)
#	define NUT_PLATFORM_CC_VC
#elif defined(__GNUG__) || defined(__GNUC__)
#	define NUT_PLATFORM_CC_GCC
#else
#   error unknown compiler
#endif


#endif /* head file guarder */
