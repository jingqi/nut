﻿
#ifndef ___HEADFILE_035AE1B2_4A92_4FE9_AA5C_A73C6B52447C_
#define ___HEADFILE_035AE1B2_4A92_4FE9_AA5C_A73C6B52447C_

#include <string.h>
#include <memory>
#include <algorithm>

#include <nut/rc/rc_new.h>
#include <nut/rc/enrc.h>


namespace nut
{

template <typename T>
class Array
{
public:
    typedef size_t   size_type;
    typedef T*       iterator;
    typedef const T* const_iterator;

private:
    typedef Array<T> self_type;

public:
    explicit Array(size_type init_cap = 16)
    {
        ensure_cap<T>(init_cap);
    }

    Array(size_type sz, const T& fillv)
    {
        ensure_cap<T>(sz);
        std::uninitialized_fill(_buf, _buf + sz, fillv);
        _size = sz;
    }

    Array(const T *data, size_type sz)
    {
        ensure_cap<T>(sz);
        std::uninitialized_copy(data, data + sz, _buf);
        _size = sz;
    }

    template <typename Iter>
    Array(const Iter& b, const Iter& e)
    {
        assert(b <= e);
        const size_t sz = e - b;
        ensure_cap<T>(sz);
        std::uninitialized_copy(b, e, _buf);
        _size = sz;
    }

    Array(self_type&& x)
        : _buf(x._buf), _size(x._size), _cap(x._cap)
    {
        x._buf = nullptr;
        x._size = 0;
        x._cap = 0;
    }

    Array(const self_type& x)
    {
        ensure_cap<T>(x._size);
        std::uninitialized_copy(x._buf, x._buf + x._size, _buf);
        _size = x._size;
    }

    ~Array()
    {
        clear();
        if (nullptr != _buf)
            ::free(_buf);
        _buf = nullptr;
        _cap = 0;
    }

    self_type& operator=(self_type&& x)
    {
        if (this == &x)
            return *this;

        clear();
        if (nullptr != _buf)
            ::free(_buf);

        _buf = x._buf;
        _size = x._size;
        _cap = x._cap;

        x._buf = nullptr;
        x._size = 0;
        x._cap = 0;

        return *this;
    }

    self_type& operator=(const self_type& x)
    {
        if (this == &x)
            return *this;

        clear();

        ensure_cap<T>(x._size);
        std::uninitialized_copy(x._buf, x._buf + x._size, _buf);
        _size = x._size;

        return *this;
    }

    bool operator==(const self_type& x) const
    {
        if (this == &x)
            return true;
        else if (_size != x._size)
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

    bool operator<(const self_type& x) const
    {
        return compare(x) < 0;
    }

    bool operator>(const self_type& x) const
    {
        return x < *this;
    }

    bool operator<=(const self_type& x) const
    {
        return !(x < *this);
    }

    bool operator>=(const self_type& x) const
    {
        return !(*this < x);
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

    int compare(const self_type& x) const
    {
        if (this == &x)
            return 0;

        size_type i = 0;
        for (; i < _size && i < x._size; ++i)
        {
            const int rs = compare(at(i), x.at(i));
            if (0 != rs)
                return rs;
        }
        return i < _size ? 1 : (i < x._size ? -1 : 0);
    }

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

    void push_back(T&& e)
    {
        ensure_cap<T>(_size + 1);
        new (_buf + _size) T(std::forward<T>(e));
        ++_size;
    }

    void push_back(const T& e)
    {
        ensure_cap<T>(_size + 1);
        new (_buf + _size) T(e);
        ++_size;
    }

    void pop_back()
    {
        assert(_size > 0);
        --_size;
        (_buf + _size)->~T();
    }

    void insert(size_type index, T&& e)
    {
        assert(index <= _size);
        ensure_cap<T>(_size + 1);
        if (index < _size)
            ::memmove(_buf + index + 1, _buf + index, sizeof(T) * (_size - index));
        new (_buf + index) T(std::forward<T>(e));
        ++_size;
    }

    void insert(size_type index, const T& e)
    {
        assert(index <= _size);
        ensure_cap<T>(_size + 1);
        if (index < _size)
            ::memmove(_buf + index + 1, _buf + index, sizeof(T) * (_size - index));
        new (_buf + index) T(e);
        ++_size;
    }

    template <typename Iter>
    void insert(size_type index, const Iter& b, const Iter& e)
    {
        assert(index <= _size && b <= e);
        const size_type len = e - b;
        ensure_cap<T>(_size + len);
        if (index < _size)
            ::memmove(_buf + index + len, _buf + index, sizeof(T) * (_size - index));
        std::uninitialized_copy(b, e, _buf + index);
        _size += len;
    }

    template <typename Iter>
    void append(const Iter& b, const Iter& e)
    {
        insert(size(), b, e);
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
        ensure_cap<T>(new_size);
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

private:
    template <typename U>
    typename std::enable_if<std::is_trivially_copyable<U>::value, void>::type
    ensure_cap(size_t new_size)
    {
        if (new_size <= _cap)
            return;

        size_type new_cap = _cap * 3 / 2;
        if (new_cap < new_size)
            new_cap = new_size;

        _buf = (U*) ::realloc(_buf, sizeof(T) * new_cap);
        assert(nullptr != _buf);
        _cap = new_cap;
    }

    template <typename U>
    typename std::enable_if<!std::is_trivially_copyable<U>::value, void>::type
    ensure_cap(size_t new_size)
    {
        if (new_size <= _cap)
            return;

        size_type new_cap = _cap * 3 / 2;
        if (new_cap < new_size)
            new_cap = new_size;

        U *new_buf = (U*) ::malloc(sizeof(U) * new_cap);
        assert(nullptr != new_buf);
        for (size_t i = 0; i < _size; ++i)
        {
            new (new_buf + i) U(std::move(_buf[i]));
            (_buf + i)->~U();
        }
        ::free(_buf);
        _buf = new_buf;
        _cap = new_cap;
    }

private:
    T *_buf = nullptr;
    size_type _size = 0, _cap = 0;
};

/**
 * copy-on-write array
 */
template <typename T>
class COWArray
{
public:
    typedef typename Array<T>::size_type      size_type;
    typedef typename Array<T>::iterator       iterator;
    typedef typename Array<T>::const_iterator const_iterator;

private:
    typedef Array<T>         array_type;
    typedef enrc<array_type> rcarray_type;
    typedef COWArray<T>      self_type;

public:
    COWArray(size_type init_cap = 16)
        : _array(rc_new<rcarray_type>(init_cap))
    {}

    COWArray(size_type sz, const T& fillv)
        : _array(rc_new<rcarray_type>(sz, fillv))
    {}

    COWArray(const T *data, size_type sz)
        : _array(rc_new<rcarray_type>(data, sz))
    {}

    template <typename Iter>
    COWArray(const Iter& b, const Iter& e)
        : _array(rc_new<rcarray_type>(b, e))
    {}

    COWArray(const self_type& x)
        : _array(x._array)
    {}

    self_type& operator=(const self_type& x)
    {
        _array = x._array;
        return *this;
    }

    bool operator==(const self_type& x) const
    {
        return _array->operator==(*x._array);
    }

    bool operator!=(const self_type& x) const
    {
        return _array->operator!=(*x._array);
    }

    bool operator<(const self_type& x) const
    {
        return _array->operator<(*x._array);
    }

    bool operator>(const self_type& x) const
    {
        return _array->operator>(*x._array);
    }

    bool operator<=(const self_type& x) const
    {
        return _array->operator<=(*x._array);
    }

    bool operator>=(const self_type& x) const
    {
        return _array->operator>=(*x._array);
    }

    const T& operator[](size_type i) const
    {
        return _array->operator[](i);
    }

    T& operator[](size_type i)
    {
        copy_on_write();
        return _array->operator[](i);
    }

    int compare(const self_type& x) const
    {
        return _array->compare(*x._array);
    }

    const_iterator begin() const
    {
        return _array->begin();
    }

    iterator begin()
    {
        copy_on_write();
        return _array->begin();
    }

    const_iterator end() const
    {
        return _array->end();
    }

    iterator end()
    {
        copy_on_write();
        return _array->end();
    }

    size_type size() const
    {
        return _array->size();
    }

    const T& at(size_type i) const
    {
        return _array->at(i);
    }

    T& at(size_type i)
    {
        copy_on_write();
        return _array->at(i);
    }

    void push_back(T&& e)
    {
        copy_on_write();
        _array->push_back(std::forward<T>(e));
    }

    void push_back(const T& e)
    {
        copy_on_write();
        _array->push_back(e);
    }

    void pop_back()
    {
        copy_on_write();
        _array->pop_back();
    }

    void insert(size_type index, T&& e)
    {
        copy_on_write();
        _array->insert(index, std::forward<T>(e));
    }

    void insert(size_type index, const T& e)
    {
        copy_on_write();
        _array->insert(index, e);
    }

    template <typename Iter>
    void insert(size_type index, const Iter& b, const Iter& e)
    {
        copy_on_write();
        _array->insert(index, b, e);
    }

    template <typename Iter>
    void append(const Iter& b, const Iter& e)
    {
        copy_on_write();
        _array->append(b, e);
    }

    void erase(size_type index)
    {
        copy_on_write();
        _array->erase(index);
    }

    void erase(size_type b, size_type e)
    {
        copy_on_write();
        _array->erase(b, e);
    }

    void resize(size_type new_size, const T& fill = T())
    {
        copy_on_write();
        _array->resize(new_size, fill);
    }

    void clear()
    {
        const int rc = _array->get_ref();
        assert(rc >= 1);
        if (rc > 1)
            _array = rc_new<rcarray_type>(0);
        else
            _array->clear();
    }

    const T* data() const
    {
        return _array->data();
    }

    T* data()
    {
        copy_on_write();
        return _array->data();
    }

private:
    /**
     * 写时复制
     */
    void copy_on_write()
    {
        assert(_array.is_not_null());
        const int rc = _array->get_ref();
        assert(rc >= 1);
        if (rc > 1)
            _array = rc_new<rcarray_type>(*_array);
    }

private:
    rc_ptr<rcarray_type> _array;
};

}

#endif
