
#ifndef ___HEADFILE_5B961A47_38A6_4B4F_AFAA_21B942953392_
#define ___HEADFILE_5B961A47_38A6_4B4F_AFAA_21B942953392_

#include <nut/platform/platform.h>
#include <nut/platform/stdint_traits.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "atomic.h"


namespace nut
{

/**
 * 为了避免 ABA 问题而引入的带标签的指针
 *
 * 解决 ABA 问题有几个方案：
 * 1. 由于指针地址一般是对齐的，用指针末尾的几个 bit 做一个时间戳 tag
 *    缺点是 tag 位数少，可能 wrap 后依然出现 ABA 问题; 如果指针不对齐(不推荐，但是有可能出现)，则
 *    不适用
 * 2. 在指针后附加一个大字段来表示时间戳，也就是下面的方案。
 *    缺点是，操作位数变长，性能有所损失
 */
template <typename T>
class StampedPtr
{
public:
#if NUT_PLATFORM_BITS_64
    typedef uint64_t stamp_type;
#elif NUT_PLATFORM_BITS_32
    typedef uint32_t stamp_type;
#elif NUT_PLATFORM_BITS_16
    typedef uint16_t stamp_type;
#else
#   error platform not supported!
#endif

private:
    // 整体对应的CAS操作数类型
#if NUT_PLATFORM_BITS_64
#   if !NUT_PLATFORM_OS_WINDOWS || !NUT_PLATFORM_CC_VC
    typedef uint128_t cas_type;
#   endif
#elif NUT_PLATFORM_BITS_32
    typedef uint64_t cas_type;
#elif NUT_PLATFORM_BITS_16
    typedef uint32_t cas_type;
#else
#   error platform not supported!
#endif

    // FIXME 在 Windows 系统上的 MinGW 编译器有个bug: 会忽略 union 内的 "就地初始化"
    //       (c++11新特性，直接在成员后加 "=" 号来初始化)。
    //       虽然在编译时没有报错，但是会导致该成员在运行时处于未初始化状态
    union _StampedPtr
    {
        struct
        {
            T * volatile _ptr_value;
            stamp_type volatile _stamp_value;
        };

#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        struct
        {
            uint64_t volatile _low_part;
            uint64_t volatile _high_part;
        };
#else
        cas_type volatile _whole_value;
#endif
    } _stamped_ptr;

public:
    StampedPtr()
    {
        clear();
    }

    explicit StampedPtr(T *ptr, stamp_type stamp)
    {
        set(ptr, stamp);
    }

    bool operator==(const StampedPtr<T>& x) const
    {
        return pointer() == x.pointer() && stamp_value() == x.stamp_value();
    }

    bool operator!=(const StampedPtr<T>& x) const
    {
        return !(*this == x);
    }

    void clear()
    {
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        _stamped_ptr._low_part = 0;
        _stamped_ptr._high_part = 0;
#else
        _stamped_ptr._whole_value = 0;
#endif
    }

    void set(T *ptr, stamp_type stamp)
    {
        _stamped_ptr._ptr_value = ptr;
        _stamped_ptr._stamp_value = stamp;
    }

    void set_pointer(T *ptr)
    {
        _stamped_ptr._ptr_value = ptr;
    }

    T* pointer() const
    {
        return _stamped_ptr._ptr_value;
    }


    stamp_type stamp_value() const
    {
        return _stamped_ptr._stamp_value;
    }

    /**
     * cas 操作
     */
    bool compare_and_set(const StampedPtr<T>& comparand, T *ptr)
    {
        return compare_and_set(comparand, ptr, comparand.stamp_value() + 1);
    }

    /**
     * cas 操作
     */
    bool compare_and_set(const StampedPtr<T>& comparand, T *ptr, stamp_type stamp)
    {
        const StampedPtr<T> new_stamped_ptr(ptr, stamp);
        return compare_and_set(comparand, new_stamped_ptr);
    }

    /**
     * cas 操作
     */
    bool compare_and_set(const StampedPtr<T>& comparand, const StampedPtr<T>& new_stamped_ptr)
    {
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_BITS_64 && NUT_PLATFORM_CC_VC
        return 1 == InterlockedCompareExchange128(&(_stamped_ptr._low_part),
                                                  new_stamped_ptr._stamped_ptr._high_part,
                                                  new_stamped_ptr._stamped_ptr._low_part,
                                                  &(comparand._stamped_ptr._low_part));
#else
        return atomic_cas(&(_stamped_ptr._whole_value),
                          comparand._stamped_ptr._whole_value,
                          new_stamped_ptr._stamped_ptr._whole_value);
#endif
    }
};

static_assert(sizeof(void*) == sizeof(StampedPtr<void>::stamp_type), "StampedPtr 数据结构对齐问题");
static_assert(sizeof(void*) * 2 == sizeof(StampedPtr<void>), "StampedPtr 数据结构对齐问题");

}

#endif
