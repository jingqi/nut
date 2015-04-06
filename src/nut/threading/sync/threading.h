/**
 * @file -
 * @author jingqi
 * @lastedit 2014-08-31 17:01:49 jingqi
 */

#ifndef ___HEADFILE_4471D2BB_37EC_4263_A29B_ECDAD1482F25_
#define ___HEADFILE_4471D2BB_37EC_4263_A29B_ECDAD1482F25_

#include <nut/platform/platform.h>

/** 线程局部变量 */
#if defined(NUT_PLATFORM_OS_WINDOWS)
#   define NUT_THREAD_LOCAL __declspec(thread)
#elif defined(NUT_PLATFORM_OS_LINUX)
#   define NUT_THREAD_LOCAL __thread
#else
#   error platform not supported yet
#endif

#endif
