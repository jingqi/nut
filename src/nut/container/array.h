
#ifndef ___HEADFILE_035AE1B2_4A92_4FE9_AA5C_A73C6B52447C_
#define ___HEADFILE_035AE1B2_4A92_4FE9_AA5C_A73C6B52447C_

#include <string.h>
#include <memory>
#include <algorithm>

#include <nut/mem/sys_ma.h>
#include <nut/rc/rc_new.h>

namespace nut
{

template <typename T>
class RCArray
{
    typedef RCArray<T> self_type;

public:
    typedef size_t size_type;
    NUT_REF_COUNTABLE

private:
    const rc_ptr<memory_allocator> m_alloc;
    T *m_buf;
    size_type m_size, m_cap;

private:
    RCArray(const self_type&);

public:
    RCArray(size_type init_cap = 16, memory_allocator *ma = NULL)
        : m_alloc(ma), m_buf(NULL), m_size(0), m_cap(0)
    {
        ensure_cap(init_cap);
    }

    ~RCArray()
    {
        clear();
        if (NULL != m_buf)
            ma_free(m_alloc.pointer(), m_buf);
        m_buf = NULL;
        m_cap = 0;
    }

    memory_allocator* allocator() const
    {
        return m_alloc.pointer();
    }

private:
    void ensure_cap(size_type new_size)
    {
        if (new_size <= m_cap)
            return;

        size_type new_cap = m_cap * 3 / 2;
        if (new_cap < new_size)
            new_cap = new_size;

        m_buf = (T*) ma_realloc(m_alloc.pointer(), m_buf, sizeof(T) * new_cap);
        assert(NULL != m_buf);
        m_cap = new_cap;
    }

public:
    self_type& operator=(const self_type& x)
    {
        clear();
        ensure_cap(x.m_size);
        std::uninitialized_copy(x.m_buf, x.m_buf + x.m_size, m_buf);
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
    rc_ptr<self_type> clone() const
    {
        rc_ptr<self_type> ret = rca_new<self_type,int>(m_alloc.pointer(), m_size, m_alloc.pointer());
        std::uninitialized_copy(m_buf, m_buf + m_size, ret->m_buf);
		ret->m_size = m_size;
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
		std::uninitialized_copy(b, e, m_buf + index);
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
		if (m_size < new_size)
			std::uninitialized_fill_n(m_buf + m_size, new_size - m_size, fill);
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

template <typename T>
class Array
{
    typedef Array<T> self_type;
    typedef RCArray<T> rcarray_type;

public:
    typedef typename rcarray_type::size_type size_type;
    typedef typename rcarray_type::iterator iterator;
    typedef typename rcarray_type::const_iterator const_iterator;

private:
    rc_ptr<rcarray_type> m_array;

    /**
     * 写时复制
     */
    void copy_on_write()
    {
        assert(m_array.is_not_null());
        const int rc = m_array->get_ref();
        assert(rc >= 1);
        if (rc > 1)
            m_array = m_array->clone();
    }

public:
    Array(size_type init_cap = 16, memory_allocator *ma = NULL)
        : m_array(rca_new<rcarray_type>(ma, init_cap, ma))
    {}

    Array(const self_type& x)
        : m_array(x.m_array)
    {}

    self_type& operator=(const self_type& x)
    {
        m_array = x.m_array;
        return *this;
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
