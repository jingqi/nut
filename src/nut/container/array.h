
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
    const rc_ptr<memory_allocator> _alloc;
    T *_buf = NULL;
    size_type _size = 0, _cap = 0;

private:
    RCArray(const self_type&);

public:
    RCArray(size_type init_cap = 16, memory_allocator *ma = NULL)
        : _alloc(ma)
    {
        ensure_cap(init_cap);
    }

    ~RCArray()
    {
        clear();
        if (NULL != _buf)
            ma_free(_alloc.pointer(), _buf);
        _buf = NULL;
        _cap = 0;
    }

    memory_allocator* allocator() const
    {
        return _alloc.pointer();
    }

private:
    void ensure_cap(size_type new_size)
    {
        if (new_size <= _cap)
            return;

        size_type new_cap = _cap * 3 / 2;
        if (new_cap < new_size)
            new_cap = new_size;

        _buf = (T*) ma_realloc(_alloc.pointer(), _buf, sizeof(T) * new_cap);
        assert(NULL != _buf);
        _cap = new_cap;
    }

public:
    self_type& operator=(const self_type& x)
    {
        if (this != &x)
        {
            clear();
            ensure_cap(x._size);
            std::uninitialized_copy(x._buf, x._buf + x._size, _buf);
            _size = x._size;
        }
        return *this;
    }

    self_type& operator=(self_type&& x)
    {
        if (this != &x)
        {
            if (_alloc == x._alloc)
            {
                clear();
                if (NULL != _buf)
                    ma_free(_alloc.pointer(), _buf);

                _buf = x._buf;
                _size = x._size;
                _cap = x._cap;

                x._buf = NULL;
                x._size = 0;
                x._cap = 0;
            }
            else
            {
                clear();
                ensure_cap(x._size);
                std::uninitialized_copy(x._buf, x._buf + x._size, _buf);
                _size = x._size;
            }
        }
        return *this;
    }

    bool operator==(const self_type& x) const
    {
        if (this == &x)
            return true;
        if (_size != x._size)
            return false;
        for (size_type i = 0; i < _size; ++i)
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
        assert(i < _size);
        return _buf[i];
    }

    T& operator[](size_type i)
    {
        assert(i < _size);
        return const_cast<T&>(static_cast<const self_type&>(*this)[i]);
    }

public:
    typedef T* iterator;
    typedef const T* const_iterator;

    const_iterator begin() const
    {
        return _buf;
    }

    iterator begin()
    {
        return _buf;
    }

    const_iterator end() const
    {
        return _buf + _size;
    }

    iterator end()
    {
        return _buf + _size;
    }

public:
    rc_ptr<self_type> clone() const
    {
        rc_ptr<self_type> ret = rca_new<self_type,int>(_alloc.pointer(), _size, _alloc.pointer());
        std::uninitialized_copy(_buf, _buf + _size, ret->_buf);
		ret->_size = _size;
        return ret;
    }

    size_type size() const
    {
        return _size;
    }

    size_type capacity() const
    {
        return _cap;
    }

    const T& at(size_type i) const
    {
        assert(i < _size);
        return _buf[i];
    }

    T& at(size_type i)
    {
        assert(i < _size);
        return const_cast<T&>(static_cast<const self_type&>(*this).at(i));
    }

    void push_back(const T& e)
    {
        ensure_cap(_size + 1);
        new (_buf + _size) T(e);
        ++_size;
    }

    void pop_back()
    {
        assert(_size > 0);
        --_size;
        (_buf + _size)->~T();
    }

    void insert(size_type index, const T& e)
    {
        assert(index <= _size);
        ensure_cap(_size + 1);
        if (index < _size)
            ::memmove(_buf + index + 1, _buf + index, sizeof(T) * (_size - index));
        new (_buf + index) T(e);
        ++_size;
    }

    template <typename Iter>
    void insert(size_type index, const Iter& b, const Iter& e)
    {
        assert(index <= _size);
        const size_type len = e - b;
        ensure_cap(_size + len);
        if (index < _size)
            ::memmove(_buf + index + len, _buf + index, sizeof(T) * (_size - index));
		std::uninitialized_copy(b, e, _buf + index);
        _size += len;
    }

    void erase(size_type index)
    {
        assert(index < _size);
        (_buf + index)->~T();
        if (index < _size - 1)
            ::memmove(_buf + index, _buf + index + 1, sizeof(T) * (_size - index - 1));
        --_size;
    }

    void erase(size_type b, size_type e)
    {
        assert(b <= e && e <= _size);
        for (size_type i = b; i < e; ++i)
            (_buf + i)->~T();
        if (b < e && e < _size)
            ::memmove(_buf + b, _buf + e, sizeof(T) * (_size - e));
        _size -= e - b;
    }

    void resize(size_type new_size, const T& fill = T())
    {
        ensure_cap(new_size);
        for (size_type i = new_size; i < _size; ++i)
            (_buf + i)->~T();
		if (_size < new_size)
			std::uninitialized_fill_n(_buf + _size, new_size - _size, fill);
        _size = new_size;
    }

    void clear()
    {
        for (size_type i = 0; i < _size; ++i)
            (_buf + i)->~T();
        _size = 0;
    }

    const T* data() const
    {
        return _buf;
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
