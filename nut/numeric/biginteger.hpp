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

#include <nut/platform/stdint.hpp>
#include <nut/debugging/static_assert.hpp>

#include "word_array_signed_number.hpp"

namespace nut
{

/**
 * 无限大整数
 */
template <typename T>
class _BigInteger
{
public:
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

private:
    typedef _BigInteger<T> self;

    /** 缓冲区, little-endian, 带符号 */
    word_type *m_buffer;
    /** 缓冲区长度 */
    size_t m_buffer_cap;
    /** 有效字长度 */
    size_t m_significant_len;

private:
    /**
     * 确保缓冲区有足够的空间
     */
    void ensure_cap(size_t size_needed)
    {
        // 分配内存足够了，无需调整
        if (m_buffer_cap >= size_needed)
            return;

        // 计算新内存块的大小
        size_t newcap = m_buffer_cap * 3 / 2;
        if (newcap < size_needed)
            newcap = size_needed;

        // 分配新内存块并拷贝数据
        if (NULL == m_buffer)
            m_buffer = (word_type*) ::malloc(sizeof(word_type) * newcap);
        else
            m_buffer = (word_type*) ::realloc(m_buffer, sizeof(word_type) * newcap);
        assert(NULL != m_buffer);
        m_buffer_cap = newcap;
    }

    /**
     * 释放缓冲区
     */
    void free_mem()
    {
        if (NULL != m_buffer)
            ::free(m_buffer);
        m_buffer = NULL;
        m_buffer_cap = 0;
        m_significant_len = 0;
    }
    
    /**
     * 最小化有效字节长度
     */
    inline void minimize_significant_len()
    {
        m_significant_len = significant_size(m_buffer, m_significant_len);
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
        expand(m_buffer, m_significant_len, m_buffer, siglen);
        m_significant_len = siglen;
    }

public:
    _BigInteger()
        : m_buffer(NULL), m_buffer_cap(0), m_significant_len(0)
    {
        ensure_cap(1);
        m_buffer[0] = 0;
        m_significant_len = 1;
    }
    
    explicit _BigInteger(long long v)
        : m_buffer(NULL), m_buffer_cap(0), m_significant_len(0)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        ensure_cap(sizeof(v) / sizeof(word_type));
        ::memcpy(m_buffer, &v, sizeof(v));
        m_significant_len = sizeof(v) / sizeof(word_type);
        minimize_significant_len();
    }

    template <typename U>
    _BigInteger(const U *buf, size_t len, bool withSign)
        : m_buffer(NULL), m_buffer_cap(0), m_significant_len(0)
    {
        assert(NULL != buf && len > 0);

        const uint8_t fill = (withSign ? (nut::is_positive(buf, len) ? 0 : 0xFF) : 0);
        const size_t min_sig = sizeof(U) * len / sizeof(word_type) + 1; // 保证一个空闲字节放符号位
        ensure_cap(min_sig);
        ::memcpy(m_buffer, buf, sizeof(U) * len);
        ::memset(((U*)m_buffer) + len, fill, min_sig * sizeof(word_type) - sizeof(U) * len);
        m_significant_len = min_sig;
        minimize_significant_len();
    }

    // 上述模板函数的一个特化
    _BigInteger(const word_type *buf, size_t len, bool withSign)
    {
        assert(NULL != buf && len > 0);
        if (withSign || is_positive(buf, len))
        {
            ensure_cap(len);
            ::memcpy(m_buffer, buf, len);
            m_significant_len = len;
        }
        else
        {
            ensure_cap(len + 1);
            ::memcpy(m_buffer, buf, sizeof(word_type) * len);
            m_buffer[len] = 0;
            m_significant_len = len + 1;
        }
        minimize_significant_len();
    }

    _BigInteger(const self& x)
        : m_buffer(NULL), m_buffer_cap(0), m_significant_len(0)
    {
        *this = x;
    }

    ~_BigInteger()
    {
        free_mem();
    }
    
public:
    self& operator=(const self& x)
    {
        ensure_cap(x.m_significant_len);
        if (x.m_significant_len > 0)
            ::memcpy(m_buffer, x.m_buffer, sizeof(word_type) * x.m_significant_len);
        m_significant_len = x.m_significant_len;

        return *this;
    }

    self& operator=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        ensure_cap(sizeof(v) / sizeof(word_type));
        ::memcpy(m_buffer, &v, sizeof(v));
        m_significant_len = sizeof(v) / sizeof(word_type);
        minimize_significant_len();

        return *this;
    }
    
    inline bool operator==(const self& x) const
    {
        return equals(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len);
    }

    inline bool operator==(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        return equals(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type));
    }

    inline bool operator!=(const self& x) const
    {
        return !(*this == x);
    }

    inline bool operator!=(long long v) const
    {
        return !(*this == v);
    }
    
    inline bool operator<(const self& x) const
    {
        return less_than(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len);
    }

    inline bool operator<(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        return less_than(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type));
    }
    
    inline bool operator>(const self& x) const
    {
        return x < *this;
    }

    inline bool operator>(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        return less_than((word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer, m_significant_len);
    }
    
    inline bool operator<=(const self& x) const
    {
        return !(x < *this);
    }

    inline bool operator<=(long long v) const
    {
        return !(*this > v);
    }
    
    inline bool operator>=(const self& x) const
    {
        return !(*this < x);
    }

    inline bool operator>=(long long v) const
    {
        return !(*this < v);
    }
    
    self operator+(const self& x) const
    {
        self ret;
        const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
        ret.ensure_cap(max_len + 1);
        add(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }

    self operator+(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        self ret;
        const size_t max_len = (m_significant_len > sizeof(v) / sizeof(word_type) ? m_significant_len : sizeof(v) / sizeof(word_type));
        ret.ensure_cap(max_len + 1);
        add(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), ret.m_buffer, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }
    
    self operator-(const self& x) const
    {
        self ret;
        const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
        ret.ensure_cap(max_len + 1);
        sub(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }

    self operator-(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        self ret;
        const size_t max_len = (m_significant_len > sizeof(v) / sizeof(word_type) ? m_significant_len : sizeof(v) / sizeof(word_type));
        ret.ensure_cap(max_len + 1);
        sub(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), ret.m_buffer, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }
    
    self operator-() const
    {
        self ret;
        ret.ensure_cap(m_significant_len + 1);
        negate(m_buffer, m_significant_len, ret.m_buffer, m_significant_len + 1);
        ret.m_significant_len = m_significant_len + 1;
        ret.minimize_significant_len();
        return ret;
    }
    
    self operator*(const self& x) const
    {
        self ret;
        ret.ensure_cap(m_significant_len + x.m_significant_len);
        multiply(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, m_significant_len + x.m_significant_len);
        ret.m_significant_len = m_significant_len + x.m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }

    self operator*(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        self ret;
        ret.ensure_cap(m_significant_len + sizeof(v) / sizeof(word_type));
        multiply(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), ret.m_buffer, m_significant_len + sizeof(v) / sizeof(word_type));
        ret.m_significant_len = m_significant_len + sizeof(v) / sizeof(word_type);
        ret.minimize_significant_len();
        return ret;
    }
    
    self operator/(const self& x) const
    {
        assert(!x.is_zero());

        self ret;
        ret.ensure_cap(m_significant_len);
        divide(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, m_significant_len, (word_type*)NULL, 0);
        ret.m_significant_len = m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }

    self operator/(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);
        assert(0 != v);

        self ret;
        ret.ensure_cap(m_significant_len);
        divide(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), ret.m_buffer, m_significant_len, (word_type*)NULL, 0);
        ret.m_significant_len = m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }
    
    self operator%(const self& x) const
    {
        assert(!x.is_zero());

        if (is_positive() && x.is_positive() && *this < x) // 小幅度优化
            return *this;
        
        self ret;
        ret.ensure_cap(x.m_significant_len);
        divide(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, (word_type*)NULL, 0, ret.m_buffer, x.m_significant_len);
        ret.m_significant_len = x.m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }

    self operator%(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);
        assert(0 != v);

        self ret;
        ret.ensure_cap(sizeof(v) / sizeof(word_type));
        divide(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), (word_type*)NULL, 0, ret.m_buffer, sizeof(v) / sizeof(word_type));
        ret.m_significant_len = sizeof(v);
        ret.minimize_significant_len();
        return ret;
    }
    
    self& operator+=(const self& x)
    {
    	const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
    	ensure_cap(max_len + 1);
    	add(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, m_buffer, max_len + 1);
        m_significant_len = max_len + 1;
    	minimize_significant_len();
        return *this;
    }

    self& operator+=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        const size_t max_len = (m_significant_len > sizeof(v) / sizeof(word_type) ? m_significant_len : sizeof(v) / sizeof(word_type));
        ensure_cap(max_len + 1);
        add(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer, max_len + 1);
        m_significant_len = max_len + 1;
        minimize_significant_len();
        return *this;
    }

    self& operator-=(const self& x)
    {
    	const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
    	ensure_cap(max_len + 1);
    	sub(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, m_buffer, max_len + 1);
        m_significant_len = max_len + 1;
    	minimize_significant_len();
        return *this;
    }

    self& operator-=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        const size_t max_len = (m_significant_len > sizeof(v) / sizeof(word_type) ? m_significant_len : sizeof(v) / sizeof(word_type));
        ensure_cap(max_len + 1);
        sub(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer, max_len + 1);
        m_significant_len = max_len + 1;
        minimize_significant_len();
        return *this;
    }

    self& operator*=(const self& x)
    {
    	ensure_cap(m_significant_len + x.m_significant_len);
    	multiply(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, m_buffer, m_significant_len + x.m_significant_len);
        m_significant_len += x.m_significant_len;
    	minimize_significant_len();
        return *this;
    }

    self& operator*=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        ensure_cap(m_significant_len + sizeof(v) / sizeof(word_type));
        multiply(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer, m_significant_len + sizeof(v) / sizeof(word_type));
        m_significant_len += sizeof(v);
        minimize_significant_len();
        return *this;
    }

    self& operator/=(const self& x)
    {
        assert(!x.is_zero());

    	divide(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, m_buffer, m_significant_len, (word_type*)NULL, 0);
    	minimize_significant_len();
        return *this;
    }

    self& operator/=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);
        assert(0 != v);

        divide(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer, m_significant_len, (word_type*)NULL, 0);
        minimize_significant_len();
        return *this;
    }

    inline self& operator%=(const self& x)
    {
        assert(!x.is_zero());

        if (is_positive() && x.is_positive() && *this < x) // 小幅度优化
            return *this;

        *this = *this % x;
        return *this;
    }

    inline self& operator%=(long long v)
    {
        assert(0 != v);

        *this = *this % v;
        return *this;
    }
    
    self& operator++()
    {
        ensure_significant_len(m_significant_len + 1);
        increase(m_buffer, m_significant_len);
        minimize_significant_len();
        return *this;
    }

    inline self operator++(int)
    {
        self ret(*this);
        ++*this;
        return ret;
    }
    
    self& operator--()
    {
        ensure_significant_len(m_significant_len + 1);
        decrease(m_buffer, m_significant_len);
        minimize_significant_len();
        return *this;
    }
    
    inline self operator--(int)
    {
        self ret(*this);
        --*this;
        return ret;
    }
    
    self operator<<(size_t count) const
    {
        if (0 == count)
            return *this;

        self ret(*this);
        ret.ensure_significant_len(m_significant_len + (count - 1) / (8 * sizeof(word_type)) + 1);
        shift_left(ret.m_buffer, ret.m_buffer, ret.m_significant_len, count);
        ret.minimize_significant_len();
        return ret;
    }
    
    /**
     * 符号扩展的右移
     */
    self operator>>(size_t count) const
    {
        if (0 == count)
            return *this;

        self ret(*this);
        shift_right(ret.m_buffer, ret.m_buffer, ret.m_significant_len, count);
        ret.minimize_significant_len();
        return ret;
    }

    inline self& operator<<=(size_t count)
    {
        if (0 == count)
            return *this;

        ensure_significant_len(m_significant_len + (count - 1) / (8 * sizeof(word_type)) + 1);
        shift_left(m_buffer, m_buffer, m_significant_len, count);
        minimize_significant_len();
        return *this;
    }

    inline self& operator>>=(size_t count)
    {
        if (0 == count)
            return *this;

        shift_right(m_buffer, m_buffer, m_significant_len, count);
        minimize_significant_len();
        return *this;
    }

public:
    inline void set_zero()
    {
        m_buffer[0] = 0;
        m_significant_len = 1;
    }

    inline bool is_zero() const
    {
        return nut::is_zero(m_buffer, m_significant_len);
    }
    
    inline bool is_positive() const
    {
        return nut::is_positive(m_buffer, m_significant_len);
    }

    inline const word_type* buffer() const
    {
        return m_buffer;
    }

    inline word_type* buffer()
    {
        return const_cast<word_type*>(static_cast<const self&>(*this).bytes());
    }

    inline void resize(size_t n)
    {
        assert(n > 0);
        ensure_significant_len(n);
        m_significant_len = n;
    }

    /**
     * 以word_type为单位计算有效字长度
     */
    inline size_t significant_words_length() const
    {
        return m_significant_len;
    }
    
    /**
     * 以8比特字节为单位计算有效字节长度
     *
     * NOTE: 可能不是以 word_type 为单位有效长度的倍数
     */
    inline size_t significant_bytes_length() const
    {
        const bool positive = is_positive();
        size_t ret = m_significant_len * sizeof(word_type);
        word_type last_word = m_buffer[m_significant_len - 1];
        for (int i = 1; i < sizeof(word_type); ++i)
        {
            const uint8_t high = (uint8_t) (last_word >> (8 * (sizeof(word_type) - i)));
            if (high != (positive ? 0 : 0xFF))
                break;

            const uint8_t next_high = (uint8_t) (last_word >> (8 * (sizeof(word_type) - i - 1)));
            if ((next_high & 0x80) != (positive ? 0 : 0x80))
                break;

            --ret;
        }
        return ret;
    }

    /**
     * 返回比特位
     *
     * @return 0 or 1
     */
    inline int bit_at(size_t i) const
    {
        if (i / (8 * sizeof(word_type)) >= m_significant_len)
            return is_positive() ? 0 : 1;
        return (m_buffer[i / (8 * sizeof(word_type))] >> (i % (8 * sizeof(word_type)))) & 0x01;
    }

    /**
     * @param v 0 or 1
     */
    void set_bit(size_t i, int v)
    {
    	assert(v == 0 || v == 1);
    	ensure_significant_len((i + 1) / (8 * sizeof(word_type)) + 1); // 避免符号位被覆盖
    	if (0 == v)
    		m_buffer[i / (8 * sizeof(word_type))] &= ~(((word_type) 1) << (i % (8 * sizeof(word_type))));
    	else
    		m_buffer[i / (8 * sizeof(word_type))] |= ((word_type)1) << (i % (8 * sizeof(word_type)));
    }
    
    size_t bit_length() const
    {
    	if (is_positive())
    		return nut::bit_length((uint8_t*)m_buffer, sizeof(word_type) * m_significant_len);
    	else
    		return bit0_length((uint8_t*)m_buffer, sizeof(word_type) * m_significant_len);
    }

    /**
     * 正数返回 bit 1 计数，负数则返回 bit 0 计数
     */
    size_t bit_count()
    {
    	const size_t bc = nut::bit_count((uint8_t*)m_buffer, sizeof(word_type) * m_significant_len);
    	if (is_positive())
    		return bc;
    	return 8 * sizeof(word_type) * m_significant_len - bc;
    }

    int lowest_bit() const
    {
        return nut::lowest_bit((uint8_t*)m_buffer, sizeof(word_type) * m_significant_len);
    }

    long long llong_value() const
    {
        NUT_STATIC_ASSERT(sizeof(long long) % sizeof(word_type) == 0);

        long long ret = 0;
        expand(m_buffer, m_significant_len, (word_type*)&ret, sizeof(ret) / sizeof(word_type));
        return ret;
    }

    self nextProbablePrime() const
    {
        return nut::nextProbablePrime(*this);
    }
    
    /**
     * 取 [a, b) 范围内的随机数
     */
    static self rand_between(const self& a, const self& b)
    {
    	assert(a != b);

    	const bool a_is_bigger = (a > b);
    	self n = (a_is_bigger ? a - b : b - a);
    	assert(n.is_positive());

    	self ret;
    	ret.ensure_cap(n.m_significant_len + 1);
    	for (register size_t i = 0; i < n.m_significant_len; ++i)
        {
            for (register size_t j = 0; j < sizeof(word_type); ++j)
            {
                ret.m_buffer[i] <<= 8;
    		    ret.m_buffer[i] += rand() & 0xFF;
            }
        }
    	ret.m_buffer[n.m_significant_len] = 0; // 保证是正数
    	ret.m_significant_len = n.m_significant_len + 1;

    	ret %= n;
    	ret += (a_is_bigger ? b : a);
    	return ret;
    }

    /**
     * 值交换
     */
    static void swap(self *a, self *b)
    {
        assert(NULL != a && NULL != b);
        word_type *p = a->m_buffer;
        size_t c = a->m_buffer_cap;
        size_t s = a->m_significant_len;
        a->m_buffer = b->m_buffer;
        a->m_buffer_cap = b->m_buffer_cap;
        a->m_significant_len = b->m_significant_len;
        b->m_buffer = p;
        b->m_buffer_cap = c;
        b->m_significant_len = s;
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
        self tmp(*this);
        const bool positive = tmp.is_positive();
        if (!positive)
            tmp = -tmp;

        const self RADIX(radix);
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
        self tmp(*this);
        const bool positive = tmp.is_positive();
        if (!positive)
            tmp = -tmp;

        const self RADIX(radix);
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
    static self valueOf(const std::string& s, size_t radix = 10)
    {
        assert(radix > 1 && radix <= 36);
        self ret;

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
        const self RADIX(radix);
        while (index < s.length() && is_valid_char(s[index], radix))
        {
            ret *= RADIX;
            ret += self(char2num(s[index]));
            index = skip_blank(s, index + 1);
        }
        if (!positive)
            ret = -ret;
        return ret;
    }

    static self valueOf(const std::wstring& s, size_t radix = 10)
    {
        assert(radix > 1 && radix <= 36);
        self ret;

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
        const self RADIX(radix);
        while (index < s.length() && is_valid_char(s[index], radix))
        {
            ret *= RADIX;
            ret += self(char2num(s[index]));
            index = skip_blank(s, index + 1);
        }
        if (!positive)
            ret = -ret;
        return ret;
    }
};

typedef _BigInteger<uint32_t> BigInteger;

}

#endif /* head file guarder */

