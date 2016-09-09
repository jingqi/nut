
#include <assert.h>
#include <stdlib.h> // for NULL

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "atomic.h"

namespace nut
{

/**
 * 指针CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(void * volatile *dest, void *oldval, void *newval)
{
    assert(NULL != dest);
#if NUT_PLATFORM_OS_LINUX || NUT_PLATFORM_OS_MAC
    return __sync_val_compare_and_swap(dest, oldval, newval) == oldval;
#elif NUT_PLATFORM_OS_WINDOWS
    return InterlockedCompareExchangePointer(dest, newval, oldval) == oldval;
#else
#   error platform not supported!
#endif
}


#if NUT_PLATFORM_BITS_64

/**
 * 128位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(int128_t volatile *dest, int128_t oldval, int128_t newval)
{
    assert(NULL != dest);
#if NUT_PLATFORM_OS_LINUX || NUT_PLATFORM_OS_MAC
    /** __sync_val_compare_and_swap() does not support 128 bits, so we get it by ourself */
    uint64_t old_low = (uint64_t)oldval, old_high = (uint64_t)(oldval >> 64);
    uint64_t new_low = (uint64_t)newval, new_high = (uint64_t)(newval >> 64);
    bool  ret;
    __asm__ __volatile__(
        "lock cmpxchg16b %1;\n"
        "sete %0;\n"
        :"=m"(ret),"+m" (*dest)
        :"a" (old_low), "d" (old_high), "b" (new_low), "c" (new_high));
    return ret;
#elif NUT_PLATFORM_OS_WINDOWS
    return InterlockedCompareExchange128(dest, newval, oldval) == oldval;
#else
#   error platform not supported!
#endif
}


/**
 * 128位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(uint128_t volatile *dest, uint128_t oldval, uint128_t newval)
{
    return atomic_cas(reinterpret_cast<int128_t volatile*>(dest), static_cast<int128_t>(oldval), static_cast<int128_t>(newval));
}

#endif

/**
 * 64位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(int64_t volatile *dest, int64_t oldval, int64_t newval)
{
    assert(NULL != dest);
#if NUT_PLATFORM_OS_LINUX || NUT_PLATFORM_OS_MAC
    return __sync_val_compare_and_swap(dest, oldval, newval) == oldval;
#elif NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_VC
    return InterlockedCompareExchange64(dest, newval, oldval) == oldval;
#elif NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_GCC

    // 以下这个宏逻辑摘自 mingw 下 InterlockedCompareExchange64() 的头文件声明位置
#   if defined(__MINGW_INTRIN_INLINE) && (defined(__GNUC__) && (__MINGW_GNUC_PREREQ(4, 9) || (__MINGW_GNUC_PREREQ(4, 8) && __GNUC_PATCHLEVEL__ >= 2)))
    return InterlockedCompareExchange64(dest, newval, oldval) == oldval;
#   else
    int64_t prev;
    __asm__ __volatile__(
        "lock ; cmpxchgq %1,%2"
        : "=a" (prev)
        : "q" (newval), "m" (*dest), "0" (oldval)
        : "memory");
    return prev == oldval;
#   endif

#else
#   error platform not supported!
#endif
}

/**
 * 64位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(uint64_t volatile *dest, uint64_t oldval, uint64_t newval)
{
    assert(NULL != dest);
    return atomic_cas(reinterpret_cast<int64_t volatile*>(dest), static_cast<int64_t>(oldval), static_cast<int64_t>(newval));
}

/**
 * 32位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(int32_t volatile *dest, int32_t oldval, int32_t newval)
{
    assert(NULL != dest);
#if NUT_PLATFORM_OS_LINUX || NUT_PLATFORM_OS_MAC
    return __sync_val_compare_and_swap(dest, oldval, newval) == oldval;
#elif NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_VC
    return InterlockedCompareExchange(reinterpret_cast<uint32_t volatile*>(dest), static_cast<uint32_t>(newval), static_cast<uint32_t>(oldval)) == oldval;
#elif NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_GCC
    static_assert(sizeof(int32_t) == sizeof(LONG), "atomic_cas() 匹配API问题");
    return InterlockedCompareExchange(reinterpret_cast<LONG volatile*>(dest), static_cast<LONG>(newval), static_cast<LONG>(oldval)) == oldval;
#else
#   error platform not supported!
#endif
}

/**
 * 32位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(uint32_t volatile *dest, uint32_t oldval, uint32_t newval)
{
    assert(NULL != dest);
    return atomic_cas(reinterpret_cast<int32_t volatile*>(dest), static_cast<int32_t>(oldval), static_cast<int32_t>(newval));
}

/**
 * 16位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(int16_t volatile *dest, int16_t oldval, int16_t newval)
{
    assert(NULL != dest);
#if NUT_PLATFORM_OS_LINUX || NUT_PLATFORM_OS_MAC
    return __sync_val_compare_and_swap(dest, oldval, newval) == oldval;
#elif NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_VC
    return InterlockedCompareExchange16(dest, newval, oldval) == oldval;
#elif NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_GCC
    int16_t prev;
    __asm__ __volatile__(
        "lock ; cmpxchgw %w1,%2"
        :"=a"(prev)
        :"q"(newval), "m"(*dest), "0"(oldval)
        : "memory");
    return prev == oldval;
#else
#   error platform not supported!
#endif
}

/**
 * 16位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(uint16_t volatile *dest, uint16_t oldval, uint16_t newval)
{
    assert(NULL != dest);
    return atomic_cas(reinterpret_cast<int16_t volatile*>(dest), static_cast<int16_t>(oldval), static_cast<int16_t>(newval));
}


#if NUT_PLATFORM_BITS_64

/**
 * 128位原子加
 *
 * @return 返回旧值
 */
NUT_API int128_t atomic_add(int128_t volatile *addend, int128_t value)
{
    assert(NULL != addend);
#if NUT_PLATFORM_OS_WINDOWS
    return InterlockedExchangeAdd128(addend, value);
#else
    int128_t old;
    do
    {
        old = *addend;
    } while (!(atomic_cas(addend, old, old + value)));
    return old;
#endif
}

/**
 * 128位原子加
 *
 * @return 返回旧值
 */
NUT_API uint128_t atomic_add(uint128_t volatile *addend, uint128_t value)
{
    assert(NULL != addend);
    return static_cast<uint128_t>(atomic_add(reinterpret_cast<int128_t volatile*>(addend), static_cast<int128_t>(value)));
}

#endif

/**
 * 64位原子加
 *
 * @return 返回旧值
 */
NUT_API int64_t atomic_add(int64_t volatile *addend, int64_t value)
{
    assert(NULL != addend);
#if NUT_PLATFORM_OS_LINUX
    return __sync_fetch_and_add(addend, value);
#elif NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_VC
    return InterlockedExchangeAdd64(addend, value);
#else
    int64_t old;
    do
    {
        old = *addend;
    } while (!atomic_cas(addend, old, old + value));
    return old;
#endif
}

/**
 * 64位原子加
 *
 * @return 返回旧值
 */
NUT_API uint64_t atomic_add(uint64_t volatile *addend, uint64_t value)
{
    assert(NULL != addend);
    return static_cast<uint64_t>(atomic_add(reinterpret_cast<int64_t volatile*>(addend), static_cast<int64_t>(value)));
}

/**
 * 32位原子加
 *
 * @return 返回旧值
 */
NUT_API int32_t atomic_add(int32_t volatile *addend, int32_t value)
{
    assert(NULL != addend);
#if NUT_PLATFORM_OS_LINUX
    return __sync_fetch_and_add(addend, value);
#elif NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_VC
    return static_cast<int32_t>(InterlockedExchangeAdd(reinterpret_cast<uint32_t volatile*>(addend), static_cast<uint32_t>(value)));
#elif NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_GCC
    static_assert(sizeof(int32_t) == sizeof(LONG), "atomic_add() 匹配API问题");
    return static_cast<int32_t>(InterlockedExchangeAdd(reinterpret_cast<LONG volatile*>(addend), static_cast<LONG>(value)));
#else
    uint32_t old;
    do
    {
        old = *addend;
    } while (!atomic_cas(addend, old, old + value));
    return old;
#endif
}

/**
 * 32位原子加
 *
 * @return 返回旧值
 */
NUT_API uint32_t atomic_add(uint32_t volatile *addend, uint32_t value)
{
    assert(NULL != addend);
    return static_cast<uint32_t>(atomic_add(reinterpret_cast<int32_t volatile*>(addend), static_cast<int32_t>(value)));
}

/**
 * 16位原子加
 *
 * @return 返回旧值
 */
NUT_API int16_t atomic_add(int16_t volatile *addend, int16_t value)
{
    assert(NULL != addend);
#if NUT_PLATFORM_OS_LINUX
    return __sync_fetch_and_add(addend, value);
#else
    int16_t old;
    do
    {
        old = *addend;
    } while (!atomic_cas(addend, old, old + value));
    return old;
#endif
}

/**
 * 16位原子加
 *
 * @return 返回旧值
 */
NUT_API uint16_t atomic_add(uint16_t volatile *addend, uint16_t value)
{
    assert(NULL != addend);
    return static_cast<uint16_t>(atomic_add(reinterpret_cast<int16_t volatile*>(addend), static_cast<int16_t>(value)));
}

}
