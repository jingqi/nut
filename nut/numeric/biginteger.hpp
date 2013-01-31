 /**
 * @file -
 * @author jingqi
 * @date 2012-04-03
 * @last-edit 2013-01-31 10:00:22 jingqi
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

class BigInteger;
extern BigInteger nextProbablePrime(const BigInteger& n);


/**
 * 无限大整数
 */
class BigInteger
{
    /** 缓冲区, little-endian, 带符号 */
    uint8_t *m_bytes;
    /** 缓冲区长度 */
    size_t m_bytes_cap;
    /** 有效字节长度 */
    size_t m_significant_len;

private:
    /**
     * 确保缓冲区有足够的空间
     */
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
        m_bytes_cap = newcap;
    }

    /**
     * 释放缓冲区
     */
    void free_mem()
    {
        if (NULL != m_bytes)
            ::free(m_bytes);
        m_bytes = NULL;
        m_bytes_cap = 0;
        m_significant_len = 0;
    }
    
    /**
     * 最小化有效字节长度
     */
    inline void minimize_significant_len()
    {
        m_significant_len = significant_size_signed(m_bytes, m_significant_len);
    }

    /**
     * 确保有效字节长度足够长，不够长则进行符号扩展
     */
    void ensure_significant_len(size_t siglen)
    {
        assert(siglen > 0);

        if (m_significant_len >= siglen)
            return;

        ensure_cap(siglen);
        expand_signed(m_bytes, m_significant_len, m_bytes, siglen);
        m_significant_len = siglen;
    }

public:
    BigInteger()
        : m_bytes(NULL), m_bytes_cap(0), m_significant_len(0)
    {
        ensure_cap(sizeof(int));
        m_bytes[0] = 0;
        m_significant_len = 1;
    }
    
    explicit BigInteger(long long v)
        : m_bytes(NULL), m_bytes_cap(0), m_significant_len(0)
    {
        ensure_cap(sizeof(v));
        ::memcpy(m_bytes, &v, sizeof(v));
        m_significant_len = sizeof(v);
        minimize_significant_len();
    }
    
    BigInteger(const uint8_t *buf, size_t len, bool withSign)
        : m_bytes(NULL), m_bytes_cap(0), m_significant_len(0)
    {
        assert(NULL != buf && len > 0);
        if (withSign || is_positive_signed(buf, len))
        {
            ensure_cap(len);
            ::memcpy(m_bytes, buf, len);
            m_significant_len = len;
        }
        else
        {
            ensure_cap(len + 1);
            ::memcpy(m_bytes, buf, len);
            m_bytes[len] = 0;
            m_significant_len = len + 1;
        }
        minimize_significant_len();
    }

    BigInteger(const BigInteger& x)
        : m_bytes(NULL), m_bytes_cap(0), m_significant_len(0)
    {
        *this = x;
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
        m_significant_len = x.m_significant_len;

        return *this;
    }

    BigInteger& operator=(long long v)
    {
        ensure_cap(sizeof(v));
        ::memcpy(m_bytes, &v, sizeof(v));
        m_significant_len = sizeof(v);
        minimize_significant_len();

        return *this;
    }
    
    inline bool operator==(const BigInteger& x) const
    {
        return equal_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len);
    }

    inline bool operator==(long long v) const
    {
        return equal_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v));
    }

    inline bool operator!=(const BigInteger& x) const
    {
        return !(*this == x);
    }

    inline bool operator!=(long long v) const
    {
        return !(*this == v);
    }
    
    inline bool operator<(const BigInteger& x) const
    {
        return less_than_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len);
    }

    inline bool operator<(long long v) const
    {
        return less_than_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v));
    }
    
    inline bool operator>(const BigInteger& x) const
    {
        return x < *this;
    }

    inline bool operator>(long long v) const
    {
        return less_than_signed((uint8_t*)&v, sizeof(v), m_bytes, m_significant_len);
    }
    
    inline bool operator<=(const BigInteger& x) const
    {
        return !(x < *this);
    }

    inline bool operator<=(long long v) const
    {
        return !(*this > v);
    }
    
    inline bool operator>=(const BigInteger& x) const
    {
        return !(*this < x);
    }

    inline bool operator>=(long long v) const
    {
        return !(*this < v);
    }
    
    BigInteger operator+(const BigInteger& x) const
    {
        BigInteger ret;
        const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
        ret.ensure_cap(max_len + 1);
        add_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, ret.m_bytes, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }

    BigInteger operator+(long long v) const
    {
        BigInteger ret;
        const size_t max_len = (m_significant_len > sizeof(v) ? m_significant_len : sizeof(v));
        ret.ensure_cap(max_len + 1);
        add_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v), ret.m_bytes, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }
    
    BigInteger operator-(const BigInteger& x) const
    {
        BigInteger ret;
        const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
        ret.ensure_cap(max_len + 1);
        sub_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, ret.m_bytes, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }

    BigInteger operator-(long long v) const
    {
        BigInteger ret;
        const size_t max_len = (m_significant_len > sizeof(v) ? m_significant_len : sizeof(v));
        ret.ensure_cap(max_len + 1);
        sub_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v), ret.m_bytes, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }
    
    BigInteger operator-() const
    {
        BigInteger ret;
        ret.ensure_cap(m_significant_len + 1);
        negate_signed(m_bytes, m_significant_len, ret.m_bytes, m_significant_len + 1);
        ret.m_significant_len = m_significant_len + 1;
        ret.minimize_significant_len();
        return ret;
    }
    
    BigInteger operator*(const BigInteger& x) const
    {
        BigInteger ret;
        ret.ensure_cap(m_significant_len + x.m_significant_len);
        multiply_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, ret.m_bytes, m_significant_len + x.m_significant_len);
        ret.m_significant_len = m_significant_len + x.m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }

    BigInteger operator*(long long v) const
    {
        BigInteger ret;
        ret.ensure_cap(m_significant_len + sizeof(v));
        multiply_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v), ret.m_bytes, m_significant_len + sizeof(v));
        ret.m_significant_len = m_significant_len + sizeof(v);
        ret.minimize_significant_len();
        return ret;
    }
    
    BigInteger operator/(const BigInteger& x) const
    {
        assert(!x.is_zero());

        BigInteger ret;
        ret.ensure_cap(m_significant_len);
        divide_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, ret.m_bytes, m_significant_len, NULL, 0);
        ret.m_significant_len = m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }

    BigInteger operator/(long long v) const
    {
        assert(0 != v);

        BigInteger ret;
        ret.ensure_cap(m_significant_len);
        divide_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v), ret.m_bytes, m_significant_len, NULL, 0);
        ret.m_significant_len = m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }
    
    BigInteger operator%(const BigInteger& x) const
    {
        assert(!x.is_zero());
        
        BigInteger ret;
        ret.ensure_cap(x.m_significant_len);
        divide_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, NULL, 0, ret.m_bytes, x.m_significant_len);
        ret.m_significant_len = x.m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }

    BigInteger operator%(long long v) const
    {
        assert(0 != v);

        BigInteger ret;
        ret.ensure_cap(sizeof(v));
        divide_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v), NULL, 0, ret.m_bytes, sizeof(v));
        ret.m_significant_len = sizeof(v);
        ret.minimize_significant_len();
        return ret;
    }
    
    BigInteger& operator+=(const BigInteger& x)
    {
    	const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
    	ensure_cap(max_len + 1);
    	add_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, m_bytes, max_len + 1);
        m_significant_len = max_len + 1;
    	minimize_significant_len();
        return *this;
    }

    BigInteger& operator+=(long long v)
    {
        const size_t max_len = (m_significant_len > sizeof(v) ? m_significant_len : sizeof(v));
        ensure_cap(max_len + 1);
        add_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v), m_bytes, max_len + 1);
        m_significant_len = max_len + 1;
        minimize_significant_len();
        return *this;
    }

    BigInteger& operator-=(const BigInteger& x)
    {
    	const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
    	ensure_cap(max_len + 1);
    	sub_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, m_bytes, max_len + 1);
        m_significant_len = max_len + 1;
    	minimize_significant_len();
        return *this;
    }

    BigInteger& operator-=(long long v)
    {
        const size_t max_len = (m_significant_len > sizeof(v) ? m_significant_len : sizeof(v));
        ensure_cap(max_len + 1);
        sub_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v), m_bytes, max_len + 1);
        m_significant_len = max_len + 1;
        minimize_significant_len();
        return *this;
    }

    BigInteger& operator*=(const BigInteger& x)
    {
    	ensure_cap(m_significant_len + x.m_significant_len);
    	multiply_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, m_bytes, m_significant_len + x.m_significant_len);
        m_significant_len += x.m_significant_len;
    	minimize_significant_len();
        return *this;
    }

    BigInteger& operator*=(long long v)
    {
        ensure_cap(m_significant_len + sizeof(v));
        multiply_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v), m_bytes, m_significant_len + sizeof(v));
        m_significant_len += sizeof(v);
        minimize_significant_len();
        return *this;
    }

    BigInteger& operator/=(const BigInteger& x)
    {
    	divide_signed(m_bytes, m_significant_len, x.m_bytes, x.m_significant_len, m_bytes, m_significant_len, NULL, 0);
    	minimize_significant_len();
        return *this;
    }

    BigInteger& operator/=(long long v)
    {
        divide_signed(m_bytes, m_significant_len, (uint8_t*)&v, sizeof(v), m_bytes, m_significant_len, NULL, 0);
        minimize_significant_len();
        return *this;
    }

    inline BigInteger& operator%=(const BigInteger& x)
    {
        *this = *this % x;
        return *this;
    }

    inline BigInteger& operator%=(long long v)
    {
        *this = *this % v;
        return *this;
    }
    
    BigInteger& operator++()
    {
        ensure_significant_len(m_significant_len + 1);
        increase(m_bytes, m_significant_len);
        minimize_significant_len();
        return *this;
    }

    inline BigInteger operator++(int)
    {
        BigInteger ret(*this);
        ++*this;
        return ret;
    }
    
    BigInteger& operator--()
    {
        ensure_significant_len(m_significant_len + 1);
        decrease(m_bytes, m_significant_len);
        minimize_significant_len();
        return *this;
    }
    
    inline BigInteger operator--(int)
    {
        BigInteger ret(*this);
        --*this;
        return ret;
    }
    
    BigInteger operator<<(size_t count) const
    {
        BigInteger ret(*this);
        ret.ensure_significant_len(m_significant_len + count / 8 + 1);
        shift_left(ret.m_bytes, ret.m_bytes, ret.m_significant_len, count);
        ret.minimize_significant_len();
        return ret;
    }
    
    /**
     * 符号扩展的右移
     */
    BigInteger operator>>(size_t count) const
    {
        BigInteger ret(*this);
        shift_right_signed(ret.m_bytes, ret.m_bytes, ret.m_significant_len, count);
        ret.minimize_significant_len();
        return ret;
    }

    inline BigInteger& operator<<=(size_t count)
    {
        *this = *this << count;
        return *this;
    }

    inline BigInteger& operator>>=(size_t count)
    {
        *this = *this >> count;
        return *this;
    }

public:
    inline void set_zero()
    {
        m_bytes[0] = 0;
        m_significant_len = 1;
    }

    inline bool is_zero() const
    {
        return nut::is_zero(m_bytes, m_significant_len);
    }
    
    inline bool is_positive() const
    {
        return is_positive_signed(m_bytes, m_significant_len);
    }

    inline const uint8_t* bytes() const
    {
        return m_bytes;
    }

    inline uint8_t* bytes()
    {
        return const_cast<uint8_t*>(static_cast<const BigInteger&>(*this).bytes());
    }

    inline void resize(size_t n)
    {
        assert(n > 0);
        ensure_significant_len(n);
        m_significant_len = n;
    }

    inline size_t significant_length() const
    {
        return m_significant_len;
    }

    /**
     * 返回比特位
     *
     * @return 0 or 1
     */
    inline int bit_at(size_t i) const
    {
        if (i / 8 >= m_significant_len)
            return is_positive_signed(m_bytes, m_significant_len) ? 0 : 1;
        return (m_bytes[i / 8] >> (i % 8)) & 0x01;
    }

    /**
     * @param v 0 or 1
     */
    void set_bit(size_t i, int v)
    {
    	assert(v == 0 || v == 1);
    	ensure_significant_len(i / 8 + 1); // 避免符号位被覆盖
    	if (0 == v)
    		m_bytes[i / 8] &= ~(1 << (i % 8));
    	else
    		m_bytes[i / 8] |= 1 << (i % 8);
    }
    
    size_t bit_length()
    {
    	if (is_positive())
    		return nut::bit_length(m_bytes, m_significant_len);
    	else
    		return bit0_length(m_bytes, m_significant_len);
    }

    /**
     * 正数返回 bit 1 计数，负数则返回 bit 0 计数
     */
    size_t bit_count()
    {
    	const size_t bc = nut::bit_count(m_bytes, m_significant_len);
    	if (is_positive())
    		return bc;
    	return 8 * m_significant_len - bc;
    }

    long long llong_value() const
    {
        long long ret = 0;
        expand_signed(m_bytes, m_significant_len, (uint8_t*)&ret, sizeof(ret));
        return ret;
    }

    BigInteger nextProbablePrime() const
    {
        return nut::nextProbablePrime(*this);
    }
    
    /**
     * 取 [a, b) 范围内的随机数
     */
    static BigInteger rand_between(const BigInteger& a, const BigInteger& b)
    {
    	assert(a != b);

    	const bool a_is_bigger = (a > b);
    	BigInteger n = (a_is_bigger ? a - b : b - a);
    	assert(n.is_positive());

    	BigInteger ret;
    	ret.ensure_cap(n.m_significant_len + 1);
    	for (register size_t i = 0; i < n.m_significant_len; ++i)
    		ret.m_bytes[i] = rand();
    	ret.m_bytes[n.m_significant_len] = 0; // 保证是正数
    	ret.m_significant_len = n.m_significant_len + 1;

    	ret %= n;
    	ret += (a_is_bigger ? b : a);
    	return ret;
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
            const size_t n = (size_t) (tmp % RADIX).llong_value();
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
            const size_t n = (size_t) (tmp % RADIX).llong_value();
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

