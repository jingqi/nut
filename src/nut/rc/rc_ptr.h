
#ifndef ___HEADFILE___9698630E_2BC3_42BB_91C2_734500687C95_
#define ___HEADFILE___9698630E_2BC3_42BB_91C2_734500687C95_

#include <assert.h>


namespace nut
{

/**
 * 引用计数智能指针
 */
template <typename T>
class rc_ptr
{
    template <typename U> friend class rc_ptr;

public:
    constexpr rc_ptr() = default;

    /**
     * 类型转换
     */
    rc_ptr(T *p)
    {
        assign(p);
    }

    template <typename U>
    rc_ptr(rc_ptr<U>&& p)
        : _ptr(p._ptr)
    {
        p._ptr = nullptr;
    }

    /**
     * 隐式类型转换
     */
    template <typename U>
    rc_ptr(const rc_ptr<U>& p)
    {
        assign(p._ptr);
    }

    /**
     * 复制构造函数
     *  NOTE:
     *      1. 由于形式上是上一个模板函数的特化，必须放在上一个函数后面
     *      2. 复制构造函数必须显示声明, 否则编译器不会自动套用上述模板函数,
     *  反而会生成默认拷贝式默认复制构造函数
     */
    rc_ptr(const rc_ptr<T>& p)
    {
        assign(p._ptr);
    }

    ~rc_ptr()
    {
        set_null();
    }

    rc_ptr<T>& operator=(T *p)
    {
        assign(p);
        return *this;
    }

    template <typename U>
    rc_ptr<T>& operator=(rc_ptr<U>&& p)
    {
        if (_ptr == p._ptr)
            return *this;

        T *tmp = _ptr;
        _ptr = p._ptr;
        p._ptr = nullptr;
        if (nullptr != tmp)
            tmp->release_ref();

        return *this;
    }

    template <typename U>
    rc_ptr<T>& operator=(const rc_ptr<U>& p)
    {
        assign(p._ptr);
        return *this;
    }

    /**
     * 赋值操作符
     * NOTE:
     *      (参看复制构造函数的说明)
     */
    rc_ptr<T>& operator=(const rc_ptr<T>& p)
    {
        assign(p._ptr);
        return *this;
    }

    constexpr operator T*() const
    {
        return _ptr;
    }

    constexpr bool operator==(const T *p) const
    {
        return _ptr == p;
    }

    template <typename U>
    constexpr bool operator==(const rc_ptr<U>& p) const
    {
        return _ptr == p._ptr;
    }

    constexpr bool operator!=(const T *p) const
    {
        return _ptr != p;
    }

    template <typename U>
    constexpr bool operator!=(const rc_ptr<U>& p) const
    {
        return _ptr != p._ptr;
    }

    constexpr T* operator->() const
    {
#if __cplusplus >= 201402L // C++14 constexpr function feature
        assert(nullptr != _ptr);
#endif
        return _ptr;
    }

    constexpr T& operator*() const
    {
#if __cplusplus >= 201402L // C++14 constexpr function feature
        assert(nullptr != _ptr);
#endif
        return *_ptr;
    }

    void assign(T *p)
    {
        if (_ptr == p)
            return;

        // NOTE 先添加引用，以免减少 '_ptr' 的引用引发连锁反应，导致 'p' 指向的
        //      对象被意外回收
        if (p != nullptr)
            p->add_ref();
        if (_ptr != nullptr)
            _ptr->release_ref();
        _ptr = p;
    }

    constexpr T* pointer() const
    {
        return _ptr;
    }

    constexpr bool is_null() const
    {
        return nullptr == _ptr;
    }

    constexpr bool is_not_null() const
    {
        return nullptr != _ptr;
    }

    void set_null()
    {
        if (_ptr != nullptr)
        {
            _ptr->release_ref();
            _ptr = nullptr;
        }
    }

protected:
    T *_ptr = nullptr;
};

/* ******************************************************************/

template <typename T>
struct RCPtrTraits
{
    typedef T plain_type;
};

template <typename T>
struct RCPtrTraits<rc_ptr<T> >
{
    typedef typename RCPtrTraits<T>::plain_type plain_type;
};

/* ******************************************************************/

template <typename T>
struct static_rc_ptr_cast : public rc_ptr<typename RCPtrTraits<T>::plain_type>
{
    using rc_ptr<typename RCPtrTraits<T>::plain_type>::assign;

    template <typename U>
    static_rc_ptr_cast(const rc_ptr<U>& p)
    {
        assign(static_cast<typename RCPtrTraits<T>::plain_type*>(p.pointer()));
    }
};

template <typename T>
struct dynamic_rc_ptr_cast : public rc_ptr<typename RCPtrTraits<T>::plain_type>
{
    using rc_ptr<typename RCPtrTraits<T>::plain_type>::assign;

    template <typename U>
    dynamic_rc_ptr_cast(const rc_ptr<U>& p)
    {
        assign(dynamic_cast<typename RCPtrTraits<T>::plain_type*>(p.pointer()));
    }
};

}

/* ******************************************************************/

/**
 * 声明可引用计数
 *
 * @note
 *  多继承中如：
 *      A   B
 *       \ /
 *        C
 *  如果在A,B中声明了 NUT_REF_COUNTABLE 声明，那么 C 中也要声明，否则会出现有歧
 *  义的函数调用
 *
 * @return 引用计数更改之后的值
 */
#define NUT_REF_COUNTABLE                                   \
    virtual int add_ref() const noexcept = 0;               \
    virtual int release_ref() const noexcept = 0;           \
    virtual int get_ref() const noexcept = 0;               \
    template <typename ___T> friend class ::nut::rc_ptr;

#define NUT_REF_COUNTABLE_OVERRIDE                          \
    virtual int add_ref() const noexcept override = 0;      \
    virtual int release_ref() const noexcept override = 0;  \
    virtual int get_ref() const noexcept override = 0;      \
    template <typename ___T> friend class ::nut::rc_ptr;

#endif /* head file guarder */
