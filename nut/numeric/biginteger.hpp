﻿/**
 * @file -
 * @author jingqi
 * @date 2012-04-03
 * @last-edit 2012-11-05 22:29:31 jingqi
 */

#ifndef ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_
#define ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_

#include <assert.h>
#include <stdint.h>

#include "byte_array_number.hpp"

namespace nut
{

/**
 * 无限大整数
 */
class BigInteger
{
    bool m_positive;
    uint8_t *m_buffer; // 缓冲区
    size_t m_buffer_len; // 缓冲区字节长度
    size_t m_significant_len; // 缓冲区有效位字节长度

private:
    /** 释放内存 */
    void free_mem()
    {
        if (NULL != m_buffer)
            ::free(m_buffer);
        m_buffer = NULL;
        m_buffer_len = 0;
        m_significant_len = 0;
		m_positive = true;
    }

    /** 重新分配内存 */
    void ensure_cap(size_t size_needed)
    {
        // 分配内存足够了，无需调整
        if (m_buffer_len >= size_needed)
            return;

        // 计算新内存块的大小
        size_t newcap = m_buffer_len * 3 / 2;
        if (newcap < size_needed)
            newcap = size_needed;

        // 分配新内存块并拷贝数据
        if (NULL == m_buffer)
            m_buffer = (uint8_t*) ::malloc(sizeof(uint8_t) * newcap);
        else
            m_buffer = (uint8_t*) ::realloc(m_buffer, sizeof(uint8_t) * newcap);
        assert(NULL != m_buffer);
        ::memset(m_buffer + m_significant_len, 0, size_needed - m_significant_len);
        m_buffer_len = newcap;
    }

    inline void adjust_significant_len()
    {
        m_significant_len = significant_size_unsigned(m_buffer, m_significant_len);
    }

public:
    BigInteger()
        : m_positive(true), m_buffer(NULL), m_buffer_len(0), m_significant_len(0)
    {
        ensure_cap(sizeof(int));
        m_significant_len = 1;
    }

    explicit BigInteger(long v)
        : m_positive(v >= 0), m_buffer(NULL), m_buffer_len(0), m_significant_len(0)
    {
        ensure_cap(sizeof(v));
        if (!m_positive)
            v = -v; // 即使取反的过程中溢出也没问题
        ::memcpy(m_buffer, &v, sizeof(v));
        m_significant_len = sizeof(v);
        adjust_significant_len();
    }

    BigInteger(const uint8_t *buf, size_t len, bool withSign)
        : m_positive(true), m_buffer(NULL), m_buffer_len(0), m_significant_len(0)
    {
        assert(NULL != buf && len > 0);
        ensure_cap(len);
        ::memcpy(m_buffer, buf, len);
        if (withSign && !is_positive_signed(buf, len))
        {
            opposite_signed(m_buffer, m_buffer, len);
            m_positive = false;
        }
        m_significant_len = len;
        adjust_significant_len();
    }

    BigInteger(const BigInteger& x)
        : m_positive(x.m_positive), m_buffer(NULL), m_buffer_len(0), m_significant_len(0)
    {
        ensure_cap(x.m_significant_len);
        if (x.m_significant_len > 0)
            ::memcpy(m_buffer, x.m_buffer, x.m_significant_len);
        m_significant_len = x.m_significant_len;
        adjust_significant_len();
    }

    ~BigInteger()
    {
        free_mem();
    }

public:
    BigInteger& operator=(const BigInteger& x)
    {
        ensure_cap(x.m_significant_len);
        if (x.m_significant_len > 0)
            ::memcpy(m_buffer, x.m_buffer, x.m_significant_len);
        if (m_significant_len > x.m_significant_len)
            ::memset(m_buffer + x.m_significant_len, 0, m_significant_len - x.m_significant_len);
        m_positive = x.m_positive;
        m_significant_len = x.m_significant_len;
        adjust_significant_len();

        return *this;
    }

    bool operator==(const BigInteger& x) const
    {
        const bool zero1 = is_zero(), zero2 = x.is_zero();
        if (zero1 && zero2)
            return true;
        else if (zero1 || zero2)
            return false;
        else if (m_positive != x.m_positive)
            return false;

        const size_t max_sig = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
        for (register size_t i = 0; i < max_sig; ++i)
        {
            const uint8_t op1 = (i < m_significant_len ? m_buffer[i] : 0);
            const uint8_t op2 = (i < x.m_significant_len ? x.m_buffer[i] : 0);
            if (op1 != op2)
                return false;
        }
        return true;
    }

    bool operator!=(const BigInteger& x) const
    {
        return !(*this == x);
    }

    bool operator<(const BigInteger& x) const
    {
        const bool zero1 = is_zero(), zero2 = x.is_zero();
        if (zero1 && zero2)
            return false;
        else if (zero1)
            return x.m_positive;
        else if (zero2)
            return !m_positive;
        else if (m_positive != x.m_positive)
            return x.m_positive;

        // 同号非零值进行比较
        if (m_positive)
            return less_then_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len);
        else
            return less_then_unsigned(x.m_buffer, x.m_significant_len, m_buffer, m_significant_len);
    }

    bool operator>(const BigInteger& x) const
    {
        return x < *this;
    }

    bool operator<=(const BigInteger& x) const
    {
        return !(x < *this);
    }

    bool operator>=(const BigInteger& x) const
    {
        return !(*this < x);
    }

    BigInteger operator+(const BigInteger& x) const
    {
        const size_t max_sig = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
        BigInteger ret;
        ret.ensure_cap(max_sig + 1);
        if (x.m_positive == m_positive)
        {
            add_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, max_sig + 1);
            ret.m_significant_len = max_sig + 1;
            ret.m_positive = m_positive;
        }
        else
        {
            sub_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, max_sig + 1);
            ret.m_significant_len = max_sig + 1;
            ret.m_positive = m_positive;
            if (!is_positive_signed(ret.m_buffer, max_sig + 1))
            {
                opposite_signed(ret.m_buffer, ret.m_buffer, max_sig + 1);
                ret.m_positive = !m_positive;
            }
        }
        ret.adjust_significant_len();
        return ret;
    }

    BigInteger operator-(const BigInteger& x) const
    {
        return *this + (-x);
    }

    BigInteger operator-() const
    {
        BigInteger ret(*this);
        ret.m_positive = !m_positive;
        return ret;
    }

    BigInteger operator*(const BigInteger& x) const
    {
        BigInteger ret;
        ret.ensure_cap(m_significant_len + x.m_significant_len);
        multiply_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, m_significant_len + x.m_significant_len);
        ret.m_significant_len = m_significant_len + x.m_significant_len;
        ret.adjust_significant_len();
        ret.m_positive = ((m_positive && x.m_positive) || (!m_positive && !x.m_positive));
        return ret;
    }

    BigInteger operator/(const BigInteger& x) const
    {
        BigInteger ret;
        ret.ensure_cap(m_significant_len);
        divide_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, m_significant_len, NULL, 0);
        ret.m_significant_len = m_significant_len;
        ret.m_positive = ((m_positive && x.m_positive) || (!m_positive && !x.m_positive));
        ret.adjust_significant_len();
        return ret;
    }

    BigInteger operator%(const BigInteger& x) const
    {
        BigInteger ret;
        ret.ensure_cap(x.m_significant_len);
        divide_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, NULL, 0, ret.m_buffer, x.m_significant_len);
        ret.m_significant_len = x.m_significant_len;
        ret.m_positive = m_positive;
        ret.adjust_significant_len();
        return ret;
    }

    BigInteger& operator+=(const BigInteger& x)
    {
        *this = *this + x;
        return *this;
    }

    BigInteger& operator-=(const BigInteger& x)
    {
        *this = *this - x;
        return *this;
    }

    BigInteger& operator*=(const BigInteger& x)
    {
        *this = *this * x;
        return *this;
    }

    BigInteger& operator/=(const BigInteger& x)
    {
        *this = *this / x;
        return *this;
    }

    BigInteger& operator%=(const BigInteger& x)
    {
        *this = *this % x;
        return *this;
    }

    BigInteger& operator++()
    {
        *this = *this + BigInteger(1);
        return *this;
    }

    BigInteger operator++(int)
    {
        BigInteger ret(*this);
        ++*this;
        return ret;
    }

    BigInteger& operator--()
    {
        *this = *this - BigInteger(1);
        return *this;
    }

    BigInteger operator--(int)
    {
        BigInteger ret(*this);
        --*this;
        return ret;
    }

    BigInteger operator<<(size_t count) const
    {
        BigInteger ret(*this);
        ret.ensure_cap(m_significant_len + count / 8 + 1);
        shift_left(ret.m_buffer, ret.m_buffer, m_significant_len + count / 8 + 1, count);
        ret.m_significant_len = m_significant_len + count / 8 + 1;
        ret.adjust_significant_len();
        return ret;
    }

    BigInteger operator>>(size_t count) const
    {
        BigInteger ret(*this);
        shift_right_unsigned(ret.m_buffer, ret.m_buffer, ret.m_significant_len, count);
        ret.adjust_significant_len();
        return ret;
    }

    BigInteger operator<<=(size_t count)
    {
        *this = *this << count;
        return *this;
    }

    BigInteger operator>>=(size_t count)
    {
        *this = *this >> count;
        return *this;
    }

public:
    void clear()
    {
        m_significant_len = 0;
		m_positive = true;
    }

    bool is_zero() const
    {
        assert((m_buffer == NULL && m_buffer_len == 0 && m_significant_len == 0) ||
            (m_buffer != NULL && m_buffer_len > 0 && m_significant_len <= m_buffer_len));
        return 0 == m_significant_len || nut::is_zero(m_buffer, m_significant_len);
    }

    bool is_positive() const
    {
        return m_positive || is_zero();
    }

    long long_value() const
    {
        long ret = 0;
        expand_unsigned(m_buffer, m_significant_len, (uint8_t*)&ret, sizeof(ret));
        return (m_positive ? ret : -ret);
    }

private:
    static inline bool is_valid_radix(size_t radix)
    {
        return 1 < radix && radix <= 36;
    }

    static inline char num2char(size_t n)
    {
        assert(0 <= n && n < 36);
        return (n < 10 ? '0' + n : 'A' + n - 10);
    }

public:
	std::string toString(size_t radix = 10) const
	{
        assert(is_valid_radix(radix));
		BigInteger tmp(*this);
		const bool positive = tmp.is_positive();
		if (!positive)
            tmp = -tmp;

        const BigInteger RADIX(radix);
		std::string ret;
        do
        {
            const size_t n = (size_t) (tmp % RADIX).long_value();
            ret.push_back(num2char(n));

            tmp /= RADIX;
        } while (!tmp.is_zero());
        if (!positive)
            ret.push_back('-');
        std::reverse(ret.begin(), ret.end());
        return ret;
	}

private:
    static inline bool is_blank(char c)
    {
        return ' ' == c || '\t' == c;
    }

    static inline size_t skip_blank(const std::string& s, size_t start)
    {
        while (start < s.length() && is_blank(s[start]))
            ++start;
        return start;
    }

    static inline bool is_valid_char(char c, size_t radix)
    {
        assert(is_valid_radix(radix));
        if (radix <= 10)
            return '0' <= c && c <= '0' + (int) radix - 1;
        if ('0' <= c && c <= '9')
            return true;
        return 'a' <= (c | 0x20) && (c | 0x20) <= 'a' + (int) radix - 10 - 1;
    }

    static inline size_t char2num(char c)
    {
        assert(is_valid_char(c, 36));
        if ('0' <= c && c <= '9')
            return c - '0';
        return (c | 0x20) - 'a' + 10;
    }

public:
    static BigInteger valueOf(const std::string& s, size_t radix = 10)
    {
        assert(radix > 1 && radix <= 36);
        BigInteger ret;

        // 略过空白
        size_t index = skip_blank(s, 0);
        if (index >= s.length())
            return ret;

        // 正负号
        bool positive = ('-' != s[index]);
        if ('+' == s[index] || '-' == s[index])
            if ((index = skip_blank(s, index + 1)) >= s.length())
                return ret;

        // 数字值
        const BigInteger RADIX(radix);
        while (index < s.length() && is_valid_char(s[index], radix))
        {
            ret *= RADIX;
            ret += BigInteger(char2num(s[index]));
            index = skip_blank(s, index + 1);
        }
        if (!positive)
            ret = -ret;
        return ret;
    }
};

}

#endif /* head file guarder */

