/**
 * @file -
 * @author jingqi
 * @date 2011-11-11 18:49
 */

#ifndef ___HEADFILE___9698630E_2BC3_42BB_91C2_734500687C95_
#define ___HEADFILE___9698630E_2BC3_42BB_91C2_734500687C95_

namespace nut
{

/**
 * 弱引用
 */
template <typename T>
class weak_ref
{
protected:
    T *m_ptr;

public:
    /** 默认构造函数 */
    weak_ref() : m_ptr(NULL) {}

    /** 构造函数 */
    weak_ref(T *ptr) : m_ptr(ptr) {}

    /** 隐式类型转换 */
    template <typename U>
    weak_ref(const weak_ref<U>& r) : m_ptr(r.pointer()) {}

    /** 复制构造函数(因为是特化模板，故必须放在上述模板函数的后面) */
    weak_ref(const weak_ref<T>& r) : m_ptr(r.m_ptr) {}

    ~weak_ref() { m_ptr = NULL; }

public:
    /** 赋值操作符 */
    inline weak_ref<T>& operator= (const weak_ref<T>& r)
    {
        m_ptr = r.m_ptr;
        return *this;
    }

    inline bool operator== (const weak_ref<T>& r) const { return m_ptr == r.m_ptr; }
    inline bool operator!= (const weak_ref<T>& r) const { return m_ptr != r.m_ptr; }

    T* operator-> () const { return m_ptr; }
    T& operator* () const { return *m_ptr; }

public:
    inline bool isNull() const { return m_ptr == NULL; }
    inline T* pointer() const { return m_ptr; }
    inline void assign(T *p) { m_ptr = p; }
    inline void clear() { m_ptr = NULL; }
};

/**
 * 强引用
 */
template <typename T>
class ref : public weak_ref<T>
{
protected:
    using weak_ref<T>::m_ptr;

public:
    ref() {}

    /** 类型转换 */
    explicit ref(T *p) { assign(p); }

    /** 类型转换 */
    template <typename U>
    explicit ref(const weak_ref<U>& r) { assign(r.pointer()); }

    /** 隐式类型转换 */
    template <typename U>
    ref(const ref<U>& r) { assign(r.pointer()); }

    /** 复制构造函数(因为是特化模板，故必须放在上述模板函数的后面) */
    ref(const ref<T>& r) { assign(r.m_ptr); }

    ~ref() { clear(); }

public:
    ref<T>& operator= (const ref<T>& r)
    {
        assign(r.m_ptr);
        return *this;
    }
	
	bool operator== (const weak_ref<T>& r) const
	{
		return weak_ref<T>::operator==(this, r);
	}

	bool operator!= (const weak_ref<T>& r) const
	{
		return weak_ref::operator!=(this, r);
	}

public:
    void assign(T *p)
    {
        // 先添加引用，以免先减少引用的话引发连锁反应
        if (p != NULL)
            p->add_ref();
        if (m_ptr != NULL)
            m_ptr->rls_ref();
        m_ptr = p;
    }

    void clear()
    {
        if (m_ptr != NULL)
        {
            m_ptr->rls_ref();
            m_ptr = NULL;
        }
    }
};

/* ******************************************************************/

template <typename T>
struct RefTraits
{
    typedef T plain_type;
};

template <typename T>
struct RefTraits<weak_ref<T> >
{
    typedef typename RefTraits<T>::plain_type plain_type;
};

template <typename T>
struct RefTraits<ref<T> >
{
    typedef typename RefTraits<T>::plain_type plain_type;
};

/* ******************************************************************/

template <typename T>
struct static_ref_cast : public ref<typename RefTraits<T>::plain_type>
{
    template <typename U>
    static_ref_cast(const weak_ref<U>& r)
    {
        assign(static_cast<typename RefTraits<T>::plain_type*>(r.pointer()));
    }
};

template <typename T>
struct dynamic_ref_cast : public ref<typename RefTraits<T>::plain_type>
{
    template <typename U>
    dynamic_ref_cast(const weak_ref<U>& r)
    {
        assign(dynamic_cast<typename RefTraits<T>::plain_type*>(r.pointer()));
    }
};

}


/**
 * 声明可引用计数
 * @note 多继承中如：
 *      A   B
 *       \ /
 *        C
 * 如果在A,B中使用了 DECLARE_GC_ENABLE 声明， 那么 C 中也要使用，
 * 否则会出现有歧义的调用
 */
#define NUT_GC_REFERABLE \
    virtual void add_ref() = 0; \
    virtual void rls_ref() = 0; \
    template <typename ___T> friend class nut::ref;


#endif /* head file guarder */
