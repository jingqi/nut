/**
 * @file -
 * @author jingqi
 * @date 2012-04-03
 * @last-edit 2015-01-06 22:47:37 jingqi
 */

#ifndef ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_
#define ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_

#include <assert.h>
#include <stdint.h>
#include <string>
#include <algorithm> // for std::reverse()

#include <nut/platform/stdint.hpp>
#include <nut/debugging/static_assert.hpp>
#include <nut/mem/sys_ma.hpp>

#include "word_array_integer.hpp"

#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 无限大整数
 */
class BigInteger
{
    typedef BigInteger self_type;

public:
    typedef size_t size_type;
    typedef unsigned int word_type;
    typedef StdInt<word_type>::double_unsigned_type dword_type;

private:
    const rc_ptr<memory_allocator> m_alloc;
    word_type *m_buffer; // 缓冲区, little-endian, 带符号
    size_type m_cap;
    size_type m_significant_len; // 有效字长度

private:
    void ensure_cap(size_type new_size)
    {
        if (new_size <= m_cap)
            return;

        size_type new_cap = m_cap * 3 / 2;
        if (new_cap < new_size)
            new_cap = new_size;

        if (NULL == m_buffer)
            m_buffer = (word_type*) ma_alloc(m_alloc.pointer(), sizeof(word_type) * new_cap);
        else
            m_buffer = (word_type*) ma_realloc(m_alloc.pointer(), m_buffer, sizeof(word_type) * new_cap);
        assert(NULL != m_buffer);
        m_cap = new_cap;
    }

    /**
     * 确保有效字节长度足够长，不够长则进行符号扩展
     */
    void ensure_significant_len(size_type siglen)
    {
        assert(siglen > 0);
        if (siglen <= m_significant_len)
            return;

        ensure_cap(siglen);
        signed_expand(m_buffer, m_significant_len, m_buffer, siglen);
        m_significant_len = siglen;
    }

    /**
     * 最小化有效字节长度
     */
    void minimize_significant_len()
    {
        m_significant_len = signed_significant_size(m_buffer, m_significant_len);
    }

public:
    explicit BigInteger(long long v = 0, memory_allocator *ma = NULL)
        : m_alloc(ma), m_buffer(NULL), m_cap(0), m_significant_len(0)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        ensure_cap(sizeof(v) / sizeof(word_type));
        ::memcpy(m_buffer, &v, sizeof(v));
        m_significant_len = sizeof(v) / sizeof(word_type);
        minimize_significant_len();
    }

    template <typename U>
    BigInteger(const U *buf, size_type len, bool with_sign, memory_allocator *ma = NULL)
        : m_alloc(ma), m_buffer(NULL), m_cap(0), m_significant_len(0)
    {
        assert(NULL != buf && len > 0);

        const uint8_t fill = (with_sign ? (nut::is_positive(buf, len) ? 0 : 0xFF) : 0);
        const size_type min_sig = sizeof(U) * len / sizeof(word_type) + 1; // 保证一个空闲字节放符号位
        ensure_cap(min_sig);
        ::memcpy(m_buffer, buf, sizeof(U) * len);
        ::memset(((U*) m_buffer) + len, fill, min_sig * sizeof(word_type) - sizeof(U) * len);
        m_significant_len = min_sig;
        minimize_significant_len();
    }

    // 上述模板函数的一个特化
    BigInteger(const word_type *buf, size_type len, bool with_sign, memory_allocator *ma = NULL)
        : m_alloc(ma), m_buffer(NULL), m_cap(0), m_significant_len(0)
    {
        assert(NULL != buf && len > 0);

        if (with_sign || nut::is_positive(buf, len))
        {
            ensure_cap(len);
            ::memcpy(m_buffer, buf, sizeof(word_type) * len);
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

    BigInteger(const self_type& x)
        : m_alloc(x.m_alloc), m_buffer(NULL), m_cap(0), m_significant_len(x.m_significant_len)
    {
        ensure_cap(x.m_significant_len);
        ::memcpy(m_buffer, x.m_buffer, sizeof(word_type) * x.m_significant_len);
    }

    ~BigInteger()
    {
        if (NULL != m_buffer)
            ma_free(m_alloc.pointer(), m_buffer);
        m_buffer = NULL;
        m_cap = 0;
        m_significant_len = 0;
    }

public:
    self_type& operator=(const self_type& x)
    {
        if (&x == this)
            return *this;

        ensure_cap(x.m_significant_len);
        ::memcpy(m_buffer, x.m_buffer, sizeof(word_type) * x.m_significant_len);
        m_significant_len = x.m_significant_len;
        return *this;
    }

    self_type& operator=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        ensure_cap(sizeof(v) / sizeof(word_type));
        ::memcpy(m_buffer, &v, sizeof(v));
        m_significant_len = sizeof(v) / sizeof(word_type);
        minimize_significant_len();

        return *this;
    }

    bool operator==(const self_type& x) const
    {
        if (&x == this)
            return true;
        return signed_equals(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len);
    }

    bool operator==(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        return signed_equals(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type));
    }

    bool operator!=(const self_type& x) const
    {
        return !(*this == x);
    }

    bool operator!=(long long v) const
    {
        return !(*this == v);
    }

    bool operator<(const self_type& x) const
    {
        return signed_less_than(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len);
    }

    bool operator<(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        return signed_less_than(m_buffer, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type));
    }

    bool operator>(const self_type& x) const
    {
        return x < *this;
    }

    bool operator>(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        return signed_less_than((word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer, m_significant_len);
    }

    bool operator<=(const self_type& x) const
    {
        return !(x < *this);
    }

    bool operator<=(long long v) const
    {
        return !(*this > v);
    }

    bool operator>=(const self_type& x) const
    {
        return !(*this < x);
    }

    bool operator>=(long long v) const
    {
        return !(*this < v);
    }

    self_type operator+(const self_type& x) const
    {
        self_type ret(0, m_alloc.pointer());
        self_type::add(*this, x, &ret);
        return ret;
    }

    self_type operator+(long long v) const
    {
        self_type ret(0, m_alloc.pointer());
        self_type::add(*this, v, &ret);
        return ret;
    }

    self_type operator-(const self_type& x) const
    {
        self_type ret(0, m_alloc.pointer());
        self_type::sub(*this, x, &ret);
        return ret;
    }

    self_type operator-(long long v) const
    {
        self_type ret(0, m_alloc.pointer());
        self_type::sub(*this, v, &ret);
        return ret;
    }

    self_type operator-() const
    {
        self_type ret(0, m_alloc.pointer());
        self_type::negate(*this, &ret);
        return ret;
    }

    self_type operator*(const self_type& x) const
    {
        self_type ret(0, m_alloc.pointer());
        self_type::multiply(*this, x, &ret);
        return ret;
    }

    self_type operator*(long long v) const
    {
        self_type ret(0, m_alloc.pointer());
        self_type::multiply(*this, v, &ret);
        return ret;
    }

    self_type operator/(const self_type& x) const
    {
        self_type ret(0, m_alloc.pointer());
        self_type::divide(*this, x, &ret, NULL);
        return ret;
    }

    self_type operator/(long long v) const
    {
        self_type divider(v, m_alloc.pointer()), ret(0, m_alloc.pointer());
        self_type::divide(*this, divider, &ret, NULL);
        return ret;
    }

    self_type operator%(const self_type& x) const
    {
        assert(!x.is_zero());

        // 简单优化
        if (self_type::is_positive() && x.is_positive())
        {
            if (*this < x)
                return *this;
            else if (*this < (x << 1))
                return *this - x;
        }

        self_type ret(0, m_alloc.pointer());
        self_type::divide(*this, x, NULL, &ret);
        return ret;
    }

    self_type operator%(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);
        assert(0 != v);

        self_type divider(v, m_alloc.pointer()), ret(0, m_alloc.pointer());
        self_type::divide(*this, divider, NULL, &ret);
        return ret;
    }

    self_type& operator+=(const self_type& x)
    {
        self_type::add(*this, x, this);
        return *this;
    }

    self_type& operator+=(long long v)
    {
        self_type::add(*this, v, this);
        return *this;
    }

    self_type& operator-=(const self_type& x)
    {
        self_type::sub(*this, x, this);
        return *this;
    }

    self_type& operator-=(long long v)
    {
        self_type::sub(*this, v, this);
        return *this;
    }

    self_type& operator*=(const self_type& x)
    {
        self_type::multiply(*this, x, this);
        return *this;
    }

    self_type& operator*=(long long v)
    {
        self_type::multiply(*this, v, this);
        return *this;
    }

    self_type& operator/=(const self_type& x)
    {
        self_type::divide(*this, x, this, NULL);
        return *this;
    }

    self_type& operator/=(long long v)
    {
        self_type divider(v, m_alloc.pointer());
        self_type::divide(*this, divider, this, NULL);
        return *this;
    }

    self_type& operator%=(const self_type& x)
    {
        assert(!x.is_zero());

        // 简单优化
        if (self_type::is_positive() && x.is_positive())
        {
            if (*this < x)
                return *this;
            else if (*this < (x << 1))
                return *this -= x;
        }

        self_type::divide(*this, x, NULL, this);
        return *this;
    }

    self_type& operator%=(long long v)
    {
        self_type divider(v, m_alloc.pointer());
        self_type::divide(*this, divider, NULL, this);
        return *this;
    }

    self_type& operator++()
    {
        self_type::increase(this);
        return *this;
    }

    self_type operator++(int)
    {
        self_type ret(*this);
        ++*this;
        return ret;
    }

    self_type& operator--()
    {
        self_type::decrease(this);
        return *this;
    }

    self_type operator--(int)
    {
        self_type ret(*this);
        --*this;
        return ret;
    }

    self_type operator<<(size_type count) const
    {
        if (0 == count)
            return *this;

        self_type ret;
        self_type::shift_left(*this, count, &ret);
        return ret;
    }

    /**
     * 符号扩展的右移
     */
    self_type operator>>(size_type count) const
    {
        if (0 == count)
            return *this;

        self_type ret;
        self_type::shift_right(*this, count, &ret);
        return ret;
    }

    self_type& operator<<=(size_type count)
    {
        if (0 == count)
            return *this;

        self_type::shift_left(*this, count, this);
        return *this;
    }

    self_type& operator>>=(size_type count)
    {
        if (0 == count)
            return *this;

        self_type::shift_right(*this, count, this);
        return *this;
    }
public:
    static void add(const self_type& a, const self_type& b, self_type *x)
    {
        assert(NULL != x);

        const size_type max_len = (a.m_significant_len > b.m_significant_len ? a.m_significant_len : b.m_significant_len);
        x->ensure_cap(max_len + 1);
        signed_add(a.m_buffer, a.m_significant_len, b.m_buffer, b.m_significant_len, x->m_buffer, max_len + 1, a.m_alloc.pointer());
        x->m_significant_len = max_len + 1;
        x->minimize_significant_len();
    }

    static void add(const self_type& a, long long b, self_type *x)
    {
        NUT_STATIC_ASSERT(sizeof(b) % sizeof(word_type) == 0);
        assert(NULL != x);

        const size_type max_len = (a.m_significant_len > sizeof(b) / sizeof(word_type) ? a.m_significant_len : sizeof(b) / sizeof(word_type));
        x->ensure_cap(max_len + 1);
        signed_add(a.m_buffer, a.m_significant_len, (word_type*)&b, sizeof(b) / sizeof(word_type), x->m_buffer, max_len + 1, a.m_alloc.pointer());
        x->m_significant_len = max_len + 1;
        x->minimize_significant_len();
    }

    static void add(long long a, const self_type& b, self_type *x)
    {
        NUT_STATIC_ASSERT(sizeof(a) % sizeof(word_type) == 0);
        assert(NULL != x);

        const size_type max_len = (sizeof(a) / sizeof(word_type) > b.m_significant_len ? sizeof(a) / sizeof(word_type) : b.m_significant_len);
        x->ensure_cap(max_len + 1);
        signed_add((word_type*)&a, sizeof(a) / sizeof(word_type), b.m_buffer, b.m_significant_len, x->m_buffer, max_len + 1, b.m_alloc.pointer());
        x->m_significant_len = max_len + 1;
        x->minimize_significant_len();
    }

    static void sub(const self_type& a, const self_type& b, self_type *x)
    {
        assert(NULL != x);

        const size_type max_len = (a.m_significant_len > b.m_significant_len ? a.m_significant_len : b.m_significant_len);
        x->ensure_cap(max_len + 1);
        signed_sub(a.m_buffer, a.m_significant_len, b.m_buffer, b.m_significant_len, x->m_buffer, max_len + 1, a.m_alloc.pointer());
        x->m_significant_len = max_len + 1;
        x->minimize_significant_len();
    }

    static void sub(const self_type& a, long long b, self_type *x)
    {
        NUT_STATIC_ASSERT(sizeof(b) % sizeof(word_type) == 0);
        assert(NULL != x);

        const size_type max_len = (a.m_significant_len > sizeof(b) / sizeof(word_type) ? a.m_significant_len : sizeof(b) / sizeof(word_type));
        x->ensure_cap(max_len + 1);
        signed_sub(a.m_buffer, a.m_significant_len, (word_type*)&b, sizeof(b) / sizeof(word_type), x->m_buffer, max_len + 1, a.m_alloc.pointer());
        x->m_significant_len = max_len + 1;
        x->minimize_significant_len();
    }

    static void sub(long long a, const self_type& b, self_type *x)
    {
        NUT_STATIC_ASSERT(sizeof(a) % sizeof(word_type) == 0);
        assert(NULL != x);

        const size_type max_len = (sizeof(a) / sizeof(word_type) > b.m_significant_len ? sizeof(a) / sizeof(word_type) : b.m_significant_len);
        x->ensure_cap(max_len + 1);
        signed_sub((word_type*)&a, sizeof(a) / sizeof(word_type), b.m_buffer, b.m_significant_len, x->m_buffer, max_len + 1, b.m_alloc.pointer());
        x->m_significant_len = max_len + 1;
        x->minimize_significant_len();
    }

    static void negate(const self_type &a, self_type *x)
    {
        assert(NULL != x);

        x->ensure_cap(a.m_significant_len + 1);
        signed_negate(a.m_buffer, a.m_significant_len, x->m_buffer, a.m_significant_len + 1, a.m_alloc.pointer());
        x->m_significant_len = a.m_significant_len + 1;
        x->minimize_significant_len();
    }

    static void increase(self_type *x)
    {
        assert(NULL != x);
        x->ensure_significant_len(x->m_significant_len + 1);
        nut::increase(x->m_buffer, x->m_significant_len);
        x->minimize_significant_len();
    }

    static void decrease(self_type *x)
    {
        assert(NULL != x);
        x->ensure_significant_len(x->m_significant_len + 1);
        nut::decrease(x->m_buffer, x->m_significant_len);
        x->minimize_significant_len();
    }

    static void multiply(const self_type& a, const self_type& b, self_type *x)
    {
        assert(NULL != x);

        x->ensure_cap(a.m_significant_len + b.m_significant_len);
        signed_multiply(a.m_buffer, a.m_significant_len, b.m_buffer, b.m_significant_len,
            x->m_buffer, a.m_significant_len + b.m_significant_len, a.m_alloc.pointer());
        x->m_significant_len = a.m_significant_len + b.m_significant_len;
        x->minimize_significant_len();
    }

    static void multiply(const self_type& a, long long b, self_type *x)
    {
        NUT_STATIC_ASSERT(sizeof(b) % sizeof(word_type) == 0);
        assert(NULL != x);

        x->ensure_cap(a.m_significant_len + sizeof(b) / sizeof(word_type));
        signed_multiply(a.m_buffer, a.m_significant_len, (word_type*)&b, sizeof(b) / sizeof(word_type),
            x->m_buffer, a.m_significant_len + sizeof(b) / sizeof(word_type), a.m_alloc.pointer());
        x->m_significant_len = a.m_significant_len + sizeof(b) / sizeof(word_type);
        x->minimize_significant_len();
    }

    static void multiply(long long a, const self_type& b, self_type *x)
    {
        NUT_STATIC_ASSERT(sizeof(a) % sizeof(word_type) == 0);
        assert(NULL != x);

        x->ensure_cap(sizeof(a) / sizeof(word_type) + b.m_significant_len);
        signed_multiply((word_type*)&a, sizeof(a) / sizeof(word_type), b.m_buffer, b.m_significant_len,
            x->m_buffer, sizeof(a) / sizeof(word_type) + b.m_significant_len, b.m_alloc.pointer());
        x->m_significant_len = sizeof(a) / sizeof(word_type) + b.m_significant_len;
        x->minimize_significant_len();
    }

    /**
     * @param result 商
     * @param remainder 余数
     */
    static void divide(const self_type& a, const self_type& b, self_type *result, self_type *remainder)
    {
        assert(NULL != result || NULL != remainder);
        assert(!b.is_zero());

        if (NULL != result)
            result->ensure_cap(a.m_significant_len);
        if (NULL != remainder)
            remainder->ensure_cap(b.m_significant_len);

        signed_divide(a.m_buffer, a.m_significant_len, b.m_buffer, b.m_significant_len,
               (NULL == result ? NULL : result->m_buffer), a.m_significant_len,
               (NULL == remainder ? NULL : remainder->m_buffer), b.m_significant_len,
               a.m_alloc.pointer());

        if (NULL != result)
        {
            result->m_significant_len = a.m_significant_len;
            result->minimize_significant_len();
        }
        if (NULL != remainder)
        {
            remainder->m_significant_len = b.m_significant_len;
            remainder->minimize_significant_len();
        }
    }

    static void shift_left(const self_type& a, size_type count, self_type *x)
    {
        const size_type min_sig = a.m_significant_len + (count - 1) / (8 * sizeof(word_type)) + 1;
        x->ensure_cap(min_sig);
        signed_shift_left(a.m_buffer, a.m_significant_len, x->m_buffer, min_sig, count);
        x->m_significant_len = min_sig;
        x->minimize_significant_len();
    }

    static void shift_right(const self_type& a, size_type count, self_type *x)
    {
        x->ensure_cap(a.m_significant_len);
        signed_shift_right(a.m_buffer, a.m_significant_len, x->m_buffer, a.m_significant_len, count);
        x->m_significant_len = a.m_significant_len;
        x->minimize_significant_len();
    }

public:
    void set_zero()
    {
        m_buffer[0] = 0;
        m_significant_len = 1;
    }

    bool is_zero() const
    {
        return nut::is_zero(m_buffer, m_significant_len);
    }

    bool is_positive() const
    {
        return nut::is_positive(m_buffer, m_significant_len);
    }

    void resize(size_type n)
    {
        assert(n > 0);
        ensure_significant_len(n);
        m_significant_len = n;
    }

    /**
     * 使值恒为正数，且比特长度小于 bit_len
     *
     * @return 注意，返回为正数
     */
    void limit_positive_bits_to(size_type bit_len)
    {
        assert(bit_len > 0);

#if (OPTIMIZE_LEVEL == 0)
        const size_type new_sig = bit_len / (8 * sizeof(word_type)) + 1;
        ensure_significant_len(new_sig);
        const size_type bits_shift = 8 * sizeof(word_type) - bit_len % (8 * sizeof(word_type));
        m_buffer[new_sig - 1] <<= bits_shift;
        m_buffer[new_sig - 1] >>= bits_shift;
        m_significant_len = new_sig;
        minimize_significant_len();
#else
        // 正数且有效位数较小，无需做任何事情
        const size_type min_sig = bit_len / (8 * sizeof(word_type));
        if (m_significant_len <= min_sig && self_type::is_positive())
            return;

        // 需要扩展符号位，或者需要截断
        const size_type bits_res = bit_len % (8 * sizeof(word_type));
        if (0 != bits_res || 0 == bit_at(bit_len - 1))
        {
            // 无需附加符号位
            const size_type new_sig = (bit_len + 8 * sizeof(word_type) - 1) / (8 * sizeof(word_type));
            ensure_significant_len(new_sig);
            if (0 != bits_res)
            {
                const size_type bits_shift = 8 * sizeof(word_type) - bits_res;
                m_buffer[new_sig - 1] <<= bits_shift;
                m_buffer[new_sig - 1] >>= bits_shift;
            }
            m_significant_len = new_sig;
        }
        else
        {
            // 需要附加符号位，以便保证结果是正数
            ensure_significant_len(min_sig + 1);
            m_buffer[min_sig] = 0;
            m_significant_len = min_sig + 1;
        }
        minimize_significant_len();
#endif
    }

    /**
     * 乘以a, 然后将比特长限制为小于 bit_len 的正数
     *
     * @return 注意，返回为正数
     */
    void multiply_to_len(const self_type& a, size_type bit_len)
    {
        const size_type words_len = (bit_len + 8 * sizeof(word_type) - 1) / (8 * sizeof(word_type));
        ensure_cap(words_len);
        signed_multiply(m_buffer, m_significant_len, a.m_buffer, a.m_significant_len, m_buffer, words_len, m_alloc.pointer());
        m_significant_len = words_len;
        limit_positive_bits_to(bit_len);
    }

    /**
     * 以word_type为单位计算有效字长度
     */
    size_type significant_words_length() const
    {
        return m_significant_len;
    }

    memory_allocator* allocator() const
    {
        return m_alloc.pointer();
    }

    const word_type* data() const
    {
        return m_buffer;
    }

    word_type* data()
    {
        return const_cast<word_type*>(static_cast<const self_type&>(*this).data());
    }

    /**
     * 返回比特位
     *
     * @return 0 or 1
     */
    int bit_at(size_type i) const
    {
        if (i / (8 * sizeof(word_type)) >= m_significant_len)
        {
#if (OPTIMIZE_LEVEL == 0)
            return self_type::is_positive() ? 0 : 1;
#else
            return m_buffer[m_significant_len - 1] >> (8 * sizeof(word_type) - 1);
#endif
        }
        return (m_buffer[i / (8 * sizeof(word_type))] >> (i % (8 * sizeof(word_type)))) & 0x01;
    }

    /**
     * 返回字值
     */
    word_type word_at(size_type i) const
    {
        if (i >= m_significant_len)
            return self_type::is_positive() ? 0 : ~(word_type)0;
        return m_buffer[i];
    }

    /**
     * @param v 0 or 1
     */
    void set_bit(size_type i, int v)
    {
    	assert(v == 0 || v == 1);
    	ensure_significant_len((i + 1) / (8 * sizeof(word_type)) + 1); // 避免符号位被覆盖
    	if (0 == v)
            m_buffer[i / (8 * sizeof(word_type))] &= ~(((word_type) 1) << (i % (8 * sizeof(word_type))));
    	else
            m_buffer[i / (8 * sizeof(word_type))] |= ((word_type)1) << (i % (8 * sizeof(word_type)));
    }

    void set_word(size_type i, word_type v)
    {
        ensure_significant_len(i + 1 + 1); // 避免符号位被覆盖
        m_buffer[i] = v;
    }

    size_type bit_length() const
    {
        if (self_type::is_positive())
            return nut::bit_length((uint8_t*)m_buffer, sizeof(word_type) * m_significant_len);
    	else
            return nut::bit0_length((uint8_t*)m_buffer, sizeof(word_type) * m_significant_len);
    }

    /**
     * 正数返回 bit 1 计数，负数则返回 bit 0 计数
     */
    size_type bit_count() const
    {
        const size_type bc = nut::bit_count((uint8_t*)m_buffer, sizeof(word_type) * m_significant_len);
        if (self_type::is_positive())
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
        signed_expand(m_buffer, m_significant_len, (word_type*)&ret, sizeof(ret) / sizeof(word_type));
        return ret;
    }

    /**
     * 取 [a, b) 范围内的随机数
     */
    static self_type rand_between(const self_type& a, const self_type& b)
    {
    	assert(a != b);

    	const bool a_is_bigger = (a > b);
        const self_type n = (a_is_bigger ? a - b : b - a);
    	assert(n.is_positive());

        self_type ret(0, a.allocator());
        ret.ensure_cap(n.m_significant_len + 1);
        for (size_type i = 0; i < n.m_significant_len; ++i)
        {
            for (size_type j = 0; j < sizeof(word_type); ++j)
            {
                ret.m_buffer[i] <<= 8;
                ret.m_buffer[i] += ::rand() & 0xFF;
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
    static void swap(self_type *a, self_type *b)
    {
        assert(NULL != a && NULL != b);
        word_type *tmp = a->m_buffer;
        const size_type tmp_sig = a->m_significant_len;
        a->m_buffer = NULL;
        a->m_cap = 0;

        a->ensure_cap(b->m_significant_len);
        ::memcpy(a->m_buffer, b->m_buffer, sizeof(word_type) * b->m_significant_len);
        a->m_significant_len = b->m_significant_len;

        b->ensure_cap(tmp_sig);
        ::memcpy(b->m_buffer, tmp, sizeof(word_type) * tmp_sig);
        b->m_significant_len = tmp_sig;

        ma_free(a->m_alloc.pointer(), tmp);
    }

private:
    static bool is_valid_radix(size_type radix)
    {
        return 1 < radix && radix <= 36;
    }

    static char num2char(size_type n)
    {
        assert(0 <= n && n < 36);
        return (n < 10 ? '0' + n : 'A' + n - 10);
    }

    static wchar_t num2wchar(size_type n)
    {
        assert(0 <= n && n < 36);
        return (n < 10 ? L'0' + n : L'A' + n - 10);
    }

public:
    void to_string(std::string *appended, size_type radix = 10) const
    {
        assert(NULL != appended && is_valid_radix(radix));
        self_type tmp(*this);
        const bool positive = tmp.is_positive();
        if (!positive)
            self_type::negate(tmp, &tmp);

        const self_type RADIX(radix);
        const size_t mark = appended->length();
        do
        {
            const size_type n = (size_t) (tmp % RADIX).llong_value();
            appended->push_back(num2char(n));

            tmp /= RADIX;
        } while (!tmp.is_zero());
        if (!positive)
            appended->push_back('-');
        std::reverse(appended->begin() + mark, appended->end());
    }

    void to_string(std::wstring *appended, size_type radix = 10) const
    {
        assert(NULL != appended && is_valid_radix(radix));
        self_type tmp(*this);
        const bool positive = tmp.is_positive();
        if (!positive)
            self_type::negate(tmp, &tmp);

        const self_type RADIX(radix, m_alloc.pointer());
        const size_t mark = appended->length();
        do
        {
            const size_type n = (size_t) (tmp % RADIX).llong_value();
            appended->push_back(num2wchar(n));

            tmp /= RADIX;
        } while (!tmp.is_zero());
        if (!positive)
            appended->push_back(L'-');
        std::reverse(appended->begin() + mark, appended->end());
    }

private:
    static bool is_blank(char c)
    {
        return ' ' == c || '\t' == c;
    }

    static bool is_blank(wchar_t c)
    {
        return L' ' == c || L'\t' == c;
    }

    static size_type skip_blank(const std::string& s, size_type start)
    {
        while (start < s.length() && is_blank(s[start]))
            ++start;
        return start;
    }

    static size_type skip_blank(const std::wstring& s, size_type start)
    {
        while (start < s.length() && is_blank(s[start]))
            ++start;
        return start;
    }

    static bool is_valid_char(char c, size_type radix)
    {
        assert(is_valid_radix(radix));
        if (radix <= 10)
            return '0' <= c && c <= '0' + (int) radix - 1;
        if ('0' <= c && c <= '9')
            return true;
        return 'a' <= (c | 0x20) && (c | 0x20) <= 'a' + (int) radix - 10 - 1;
    }

    static bool is_valid_char(wchar_t c, size_type radix)
    {
        assert(is_valid_radix(radix));
        if (radix <= 10)
            return L'0' <= c && c <= L'0' + (int) radix - 1;
        if (L'0' <= c && c <= L'9')
            return true;
        return L'a' <= (c | 0x20) && (c | 0x20) <= L'a' + (int) radix - 10 - 1;
    }

    static size_type char2num(char c)
    {
        assert(is_valid_char(c, 36));
        if ('0' <= c && c <= '9')
            return c - '0';
        return (c | 0x20) - 'a' + 10;
    }

    static size_type char2num(wchar_t c)
    {
        assert(is_valid_char(c, 36));
        if (L'0' <= c && c <= L'9')
            return c - L'0';
        return (c | 0x20) - L'a' + 10;
    }

public:
    static self_type value_of(const std::string& s, size_type radix = 10)
    {
        assert(radix > 1 && radix <= 36);
        self_type ret;

        // 略过空白
        size_type index = skip_blank(s, 0);
        if (index >= s.length())
            return ret;

        // 正负号
        bool positive = ('-' != s[index]);
        if ('+' == s[index] || '-' == s[index])
            if ((index = skip_blank(s, index + 1)) >= s.length())
                return ret;

        // 数字值
        while (index < s.length() && is_valid_char(s[index], radix))
        {
            ret *= radix;
            ret += char2num(s[index]);
            index = skip_blank(s, index + 1);
        }
        if (!positive)
            self_type::negate(ret, &ret);
        return ret;
    }

    static self_type value_of(const std::wstring& s, size_type radix = 10)
    {
        assert(radix > 1 && radix <= 36);
        self_type ret;

        // 略过空白
        size_type index = skip_blank(s, 0);
        if (index >= s.length())
            return ret;

        // 正负号
        bool positive = (L'-' != s[index]);
        if (L'+' == s[index] || L'-' == s[index])
            if ((index = skip_blank(s, index + 1)) >= s.length())
                return ret;

        // 数字值
        while (index < s.length() && is_valid_char(s[index], radix))
        {
            ret *= radix;
            ret += char2num(s[index]);
            index = skip_blank(s, index + 1);
        }
        if (!positive)
            self_type::negate(ret, &ret);
        return ret;
    }
};

#undef OPTIMIZE_LEVEL

}

#endif /* head file guarder */
