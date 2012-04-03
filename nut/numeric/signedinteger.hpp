/**
 * @file -
 * @author jingqi
 * @date 2011-12-16
 * @last-edit 2012-03-25 21:16:32 jingqi
 */
#ifndef ___HEADFILE_0CE9CDA7_BE91_4F03_AD03_F54E44BBC782_
#define ___HEADFILE_0CE9CDA7_BE91_4F03_AD03_F54E44BBC782_

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "byte_array_number.hpp"

namespace nut
{

template <size_t N>
class SignedInteger
{
    uint8_t m_bytes[N];

public :
    SignedInteger()
    {
        ::memset(m_bytes, 0, N);
    }

    explicit SignedInteger(long v)
    {
        expand_signed(reinterpret_cast<const uint8_t*>(&v), sizeof(v), m_bytes, N);
    }

    /** 
     * @param withSign
     *      传入的缓冲区是否带符号，如果不带符号，将被视为无符号正数
     */
    SignedInteger(const uint8_t *buf, size_t len, bool withSign)
    {
        assert(NULL != buf && len > 0);
        if (withSign)
            signed_expand(buf, len, m_bytes, N);
        else
            unsigned_expand(buf, len, m_bytes, N);
    }

    template <size_t M>
    explicit SignedInteger(const SignedInteger<M>& x)
    {
        signed_expand(x.m_bytes, M, m_bytes, N);
    }

    SignedInteger(const SignedInteger<N>& x)
    {
        ::memcpy(m_bytes, x.m_bytes, N);
    }

#ifndef NDEBUG
    ~SignedInteger()
    {
        ::memset(m_bytes, 0xFE, N);
    }
#endif

public :
    SignedInteger<N>& operator=(const SignedInteger<N>& x)
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
        return const_cast<uint8_t&>(static_cast<const SignedInteger<N>&>(*this)[i]);
    }

    SignedInteger<N> operator+(const SignedInteger<N>& x) const
    {
        SignedInteger<N> ret;
        add(m_bytes, x.m_bytes, ret.m_bytes, N);
        return ret;
    }

    SignedInteger<N> operator-(const SignedInteger<N>& x) const
    {
        SignedInteger<N> ret;
        sub(m_bytes, x.m_bytes, ret.m_bytes, N);
        return ret;
    }

	SignedInteger<N> operator-() const
	{
		SignedInteger<N> ret;
		signed_opposite(m_bytes, ret.m_bytes, N);
		return ret;
	}

    SignedInteger<N> operator*(const SignedInteger<N>& x) const
    {
        SignedInteger<N> ret;
        multiply(m_bytes, x.m_bytes, ret.m_bytes, N);
        return ret;
    }

	SignedInteger<N> operator/(const SignedInteger<N>& x) const
	{
		SignedInteger<N> ret;
		divide_signed(m_bytes, x.m_bytes, ret.m_bytes, NULL, N);
		return ret;
	}

	SignedInteger<N> operator%(const SignedInteger<N>& x) const
	{
		SignedInteger<N> ret;
		divide_signed(m_bytes, x.m_bytes, NULL, ret.m_bytes, N);
		return ret;
	}

    SignedInteger<N>& operator+=(const SignedInteger<N>& x)
    {
        add(m_bytes, x.m_bytes, m_bytes, N);
        return *this;
    }

    SignedInteger<N>& operator-=(const SignedInteger<N>& x)
    {
        sub(m_bytes, x.m_bytes, m_bytes, N);
        return *this;
    }

    SignedInteger<N>& operator*=(const SignedInteger<N>& x)
    {
        multiply(m_bytes, x.m_bytes, m_bytes, N);
        return *this;
    }

	SignedInteger<N>& operator/=(const SignedInteger<N>& x)
	{
		signed_divide(m_bytes, x.m_bytes, m_bytes, NULL, N);
		return *this;
	}

	SignedInteger<N>& operator%=(const SignedInteger<N>& x)
	{
		signed_divide(m_bytes, x.m_bytes, NULL, m_bytes, N);
		return *this;
	}

	SignedInteger<N>& operator++()
	{
		increase(m_bytes, m_bytes, N);
		return *this;
	}

	SignedInteger<N> operator++(int)
	{
		SignedInteger<N> ret(*this);
		increase(m_bytes, m_bytes, N);
		return ret;
	}

	SignedInteger<N>& operator--()
	{
		decrease(m_bytes, m_bytes, N);
		return *this;
	}

	SignedInteger<N> operator--(int)
	{
		SignedInteger<N> ret(*this);
		decrease(m_bytes, m_bytes, N);
		return ret;
	}

	SignedInteger<N> operator|(const  SignedInteger<N>& x) const
	{
		SignedInteger<N> ret;
		bit_or(m_bytes, x.m_bytes, ret.m_bytes, N);
		return ret;
	}

	SignedInteger<N> operator&(const SignedInteger<N>& x) const
	{
		SignedInteger<N> ret;
		bit_and(m_bytes, x.m_bytes, ret.m_bytes, N);
		return ret;
	}

	SignedInteger<N> operator^(const SignedInteger<N>& x) const
	{
		SignedInteger<N> ret;
		bit_xor(m_bytes, x.m_bytes, ret.m_bytes, N);
		return ret;
	}

	SignedInteger<N> operator~() const
	{
		SignedInteger<N> ret;
		bit_not(m_bytes, ret.m_bytes, N);
		return ret;
	}

	SignedInteger<N>& operator|=(const SignedInteger<N>& x)
	{
		bit_or(m_bytes, x.m_bytes, m_bytes, N);
		return *this;
	}

	SignedInteger<N>& operator&=(const SignedInteger<N>& x)
	{
		bit_and(m_bytes, x.m_bytes, m_bytes, N);
		return *this;
	}

	SignedInteger<N>& operator^=(const SignedInteger<N>& x)
	{
		bit_xor(m_bytes, x.m_bytes, m_bytes, N);
		return *this;
	}

	SignedInteger<N> operator<<(size_t count) const
	{
		SignedInteger<N> ret;
		shift_left(m_bytes, ret.m_bytes, N, count);
		return ret;
	}

	SignedInteger<N> operator>>(size_t count) const
	{
		SignedInteger<N> ret;
		shift_right_signed(m_bytes, ret.m_bytes, N, count);
		return ret;
	}

	SignedInteger<N>& operator<<=(size_t count) const
	{
		shift_left(m_bytes, m_bytes, N, count);
		return *this;
	}

	SignedInteger<N>& operator>>=(size_t count) const
	{
		signed_shift_right(m_bytes, m_bytes, N, count);
		return *this;
	}

    bool operator==(const SignedInteger<N>& x) const
    {
        return 0 == ::memcmp(m_bytes, x.m_bytes, N);
    }

    bool operator!=(const SignedInteger<N>& x) const
    {
        return !(*this == x);
    }

    bool operator<(const SignedInteger<N>& x) const
    {
        return less_then_signed(m_bytes, x.m_bytes, N);
    }

    bool operator>(const SignedInteger<N>& x) const
    {
        return x < *this;
    }

    bool operator<=(const SignedInteger<N>& x) const
    {
        return !(x < *this);
    }

    bool operator>=(const SignedInteger<N>& x) const
    {
        return !(*this < x);
    }

public:
	SignedInteger<N> circle_shift_left(size_t count) const
	{
		SignedInteger ret;
		circle_shift_left(m_bytes, ret.m_bytes, N, count);
		return ret;
	}

	SignedInteger<N>& self_circle_shift_left(size_t count)
	{
		circle_shift_left(m_bytes, m_bytes, N, count);
		return *this;
	}

	SignedInteger<N> circle_shift_right(size_t count) const
	{
		SignedInteger ret;
		circle_shift_right(m_bytes, ret.m_bytes, N, count);
		return ret;
	}

	SignedInteger<N>& self_circle_shift_right(size_t count)
	{
		circle_shift_right(m_bytes, m_bytes, N, count);
		return *this;
	}

    bool is_positive() const
    {
        return signed_is_positive(m_bytes, N);
    }

    bool is_zero() const
    {
        return is_zero(m_bytes, N);
    }

    const uint8_t* buffer() const
    {
        return m_bytes;
    }

    uint8_t* buffer()
    {
        return const_cast<uint8_t*>(static_cast<const SignedInteger<N>&>(*this).buffer());
    }

    /** 能够存储数据而不丢失符号的最小字节数组长度 */
    int significant_size() const
    {
		return significant_size_signed(m_bytes, N);
	}

    long long_value() const
    {
        long ret = 0;
        expand_signed(m_bytes, N, reinterpret_cast<uint8_t*>(&ret), sizeof(long));
        return ret;
    }
};

}

#endif /* head file guarder */


