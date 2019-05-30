
#ifndef ___HEADFILE_4471D2BB_37EC_4263_A29B_ECDAD1482F25_
#define ___HEADFILE_4471D2BB_37EC_4263_A29B_ECDAD1482F25_

#include "../platform/platform.h"

/**
 * 线程局部变量
 * NOTE
 * - 在 macOS 上需要开启 -stdlib=libc++，而不能使用 -stdlib=libstdc++
 *   否则过时的 libstdc++ 不支持 thread-local 变量而导致编译错误
 * - GCC 要求 thread_local 的对象是 trivial 构造/析构的
 */
#if defined(__cplusplus) && __cplusplus >= 201103L
    // C++11 关键字
#   define NUT_THREAD_LOCAL thread_local
#elif NUT_PLATFORM_OS_MACOS
#   define NUT_THREAD_LOCAL thread_local
#elif NUT_PLATFORM_CC_VC
#   define NUT_THREAD_LOCAL __declspec(thread)
#elif NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
#   define NUT_THREAD_LOCAL __thread
#else
#   error platform not supported yet
#endif

#endif
