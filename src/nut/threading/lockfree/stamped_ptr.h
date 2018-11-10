
#ifndef ___HEADFILE_A6EE8581_AE7D_42C8_A4F8_0F10CE4F4E7E_
#define ___HEADFILE_A6EE8581_AE7D_42C8_A4F8_0F10CE4F4E7E_

#include <type_traits>


namespace nut
{

// FIXME 至少在 MacOS 上，放入 std::atomic<> 的结构体，大小必须是 1/2/4/8/16 字
//       节的，因为 CAS 操作的内存颗粒大小只能是这些。
//       eg. 结构体大小为 6 字节, CAS 操作每次依然对比 8 字节，会导致尾部内存参
//       与 CAS，一定概率导致 CAS 失败，甚至某个循环 CAS 无限卡住

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
#pragma pack(1)
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
    T *ptr;
    stamp_type stamp;
};
#pragma pack()

static_assert(
    sizeof(StampedPtr<void>) == 2 || sizeof(StampedPtr<void>) == 4 ||
    sizeof(StampedPtr<void>) == 8 || sizeof(StampedPtr<void>) == 16,
    "StampedPtr<> size or alignment error");

}

#endif
