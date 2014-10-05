/**
 * @file -
 * @author jingqi
 * @date 2012-07-14
 * @last-edit 2014-10-05 17:15:21 jingqi
 */

#ifndef ___HEADFILE_D320BBBE_2351_4EBC_AEBA_E426DF662D53_
#define ___HEADFILE_D320BBBE_2351_4EBC_AEBA_E426DF662D53_

#include <assert.h>

namespace nut
{

template <typename T>
class ptr
{
protected:
    T *m_ptr;

public:
    /** 构造函数 */
    ptr(T *ptr = NULL)
        : m_ptr(ptr)
    {}

    /** 隐式类型转换 */
    template <typename U>
    ptr(const ptr<U>& r)
        : m_ptr(r.pointer())
    {}

    /** 复制构造函数(因为是特化模板，故必须放在上述模板函数的后面) */
    ptr(const ptr<T>& r)
        : m_ptr(r.m_ptr)
    {}

    ~ptr()
    {
        m_ptr = NULL;
    }

public:
    inline ptr<T>& operator=(T* p)
    {
        m_ptr = p;
    }

    inline ptr<T>& operator=(const ptr<T>& r)
    {
        m_ptr = r.m_ptr;
        return *this;
    }

    inline operator T*() const
    {
        return m_ptr;
    }

    inline bool operator==(const T* r) const
    {
        return m_ptr == r;
    }

    inline bool operator!=(const T* r) const
    {
        return m_ptr != r;
    }

    inline bool operator==(const ptr<T>& r) const
    {
        return m_ptr == r.m_ptr;
    }

    inline bool operator!=(const ptr<T>& r) const
    {
        return m_ptr != r.m_ptr;
    }

    inline T* operator->() const
    {
        assert(NULL != m_ptr);
        return m_ptr;
    }

    inline T& operator*() const
    {
        assert(NULL != m_ptr);
        return *m_ptr;
    }

public:
    inline bool is_null() const
    {
        return m_ptr == NULL;
    }

    inline T* pointer() const
    {
        return m_ptr;
    }

    inline void assign(T *p)
    {
        m_ptr = p;
    }

    inline void clear()
    {
        m_ptr = NULL;
    }
};

}

#endif
