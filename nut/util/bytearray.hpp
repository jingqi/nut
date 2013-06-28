/**
 * @file -
 * @author jingqi
 * @date 2010-7-22
 */

#ifndef ___HEADFILE___5FC80ECA_3FD3_11E0_A703_485B391122F8_
#define ___HEADFILE___5FC80ECA_3FD3_11E0_A703_485B391122F8_


#include <assert.h>
#include <string.h>  /* for memset() and memcpy() */
#include <stdlib.h>  /* for malloc() and free() */
#include <stdio.h> /* for size_t */
#include <stdint.h>
#include <string>

#include <nut/memtool/copyonwrite.hpp>

namespace nut
{

class ByteArray
{
    // 使用 copy-on-write 技术
    ref<FixedBuf<uint8_t> > m_buf;
    size_t m_datalen;

private :
    /**
     * 写数据之前调用
     *
     * @param new_size 下一步操作需要的缓冲区大小，用于分配下一步的空间
     * @param extra_space 是否多分配一些空间，否则使用 new_size 作为新缓冲区大小
     */
    void _copy_on_write(size_t new_size = 0, bool extra_space = true)
    {
        new_size = (new_size > m_datalen ? new_size : m_datalen);

        // buffer 为 null, 则需要生成
        if (m_buf.isNull())
        {
            assert(0 == m_datalen);
            if (new_size > 0)
                m_buf = gc_new<FixedBuf<uint8_t>, RefCounterSync>(new_size);
            return;
        }

        // 原本 buffer 足够长，则 copy-on-write
        const int rc = m_buf->get_ref();
        assert(rc >= 1);
        const size_t old_cap = m_buf->len;
        if (old_cap >= new_size)
        {
            if (rc > 1)
            {
                ref<FixedBuf<uint8_t> > new_buf = gc_new<FixedBuf<uint8_t>, RefCounterSync>(new_size);
                ::memcpy(new_buf->buf, m_buf->buf, m_datalen * sizeof(uint8_t));
                m_buf = new_buf;
            }
            return;
        }

        // new capacity
        const size_t new_cap = (!extra_space ? new_size :
            (old_cap * 3 / 2 < new_size ? new_size : old_cap * 3 / 2));
        if (rc == 1)
        {
            m_buf->realloc(new_cap);
        }
        else
        {
            ref<FixedBuf<uint8_t> > new_buf = gc_new<FixedBuf<uint8_t>, RefCounterSync>(new_cap);
            ::memcpy(new_buf->buf, m_buf->buf, m_datalen * sizeof(uint8_t));
            m_buf = new_buf;
        }
    }

public :
    ByteArray() : m_datalen(0) {}

    /**
     * @param len initial data size
     * @param fillv initial data filling
     */
    explicit ByteArray(size_t len, uint8_t fillv = 0)
        : m_datalen(0)
    {
        _copy_on_write(len, false);
        ::memset(m_buf->buf, fillv, len * sizeof(uint8_t));
        m_datalen = len;
    }

    /**
     * @param buf initial data
     * @param len initial data size
     */
    ByteArray(const void *buf, size_t len)
        : m_datalen(0)
    {
        assert(NULL != buf);
        if (NULL == buf)
            return;

        _copy_on_write(len, false);
        ::memcpy(m_buf->buf, buf, len * sizeof(uint8_t));
        m_datalen = len;
    }

    /**
     * @param termByte the initial data terminated with 'termByte'
     * @param includeTermByte if True, the 'termByte' is part of initial data
     */
    ByteArray(const void *buf, unsigned char termByte, bool includeTermByte)
        : m_datalen(0)
    {
        assert(NULL != buf);
        if (NULL == buf)
            return;

        size_t len = 0;
        while (((const uint8_t*)buf)[len] != termByte)
            ++len;
        if (includeTermByte)
            ++len;

        _copy_on_write(len, false);
        ::memcpy(m_buf->buf, buf, len * sizeof(uint8_t));
        m_datalen = len;
    }

    ByteArray(const void *buf, size_t index, size_t size)
        : m_datalen(0)
    {
        assert(NULL != buf);
        if (NULL == buf)
            return;

        _copy_on_write(size, false);
        ::memcpy(m_buf->buf, ((const uint8_t*)buf) + index, size * sizeof(uint8_t));
        m_datalen = size;
    }

    ByteArray(const ByteArray& x)
        : m_buf(x.m_buf), m_datalen(x.m_datalen) // copy-on-write
    {}

    ~ByteArray()
    {
        m_datalen = 0;
    }

    /**
     * clear data
     */
    void clear() { m_datalen = 0; }

    ByteArray& operator=(const ByteArray &x)
    {
        if (&x == this)
            return *this;

        // copy-on-write
        m_buf = x.m_buf;
        m_datalen = x.m_datalen;
        return *this;
    }

    /**
     * works like memcmp()
     */
    bool operator==(const ByteArray &x) const
    {
        if (m_datalen != x.m_datalen)
            return false;
        if (m_buf == x.m_buf)
            return true;
        for (register size_t i = 0; i < m_datalen; ++i)
            if (m_buf->buf[i] != x.m_buf->buf[i])
                return false;
        return true;
    }

    bool operator!=(const ByteArray &x) const { return !(*this == x); }

    const uint8_t& operator[](size_t idx) const
    {
        assert(idx < m_datalen);
        return m_buf->buf[idx];
    }

    uint8_t& operator[](size_t idx)
    {
        assert(idx < m_datalen);
        return const_cast<uint8_t&>(static_cast<const ByteArray&>(*this)[idx]);
    }

    /**
     * resize the data size
     * @param fillv, If new bytes are added, this will fill them
     */
    void resize(size_t n, uint8_t fillv = 0)
    {
        if (n > m_datalen)
        {
            _copy_on_write(n);
            ::memset(m_buf->buf + m_datalen, fillv, n - m_datalen);
        }
        m_datalen = n;
    }

    void append(const ByteArray &x)
    {
        _copy_on_write(m_datalen + x.m_datalen);
        ::memcpy(m_buf->buf + m_datalen, x.m_buf->buf, x.m_datalen);
        m_datalen += x.m_datalen;
    }

    void append(size_t len, uint8_t fillv = 0)
    {
        _copy_on_write(m_datalen + len);
        ::memset(m_buf->buf + m_datalen, fillv, len);
        m_datalen += len;
    }

    void append(const void *buf, size_t len)
    {
        assert(0 == len || NULL != buf);
        if (0 == len || NULL == buf)
            return;

        _copy_on_write(m_datalen + len);
        ::memcpy(m_buf->buf + m_datalen, buf, len);
        m_datalen += len;
    }

    void append(const void *buf, uint8_t termByte, bool includeTermByte)
    {
        assert(NULL != buf);
        if (NULL == buf)
            return;

        size_t len = 0;
        while (((const uint8_t*)buf)[len] != termByte)
            ++len;
        if (includeTermByte)
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

    /**
     * get the naked pointer
     */
    inline const uint8_t* buffer() const
    {
        if (m_buf.isNull())
            return NULL;
        return m_buf->buf;
    }

    inline uint8_t* buffer()
    {
        _copy_on_write();
        return const_cast<uint8_t*>(static_cast<const ByteArray&>(*this).buffer());
    }

    inline size_t length() const { return size(); }

    inline size_t size() const { return m_datalen; }

    inline size_t capasity() const { return m_buf->len; }

    /**
     * 将二进制 0x51 0x5e 0x30 转换为字符串 "515e30"
     */
    std::string toString(size_t from = 0, size_t to = ~(size_t)0) const
    {
        assert(from <= to);

        std::string ret;
        const size_t limit = (m_datalen < to ? m_datalen : to);
        for (register size_t i = from; i < limit; ++i)
        {
            const uint8_t b = m_buf->buf[i];
            int n = (b >> 4) & 0xF;
            ret.push_back(n < 10 ? ('0' + n) : ('A' + (n - 10)));

            n = b & 0xF;
            ret.push_back(n < 10 ? ('0' + n) : ('A' + (n - 10)));
        }
        return ret;
    }

    std::wstring toWString(size_t from = 0, size_t to = ~(size_t)0) const
    {
        assert(from <= to);

        std::wstring ret;
        const size_t limit = (m_datalen < to ? m_datalen : to);
        for (register size_t i = from; i < limit; ++i)
        {
            const uint8_t b = m_buf->buf[i];
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
    static ByteArray valueOf(const std::string& s, size_t from = 0, size_t to = ~(size_t)0)
    {
        assert(from <= to);

        ByteArray ret;
        const size_t limit = (s.length() < to ? s.length() : to);
        for (register size_t i = 0; from + i * 2 + 1 < limit; ++i)
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

    static ByteArray valueOf(const std::wstring& s, size_t from = 0, size_t to = ~(size_t)0)
    {
        assert(from <= to);

        ByteArray ret;
        const size_t limit = (s.length() < to ? s.length() : to);
        for (register size_t i = 0; from + i * 2 + 1 < limit; ++i)
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

