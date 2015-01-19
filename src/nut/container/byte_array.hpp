/**
 * @file -
 * @author jingqi
 * @date 2010-7-22
 * @last-edit 2015-01-06 20:05:47 jingqi
 */

#ifndef ___HEADFILE___5FC80ECA_3FD3_11E0_A703_485B391122F8_
#define ___HEADFILE___5FC80ECA_3FD3_11E0_A703_485B391122F8_


#include <assert.h>
#include <string.h>  /* for memset() and memcpy() */
#include <stdlib.h>  /* for malloc() and free() */
#include <stdio.h> /* for size_t */
#include <stdint.h>
#include <string>

#include <nut/mem/sys_ma.hpp>
#include "array.hpp"

namespace nut
{

template<typename MemAlloc = sys_ma>
class ByteArray
{
    typedef ByteArray<MemAlloc> self_type;
    typedef RCArray<uint8_t,MemAlloc> rcarray_type;

public:
    typedef typename rcarray_type::size_type size_type;
    typedef typename rcarray_type::iterator iterator;
    typedef typename rcarray_type::const_iterator const_iterator;

private:
    rcarray_type *m_array;

    /**
     * 写前复制
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

public :
    ByteArray(MemAlloc *ma = NULL)
        : m_array(NULL)
    {
        m_array = rcarray_type::create(0, ma);
    }

    /**
     * @param len initial data size
     * @param fillv initial data filling
     */
    explicit ByteArray(size_type len, uint8_t fillv = 0, MemAlloc *ma = NULL)
        : m_array(NULL)
    {
        m_array = rcarray_type::create(len, ma);
        m_array->resize(len, fillv);
    }

    /**
     * @param buf initial data
     * @param len initial data size
     */
    ByteArray(const void *buf, size_type len, MemAlloc *ma = NULL)
        : m_array(NULL)
    {
        assert(NULL != buf);
        m_array = rcarray_type::create(len, ma);
        if (NULL == buf)
            return;
        m_array->insert(0, (const uint8_t*)buf, ((const uint8_t*)buf) + len);
    }

    /**
     * @param term_byte the initial data terminated with 'term_byte'
     * @param include_term_byte if True, the 'term_byte' is part of initial data
     */
    ByteArray(const void *buf, unsigned char term_byte, bool include_term_byte, MemAlloc *ma = NULL)
        : m_array(NULL)
    {
        assert(NULL != buf);
        if (NULL == buf)
        {
            m_array = rcarray_type::create(0, ma);
            return;
        }

        size_type len = 0;
        while (((const uint8_t*)buf)[len] != term_byte)
            ++len;
        if (include_term_byte)
            ++len;

        m_array = rcarray_type::create(len, ma);
        m_array->insert(0, (const uint8_t*)buf, ((const uint8_t*)buf) + len);
    }

    ByteArray(const void *buf, size_type index, size_type size, MemAlloc *ma = NULL)
        : m_array(NULL)
    {
        assert(NULL != buf);
        m_array = rcarray_type::create(size, ma);
        if (NULL == buf)
            return;
        m_array->insert(0, ((const uint8_t*)buf) + index, ((const uint8_t*)buf) + index + size);
    }

    ByteArray(const self_type& x)
        : m_array(x.m_array)
    {
        m_array->add_ref();
    }

    ~ByteArray()
    {
        m_array->rls_ref();
        m_array = NULL;
    }

    /**
     * clear data
     */
    void clear()
    {
        copy_on_write();
        m_array->clear();
    }

    self_type& operator=(const self_type& x)
    {
        x.m_array->add_ref();
        m_array->rls_ref();
        m_array = x.m_array;
        return *this;
    }

    /**
     * works like memcmp()
     */
    bool operator==(const self_type& x) const
    {
        return m_array->operator==(x.m_array);
    }

    bool operator!=(const self_type& x) const
    {
        return m_array->operator!=(x.m_array);
    }

    const uint8_t& operator[](size_type idx) const
    {
        return m_array->operator[](idx);
    }

    uint8_t& operator[](size_type idx)
    {
        copy_on_write();
        return m_array->operator[](idx);
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

    const uint8_t& at(size_type idx) const
    {
        return m_array->at(idx);
    }

    uint8_t& at(size_type idx)
    {
        copy_on_write();
        return m_array->at(idx);
    }

    /**
     * resize the data size
     * @param fillv, If new bytes are added, this will fill them
     */
    void resize(size_type n, uint8_t fillv = 0)
    {
        copy_on_write();
        m_array->resize(n, fillv);
    }

    void append(const self_type &x)
    {
        copy_on_write();
        m_array->insert(size(), x.begin(), x.end());
    }

    void append(size_type len, uint8_t fillv = 0)
    {
        copy_on_write();
        m_array->resize(size() + len, fillv);
    }

    void append(const void *buf, size_t len)
    {
        assert(0 == len || NULL != buf);
        if (0 == len || NULL == buf)
            return;

        copy_on_write();
        m_array->insert(size(), (const uint8_t*)buf, ((const uint8_t*)buf) + len);
    }

    void append(const void *buf, uint8_t term_byte, bool include_term_byte)
    {
        assert(NULL != buf);
        if (NULL == buf)
            return;

        size_t len = 0;
        while (((const uint8_t*)buf)[len] != term_byte)
            ++len;
        if (include_term_byte)
            ++len;
        append(buf, len);
    }

    void append(const void *buf, size_t index, size_t size)
    {
        assert(NULL != buf);
        if (NULL == buf)
            return;

        append(((const uint8_t*)buf) + index, size);
    }

    const uint8_t* data() const
    {
        return m_array->data();
    }

    uint8_t* data()
    {
        copy_on_write();
        return m_array->data();
    }

    size_t length() const
    {
        return size();
    }

    size_t size() const
    {
        return m_array->size();
    }

    /**
     * 将二进制 0x51 0x5e 0x30 转换为字符串 "515e30"
     */
    std::string to_string(size_t from = 0, size_t to = ~(size_t)0) const
    {
        assert(from <= to);

        std::string ret;
        const size_t limit = (size() < to ? size() : to);
        for (size_t i = from; i < limit; ++i)
        {
            const uint8_t b = at(i);
            int n = (b >> 4) & 0xF;
            ret.push_back(n < 10 ? ('0' + n) : ('A' + (n - 10)));

            n = b & 0xF;
            ret.push_back(n < 10 ? ('0' + n) : ('A' + (n - 10)));
        }
        return ret;
    }

    std::wstring to_wstring(size_t from = 0, size_t to = ~(size_t)0) const
    {
        assert(from <= to);

        std::wstring ret;
        const size_t limit = (size() < to ? size() : to);
        for (size_t i = from; i < limit; ++i)
        {
            const uint8_t b = at(i);
            int n = (b >> 4) & 0xF;
            ret.push_back(n < 10 ? (L'0' + n) : (L'A' + (n - 10)));

            n = b & 0xF;
            ret.push_back(n < 10 ? (L'0' + n) : (L'A' + (n - 10)));
        }
        return ret;
    }

    /**
     * 将类似于 "1234ab" 的字符串转为二进制 0x12 0x34 0xab
     * NOTE:
     *      如果限定范围的字符串长度不是偶数，最后一个字符被忽略
     *      如果字符串中间出现非16进制字符，则转换过程立即停止
     */
    static self_type value_of(const std::string& s, size_t from = 0, size_t to = ~(size_t)0)
    {
        assert(from <= to);

        self_type ret;
        const size_t limit = (s.length() < to ? s.length() : to);
        for (size_t i = 0; from + i * 2 + 1 < limit; ++i)
        {
            int p1, p2;
            char c = s.at(from + i * 2);
            if ('0' <= c && c <= '9')
                p1 = c - '0';
            else if (('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
                p1 = (c | 0x20) - 'a' + 10;
            else
                break;

            c = s.at(from + i * 2 + 1);
            if ('0' <= c && c <= '9')
                p2 = c - '0';
            else if (('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
                p2 = (c | 0x20) - 'a' + 10;
            else
                break;

            ret.append(1, (uint8_t)(p1 * 16 + p2));
        }
        return ret;
    }

    static self_type value_of(const std::wstring& s, size_t from = 0, size_t to = ~(size_t)0)
    {
        assert(from <= to);

        self_type ret;
        const size_t limit = (s.length() < to ? s.length() : to);
        for (size_t i = 0; from + i * 2 + 1 < limit; ++i)
        {
            int p1, p2;
            wchar_t c = s.at(from + i * 2);
            if (L'0' <= c && c <= L'9')
                p1 = c - L'0';
            else if ((L'a' <= c && c <= L'f') || (L'A' <= c && c <= L'F'))
                p1 = (c & 0x20) - L'a' + 10;
            else
                break;

            c = s.at(from + i * 2 + 1);
            if (L'0' <= c && c <= L'9')
                p2 = c - L'0';
            else if ((L'a' <= c && c <= L'f') || (L'A' <= c && c <= L'F'))
                p2 = (c & 0x20) - L'a' + 10;
            else
                break;

            ret.append(1, (uint8_t)(p1 * 16 + p2));
        }
        return ret;
    }
};

}

#endif /* head file guarder */
