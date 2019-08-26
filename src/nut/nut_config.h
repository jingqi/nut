
#ifndef ___HEADFILE_80E658A8_312C_49D2_A9B0_315AB18D26F7_
#define ___HEADFILE_80E658A8_312C_49D2_A9B0_315AB18D26F7_

#include "platform/platform.h"

#if defined(STATIC_NUT_API)
#   define NUT_API
#elif defined(EXPORT_NUT_API)
#   define NUT_API DLL_EXPORT
#else // default IMPORT_NUT_API
#   define NUT_API DLL_IMPORT
#endif

// 允许最大的 ::alloca() 分配内存大小
#define NUT_MAX_ALLOCA_SIZE 2048

#endif
