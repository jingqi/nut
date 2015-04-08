/**
 * @file -
 * @author jingqi
 * @date 2012-07-14
 * @last-edit 2015-01-06 22:35:46 jingqi
 */

#ifndef ___HEADFILE_D320BBBE_2351_4EBC_AEBA_E426DF662D53_
#define ___HEADFILE_D320BBBE_2351_4EBC_AEBA_E426DF662D53_

#include <assert.h>

namespace nut
{

/**
 * 辅助工具，用于便捷的指针NULL初始化
 */
template <typename T>
class ptr
{
protected:
    T *m_ptr;

public:
    /**
     * 构造函数
     */
    ptr(T *p = NULL)
        : m_ptr(p)
    {}

    /**
     * 隐式类型转换(该模板不会影响默认复制构造函数)
     */
    template <typename U>
    ptr(const ptr<U>& p)
        : m_ptr(p.pointer())
    {}

    // 默认复制构造函数，保持不变

    ~ptr()
    {
        m_ptr = NULL;
    }

public:
    ptr<T>& operator=(T* p)
    {
        m_ptr = p;
    }

    /**
     * (该模板不会影响默认赋值操作符)
     */
    template <typename U>
    ptr<T>& operator=(const ptr<U>& p)
    {
        m_ptr = p.m_ptr;
        return *this;
    }

    // 默认赋值操作符，保持不变

    operator T*() const
    {
        return m_ptr;
    }

    bool operator==(const T* p) const
    {
        return m_ptr == p;
    }

    template <typename U>
    bool operator==(const ptr<U>& p) const
    {
        return m_ptr == p.m_ptr;
    }

    bool operator!=(const T* p) const
    {
        return m_ptr != p;
    }

    template <typename U>
    bool operator!=(const ptr<U>& p) const
    {
        return m_ptr != p.m_ptr;
    }

    T* operator->() const
    {
        assert(NULL != m_ptr);
        return m_ptr;
    }

    T& operator*() const
    {
        assert(NULL != m_ptr);
        return *m_ptr;
    }

public:
    void assign(T *p)
    {
        m_ptr = p;
    }

    T* pointer() const
    {
        return m_ptr;
    }

    bool is_null() const
    {
        return NULL == m_ptr;
    }

    bool is_not_null() const
    {
        return NULL != m_ptr;
    }

    void set_null()
    {
        m_ptr = NULL;
    }
};

}

#endif
