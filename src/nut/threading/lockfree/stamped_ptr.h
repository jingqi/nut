
#ifndef ___HEADFILE_A6EE8581_AE7D_42C8_A4F8_0F10CE4F4E7E_
#define ___HEADFILE_A6EE8581_AE7D_42C8_A4F8_0F10CE4F4E7E_

#include <assert.h>
#include <string.h> // for ::memcpy()
#include <type_traits>
#include <atomic>

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "../../platform/int_type.h"


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
class alignas(sizeof(void*) * 2) StampedPtr
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

    bool operator==(const StampedPtr<T>& x) const noexcept
    {
        return ptr == x.ptr && stamp == x.stamp;
    }

    bool operator!=(const StampedPtr<T>& x) const noexcept
    {
        return !(*this == x);
    }

    void clear() noexcept
    {
        ptr = nullptr;
        stamp = 0;
    }

    void set(T *p, stamp_type s) noexcept
    {
        ptr = p;
        stamp = s;
    }

public:
    T *ptr;           // low part
    stamp_type stamp; // high part
};

static_assert(sizeof(StampedPtr<void>) == sizeof(void*) * 2, "StampedPtr<> size error");


/**
 * 在 Windows 64 MSVC 和部分 linux 下，std::atomic<StampedPtr<>> 不是 lockfree
 * 的而是用 spinlock 实现的，而某些无锁算法需要访问已经析构甚至 free 的内存中的
 * 指针和标记，需要这块内存是 lockfree 的。
 *     故使用 AtomicStampedPtr<> 替代 std::atomic<StampedPtr<>>，以保证其是
 * lockfree 的。
 */
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC

/**
 * 基于 windows 64 MSVC 的 ::InterlockedCompareExchange128() 实现
 *
 * NOTE
 * - 为了避免在 Windows 64 下 ::InterlockedCompareExchange128() 操作崩溃，需要
 *   声明为 16 字节对齐
 * - ::InterlockedCompareExchange128() 自带完全内存栅栏
 *
 * SEE https://docs.microsoft.com/en-us/windows/desktop/api/winnt/nf-winnt-interlockedcompareexchange128
 */
template <typename T>
class alignas(sizeof(StampedPtr<void>)) AtomicStampedPtr
{
public:
    AtomicStampedPtr() noexcept
    {}

    AtomicStampedPtr(T *p, typename StampedPtr<T>::stamp_type s) noexcept
    {
        assert(sizeof(AtomicStampedPtr<T>) == sizeof(StampedPtr<T>));
        reinterpret_cast<StampedPtr<T>*>(this)->set(p, s);
    }

    StampedPtr<T> load(std::memory_order order = std::memory_order_seq_cst) const noexcept
    {
        StampedPtr<T> ret;
        atomic_cas(const_cast<int64_t volatile*>(&_low_part),
                   reinterpret_cast<int64_t*>(&ret), 0, 0);
        return ret;
    }

    void store(const StampedPtr<T>& desired,
               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        int64_t expected[2];
        ::memcpy(expected, const_cast<int64_t*>(&_low_part), sizeof(StampedPtr<T>));
        const int64_t *raw_desired = reinterpret_cast<const int64_t*>(&desired);
        while (!atomic_cas(&_low_part, expected, raw_desired[0], raw_desired[1]))
        {}
    }

    StampedPtr<T> exchange(const StampedPtr<T>& desired,
                           std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        StampedPtr<T> ret;
        ::memcpy(&ret, const_cast<int64_t*>(&_low_part), sizeof(StampedPtr<T>));
        const int64_t *raw_desired = reinterpret_cast<const int64_t*>(&desired);
        while (!atomic_cas(&_low_part, reinterpret_cast<int64_t*>(&ret),
                           raw_desired[0], raw_desired[1]))
        {}

        return ret;
    }

    bool compare_exchange_weak(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                               std::memory_order success, std::memory_order failure) noexcept
    {
        assert(nullptr != expected);
        return compare_exchange_strong(expected, desired, success, failure);
    }

    bool compare_exchange_weak(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        assert(nullptr != expected);
        return compare_exchange_weak(expected, desired, order, order);
    }

    bool compare_exchange_strong(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                                 std::memory_order success, std::memory_order failure) noexcept
    {
        assert(nullptr != expected);

        const int64_t *raw_desired = reinterpret_cast<const int64_t*>(&desired);
        if (!atomic_cas(&_low_part, reinterpret_cast<int64_t*>(expected),
                        raw_desired[0], raw_desired[1]))
            return false;

        // Success
        return true;
    }

    bool compare_exchange_strong(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                            std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        assert(nullptr != expected);
        return compare_exchange_strong(expected, desired, order, order);
    }

private:
    static bool atomic_cas(int64_t volatile* dest, int64_t *expected,
                           int64_t low_desired, int64_t high_desired) noexcept
    {
        assert(nullptr != dest && nullptr != expected);

        // NOTE 根据微软的文档，无论 CAS 是否成功，'dup_expected' 都会返回原始值
        alignas(16) int64_t dup_expected[2];
        ::memcpy(dup_expected, expected, sizeof(StampedPtr<T>));
        const int rs = ::InterlockedCompareExchange128(
            dest, high_desired, low_desired, dup_expected); // 'high_desired' comes first by API spec
        if (1 == rs)
            return true;

        ::memcpy(expected, dup_expected, sizeof(StampedPtr<T>));
        return false;
    }

private:
    int64_t volatile _low_part = 0; // NOTE Low-part comes first, followed by high-part
    int64_t volatile _high_part = 0;
};

#elif (NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_GCC) || \
    (NUT_PLATFORM_OS_LINUX && NUT_PLATFORM_BITS_64)

/**
 * 基于 gcc 内建的 __atomic_xxx() 函数实现
 *
 * SEE https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
 */
template <typename T>
class alignas(sizeof(StampedPtr<void>)) AtomicStampedPtr
{
private:
    typedef typename std::conditional<
        sizeof(T*) == sizeof(uint8_t), uint16_t,
        typename std::conditional<
            sizeof(T*) == sizeof(uint16_t), uint32_t,
            typename std::conditional<
                sizeof(T*) == sizeof(uint32_t), uint64_t,
                uint128_t>::type>::type>::type cas_type;

    static_assert(sizeof(cas_type) == sizeof(T*) * 2, "cas_type size error");

public:
    AtomicStampedPtr() noexcept
    {}

    AtomicStampedPtr(T *p, typename StampedPtr<T>::stamp_type s) noexcept
    {
        assert(sizeof(AtomicStampedPtr<T>) == sizeof(StampedPtr<T>));
        reinterpret_cast<StampedPtr<T>*>(this)->set(p, s);
    }

    StampedPtr<T> load(std::memory_order order = std::memory_order_seq_cst) const noexcept
    {
        StampedPtr<T> ret;
        *reinterpret_cast<cas_type*>(&ret) = __atomic_load_n(
            &_cas_value, to_builtin_memorder(order));
        return ret;
    }

    void store(const StampedPtr<T>& desired,
               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        __atomic_store_n(
            &_cas_value, *reinterpret_cast<const cas_type*>(&desired),
            to_builtin_memorder(order));
    }

    StampedPtr<T> exchange(const StampedPtr<T>& desired,
                           std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        StampedPtr<T> ret;
        *reinterpret_cast<cas_type*>(&ret) = __atomic_exchange_n(
            &_cas_value, *reinterpret_cast<const cas_type*>(&desired),
            to_builtin_memorder(order));
        return ret;
    }

    bool compare_exchange_weak(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                               std::memory_order success, std::memory_order failure) noexcept
    {
        assert(nullptr != expected);
        return __atomic_compare_exchange_n(
            &_cas_value, reinterpret_cast<cas_type*>(expected),
            *reinterpret_cast<const cas_type*>(&desired), true,
            to_builtin_memorder(success), to_builtin_memorder(failure));
    }

    bool compare_exchange_weak(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        assert(nullptr != expected);
        const int builtin_order = to_builtin_memorder(order);
        return __atomic_compare_exchange_n(
            &_cas_value, reinterpret_cast<cas_type*>(expected),
            *reinterpret_cast<const cas_type*>(&desired), true,
            builtin_order, builtin_order);
    }

    bool compare_exchange_strong(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                                 std::memory_order success, std::memory_order failure) noexcept
    {
        assert(nullptr != expected);

        return __atomic_compare_exchange_n(
            &_cas_value, reinterpret_cast<cas_type*>(expected),
            *reinterpret_cast<const cas_type*>(&desired), false,
            to_builtin_memorder(success), to_builtin_memorder(failure));
    }

    bool compare_exchange_strong(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                            std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        assert(nullptr != expected);
        const int builtin_order = to_builtin_memorder(order);
        return __atomic_compare_exchange_n(
            &_cas_value, reinterpret_cast<cas_type*>(expected),
            *reinterpret_cast<const cas_type*>(&desired), false,
            builtin_order, builtin_order);
    }

private:
    static int to_builtin_memorder(std::memory_order order) noexcept
    {
        switch (order)
        {
        case std::memory_order_relaxed:
            return __ATOMIC_RELAXED;

        case std::memory_order_consume:
            return __ATOMIC_CONSUME;

        case std::memory_order_acquire:
            return __ATOMIC_ACQUIRE;

        case std::memory_order_release:
            return __ATOMIC_RELEASE;

        case std::memory_order_acq_rel:
            return __ATOMIC_ACQ_REL;

        case std::memory_order_seq_cst:
            return __ATOMIC_SEQ_CST;
        }

        assert(false);
        return __ATOMIC_SEQ_CST;
    }

private:
    cas_type _cas_value = 0;
};

#else

/**
 * 基于 std::atomic<> 实现
 */
template <typename T>
class alignas(sizeof(StampedPtr<void>)) AtomicStampedPtr
{
public:
    AtomicStampedPtr() noexcept
    {
        assert(_stamped_ptr.is_lock_free());
    }

    AtomicStampedPtr(T *p, typename StampedPtr<T>::stamp_type s) noexcept
        : _stamped_ptr(StampedPtr<T>(p, s))
    {
        assert(_stamped_ptr.is_lock_free());
    }

    StampedPtr<T> load(std::memory_order order = std::memory_order_seq_cst) const noexcept
    {
        return _stamped_ptr.load(order);
    }

    void store(const StampedPtr<T>& desired,
               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        _stamped_ptr.store(desired, order);
    }

    StampedPtr<T> exchange(const StampedPtr<T>& desired,
                           std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        return _stamped_ptr.exchange(desired, order);
    }

    bool compare_exchange_weak(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                               std::memory_order success, std::memory_order failure) noexcept
    {
        assert(nullptr != expected);
        return _stamped_ptr.compare_exchange_weak(*expected, desired, success, failure);
    }

    bool compare_exchange_weak(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        assert(nullptr != expected);
        return _stamped_ptr.compare_exchange_weak(*expected, desired, order);
    }

    bool compare_exchange_strong(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                                 std::memory_order success, std::memory_order failure) noexcept
    {
        assert(nullptr != expected);
        return _stamped_ptr.compare_exchange_strong(*expected, desired, success, failure);
    }

    bool compare_exchange_strong(StampedPtr<T> *expected, const StampedPtr<T>& desired,
                            std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        assert(nullptr != expected);
        return _stamped_ptr.compare_exchange_strong(*expected, desired, order);
    }

private:
    std::atomic<StampedPtr<T>> _stamped_ptr = ATOMIC_VAR_INIT(StampedPtr<T>());
};

#endif

static_assert(sizeof(AtomicStampedPtr<void>) == sizeof(void*) * 2, "AtomicStampedPtr<> size or align error");

#pragma pack()

}

#endif
