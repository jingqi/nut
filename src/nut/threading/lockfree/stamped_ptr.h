
#ifndef ___HEADFILE_A6EE8581_AE7D_42C8_A4F8_0F10CE4F4E7E_
#define ___HEADFILE_A6EE8581_AE7D_42C8_A4F8_0F10CE4F4E7E_

#include <string.h> // for memcpy()
#include <type_traits>
#include <atomic>

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include <nut/platform/int_type.h>


namespace nut
{

/**
 * FIXME 已知在 MacOS 上，放入 std::atomic<> 的结构体，大小必须是 1/2/4/8/16 字
 *       节的，因为 lockfree CAS 操作的内存颗粒大小只能是这些。
 *       eg. 结构体大小为 6 字节, CAS 操作每次依然对比 8 字节，会导致尾部垃圾内
 *       存参与 CAS，一定概率导致 CAS 失败，甚至某个循环 CAS 无限卡住
 */
#pragma pack(1)

template <typename T>
class AtomicStampedPtr;

/**
 * 为了避免 atomic ABA 问题而引入的带标签的指针
 *
 * 解决 ABA 问题有几个方案：
 * 1. 由于指针地址一般是对齐的，用指针末尾的几个 bit 做一个时间戳 tag。
 *    缺点是 tag 位数少，可能 wrap 后依然出现 ABA 问题。
 * 2. 在指针后附加一个大字段来表示时间戳，也就是下面的方案。
 *    缺点是，操作位数变长，性能有所损失。
 */
template <typename T>
class StampedPtr
{
    template <typename U> friend class AtomicStampedPtr;

public:
    typedef typename std::conditional<
        sizeof(T*) == sizeof(uint8_t), uint8_t,
        typename std::conditional<
            sizeof(T*) == sizeof(uint16_t), uint16_t,
            typename std::conditional<
                sizeof(T*) == sizeof(uint32_t), uint32_t,
                uint64_t>::type>::type>::type stamp_type;

    static_assert(sizeof(stamp_type) == sizeof(T*), "stamp_type size error");

public:
    StampedPtr() noexcept
        : ptr(nullptr), stamp(0)
    {}

    StampedPtr(T *p, stamp_type s) noexcept
        : ptr(p), stamp(s)
    {}

    bool operator==(const StampedPtr<T>& x) const
    {
        return ptr == x.ptr && stamp == x.stamp;
    }

    bool operator!=(const StampedPtr<T>& x) const
    {
        return !(*this == x);
    }

    void clear()
    {
        ptr = nullptr;
        stamp = 0;
    }

    void set(T *p, stamp_type s)
    {
        ptr = p;
        stamp = s;
    }

public:
    T *ptr;           // low part
    stamp_type stamp; // high part
};

static_assert(sizeof(StampedPtr<void>) == 2 * sizeof(void*), "StampedPtr<> size error");


/**
 * 在 Windows 64 MSVC 和部分 linux 下，std::atomic<StampedPtr<>> 不是 lockfree
 * 的而是用 spinlock 实现的，而某些无锁算法需要访问已经析构甚至 free 的内存中的
 * 指针和标记，需要这块内存是 lockfree 的。
 * 故使用 AtomicStampedPtr<> 替代 std::atomic<StampedPtr<>>，以保证其是 lockfree
 * 的。
 *
 * NOTE 为了避免在 Windows 64 下 InterlockedCompareExchange128() 操作崩溃，需要
 *      声明为 16 字节对齐
 */
template <typename T>
class alignas(sizeof(StampedPtr<void>)) AtomicStampedPtr
{
#if NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64
private:
    typedef typename std::conditional<
        sizeof(T*) == sizeof(uint8_t), uint16_t,
        typename std::conditional<
            sizeof(T*) == sizeof(uint16_t), uint32_t,
            typename std::conditional<
                sizeof(T*) == sizeof(uint32_t), uint64_t,
                uint128_t>::type>::type>::type cas_type;

    static_assert(sizeof(cas_type) == sizeof(T*) * 2, "cas_type size error");
#endif

public:
    AtomicStampedPtr()
    {
#if !(NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC) && \
    !(NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64)
        assert(_stamped_ptr.is_lock_free());
#endif
    }

    AtomicStampedPtr(T *p, typename StampedPtr<T>::stamp_type s)
    {
#if (NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC) || \
    (NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64)
        assert(sizeof(AtomicStampedPtr<T>) == sizeof(StampedPtr<T>));
        ((StampedPtr<T>*) this)->set(p, s);
#else
        assert(_stamped_ptr.is_lock_free());
        _stamped_ptr.store({p, s}, std::memory_order_relaxed);
#endif
    }

    StampedPtr<T> load(std::memory_order order = std::memory_order_seq_cst) const noexcept
    {
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        StampedPtr<T> ret;
        atomic_cas(const_cast<int64_t volatile*>(&_low_part), (int64_t*) &ret, 0, 0);

        if (order != std::memory_order_relaxed)
            std::atomic_thread_fence(order); // Usually 'acquire'

        return ret;
#elif NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64
        StampedPtr<T> ret;
        atomic_cas(const_cast<uint128_t volatile*>(&_cas_value), (uint128_t*) &ret, 0);

        if (order != std::memory_order_relaxed)
            std::atomic_thread_fence(order); // Usually 'acquire'

        return ret;
#else
        return _stamped_ptr.load(order);
#endif
    }

    void store(const StampedPtr<T>& desired,
               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        if (order != std::memory_order_relaxed)
            std::atomic_thread_fence(order); // Usually 'release'

        int64_t expected[2];
        ::memcpy(expected, (void*) &_low_part, sizeof(StampedPtr<T>));
        const int64_t *raw_desired = (const int64_t*) &desired;
        while (!atomic_cas(&_low_part, expected, raw_desired[0], raw_desired[1]))
        {}
#elif NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64
        if (order != std::memory_order_relaxed)
            std::atomic_thread_fence(order); // Usually 'release'

        uint128_t expected = _cas_value;
        const uint128_t *raw_desired = (const uint128_t*) &desired;
        while (!atomic_cas(&_cas_value, &expected, *raw_desired))
        {}
#else
        _stamped_ptr.store(desired, order);
#endif
    }

    StampedPtr<T> exchange(const StampedPtr<T>& desired,
                           std::memory_order order = std::memory_order_seq_cst) noexcept
    {
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        if (order != std::memory_order_relaxed)
            std::atomic_thread_fence(order); // Usually 'release'

        StampedPtr<T> ret;
        ::memcpy(&ret, (void*) &_low_part, sizeof(StampedPtr<T>));
        const int64_t *raw_desired = (const int64_t*) &desired;
        while (!atomic_cas(&_low_part, (int64_t*) &ret, raw_desired[0], raw_desired[1]))
        {}

        return ret;
#elif NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64
        if (order != std::memory_order_relaxed)
            std::atomic_thread_fence(order); // Usually 'release'

        StampedPtr<T> ret;
        ::memcpy(&ret, (void*) &_cas_value, sizeof(StampedPtr<T>));
        const uint128_t *raw_desired = (const uint128_t*) &desired;
        while (!atomic_cas(&_cas_value, (uint128_t*) &ret, *raw_desired))
        {}

        return ret;
#else
        return _stamped_ptr.exchange(desired, order);
#endif
    }

    bool compare_exchange_weak(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                               std::memory_order success, std::memory_order failure) noexcept
    {
        assert(nullptr != expected);
#if (NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC) || \
    (NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64)
        return compare_exchange_strong(expected, desired, success, failure);
#else
        return _stamped_ptr.compare_exchange_weak(*expected, desired, success, failure);
#endif
    }

    bool compare_exchange_weak(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        assert(nullptr != expected);
#if (NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC) || \
    (NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64)
        return compare_exchange_weak(expected, desired, order, order);
#else
        return _stamped_ptr.compare_exchange_weak(*expected, desired, order);
#endif
    }

    bool compare_exchange_strong(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                                 std::memory_order success, std::memory_order failure) noexcept
    {
        assert(nullptr != expected);
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        // NOTE 无法提前预知是否 success, 这里只能无论是否 success 都设置内存栅栏
        if (success != std::memory_order_relaxed)
            std::atomic_thread_fence(success); // Usually 'release'

        const int64_t *raw_desired = (const int64_t*) &desired;
        if (!atomic_cas(&_low_part, (int64_t*) expected, raw_desired[0], raw_desired[1]))
        {
            // Failed
            if (failure != std::memory_order_relaxed)
                std::atomic_thread_fence(failure); // Usually 'acquire'
            return false;
        }

        // Success
        return true;
#elif NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64
        // NOTE 无法提前预知是否 success, 这里只能无论是否 success 都设置内存栅栏
        if (success != std::memory_order_relaxed)
            std::atomic_thread_fence(success); // Usually 'release'

        const uint128_t *raw_desired = (const uint128_t*) &desired;
        if (!atomic_cas(&_cas_value, (uint128_t*) expected, *raw_desired))
        {
            // Failed
            if (failure != std::memory_order_relaxed)
                std::atomic_thread_fence(failure); // Usually 'acquire'
            return false;
        }

        // Success
        return true;
#else
        return _stamped_ptr.compare_exchange_strong(*expected, desired, success, failure);
#endif
    }

    bool compare_exchange_strong(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                            std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        assert(nullptr != expected);
#if (NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC) || \
    (NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64)
        return compare_exchange_strong(expected, desired, order, order);
#else
        return _stamped_ptr.compare_exchange_strong(*expected, desired, order);
#endif
    }

private:
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
    static bool atomic_cas(int64_t volatile* dest, int64_t *expected,
                           int64_t low_desire, int64_t high_desire)
    {
        assert(nullptr != dest && nullptr != expected);

        // NOTE 根据微软的文档，无论 CAS 是否成功，'dup_expected' 都会返回原始值
        int64_t dup_expected[2];
        dup_expected[0] = expected[0];
        dup_expected[1] = expected[1];
        const int rs = ::InterlockedCompareExchange128(
            dest, high_desire, low_desire, dup_expected); // 'high_desire' comes first by API spec
        if (1 == rs)
            return true;

        expected[0] = dup_expected[0];
        expected[1] = dup_expected[1];
        return false;
    }
#elif NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64
    static bool atomic_cas(uint128_t volatile *dest, uint128_t *expected, uint128_t desired)
    {
        assert(nullptr != dest && nullptr != expected);

        // NOTE '__sync_val_compare_and_swap()' doesn't support 128 bits, so we
        //      get it by ourself
        uint64_t low_expected = ((uint64_t*) expected)[0],
            high_expected = ((uint64_t*) expected)[1];
        const uint64_t low_desired = (uint64_t) desired,
            high_desired = (uint64_t) (desired >> 64);
        bool rs = false;
        __asm__ __volatile__ (
            "lock cmpxchg16b %1;\n"
            "sete %0;\n"
            : "=m"(rs), "+m" (*dest), "+a" (low_expected), "+d" (high_expected)
            : "b" (low_desired), "c" (high_desired));
        if (!rs)
            *expected = (((uint128_t) high_expected) << 64) | low_expected;
        return rs;
    }
#endif

private:
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
    int64_t volatile _low_part = 0; // NOTE Low-part comes first, followed by high-part
    int64_t volatile _high_part = 0;
#elif NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64
    cas_type volatile _cas_value = 0;
#else
    std::atomic<StampedPtr<T>> _stamped_ptr = ATOMIC_VAR_INIT(StampedPtr<T>());
#endif
};

static_assert(sizeof(AtomicStampedPtr<void>) == 2 * sizeof(void*), "AtomicStampedPtr<> size or align error");

#pragma pack()

}

#endif
