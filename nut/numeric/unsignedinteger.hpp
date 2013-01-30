/**
 * @file -
 * @author jingqi
 * @date 2011-12-17
 * @last-edit 2011-12-17 17:07:50 jingqi
 */

#ifndef ___HEADFILE_070364DB_A9E6_411B_A19E_5F9711AFD997_
#define ___HEADFILE_070364DB_A9E6_411B_A19E_5F9711AFD997_

#include <assert.h>
#include "byte_array_number.hpp"

namespace nut
{

template <size_t N>
class UnsignedInteger
{
    uint8_t m_bytes[N];

public:
    UnsignedInteger()
    {
        ::memset(m_bytes, 0, N);
    }

    explicit UnsignedInteger(unsigned long v)
    {
        expand_unsigned(reinterpret_cast<const uint8_t*>(&v), sizeof(v), m_bytes, N);
    }

    UnsignedInteger(const uint8_t *buf, size_t len)
    {
        assert(NULL != buf);
        expand_unsigned(buf, len, m_bytes, N);
    }

    template <size_t M>
    explicit UnsignedInteger(const UnsignedInteger<M>& x)
    {
        unsigned_expand(x.m_bytes, M, m_bytes, N);
    }

    UnsignedInteger(const UnsignedInteger<N>& x)
    {
        ::memcpy(m_bytes, x.m_bytes, N);
    }

#ifndef NDEBUG
    ~UnsignedInteger()
    {
        ::memset(m_bytes, 0xFE, N);
    }
#endif

public:
    UnsignedInteger<N>& operator=(const UnsignedInteger<N>& x)
    {
        ::memcpy(m_bytes, x.m_bytes, N);
        return *this;
    }

    const uint8_t& operator[](size_t i) const
    {
        assert(0 <= i && i < N);
        return m_bytes[i];
    }

    uint8_t& operator[](size_t i)
    {
        assert(0 <= i && i < N);
        return const_cast<uint8_t&>(static_cast<const UnsignedInteger<N>&>(*this)[i]);
    }

    UnsignedInteger<N> operator+(const UnsignedInteger<N>& x) const
    {
        UnsignedInteger<N> ret;
        add(m_bytes, x.m_bytes, ret.m_bytes, N);
        return ret;
    }

    UnsignedInteger<N> operator-(const UnsignedInteger<N>& x) const
    {
        UnsignedInteger<N> ret;
        sub(m_bytes, x.m_bytes, ret.m_bytes, N);
        return ret;
    }

    UnsignedInteger<N> operator*(const UnsignedInteger<N>& x) const
    {
        UnsignedInteger<N> ret;
        multiply(m_bytes, x.m_bytes, ret.m_bytes, N);
        return ret;
    }

    UnsignedInteger<N> operator/(const UnsignedInteger<N>& x) const
    {
        UnsignedInteger<N> ret;
        divide_unsigned(m_bytes, x.m_bytes, ret.m_bytes, NULL, N);
        return ret;
    }

    UnsignedInteger<N> operator%(const UnsignedInteger<N>& x) const
    {
        UnsignedInteger<N> ret;
        divide_unsigned(m_bytes, x.m_bytes, NULL, ret.m_bytes, N);
        return ret;
    }

    UnsignedInteger<N>& operator+=(const UnsignedInteger<N>& x)
    {
        add(m_bytes, x.m_bytes, m_bytes, N);
        return *this;
    }

    UnsignedInteger<N>& operator-=(const UnsignedInteger<N>& x)
    {
        sub(m_bytes, x.m_bytes, m_bytes, N);
        return *this;
    }

    UnsignedInteger<N>& operator*=(const UnsignedInteger<N>& x)
    {
        multiply(m_bytes, x.m_bytes, m_bytes, N);
        return *this;
    }

    UnsignedInteger<N>& operator/=(const UnsignedInteger<N>& x)
    {
        divide_unsigned(m_bytes, x.m_bytes, m_bytes, NULL, N);
        return *this;
    }

    UnsignedInteger<N>& operator%=(const UnsignedInteger<N>& x)
    {
        divide_unsigned(m_bytes, x.m_bytes, NULL, m_bytes, N);
        return *this;
    }

    UnsignedInteger<N>& operator++()
    {
        increase(m_bytes, N);
        return *this;
    }

    UnsignedInteger<N> operator++(int)
    {
        UnsignedInteger<N> ret(*this);
        increase(m_bytes, N);
        return ret;
    }

    UnsignedInteger<N>& operator--()
    {
        decrease(m_bytes, N);
        return *this;
    }

    UnsignedInteger<N> operator--(int)
    {
        UnsignedInteger<N> ret(*this);
        decrease(m_bytes, m_bytes, N);
        return ret;
    }

    UnsignedInteger<N> operator|(const  UnsignedInteger<N>& x) const
    {
        UnsignedInteger<N> ret;
        bit_or(m_bytes, x.m_bytes, ret.m_bytes, N);
        return ret;
    }

    UnsignedInteger<N> operator&(const UnsignedInteger<N>& x) const
    {
        UnsignedInteger<N> ret;
        bit_and(m_bytes, x.m_bytes, ret.m_bytes, N);
        return ret;
    }

    UnsignedInteger<N> operator^(const UnsignedInteger<N>& x) const
    {
        UnsignedInteger<N> ret;
        bit_xor(m_bytes, x.m_bytes, ret.m_bytes, N);
        return ret;
    }

    UnsignedInteger<N> operator~() const
    {
        UnsignedInteger<N> ret;
        bit_not(m_bytes, ret.m_bytes, N);
        return ret;
    }

    UnsignedInteger<N>& operator|=(const UnsignedInteger<N>& x)
    {
        bit_or(m_bytes, x.m_bytes, m_bytes, N);
        return *this;
    }

    UnsignedInteger<N>& operator&=(const UnsignedInteger<N>& x)
    {
        bit_and(m_bytes, x.m_bytes, m_bytes, N);
        return *this;
    }

    UnsignedInteger<N>& operator^=(const UnsignedInteger<N>& x)
    {
        bit_xor(m_bytes, x.m_bytes, m_bytes, N);
        return *this;
    }

    UnsignedInteger<N> operator<<(size_t count) const
    {
        UnsignedInteger<N> ret;
        shift_left(m_bytes, ret.m_bytes, N, count);
        return ret;
    }

    UnsignedInteger<N> operator>>(size_t count) const
    {
        UnsignedInteger<N> ret;
        shift_right_unsigned(m_bytes, ret.m_bytes, N, count);
        return ret;
    }

    UnsignedInteger<N>& operator<<=(size_t count) const
    {
        shift_left(m_bytes, m_bytes, N, count);
        return *this;
    }

    UnsignedInteger<N>& operator>>=(size_t count) const
    {
        shift_right_unsigned(m_bytes, m_bytes, N, count);
        return *this;
    }

    bool operator==(const UnsignedInteger<N>& x) const
    {
        return 0 == ::memcmp(m_bytes, x.m_bytes, N);
    }

    bool operator!=(const UnsignedInteger<N>& x) const
    {
        return !(*this == x);
    }

    bool operator<(const UnsignedInteger<N>& x) const
    {
        return less_then_unsigned(m_bytes, x.m_bytes, N);
    }

    bool operator>(const UnsignedInteger<N>& x) const
    {
        return x < *this;
    }

    bool operator<=(const UnsignedInteger<N>& x) const
    {
        return !(x < *this);
    }

    bool operator>=(const UnsignedInteger<N>& x) const
    {
        return !(*this < x);
    }

public:
    /**
     * 循环左移
     */
    UnsignedInteger<N> circle_shift_left(size_t count) const
    {
        UnsignedInteger<N> ret;
        circle_shift_left(m_bytes, ret.m_bytes, N, count);
        return ret;
    }

    /**
     * 循环左移
     */
    UnsignedInteger<N>& self_circle_shift_left(size_t count) const
    {
        circle_shift_left(m_bytes, m_bytes, N, count);
        return *this;
    }

    /**
     * 循环右移
     */
    UnsignedInteger<N> circle_shift_right(size_t count) const
    {
        UnsignedInteger<N> ret;
        circle_shift_right(m_bytes, ret.m_bytes, N, count);
        return ret;
    }

    /**
     * 循环右移
     */
    UnsignedInteger<N>& self_circle_shift_right(size_t count) const
    {
        circle_shift_right(m_bytes, m_bytes, N, count);
        return *this;
    }

    bool is_zero() const
    {
        return nut::is_zero(m_bytes, N);
    }

    const uint8_t* buffer() const
    {
        return m_bytes;
    }

    uint8_t* buffer()
    {
        return const_cast<uint8_t*>(static_cast<const UnsignedInteger<N>&>(*this).buffer());
    }

    /**
     * 能够存储数据而不丢失符号的最小字节数组长度
     *
     * @return 返回值>=1
     */
    int significant_size() const
    {
        return nut::significant_size_unsigned(m_bytes, N);
    }

    unsigned long ulong_value() const
    {
        unsigned long ret = 0;
        expand_signed(m_bytes, N, reinterpret_cast<uint8_t*>(&ret), sizeof(ret));
        return ret;
    }

    /**
     * @return 0 or 1
     */
    int bit_at(size_t i) const
    {
        assert(i < N * 8);
        return (m_bytes[i / 8] >> (i % 8)) & 0x01;
    }
};

}

#endif /* head file guarder */

