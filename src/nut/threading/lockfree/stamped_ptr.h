
#ifndef ___HEADFILE_A6EE8581_AE7D_42C8_A4F8_0F10CE4F4E7E_
#define ___HEADFILE_A6EE8581_AE7D_42C8_A4F8_0F10CE4F4E7E_

#include <string.h> // for memcpy()
#include <type_traits>

#define _ENABLE_ATOMIC_ALIGNMENT_FIX // VS2015 SP2 BUG
#include <atomic>

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include <nut/platform/int_type.h>


namespace nut
{

// FIXME 至少在 MacOS 上，放入 std::atomic<> 的结构体，大小必须是 1/2/4/8/16 字
//       节的，因为 CAS 操作的内存颗粒大小只能是这些。
//       eg. 结构体大小为 6 字节, CAS 操作每次依然对比 8 字节，会导致尾部内存参
//       与 CAS，一定概率导致 CAS 失败，甚至某个循环 CAS 无限卡住
#pragma pack(1)

template <typename T>
class AtomicStampedPtr;

/**
 * 为了避免 atomic ABA 问题而引入的带标签的指针
 *
 * 解决 ABA 问题有几个方案：
 * 1. 由于指针地址一般是对齐的，用指针末尾的几个 bit 做一个时间戳 tag
 *    缺点是 tag 位数少，可能 wrap 后依然出现 ABA 问题; 如果指针不对齐(不推荐，
 *    但是有可能出现)，则不适用
 * 2. 在指针后附加一个大字段来表示时间戳，也就是下面的方案。
 *    缺点是，操作位数变长，性能有所损失
 */
template <typename T>
class StampedPtr
{
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

    template <typename U> friend class AtomicStampedPtr;
};

static_assert(sizeof(StampedPtr<void>) == 2 * sizeof(void*), "StampedPtr<> size error");


/**
 * NOTE
 * - 为了避免在 Windows 64 MSVC 下 CAS 操作崩溃，使用该结构体时需要声明对齐方式，
 *   例如：
 *     alignas(sizeof(AtomicStampedPtr<Node>)) AtomicStampedPtr<Node> next;
 */
template <typename T>
class AtomicStampedPtr
{
public:
    AtomicStampedPtr()
    {
#if !NUT_PLATFORM_OS_WINDOWS || !NUT_PLATFORM_BITS_64 || !NUT_PLATFORM_CC_VC
        assert(_stamped_ptr.is_lock_free());
#endif
    }

#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
    AtomicStampedPtr(T *p, typename StampedPtr<T>::stamp_type s)
    {
        ((StampedPtr<T>*) this)->set(p, s);
    }
#else
    AtomicStampedPtr(T *p, typename StampedPtr<T>::stamp_type s)
    {
        assert(_stamped_ptr.is_lock_free());
        _stamped_ptr.store({p, s}, std::memory_order_relaxed);
    }
#endif

    StampedPtr<T> load(std::memory_order order = std::memory_order_seq_cst) const noexcept
    {
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        if (order != std::memory_order_relaxed)
            std::atomic_thread_fence(order);

        // 根据微软的文档，无论 CAS 是否成功，'ret' 都会返回当前值
        StampedPtr<T> ret;
        ::InterlockedCompareExchange128(
            const_cast<int64_t volatile*>(&_low_part), 0, 0, (int64_t*) &ret);
        return ret;
#else
        return _stamped_ptr.load(order);
#endif
    }

    void store(const StampedPtr<T>& desired,
               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        int64_t expected[2];
        ::memcpy(expected, (void*) &_low_part, sizeof(StampedPtr<T>));
        const int64_t *raw_desired = (const int64_t*) &desired;
        while (1 != ::InterlockedCompareExchange128(
                   &_low_part, raw_desired[1], raw_desired[0], expected))
        {}

        if (order != std::memory_order_relaxed)
            std::atomic_thread_fence(order);
#else
        _stamped_ptr.store(desired, order);
#endif
    }

    StampedPtr<T> exchange(const StampedPtr<T>& desired,
                           std::memory_order order = std::memory_order_seq_cst) noexcept
    {
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        StampedPtr<T> ret(_stamped_ptr);
        const int64_t *raw_desired = (const int64_t*) &desired;
        while (1 != ::InterlockedCompareExchange128(
            &_low_part, raw_desired[1], raw_desired[0], (int64_t*) &ret))
        {}

        if (order != std::memory_order_relaxed)
            std::atomic_thread_fence(order);

        return ret;
#else
        return _stamped_ptr.exchange(desired, order);
#endif
    }

    bool compare_exchange_weak(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                               std::memory_order success, std::memory_order failure) noexcept
    {
        assert(nullptr != expected);
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        return compare_exchange_strong(expected, desired, success, failure);
#else
        return _stamped_ptr.compare_exchange_weak(*expected, desired, success, failure);
#endif
    }

    bool compare_exchange_weak(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        assert(nullptr != expected);
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
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
        int64_t dup_expected[2];
        ::memcpy(dup_expected, expected, sizeof(StampedPtr<T>));
        const int64_t *raw_desired = (const int64_t*) &desired;
        if (1 != ::InterlockedCompareExchange128(
            &_low_part, raw_desired[1], raw_desired[0], dup_expected))
        {
            // failed
            ::memcpy(expected, dup_expected, sizeof(StampedPtr<T>));

            if (failure != std::memory_order_relaxed)
                std::atomic_thread_fence(failure);
            return false;
        }

        // success, keep 'expected' unchanged
        if (success != std::memory_order_relaxed)
            std::atomic_thread_fence(success);
        return true;
#else
        return _stamped_ptr.compare_exchange_strong(*expected, desired, success, failure);
#endif
    }

    bool compare_exchange_strong(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                            std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        assert(nullptr != expected);
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        return compare_exchange_strong(expected, desired, order, order);
#else
        return _stamped_ptr.compare_exchange_strong(*expected, desired, order);
#endif
    }

private:
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
    int64_t volatile _low_part = 0; // NOTE low part first
    int64_t volatile _high_part = 0;
#else
    std::atomic<StampedPtr<T>> _stamped_ptr = ATOMIC_VAR_INIT(StampedPtr<T>());
#endif
};

static_assert(sizeof(AtomicStampedPtr<void>) == 2 * sizeof(void*), "AtomicStampedPtr<> size or align error");

#pragma pack()

}

#endif
