
#ifndef ___HEADFILE_A6EE8581_AE7D_42C8_A4F8_0F10CE4F4E7E_
#define ___HEADFILE_A6EE8581_AE7D_42C8_A4F8_0F10CE4F4E7E_


namespace nut
{

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
    typedef int stamp_type;

    T *ptr;
    stamp_type stamp;

public:
    StampedPtr()
        : ptr(nullptr), stamp(0)
    {}

    StampedPtr(T *p, stamp_type s)
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
        stamp = p;
    }
};

}

#endif
