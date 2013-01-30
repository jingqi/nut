/**
 * @file -
 * @author jingqi
 * @date 2012-04-03
 * @last-edit 2012-11-13 21:21:56 jingqi
 */

#ifndef ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_
#define ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_

#include <assert.h>
#include <stdint.h>
#include <string>
#include <algorithm> // for std::reverse()

#include "byte_array_number.hpp"

namespace nut
{

/**
 * 无限大整数
 */
class BigInteger
{
    int m_signum; // -1 小于0, 0 等于0, 1 大于0
    uint8_t *m_bytes; // 缓冲区
    size_t m_bytes_cap; // 缓冲区字节长度
    size_t m_significant_len; // 缓冲区有效位字节长度

private:
    /** 释放内存 */
    void free_mem()
    {
        if (NULL != m_bytes)
            ::free(m_bytes);
        m_bytes = NULL;
        m_bytes_cap = 0;
        m_significant_len = 0;
		m_signum = 0;
    }

    /** 重新分配内存 */
    void ensure_cap(size_t size_needed)
    {
        // 分配内存足够了，无需调整
        if (m_bytes_cap >= size_needed)
            return;

        // 计算新内存块的大小
        size_t newcap = m_bytes_cap * 3 / 2;
        if (newcap < size_needed)
            newcap = size_needed;

        // 分配新内存块并拷贝数据
        if (NULL == m_bytes)
            m_bytes = (uint8_t*) ::malloc(sizeof(uint8_t) * newcap);
        else
            m_bytes = (uint8_t*) ::realloc(m_bytes, sizeof(uint8_t) * newcap);
        assert(NULL != m_bytes);
        ::memset(m_bytes + m_significant_len, 0, size_needed - m_significant_len);
        m_bytes_cap = newcap;
    }

    inline void adjust_significant_len()
    {
        m_significant_len = significant_size_unsigned(m_bytes, m_significant_len);
    }

public:
    BigInteger()
        : m_signum(0), m_bytes(NULL), m_bytes_cap(0), m_significant_len(0)
    {
        ensure_cap(sizeof(int));
        m_significant_len = 1;
    }

    explicit BigInteger(long v)
        : m_signum(v > 0 ? 1 : (v == 0 ? 0 : -1)), m_bytes(NULL), m_bytes_cap(0), m_significant_len(0)
    {
        ensure_cap(sizeof(v));
        if (v < 0)
            v = -v; // 即使取反的过程中溢出也没问题
        ::memcpy(m_bytes, &v, sizeof(v));
        m_significant_len = sizeof(v);
        adjust_significant_len();
    }

    BigInteger(const uint8_t *buf, size_t len, bool withSign)
        : m_signum(0), m_bytes(NULL), m_bytes_cap(0), m_significant_len(0)
    {
        assert(NULL != buf && len > 0);
        ensure_cap(len);
        ::memcpy(m_bytes, buf, len);
        if (withSign && !is_positive_signed(buf, len))
        {
            negate_signed(m_bytes, m_bytes, len);
            m_signum = -1;
        }
        else if (nut::is_zero(buf, len))
        {
            m_signum = 0;
        }
        else
        {
            m_signum = 1;
        }
        m_significant_len = len;
        adjust_significant_len();
    }

    BigInteger(const BigInteger& x)
        : m_signum(x.m_signum), m_bytes(NULL), m_bytes_cap(0), m_significant_len(0)
    {
        ensure_cap(x.m_significant_len);
        if (x.m_significant_len > 0)
            ::memcpy(m_bytes, x.m_bytes, x.m_significant_len);
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
            ::memcpy(m_bytes, x.m_bytes, x.m_significant_len);
        m_signum = x.m_signum;
        m_significant_len = x.m_significant_len;
        adjust_significant_len();

        return *this;
    }

    bool operator==(const BigInteger& x) const
    {
        if (m_signum != x.m_signum)
            return false;
        else if (0 == m_signum && 0 == x.m_signum)
            return true;

        return equal_unsigned(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len);
    }

    bool operator!=(const BigInteger& x) const
    {
        return !(*this == x);
    }

    bool operator<(const BigInteger& x) const
    {
        if (m_signum < x.m_signum)
            return true;
        else if (m_signum > x.m_signum)
            return false;
        else if (0 == m_signum && 0 == m_signum)
            return false;

        // 同号非零值进行比较
        if (m_signum > 0)
            return less_then_unsigned(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len);
        else
            return less_then_unsigned(x.m_bytes, x.m_significant_len, m_bytes, m_significant_len);
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
        if (0 == m_signum)
            return x;
        else if (0 == x.m_signum)
            return *this;

        BigInteger ret;
        const size_t max_sig = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
        ret.ensure_cap(max_sig + 1);
        if (m_signum == x.m_signum)
        {
            add_unsigned(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, ret.m_bytes, max_sig + 1);
            ret.m_significant_len = max_sig + 1;
            ret.m_signum = m_signum;
        }
        else
        {
            sub_unsigned(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, ret.m_bytes, max_sig + 1);
            ret.m_significant_len = max_sig + 1;
            ret.m_signum = m_signum;
            if (!is_positive_signed(ret.m_bytes, max_sig + 1))
            {
                negate_signed(ret.m_bytes, ret.m_bytes, max_sig + 1);
                ret.m_signum = (m_signum > 0 ? -1 : 1);
            }
            else if (nut::is_zero(ret.m_bytes, max_sig + 1))
            {
                ret.m_signum = 0;
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
        if (0 == m_signum)
            return *this;

        BigInteger ret(*this);
        ret.m_signum = -m_signum;
        return ret;
    }

    BigInteger operator*(const BigInteger& x) const
    {
        BigInteger ret;
        if (0 == m_signum || 0 == x.m_signum)
            return ret;

        ret.ensure_cap(m_significant_len + x.m_significant_len);
        multiply_unsigned(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, ret.m_bytes, m_significant_len + x.m_significant_len);
        ret.m_significant_len = m_significant_len + x.m_significant_len;
        ret.adjust_significant_len();
        ret.m_signum = (m_signum == x.m_signum ? 1 : -1);
        return ret;
    }

    BigInteger operator/(const BigInteger& x) const
    {
        assert(!x.is_zero());

        BigInteger ret;
        if (0 == m_signum)
            return ret;

        ret.ensure_cap(m_significant_len);
        divide_unsigned(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, ret.m_bytes, m_significant_len, NULL, 0);
        ret.m_significant_len = m_significant_len;
        ret.m_signum = (m_signum == x.m_signum ? 1 : -1);
        if (nut::is_zero(ret.m_bytes, m_significant_len))
            ret.m_signum = 0;
        ret.adjust_significant_len();
        return ret;
    }

    BigInteger operator%(const BigInteger& x) const
    {
        assert(!x.is_zero());

        BigInteger ret;
        if (0 == m_signum)
            return ret;

        ret.ensure_cap(x.m_significant_len);
        divide_unsigned(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, NULL, 0, ret.m_bytes, x.m_significant_len);
        ret.m_significant_len = x.m_significant_len;
        ret.m_signum = m_signum;
        if (nut::is_zero(ret.m_bytes, m_significant_len))
            ret.m_signum = 0;
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
        shift_left(ret.m_bytes, ret.m_bytes, m_significant_len + count / 8 + 1, count);
        ret.m_significant_len = m_significant_len + count / 8 + 1;
        ret.adjust_significant_len();
        return ret;
    }

    BigInteger operator>>(size_t count) const
    {
        BigInteger ret(*this);
        shift_right_unsigned(ret.m_bytes, ret.m_bytes, ret.m_significant_len, count);
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
        m_signum = 0;
    }

    bool is_zero() const
    {
        return 0 == m_signum;
    }

    bool is_positive() const
    {
        return m_signum >= 0;
    }

    const uint8_t* bytes() const
    {
        return m_bytes;
    }

    uint8_t* bytes()
    {
        return const_cast<uint8_t*>(static_cast<const BigInteger&>(*this).bytes());
    }

    int significant_size() const
    {
        return m_significant_len;
    }
    
    /**
     * 返回正数比特位
     *
     * @return 0 or 1
     */
    int bit_at(size_t i) const
    {
        if (i >= m_significant_len * 8)
            return 0;
        return (m_bytes[i / 8] >> (i % 8)) & 0x01;
    }

    long long_value() const
    {
        long ret = 0;
        expand_unsigned(m_bytes, m_significant_len, (uint8_t*)&ret, sizeof(ret));
        return (m_signum > 0 ? ret : -ret);
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

    static inline wchar_t num2wchar(size_t n)
    {
        assert(0 <= n && n < 36);
        return (n < 10 ? L'0' + n : L'A' + n - 10);
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

	std::wstring toWString(size_t radix = 10) const
	{
        assert(is_valid_radix(radix));
		BigInteger tmp(*this);
		const bool positive = tmp.is_positive();
		if (!positive)
            tmp = -tmp;

        const BigInteger RADIX(radix);
		std::wstring ret;
        do
        {
            const size_t n = (size_t) (tmp % RADIX).long_value();
            ret.push_back(num2wchar(n));

            tmp /= RADIX;
        } while (!tmp.is_zero());
        if (!positive)
            ret.push_back(L'-');
        std::reverse(ret.begin(), ret.end());
        return ret;
	}

private:
    static inline bool is_blank(char c)
    {
        return ' ' == c || '\t' == c;
    }

    static inline bool is_blank(wchar_t c)
    {
        return L' ' == c || L'\t' == c;
    }

    static inline size_t skip_blank(const std::string& s, size_t start)
    {
        while (start < s.length() && is_blank(s[start]))
            ++start;
        return start;
    }

    static inline size_t skip_blank(const std::wstring& s, size_t start)
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

    static inline bool is_valid_char(wchar_t c, size_t radix)
    {
        assert(is_valid_radix(radix));
        if (radix <= 10)
            return L'0' <= c && c <= L'0' + (int) radix - 1;
        if (L'0' <= c && c <= L'9')
            return true;
        return L'a' <= (c | 0x20) && (c | 0x20) <= L'a' + (int) radix - 10 - 1;
    }

    static inline size_t char2num(char c)
    {
        assert(is_valid_char(c, 36));
        if ('0' <= c && c <= '9')
            return c - '0';
        return (c | 0x20) - 'a' + 10;
    }

    static inline size_t char2num(wchar_t c)
    {
        assert(is_valid_char(c, 36));
        if (L'0' <= c && c <= L'9')
            return c - L'0';
        return (c | 0x20) - L'a' + 10;
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

    static BigInteger valueOf(const std::wstring& s, size_t radix = 10)
    {
        assert(radix > 1 && radix <= 36);
        BigInteger ret;

        // 略过空白
        size_t index = skip_blank(s, 0);
        if (index >= s.length())
            return ret;

        // 正负号
        bool positive = (L'-' != s[index]);
        if (L'+' == s[index] || L'-' == s[index])
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

