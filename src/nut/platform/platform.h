/**
 * see http://sourceforge.net/p/predef/wiki/Home/
 */

#ifndef ___HEADFILE___6689E00A_B262_4C15_A826_19665AF8EC32_
#define ___HEADFILE___6689E00A_B262_4C15_A826_19665AF8EC32_


/**
 * 操作系统
 *
 * 支持检测：
 *  NUT_PLATFORM_OS_WINDOWS
 *  NUT_PLATFORM_OS_MACOS
 *  NUT_PLATFORM_OS_LINUX
 */
#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64) || \
    defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#   define NUT_PLATFORM_OS_WINDOWS 1
#   define NUT_PLATFORM_OS_MACOS   0
#   define NUT_PLATFORM_OS_LINUX   0
#elif defined(__APPLE__) || defined(__apple__) || defined(__MACOSX) ||  \
    defined(__MACOS_CLASSIC__) || defined(__MACH__)
#   define NUT_PLATFORM_OS_WINDOWS 0
#   define NUT_PLATFORM_OS_MACOS   1
#   define NUT_PLATFORM_OS_LINUX   0
#elif defined(__linux__) || defined(linux) || defined(__linux) || defined(__LINUX__) || \
    defined(LINUX) || defined(_LINUX)
#   define NUT_PLATFORM_OS_WINDOWS 0
#   define NUT_PLATFORM_OS_MACOS   0
#   define NUT_PLATFORM_OS_LINUX   1
#else
#   error Unknown OS
#endif

/**
 * 机器字长
 *
 * 支持检测：
 *  NUT_PLATFORM_BITS_32
 *  NUT_PLATFORM_BITS_64
 */
#if defined(_WIN64) || defined(__amd64__) || defined(__amd64) ||        \
    defined(__LP64__) || defined(_LP64) || defined(__x86_64__) || defined(__x86_64) || \
    defined(_M_X64) || defined(_M_AMD64) || defined(__aarch64__) ||     \
    defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) || \
    defined(_M_IA64)
#   define NUT_PLATFORM_BITS_64 1
#   define NUT_PLATFORM_BITS_32 0
#   define NUT_PLATFORM_BITS_16 0
#elif defined(_WIN32) || defined(__32BIT__) || defined(__ILP32__) ||    \
    defined(_ILP32) || defined(i386) || defined(__i386__) || defined(__i486__) || \
    defined(__i586__) || defined(__i686__) || defined(__i386) || defined(_M_IX86) || \
    defined(__X86__) || defined(_X86_) || defined(__I86__) || defined(__IA32__)
#   define NUT_PLATFORM_BITS_64 0
#   define NUT_PLATFORM_BITS_32 1
#   define NUT_PLATFORM_BITS_16 0
#elif defined(_WIN16) || defined(_M_I86)
#   define NUT_PLATFORM_BITS_64 0
#   define NUT_PLATFORM_BITS_32 0
#   define NUT_PLATFORM_BITS_16 1
#else
#   error Unknown system bit-length
#endif

/**
 * 编译器
 *
 * 支持检测：
 *  NUT_PLATFORM_CC_VC
 *  NUT_PLATFORM_CC_MINGW
 *  NUT_PLATFORM_CC_GCC
 */
#if defined(_MSC_VER)
#   define NUT_PLATFORM_CC_VC    1
#   define NUT_PLATFORM_CC_MINGW 0
#   define NUT_PLATFORM_CC_GCC   0
#elif defined(__MINGW32__) || defined(__MINGW64__)
#   define NUT_PLATFORM_CC_VC    0
#   define NUT_PLATFORM_CC_MINGW 1
#   define NUT_PLATFORM_CC_GCC   1
#elif defined(__GNUG__) || defined(__GNUC__)
#   define NUT_PLATFORM_CC_VC    0
#   define NUT_PLATFORM_CC_MINGW 0
#   define NUT_PLATFORM_CC_GCC   1
#else
#   error Unknown compiler
#endif

/** 模块 API 定义工具 */
#define EXTERN_C extern "C"

#if NUT_PLATFORM_OS_WINDOWS
#   define DLL_IMPORT __declspec(dllimport)
#   define DLL_EXPORT __declspec(dllexport)
#   define DLL_LOCAL
#else
#   if defined(__GNUC__) && __GNUC__ >= 4
#       define DLL_IMPORT __attribute__((visibility("default")))
#       define DLL_EXPORT __attribute__((visibility("default")))
#       define DLL_LOCAL  __attribute__((visibility("hidden")))
#   else
#       define DLL_IMPORT
#       define DLL_EXPORT
#       define DLL_LOCAL
#   endif
#endif

/** 用于消除编译时 unused parameter 警告 */
#ifndef UNUSED
#   define UNUSED(x) ((void)x)
#endif

#endif /* head file guarder */
