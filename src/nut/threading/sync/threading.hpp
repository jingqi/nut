/**
 * @file -
 * @author jingqi
 * @lastedit 2011-11-22 17:27:51 jingqi
 */

#include <nut/platform/platform.hpp>

/** 线程局部变量 */
#if defined(NUT_PLATFORM_OS_WINDOWS)
#   define NUT_THREAD_LOCAL __declspec(thread)
#elif defined(NUT_PLATFORM_OS_LINUX)
#   define NUT_THREAD_LOCAL __thread
#else
#   error platform not supported yet
#endif


