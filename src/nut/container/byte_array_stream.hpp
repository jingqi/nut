/**
 * @file -
 * @author jingqi
 * @date 2013-10-26
 * @last-edit 2015-01-06 20:26:15 jingqi
 * @brief
 */

#ifndef ___HEADFILE_27BA84B2_4461_4A06_A9AE_5E2471B5521C_
#define ___HEADFILE_27BA84B2_4461_4A06_A9AE_5E2471B5521C_

#include <assert.h>

#include "byte_array.hpp"

namespace nut
{

/**
 * 处理字节数组流中的大端(Big-Endian)和小端(Little-Endian)字节序读写
 */
template <typename MemAlloc = sys_ma>
class ByteArrayStream
{
    typedef ByteArrayStream<MemAlloc> self_type;
    typedef ByteArray<MemAlloc> array_type;

    array_type m_data;
    size_t m_index;
    bool m_little_endian;

public:
    ByteArrayStream(MemAlloc *ma = NULL)
        : m_data(ma), m_index(0), m_little_endian(true)
    {}

    ByteArrayStream(const self_type& data)
        : m_data(data.m_data), m_index(0), m_little_endian(true)
    {}

    bool is_little_endian() const
    {
        return m_little_endian;
    }

    void set_little_endian(bool little_endian = true)
    {
        m_little_endian = little_endian;
    }

    size_t size() const
    {
        return m_data.size();
    }

    size_t tell() const
    {
        return m_index;
    }

    void seek(size_t index)
    {
        assert(index <= m_data.size());
        m_index = index;
    }

    void resize(size_t new_size)
    {
        m_data.resize(new_size);
        if (m_index > new_size)
            m_index = new_size;
    }

    void skip(size_t cb)
    {
        assert(m_index + cb <= m_data.size());
        m_index += cb;
    }

    uint8_t read_uint8()
    {
        assert(m_index + 1 <= m_data.size());
        const array_type& data = m_data;
        return data.at(m_index++);
    }

    uint16_t read_uint16()
    {
        assert(m_index + 2 <= m_data.size());
        const array_type& data = m_data;
        uint16_t ret = 0;
        if (m_little_endian)
        {
            ret = data.at(m_index++);
            ret += (uint16_t) (((uint16_t) data.at(m_index++)) << 8);
        }
        else
        {
            ret = (uint16_t) (((uint16_t) data.at(m_index++)) << 8);
            ret += data.at(m_index++);
        }
        return ret;
    }

    uint32_t read_uint32()
    {
        assert(m_index + 4 <= m_data.size());
        const array_type& data = m_data;
        uint32_t ret = 0;
        if (m_little_endian)
        {
            ret = data.at(m_index++);
            ret += (uint32_t) (((uint32_t) data.at(m_index++)) << 8);
            ret += (uint32_t) (((uint32_t) data.at(m_index++)) << 16);
            ret += (uint32_t) (((uint32_t) data.at(m_index++)) << 24);
        }
        else
        {
            ret = (uint32_t) (((uint32_t) data.at(m_index++)) << 24);
            ret += (uint32_t) (((uint32_t) data.at(m_index++)) << 16);
            ret += (uint32_t) (((uint32_t) data.at(m_index++)) << 8);
            ret += data.at(m_index++);
        }
        return ret;
    }

    uint64_t read_uint64()
    {
        assert(m_index + 8 <= m_data.size());
        const array_type& data = m_data;
        uint64_t ret = 0;
        if (m_little_endian)
        {
            ret = data.at(m_index++);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 8);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 16);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 24);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 32);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 40);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 48);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 56);
        }
        else
        {
            ret = (uint64_t) (((uint64_t) data.at(m_index++)) << 56);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 48);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 40);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 32);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 24);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 16);
            ret += (uint64_t) (((uint64_t) data.at(m_index++)) << 8);
            ret += data.at(m_index++);
        }
        return ret;
    }

    float read_float()
    {
        assert(m_index + sizeof(float) <= m_data.size());
        float ret = 0;
        for (size_t i = 0; i < sizeof(float); ++i)
            reinterpret_cast<uint8_t*>(&ret)[m_little_endian ? i : (sizeof(float) - i - 1)] = read_uint8();
        return ret;
    }

    double read_double()
    {
        assert(m_index + sizeof(double) <= m_data.size());
        double ret = 0;
        for (size_t i = 0; i < sizeof(double); ++i)
            reinterpret_cast<uint8_t*>(&ret)[m_little_endian ? i : (sizeof(double)-i - 1)] = read_uint8();
        return ret;
    }

    size_t read_bytes(void *buf, size_t len)
    {
        assert(NULL != buf || 0 == len);
        const array_type& data = m_data;
        size_t ret = (len < data.size() - m_index ? len : data.size() - m_index);
        ::memcpy(buf, data.data() + m_index, ret);
        m_index += ret;
        return ret;
    }

    void write_uint8(uint8_t v)
    {
        if (m_index < m_data.size())
            m_data[m_index] = v;
        else
            m_data.append(1, v);
        ++m_index;
    }

    void write_uint16(uint16_t v)
    {
        for (size_t i = 0; i < sizeof(uint16_t); ++i)
        {
            const uint8_t b = (uint8_t) (m_little_endian ? (v >> (8 * i)) : (v >> (8 * (2 - i - 1))));
            if (m_index < m_data.size())
                m_data[m_index] = b;
            else
                m_data.append(1, b);
            ++m_index;
        }
    }

    void write_uint32(uint32_t v)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            const uint8_t b = (uint8_t) (m_little_endian ? (v >> (8 * i)) : (v >> (8 * (4 - i - 1))));
            if (m_index < m_data.size())
                m_data[m_index] = b;
            else
                m_data.append(1, b);
            ++m_index;
        }
    }

    void write_uint64(uint64_t v)
    {
        for (size_t i = 0; i < 8; ++i)
        {
            const uint8_t b = (uint8_t) (m_little_endian ? (v >> (8 * i)) : (v >> (8 * (8 - i - 1))));
            if (m_index < m_data.size())
                m_data[m_index] = b;
            else
                m_data.append(1, b);
            ++m_index;
        }
    }

    void write_bytes(const void *buf, size_t len)
    {
        assert(NULL != buf || 0 == len);
        assert(m_index <= m_data.size());
        const size_t copy = m_data.size() - m_index;
        ::memcpy(m_data.data() + m_index, buf, copy);
        m_data.append(((const uint8_t*) buf) + copy, len - copy);
        m_index += len;
    }

    void write_bytes(const array_type& ba)
    {
        if (ba.size() == 0)
            return;
        write_bytes(ba.data(), ba.size());
    }

    void write_float(float v)
    {
        for (size_t i = 0; i < sizeof(float); ++i)
            write_uint8(reinterpret_cast<const uint8_t*>(&v)[m_little_endian ? i : (sizeof(float) - i - 1)]);
    }

    void write_double(double v)
    {
        for (size_t i = 0; i < sizeof(double); ++i)
            write_uint8(reinterpret_cast<const uint8_t*>(&v)[m_little_endian ? i : (sizeof(double) - i - 1)]);
    }

    const array_type& byte_array() const
    {
        return m_data;
    }
};

}

#endif
