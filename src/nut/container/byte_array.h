
#ifndef ___HEADFILE___5FC80ECA_3FD3_11E0_A703_485B391122F8_
#define ___HEADFILE___5FC80ECA_3FD3_11E0_A703_485B391122F8_

#include <assert.h>
#include <stdio.h> /* for size_t */
#include <stdint.h>
#include <string>

#include <nut/mem/sys_ma.h>

#include "array.h"

namespace nut
{

class ByteArray
{
    typedef ByteArray self_type;
    typedef RCArray<uint8_t> rcarray_type;

public:
    typedef rcarray_type::size_type size_type;
    typedef rcarray_type::iterator iterator;
    typedef rcarray_type::const_iterator const_iterator;

private:
    rc_ptr<rcarray_type> _array;

    /**
     * 写前复制
     */
    void copy_on_write();

public :
    ByteArray(memory_allocator *ma = NULL);

    /**
     * @param len initial data size
     * @param fillv initial data filling
     */
    explicit ByteArray(size_type len, uint8_t fillv = 0, memory_allocator *ma = NULL);

    /**
     * @param buf initial data
     * @param len initial data size
     */
    ByteArray(const void *buf, size_type len, memory_allocator *ma = NULL);

    /**
     * @param term_byte the initial data terminated with 'term_byte'
     * @param include_term_byte if True, the 'term_byte' is part of initial data
     */
    ByteArray(const void *buf, unsigned char term_byte, bool include_term_byte, memory_allocator *ma = NULL);

    ByteArray(const void *buf, size_type index, size_type size, memory_allocator *ma = NULL);

    ByteArray(const self_type& x);

    /**
     * clear data
     */
    void clear();

    self_type& operator=(const self_type& x);

    /**
     * works like memcmp()
     */
    bool operator==(const self_type& x) const
    {
        return _array->operator==(*x._array);
    }

    bool operator!=(const self_type& x) const
    {
        return !(*this == x);
    }

    const uint8_t& operator[](size_type idx) const
    {
        return _array->operator[](idx);
    }

    uint8_t& operator[](size_type idx)
    {
        copy_on_write();
        return _array->operator[](idx);
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

    const uint8_t& at(size_type idx) const
    {
        return _array->at(idx);
    }

    uint8_t& at(size_type idx)
    {
        copy_on_write();
        return _array->at(idx);
    }

    /**
     * resize the data size
     * @param fillv, If new bytes are added, this will fill them
     */
    void resize(size_type n, uint8_t fillv = 0)
    {
        copy_on_write();
        _array->resize(n, fillv);
    }

    void append(const self_type &x)
    {
        copy_on_write();
        _array->insert(size(), x.begin(), x.end());
    }

    void append(size_type len, uint8_t fillv = 0)
    {
        copy_on_write();
        _array->resize(size() + len, fillv);
    }

    void append(const void *buf, size_t len);

    void append(const void *buf, uint8_t term_byte, bool include_term_byte);

    void append(const void *buf, size_t index, size_t size)
    {
        assert(NULL != buf);
        if (NULL == buf)
            return;

        append(((const uint8_t*)buf) + index, size);
    }

    const uint8_t* data() const
    {
        return _array->data();
    }

    uint8_t* data()
    {
        copy_on_write();
        return _array->data();
    }

    size_t length() const
    {
        return size();
    }

    size_t size() const
    {
        return _array->size();
    }

    /**
     * 将二进制 0x51 0x5e 0x30 转换为字符串 "515e30"
     */
    void to_string(std::string *appended, size_t from = 0, size_t to = ~(size_t)0) const;

    void to_string(std::wstring *appended, size_t from = 0, size_t to = ~(size_t)0) const;

    /**
     * 将类似于 "1234ab" 的字符串转为二进制 0x12 0x34 0xab
     * NOTE:
     *      如果限定范围的字符串长度不是偶数，最后一个字符被忽略
     *      如果字符串中间出现非16进制字符，则转换过程立即停止
     */
    static self_type value_of(const std::string& s, size_t from = 0, size_t to = ~(size_t)0);

    static self_type value_of(const std::wstring& s, size_t from = 0, size_t to = ~(size_t)0);
};

}

#endif /* head file guarder */
