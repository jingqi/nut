/**
 * @file -
 * @author jingqi
 * @date 2012-03-25
 * @last-edit 2012-03-25 19:46:52 jingqi
 */

#ifndef ___HEADFILE_784AE107_1B8B_4833_ADE0_1C516A19FBE7_
#define ___HEADFILE_784AE107_1B8B_4833_ADE0_1C516A19FBE7_

#include "platform.hpp"

#include <stdint.h>

#if defined(NUT_PLATFORM_BITS_64)
#   if defined(NUT_PLATFORM_OS_WINDOWS)
typedef unsigned __int128 uint128_t;
typedef signed __int128 int128_t;
#   else
typedef __uint128_t uint128_t;
typedef __int128_t int128_t;
#   endif
#endif


#endif /* head file guarder */

