/**
 * @file -
 * @author jingqi
 * @date 2012-03-25
 * @last-edit 2012-03-25 18:57:56 jingqi
 */

#ifndef ___HEADFILE_8DAA589E_30D8_401D_A7C9_76E83A12867A_
#define ___HEADFILE_8DAA589E_30D8_401D_A7C9_76E83A12867A_

#include <nut/platform/platform.h>

#if !defined(STATIC_ASSERT)
#   if defined(NUT_PLATFORM_CC_VC)
#       define NUT_STATIC_ASSERT(exp) static_assert((exp), #exp)
#   else
        /** 利用除0错误来进行编译期错误提示(也可以用负数数组大小错误提示来作，不过部分编译器不起作用) */
#       define NUT_STATIC_ASSERT(exp) typedef int __static_assert_error[((int)(exp)) / ((int)(exp))]
#   endif
#endif

#endif /* head file guarder */

