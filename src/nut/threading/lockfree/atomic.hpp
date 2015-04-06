/**
 *  基本上大部分的无锁并发数据结构都是依靠处理器提供的 CAS(compare and swap)操作来
 * 实现的; 包括原子加、原子减、自旋锁等也是依靠这个来实现的。
 */

#ifndef ___HEADFILE_CBAE01C9_CF0C_4836_A4DC_E7B0934DEA6E_
#define ___HEADFILE_CBAE01C9_CF0C_4836_A4DC_E7B0934DEA6E_

#include <nut/platform/platform.hpp>
#include <nut/platform/stdint.hpp>
#include <nut/debugging/static_assert.hpp>

namespace nut
{

/**
 * 指针CAS操作
 *
 * @return 操作成功则返回true
 */
bool atomic_cas(void * volatile *dest, void *oldval, void *newval);


#if defined(NUT_PLATFORM_BITS_64)

/**
 * 128位CAS操作
 *
 * @return 操作成功则返回true
 */
bool atomic_cas(int128_t volatile *dest, int128_t oldval, int128_t newval);


/**
 * 128位CAS操作
 *
 * @return 操作成功则返回true
 */
bool atomic_cas(uint128_t volatile *dest, uint128_t oldval, uint128_t newval);

#endif

/**
 * 64位CAS操作
 *
 * @return 操作成功则返回true
 */
bool atomic_cas(int64_t volatile *dest, int64_t oldval, int64_t newval);

/**
 * 64位CAS操作
 *
 * @return 操作成功则返回true
 */
bool atomic_cas(uint64_t volatile *dest, uint64_t oldval, uint64_t newval);

/**
 * 32位CAS操作
 *
 * @return 操作成功则返回true
 */
bool atomic_cas(int32_t volatile *dest, int32_t oldval, int32_t newval);

/**
 * 32位CAS操作
 *
 * @return 操作成功则返回true
 */
bool atomic_cas(uint32_t volatile *dest, uint32_t oldval, uint32_t newval);

/**
 * 16位CAS操作
 *
 * @return 操作成功则返回true
 */
bool atomic_cas(int16_t volatile *dest, int16_t oldval, int16_t newval);

/**
 * 16位CAS操作
 *
 * @return 操作成功则返回true
 */
bool atomic_cas(uint16_t volatile *dest, uint16_t oldval, uint16_t newval);


#if defined(NUT_PLATFORM_BITS_64)

/**
 * 128位原子加
 *
 * @return 返回旧值
 */
int128_t atomic_add(int128_t volatile *addend, int128_t value);

/**
 * 128位原子加
 *
 * @return 返回旧值
 */
uint128_t atomic_add(uint128_t volatile *addend, uint128_t value);

#endif

/**
 * 64位原子加
 *
 * @return 返回旧值
 */
int64_t atomic_add(int64_t volatile *addend, int64_t value);

/**
 * 64位原子加
 *
 * @return 返回旧值
 */
uint64_t atomic_add(uint64_t volatile *addend, uint64_t value);

/**
 * 32位原子加
 *
 * @return 返回旧值
 */
int32_t atomic_add(int32_t volatile *addend, int32_t value);

/**
 * 32位原子加
 *
 * @return 返回旧值
 */
uint32_t atomic_add(uint32_t volatile *addend, uint32_t value);

/**
 * 16位原子加
 *
 * @return 返回旧值
 */
int16_t atomic_add(int16_t volatile *addend, int16_t value);

/**
 * 16位原子加
 *
 * @return 返回旧值
 */
uint16_t atomic_add(uint16_t volatile *addend, uint16_t value);

/**
 * 为了避免ABA问题而引入的带标签的指针
 */
template <typename T>
union TagedPtr
{
    /** 整体对应的CAS操作数类型 */
#if defined(NUT_PLATFORM_BITS_64)
    typedef uint64_t tag_type;
    typedef uint128_t cas_type;
#elif defined(NUT_PLATFORM_BITS_32)
    typedef uint32_t tag_type;
    typedef uint64_t cas_type;
#elif defined(NUT_PLATFORM_BITS_16)
    typedef uint16_t tag_type;
    typedef uint32_t cas_type;
#else
#   error platform not supported!
#endif

    struct
    {
        T *ptr;
        tag_type tag;
    };

    cas_type cas;

    TagedPtr()
        : cas(0)
    {}

    TagedPtr(T *p, tag_type t)
        : ptr(p), tag(t)
    {}

    TagedPtr(cas_type c)
        : cas(c)
    {}
};

NUT_STATIC_ASSERT(sizeof(void*) == sizeof(TagedPtr<void>::tag_type));
NUT_STATIC_ASSERT(sizeof(void*) * 2 == sizeof(TagedPtr<void>::cas_type));
NUT_STATIC_ASSERT(sizeof(TagedPtr<void>) == sizeof(TagedPtr<void>::cas_type));

}

#endif /* head file guarder */
