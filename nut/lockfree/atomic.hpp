/**
 * @file -
 * @author jingqi
 * @date 2012-03-05
 * @last-edit 2012-03-05 20:19:43 jingqi
 * @brief
 *      基本上大部分的无锁并发数据结构都是依靠处理器提供的CAS(compare and swap)操作来
 * 实现的; 包括原子加、原子减、自旋锁等也是依靠这个来实现的。
 */

#ifndef ___HEADFILE_CBAE01C9_CF0C_4836_A4DC_E7B0934DEA6E_
#define ___HEADFILE_CBAE01C9_CF0C_4836_A4DC_E7B0934DEA6E_

#include <stdint.h>
#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#endif

namespace nut
{

/**
 * 指针CAS操作
 *
 * @return 操作成功则返回true
 */
inline bool atomic_cas(void * volatile *dest, void *oldval, void *newval)
{
#if defined(NUT_PLATFORM_OS_LINUX)
    return __sync_val_compare_and_swap(dest, oldval, newval);
#elif defined(NUT_PLATFORM_OS_WINDOWS)
    return InterlockedCompareExchangePointer(dest, newval, oldval) == oldval;
#else
#   error platform not supported!
#endif
}


#if defined(NUT_PLATFORM_BITS_64)

/**
 * 128位CAS操作
 *
 * @return 操作成功则返回true
 */
inline bool atomic_cas(int128_t volatile *dest, int128_t oldval, int128_t newval)
{
#if defined(NUT_PLATFORM_OS_LINUX)
    return __sync_val_compare_and_swap(dest, oldval, newval);
#elif defined(NUT_PLATFORM_OS_WINDOWS)
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
inline bool atomic_cas(uint128_t volatile *dest, uint128_t oldval, uint128_t newval)
{
    return atomic_cas(reinterpret_cast<int128_t volatile*>(dest), static_cast<int128_t>(oldval), static_cast<int128_t>(newval));
}

#endif

/**
 * 64位CAS操作
 *
 * @return 操作成功则返回true
 */
inline bool atomic_cas(int64_t volatile *dest, int64_t oldval, int64_t newval)
{
#if defined(NUT_PLATFORM_OS_LINUX)
    return __sync_val_compare_and_swap(dest, oldval, newval);
#elif defined(NUT_PLATFORM_OS_WINDOWS)
    return InterlockedCompareExchange64(dest, newval, oldval) == oldval;
#else
#   error platform not supported!
#endif
}


/**
 * 64位CAS操作
 *
 * @return 操作成功则返回true
 */
inline bool atomic_cas(uint64_t volatile *dest, uint64_t oldval, uint64_t newval)
{
    return atomic_cas(reinterpret_cast<int64_t volatile*>(dest), static_cast<int64_t>(oldval), static_cast<int64_t>(newval));
}


/**
 * 32位CAS操作
 *
 * @return 操作成功则返回true
 */
inline bool atomic_cas(uint32_t volatile *dest, uint32_t oldval, uint32_t newval)
{
#if defined(NUT_PLATFORM_OS_LINUX)
    return __sync_val_compare_and_swap(dest, oldval, newval);
#elif defined(NUT_PLATFORM_OS_WINDOWS)
    return InterlockedCompareExchange(dest, newval, oldval) == oldval;
#else
#   error platform not supported!
#endif
}


/**
 * 32位CAS操作
 *
 * @return 操作成功则返回true
 */
inline bool atomic_cas(int32_t volatile *dest, int32_t oldval, int32_t newval)
{
    return atomic_cas(reinterpret_cast<uint32_t volatile*>(dest), static_cast<uint32_t>(oldval), static_cast<uint32_t>(newval));
}


/**
 * 16位CAS操作
 *
 * @return 操作成功则返回true
 */
inline bool atomic_cas(int16_t volatile *dest, int16_t oldval, int16_t newval)
{
#if defined(NUT_PLATFORM_OS_LINUX)
    return __sync_val_compare_and_swap(dest, oldval, newval);
#elif defined(NUT_PLATFORM_OS_WINDOWS)
    return InterlockedCompareExchange16(dest, newval, oldval) == oldval;
#else
#   error platform not supported!
#endif
}


/**
 * 16位CAS操作
 *
 * @return 操作成功则返回true
 */
inline bool atomic_cas(uint16_t volatile *dest, uint16_t oldval, uint16_t newval)
{
    return atomic_cas(reinterpret_cast<int16_t volatile*>(dest), static_cast<int16_t>(oldval), static_cast<int16_t>(newval));
}


#if defined(NUT_PLATFORM_BITS_64)

/**
 * 128位原子加
 *
 * @return 返回旧值
 */
inline int128_t atomic_add(int128_t volatile *addend, int128_t value)
{
#   if defined(NUT_PLATFORM_OS_LINUX)
    return __sync_fetch_and_add(addend, value);
#   elif defined(NUT_PLATFORM_OS_WINDOWS)
    return InterlockedExchangeAdd128(addend, value);
#   else
    int128_t old;
    do {
        old = *addend;
    } while (!(atomic_cas(addend, old, old + value)));
    return old;
#   endif
}

/**
 * 128位原子加
 *
 * @return 返回旧值
 */
inline uint128_t atomic_add(uint128_t volatile *addend, uint128_t value)
{
    return static_cast<uint128_t>(atomic_add(reinterpret_cast<int128_t volatile*>(addend), static_cast<int128_t>(value)));
}

#endif


/**
 * 64位原子加
 *
 * @return 返回旧值
 */
inline int64_t atomic_add(int64_t volatile *addend, int64_t value)
{
#if defined(NUT_PLATFORM_OS_LINUX)
    return __sync_fetch_and_add(addend, value);
#elif defined(NUT_PLATFORM_OS_WINDOWS)
    return InterlockedExchangeAdd64(addend, value);
#else
    int64_t old;
    do {
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
inline uint64_t atomic_add(uint64_t volatile *addend, uint64_t value)
{
    return static_cast<uint64_t>(atomic_add(reinterpret_cast<int64_t volatile*>(addend), static_cast<int64_t>(value)));
}

/**
 * 32位原子加
 *
 * @return 返回旧值
 */
inline uint32_t atomic_add(uint32_t volatile *addend, uint32_t value)
{
#if defined(NUT_PLATFORM_OS_LINUX)
    return __sync_fetch_and_add(addend, value);
#elif defined(NUT_PLATFORM_OS_WINDOWS)
    return InterlockedExchangeAdd(addend, value);
#else
    uint32_t old;
    do {
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
 inline int32_t atomic_add(int32_t volatile *addend, int32_t value)
 {
    return static_cast<int32_t>(atomic_add(reinterpret_cast<uint32_t volatile*>(addend), static_cast<uint32_t>(value)));
 }

/**
 * 16位原子加
 *
 * @return 返回旧值
 */
inline int16_t atomic_add(int16_t volatile *addend, int16_t value)
{
#if defined(NUT_PLATFORM_OS_LINUX)
    return __sync_fetch_and_add(addend, value);
#else
    int16_t old;
    do {
        old = *addend;
    } while (!atomic_cas(addend, old, old + value));
    return old;
#endif
}

/**
 * 128位原子加
 *
 * @return 返回旧值
 */
 inline uint16_t atomic_add(uint16_t volatile *addend, uint16_t value)
 {
    return static_cast<uint16_t>(atomic_add(reinterpret_cast<int16_t volatile*>(addend), static_cast<int16_t>(value)));
 }

/**
 * 为了避免ABA问题而引入的带标签的指针
 */
template <typename T>
union TagedPtr
{
    /** 整体对应的CAS操作数类型 */
#if defined(NUT_PLATFORM_BITS_64)
    typedef uint128_t cas_type;
#elif defined(NUT_PLATFORM_BITS_32)
    typedef uint64_t cas_type;
#elif defined(NUT_PLATFORM_BITS_16)
    typedef uint32_t cas_type;
#else
#   error platform not supported!
#endif

    struct
    {
        T *ptr;
        unsigned int tag;
    };

    cas_type cas;

    TagedPtr() : cas(0) {}

    TagedPtr(T *p, unsigned int t) : ptr(p), tag(t) {}

    TagedPtr(cas_type c) : cas(c) {}
};
static_assert(sizeof(void*) + sizeof(unsigned int) == sizeof(TagedPtr<void>::cas_type), "size not match");
static_assert(sizeof(TagedPtr<void>) == sizeof(TagedPtr<void>::cas_type), "size not match");

}

#endif /* head file guarder */

