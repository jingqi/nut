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

namespace nut
{

class MemBuffer
{
    uint8_t *m_buf;
    size_t m_buflen;
    size_t m_datalen;

private :
    /**
     * assure enought capacity
     */
    void _assure(size_t size_needed, bool extra_space = true)
    {
        if (m_buflen >= size_needed)
            return;

        /* new size */
        size_t new_buflen = (!extra_space ? size_needed :
            (m_buflen * 3 / 2 < size_needed ? size_needed : m_buflen * 3 / 2));

        /* allocate */
        uint8_t *new_buf = (uint8_t*)::malloc(new_buflen);
        assert(NULL != new_buf);

        /* copy memory */
        if (NULL != m_buf)
        {
            ::memcpy(new_buf, m_buf, m_datalen);
            ::free(m_buf);
        }
        m_buf = new_buf;
        m_buflen = new_buflen;
    }

public :
    MemBuffer() : m_buf(NULL), m_buflen(0), m_datalen(0) {}

    /**
     * @param len initial data size
     * @param fillv initial data filling
     */
    explicit MemBuffer(size_t len, uint8_t fillv = 0)
        : m_buf(NULL), m_buflen(0), m_datalen(0)
    {
        _assure(len, false);
        ::memset(m_buf, fillv, len);
        m_datalen = len;
    }

    /**
     * @param buf initial data
     * @param len initial data size
     */
    MemBuffer(const void *buf, size_t len)
        : m_buf(NULL), m_buflen(0), m_datalen(0)
    {
        assert(NULL != buf);
        if (NULL == buf)
            return;

        _assure(len, false);
        ::memcpy(m_buf, buf, len);
        m_datalen = len;
    }

    /**
     * @param termByte the initial data terminated with 'termByte'
     * @param includeTermByte if True, the 'termByte' is part of initial data
     */
    MemBuffer(const void *buf, unsigned char termByte, bool includeTermByte)
        : m_buf(NULL), m_buflen(0), m_datalen(0)
    {
        assert(NULL != buf);
        if (NULL == buf)
            return;

        size_t len = 0;
        while (((const uint8_t*)buf)[len] != termByte)
            ++len;
        if (includeTermByte)
            ++len;

        _assure(len, false);
        ::memcpy(m_buf, buf, len);
        m_datalen = len;
    }

    MemBuffer(const void *buf, size_t index, size_t size)
        : m_buf(NULL), m_buflen(0), m_datalen(0)
    {
        assert(NULL != buf);
        if (NULL == buf)
            return;

        _assure(size, false);
        ::memcpy(m_buf, ((const uint8_t*)buf) + index, size);
        m_datalen = size;
    }

    MemBuffer(const MemBuffer& x)
        : m_buf(NULL), m_buflen(0), m_datalen(0)
    {
        if (0 == x.m_datalen)
            return;

        _assure(x.m_datalen, false);
        ::memcpy(m_buf, x.m_buf, x.m_datalen);
        m_datalen = x.m_datalen;
    }

    ~MemBuffer()
    {
        if (NULL != m_buf)
            ::free(m_buf);
        m_buf = NULL;
        m_buflen = 0;
        m_datalen = 0;
    }

    /**
     * clear data
     */
    void clear() { m_datalen = 0; }

    MemBuffer& operator=(const MemBuffer &x)
    {
        if (&x == this)
            return *this;

        _assure(x.m_datalen, false);
        ::memcpy(m_buf, x.m_buf, x.m_datalen);
        m_datalen = x.m_datalen;
        return *this;
    }

    /**
     * works like memcmp()
     */
    bool operator==(const MemBuffer &x) const
    {
        if (m_datalen != x.m_datalen)
            return false;
        for (register size_t i = 0; i < m_datalen; ++i)
            if (m_buf[i] != x.m_buf[i])
                return false;
        return true;
    }

    bool operator!=(const MemBuffer &x) const { return !(*this == x); }

    const uint8_t& operator[](size_t idx) const
    {
        assert(idx < m_datalen);
        return m_buf[idx];
    }

    uint8_t& operator[](size_t idx)
    {
        assert(idx < m_datalen);
        return const_cast<uint8_t&>(static_cast<const MemBuffer&>(*this)[idx]);
    }

    /**
     * resize the data size
     * @param fillv, If new bytes are added, this will fill them
     */
    void resize(size_t n, uint8_t fillv = 0)
    {
        _assure(n);
        if (n > m_datalen)
            ::memset(m_buf + m_datalen, fillv, n - m_datalen);
        m_datalen = n;
    }

    void append(const MemBuffer &x)
    {
        _assure(m_datalen + x.m_datalen);
        ::memcpy(m_buf + m_datalen, x.m_buf, x.m_datalen);
        m_datalen += x.m_datalen;
    }

    void append(size_t len, uint8_t fillv = 0)
    {
        _assure(m_datalen + len);
        ::memset(m_buf + m_datalen, fillv, len);
        m_datalen += len;
    }

    void append(const void *buf, size_t len)
    {
        assert(0 == len || NULL != buf);
        if (0 == len || NULL == buf)
            return;

        _assure(m_datalen + len);
        ::memcpy(m_buf + m_datalen, buf, len);
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
    inline const uint8_t* buffer() const { return m_buf; }

    inline uint8_t* buffer() { return const_cast<uint8_t*>(static_cast<const MemBuffer&>(*this).buffer()); }

    inline size_t length() const { return size(); }

    inline size_t size() const { return m_datalen; }

    inline size_t capasity() const { return m_buflen; }
};

}

#endif /* head file guarder */

