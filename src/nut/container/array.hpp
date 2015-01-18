/**
 * @file -
 * @author jingqi
 * @date 2015-01-18
 * @last-edit 2015-01-18 01:11:46 jingqi
 * @brief
 */

#ifndef ___HEADFILE_035AE1B2_4A92_4FE9_AA5C_A73C6B52447C_
#define ___HEADFILE_035AE1B2_4A92_4FE9_AA5C_A73C6B52447C_

#include <string.h>

#include <nut/mem/sys_ma.hpp>

namespace nut
{

template <typename T, typename MemAlloc = sys_ma>
class RCArray
{
    typedef RCArray<T,MemAlloc> self_type;

public:
    typedef size_t size_type;

private:
    int volatile m_ref_count;
    MemAlloc *const m_alloc;
    T *m_buf;
    size_type m_size, m_cap;

private:
    explicit RCArray(const self_type&);

    RCArray(size_type init_cap, MemAlloc *ma)
        : m_ref_count(0), m_alloc(ma), m_buf(NULL), m_size(0), m_cap(0)
    {
        if (NULL != m_alloc)
            m_alloc->add_ref();
        ensure_cap(init_cap);
    }

    ~RCArray()
    {
        clear();
        if (NULL != m_buf)
        {
            if (NULL != m_alloc)
                m_alloc->free(m_buf);
            else
                ::free(m_buf);
        }
        m_buf = NULL;
        m_cap = 0;
        if (NULL != m_alloc)
            m_alloc->rls_ref();
    }

public:
    static self_type* create(size_type init_cap = 16, MemAlloc *ma = NULL)
    {
        self_type *ret = NULL;
        if (NULL != ma)
            ret = (self_type*) ma->alloc(sizeof(self_type));
        else
            ret = (self_type*) ::malloc(sizeof(self_type));
        assert(NULL != ret);
        new (ret) self_type(init_cap, ma);
        ret->add_ref();
        return ret;
    }

    int add_ref()
    {
        return atomic_add(&m_ref_count, 1) + 1;
    }

    int rls_ref()
    {
        const int ret = atomic_add(&m_ref_count, -1) - 1;
        if (0 == ret)
        {
            MemAlloc *const ma = m_alloc;
            if (NULL != ma)
                ma->add_ref();
            this->~RCArray();
            if (NULL != ma)
            {
                ma->free(this);
                ma->rls_ref();
            }
            else
            {
                ::free(this);
            }
        }
        return ret;
    }

    int get_ref() const
    {
        return m_ref_count;
    }

private:
    void ensure_cap(size_type new_size)
    {
        if (new_size <= m_cap)
            return;
        size_type new_cap = m_cap * 3 / 2;
        if (new_cap < new_size)
            new_cap = new_size;
        if (NULL == m_buf)
        {
            if (NULL != m_alloc)
                m_buf = (T*) m_alloc->alloc(sizeof(T) * new_cap);
            else
                m_buf = (T*) ::malloc(sizeof(T) * new_cap);
        }
        else
        {
            if (NULL != m_alloc)
                m_buf = (T*) m_alloc->realloc(m_buf, sizeof(T) * new_cap);
            else
                m_buf = (T*) ::realloc(m_buf, sizeof(T) * new_cap);
        }
        assert(NULL != m_buf);
        m_cap = new_cap;
    }

public:
    self_type& operator=(const self_type& x)
    {
        clear();
        ensure_cap(x.m_size);
        for (size_type i = 0; i < x.m_size; ++i)
            new (m_buf + i) T(x.m_buf[i]);
        m_size = x.m_size;
        return *this;
    }

    bool operator==(const self_type& x) const
    {
        if (this == &x)
            return true;
        if (m_size != x.m_size)
            return false;
        for (size_type i = 0; i < m_size; ++i)
        {
            if (at(i) != x.at(i))
                return false;
        }
        return true;
    }

    bool operator!=(const self_type& x) const
    {
        return !(*this == x);
    }

    const T& operator[](size_type i) const
    {
        assert(i < m_size);
        return m_buf[i];
    }

    T& operator[](size_type i)
    {
        assert(i < m_size);
        return const_cast<T&>(static_cast<const self_type&>(*this)[i]);
    }

public:
    typedef T* iterator;
    typedef const T* const_iterator;

    const_iterator begin() const
    {
        return m_buf;
    }

    iterator begin()
    {
        return m_buf;
    }

    const_iterator end() const
    {
        return m_buf + m_size;
    }

    iterator end()
    {
        return m_buf + m_size;
    }

public:
    self_type* clone() const
    {
        self_type *ret = create(m_size, m_alloc);
        ret->insert(0, m_buf, m_buf + m_size);
        return ret;
    }

    size_type size() const
    {
        return m_size;
    }

    size_type capacity() const
    {
        return m_cap;
    }

    const T& at(size_type i) const
    {
        assert(i < m_size);
        return m_buf[i];
    }

    T& at(size_type i)
    {
        assert(i < m_size);
        return const_cast<T&>(static_cast<const self_type&>(*this).at(i));
    }

    void push_back(const T& e)
    {
        ensure_cap(m_size + 1);
        new (m_buf + m_size) T(e);
        ++m_size;
    }

    void pop_back()
    {
        assert(m_size > 0);
        --m_size;
        (m_buf + m_size)->~T();
    }

    void insert(size_type index, const T& e)
    {
        assert(index <= m_size);
        ensure_cap(m_size + 1);
        if (index < m_size)
            ::memmove(m_buf + index + 1, m_buf + index, sizeof(T) * (m_size - index));
        new (m_buf + index) T(e);
        ++m_size;
    }

    template <typename Iter>
    void insert(size_type index, const Iter& b, const Iter& e)
    {
        assert(index <= m_size);
        const size_type len = e - b;
        ensure_cap(m_size + len);
        if (index < m_size)
            ::memmove(m_buf + index + len, m_buf + index, sizeof(T) * (m_size - index));
        for (size_type i = 0; i < len; ++i)
            new (m_buf + index + i) T(*(b + i));
        m_size += len;
    }

    void erase(size_type index)
    {
        assert(index < m_size);
        (m_buf + index)->~T();
        if (index < m_size - 1)
            ::memmove(m_buf + index, m_buf + index + 1, sizeof(T) * (m_size - index - 1));
        --m_size;
    }

    void erase(size_type b, size_type e)
    {
        assert(b <= e && e <= m_size);
        for (size_type i = b; i < e; ++i)
            (m_buf + i)->~T();
        if (b < e && e < m_size)
            ::memmove(m_buf + b, m_buf + e, sizeof(T) * (m_size - e));
        m_size -= e - b;
    }

    void resize(size_type new_size, const T& fill = T())
    {
        ensure_cap(new_size);
        for (size_type i = new_size; i < m_size; ++i)
            (m_buf + i)->~T();
        for (size_type i = m_size; i < new_size; ++i)
            new (m_buf + i) T(fill);
        m_size = new_size;
    }

    void clear()
    {
        for (size_type i = 0; i < m_size; ++i)
            (m_buf + i)->~T();
        m_size = 0;
    }

    const T* data() const
    {
        return m_buf;
    }

    T* data()
    {
        return const_cast<T*>(static_cast<const self_type&>(*this).data());
    }
};

template <typename T, typename MemAlloc = sys_ma>
class Array
{
    typedef Array<T,MemAlloc> self_type;
    typedef RCArray<T,MemAlloc> rcarray_type;

public:
    typedef typename rcarray_type::size_type size_type;
    typedef typename rcarray_type::iterator iterator;
    typedef typename rcarray_type::const_iterator const_iterator;

private:
    rcarray_type *m_array;

    /**
     * 写时复制
     */
    void copy_on_write()
    {
        assert(NULL != m_array);
        const int rc = m_array->get_ref();
        assert(rc >= 1);
        if (rc > 1)
        {
            rcarray_type *x = m_array->clone();
            m_array->rls_ref();
            m_array = x;
        }
    }

public:
    Array(size_type init_cap = 16, MemAlloc *ma = NULL)
        : m_array(NULL)
    {
        m_array = rcarray_type::create(init_cap, ma);
    }

    Array(const self_type& x)
        : m_array(x.m_array)
    {
        m_array->add_ref();
    }

    ~Array()
    {
        m_array->rls_ref();
        m_array = NULL;
    }

    self_type& operator=(const self_type& x)
    {
        x.m_array->add_ref(); // if x is this, m_array will not be freeed
        m_array->rls_ref();
        m_array = x.m_array;
    }

    bool operator==(const self_type& x) const
    {
        return m_array->operator==(x.m_array);
    }

    bool operator!=(const self_type& x) const
    {
        return m_array->operator!=(x.m_array);
    }

    const T& operator[](size_type i) const
    {
        return m_array->operator[](i);
    }

    T& operator[](size_type i)
    {
        copy_on_write();
        return m_array->operator[](i);
    }

    const_iterator begin() const
    {
        return m_array->begin();
    }

    iterator begin()
    {
        copy_on_write();
        return m_array->begin();
    }

    const_iterator end() const
    {
        return m_array->end();
    }

    iterator end()
    {
        copy_on_write();
        return m_array->end();
    }

    size_type size() const
    {
        return m_array->size();
    }

    const T& at(size_type i) const
    {
        return m_array->at(i);
    }

    T& at(size_type i)
    {
        copy_on_write();
        return m_array->at(i);
    }

    void push_back(const T& e)
    {
        copy_on_write();
        m_array->push_back(e);
    }

    void pop_back()
    {
        copy_on_write();
        m_array->pop_back();
    }

    void insert(size_type index, const T& e)
    {
        copy_on_write();
        m_array->insert(index, e);
    }

    template<typename Iter>
    void insert(size_type index, const Iter& b, const Iter& e)
    {
        copy_on_write();
        m_array->insert(index, b, e);
    }

    void erase(size_type index)
    {
        copy_on_write();
        m_array->erase(index);
    }

    void erase(size_type b, size_type e)
    {
        copy_on_write();
        m_array->erase(b, e);
    }

    void resize(size_type new_size, const T& fill = T())
    {
        copy_on_write();
        m_array->resize(new_size, fill);
    }

    void clear()
    {
        copy_on_write();
        m_array->clear();
    }

    const T* data() const
    {
        return m_array->data();
    }

    T* data()
    {
        copy_on_write();
        return m_array->data();
    }
};

}

#endif
