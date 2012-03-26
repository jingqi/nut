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
        signedExpand(reinterpret_cast<const uint8_t*>(&v), sizeof(long), m_bytes, N);
    }

    /** 
     * @param withSign
     *      传入的缓冲区是否带符号，如果不带符号，将被视为无符号正数
     */
    SignedInteger(const uint8_t* buf, size_t len, bool withSign)
    {
        assert(NULL != buf && len > 0);
        if (withSign)
            signedExpand(buf, len, m_bytes, N);
        else
            unsignedExpand(buf, len, m_bytes, N);
    }

    template <size_t M>
    explicit SignedInteger(const SignedInteger<M>& x)
    {
        signedExpand(x.m_bytes, M, m_bytes, N);
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

    SignedInteger<N> operator*(const SignedInteger<N>& x) const
    {
        SignedInteger<N> ret;
        multiply(m_bytes, x.m_bytes, ret.m_bytes, N);
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

    bool operator==(const SignedInteger<N>& x) const
    {
        return 0 == ::memcmp(m_bytes, x.m_bytes);
    }

    bool operator!=(const SignedInteger<N>& x) const
    {
        return !(*this == x);
    }

    bool operator<(const SignedInteger<N>& x) const
    {
        return signedLessThen(m_bytes, x.m_bytes, N);
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

public :
    bool isPositive() const
    {
        return signedIsPositive(m_bytes, N);
    }

    bool isZero() const
    {
        return isZero(m_bytes, N);
    }

    const uint8_t* buffer() const
    {
        return m_bytes;
    }

    uint8_t* buffer()
    {
        return const_cast<uint8_t[]>(static_cast<const SignedInteger<N>&>(*this).buffer());
    }

    /** 能够存储数据而不丢失符号的最小字节数组长度 */
    int minSize() const
    {
        // TODO
        return 0;
    }

    long longValue() const
    {
        long ret = 0;
        signedExpand(m_bytes, N, reinterpret_cast<uint8_t*>(&ret), sizeof(long));
        return ret;
    }
};

}

#endif /* head file guarder */


