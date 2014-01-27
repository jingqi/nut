/**
 * @file -
 * @author jingqi
 * @date 2012-04-03
 * @last-edit 2013-02-04 19:23:46 jingqi
 */

#ifndef ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_
#define ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_

#include <assert.h>
#include <stdint.h>
#include <string>
#include <algorithm> // for std::reverse()

#include <nut/platform/stdint.hpp>
#include <nut/debugging/static_assert.hpp>
#include <nut/memtool/copyonwrite.hpp>

#include "word_array_integer.hpp"

#define OPTIMIZE_LEVEL 1000

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
    ref<FixedBuf<word_type> > m_buffer;
    /** 有效字长度 */
    size_t m_significant_len;

private:
    /**
     * copy-on-write，并确保缓冲区有足够的空间
     */
    void _copy_on_write(size_t size_needed = 0)
    {
        const size_t new_size = (size_needed > m_significant_len ? size_needed : m_significant_len);

        // buffer 为 null，则需要new
        if (m_buffer.isNull())
        {
            assert(0 == m_significant_len);
            if (new_size > 0)
                m_buffer = gc_new<FixedBuf<word_type>, RefCounterSync>(new_size);
            return;
        }

        // 原本 buffer 足够长，则 copy-on-write
        const int rc = m_buffer->get_ref();
        assert(rc >= 1);
        const size_t old_cap = m_buffer->len;
        if (old_cap >= new_size)
        {
            if (rc > 1)
            {
                ref<FixedBuf<word_type> > new_buf = gc_new<FixedBuf<word_type>, RefCounterSync>(new_size);
                ::memcpy(new_buf->buf, m_buffer->buf, m_significant_len * sizeof(word_type));
                m_buffer = new_buf;
            }
            return;
        }

        // new capacity
        size_t new_cap = old_cap * 3 / 2;
        if (new_cap < size_needed)
            new_cap = size_needed;
        if (rc == 1)
        {
            m_buffer->realloc(new_cap);
        }
        else
        {
            ref<FixedBuf<word_type> > new_buf = gc_new<FixedBuf<word_type>, RefCounterSync>(new_cap);
            ::memcpy(new_buf->buf, m_buffer->buf, m_significant_len * sizeof(word_type));
            m_buffer = new_buf;
        }
    }

    /**
     * 最小化有效字节长度
     */
    inline void minimize_significant_len()
    {
        m_significant_len = significant_size(m_buffer->buf, m_significant_len);
    }

    /**
     * 确保有效字节长度足够长，不够长则进行符号扩展
     */
    void ensure_significant_len(size_t siglen)
    {
        assert(siglen > 0);

        if (m_significant_len >= siglen)
            return;

        _copy_on_write(siglen);
        expand(m_buffer->buf, m_significant_len, m_buffer->buf, siglen);
        m_significant_len = siglen;
    }

public:
    _BigInteger()
        : m_significant_len(0)
    {
        _copy_on_write(1);
        m_buffer->buf[0] = 0;
        m_significant_len = 1;
    }
    
    explicit _BigInteger(long long v)
        : m_significant_len(0)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        _copy_on_write(sizeof(v) / sizeof(word_type));
        ::memcpy(m_buffer->buf, &v, sizeof(v));
        m_significant_len = sizeof(v) / sizeof(word_type);
        minimize_significant_len();
    }

    template <typename U>
    _BigInteger(const U *buf, size_t len, bool withSign)
        : m_significant_len(0)
    {
        assert(NULL != buf && len > 0);

        const uint8_t fill = (withSign ? (nut::is_positive(buf, len) ? 0 : 0xFF) : 0);
        const size_t min_sig = sizeof(U) * len / sizeof(word_type) + 1; // 保证一个空闲字节放符号位
        _copy_on_write(min_sig);
        ::memcpy(m_buffer->buf, buf, sizeof(U) * len);
        ::memset(((U*) m_buffer->buf) + len, fill, min_sig * sizeof(word_type) - sizeof(U) * len);
        m_significant_len = min_sig;
        minimize_significant_len();
    }

    // 上述模板函数的一个特化
    _BigInteger(const word_type *buf, size_t len, bool withSign)
        : m_significant_len(0)
    {
        assert(NULL != buf && len > 0);
        if (withSign || nut::is_positive(buf, len))
        {
            _copy_on_write(len);
            ::memcpy(m_buffer->buf, buf, sizeof(word_type) * len);
            m_significant_len = len;
        }
        else
        {
            _copy_on_write(len + 1);
            ::memcpy(m_buffer->buf, buf, sizeof(word_type) * len);
            m_buffer->buf[len] = 0;
            m_significant_len = len + 1;
        }
        minimize_significant_len();
    }

    _BigInteger(const self& x)
        : m_buffer(x.m_buffer), m_significant_len(x.m_significant_len) // copy on write
    {}

    ~_BigInteger()
    {
        m_significant_len = 0;
    }
    
public:
    self& operator=(const self& x)
    {
        if (&x == this)
            return *this;

        // copy on write
        m_buffer = x.m_buffer;
        m_significant_len = x.m_significant_len;
        return *this;
    }

    self& operator=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        _copy_on_write(sizeof(v) / sizeof(word_type));
        ::memcpy(m_buffer->buf, &v, sizeof(v));
        m_significant_len = sizeof(v) / sizeof(word_type);
        minimize_significant_len();

        return *this;
    }
    
    inline bool operator==(const self& x) const
    {
        if (&x == this)
            return true;
        if (m_significant_len == x.m_significant_len && m_buffer == x.m_buffer)
            return true;

        return equals(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len);
    }

    inline bool operator==(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        return equals(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type));
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
        return less_than(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len);
    }

    inline bool operator<(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        return less_than(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type));
    }
    
    inline bool operator>(const self& x) const
    {
        return x < *this;
    }

    inline bool operator>(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        return less_than((word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer->buf, m_significant_len);
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
        ret._copy_on_write(max_len + 1);
        add(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len, ret.m_buffer->buf, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }

    self operator+(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        self ret;
        const size_t max_len = (m_significant_len > sizeof(v) / sizeof(word_type) ? m_significant_len : sizeof(v) / sizeof(word_type));
        ret._copy_on_write(max_len + 1);
        add(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), ret.m_buffer->buf, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }
    
    self operator-(const self& x) const
    {
        self ret;
        const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
        ret._copy_on_write(max_len + 1);
        sub(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len, ret.m_buffer->buf, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }

    self operator-(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        self ret;
        const size_t max_len = (m_significant_len > sizeof(v) / sizeof(word_type) ? m_significant_len : sizeof(v) / sizeof(word_type));
        ret._copy_on_write(max_len + 1);
        sub(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), ret.m_buffer->buf, max_len + 1);
        ret.m_significant_len = max_len + 1;
        ret.minimize_significant_len();
        return ret;
    }
    
    self operator-() const
    {
        self ret;
        ret._copy_on_write(m_significant_len + 1);
        negate(m_buffer->buf, m_significant_len, ret.m_buffer->buf, m_significant_len + 1);
        ret.m_significant_len = m_significant_len + 1;
        ret.minimize_significant_len();
        return ret;
    }
    
    self operator*(const self& x) const
    {
        self ret;
        ret._copy_on_write(m_significant_len + x.m_significant_len);
        multiply(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len, ret.m_buffer->buf, m_significant_len + x.m_significant_len);
        ret.m_significant_len = m_significant_len + x.m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }

    self operator*(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        self ret;
        ret._copy_on_write(m_significant_len + sizeof(v) / sizeof(word_type));
        multiply(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), ret.m_buffer->buf, m_significant_len + sizeof(v) / sizeof(word_type));
        ret.m_significant_len = m_significant_len + sizeof(v) / sizeof(word_type);
        ret.minimize_significant_len();
        return ret;
    }
    
    self operator/(const self& x) const
    {
        assert(!x.is_zero());

        self ret;
        ret._copy_on_write(m_significant_len);
        divide(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len, ret.m_buffer->buf, m_significant_len, (word_type*)NULL, 0);
        ret.m_significant_len = m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }

    self operator/(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);
        assert(0 != v);

        self ret;
        ret._copy_on_write(m_significant_len);
        divide(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), ret.m_buffer->buf, m_significant_len, (word_type*)NULL, 0);
        ret.m_significant_len = m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }
    
    self operator%(const self& x) const
    {
        assert(!x.is_zero());

        // 小幅度优化
        if (is_positive() && x.is_positive())
        {
            if (*this < x)
                return *this;
            else if (*this < (x << 1))
                return *this - x;
        }
        
        self ret;
        ret._copy_on_write(x.m_significant_len);
        divide(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len, (word_type*)NULL, 0, ret.m_buffer->buf, x.m_significant_len);
        ret.m_significant_len = x.m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }

    self operator%(long long v) const
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);
        assert(0 != v);

        self ret;
        ret._copy_on_write(sizeof(v) / sizeof(word_type));
        divide(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), (word_type*)NULL, 0, ret.m_buffer->buf, sizeof(v) / sizeof(word_type));
        ret.m_significant_len = sizeof(v) / sizeof(word_type);
        ret.minimize_significant_len();
        return ret;
    }
    
    self& operator+=(const self& x)
    {
    	const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
    	_copy_on_write(max_len + 1);
    	add(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len, m_buffer->buf, max_len + 1);
        m_significant_len = max_len + 1;
    	minimize_significant_len();
        return *this;
    }

    self& operator+=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        const size_t max_len = (m_significant_len > sizeof(v) / sizeof(word_type) ? m_significant_len : sizeof(v) / sizeof(word_type));
        _copy_on_write(max_len + 1);
        add(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer->buf, max_len + 1);
        m_significant_len = max_len + 1;
        minimize_significant_len();
        return *this;
    }

    self& operator-=(const self& x)
    {
    	const size_t max_len = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
    	_copy_on_write(max_len + 1);
    	sub(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len, m_buffer->buf, max_len + 1);
        m_significant_len = max_len + 1;
    	minimize_significant_len();
        return *this;
    }

    self& operator-=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        const size_t max_len = (m_significant_len > sizeof(v) / sizeof(word_type) ? m_significant_len : sizeof(v) / sizeof(word_type));
        _copy_on_write(max_len + 1);
        sub(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer->buf, max_len + 1);
        m_significant_len = max_len + 1;
        minimize_significant_len();
        return *this;
    }

    self& operator*=(const self& x)
    {
    	_copy_on_write(m_significant_len + x.m_significant_len);
    	multiply(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len, m_buffer->buf, m_significant_len + x.m_significant_len);
        m_significant_len += x.m_significant_len;
    	minimize_significant_len();
        return *this;
    }

    self& operator*=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);

        _copy_on_write(m_significant_len + sizeof(v) / sizeof(word_type));
        multiply(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer->buf, m_significant_len + sizeof(v) / sizeof(word_type));
        m_significant_len += sizeof(v) / sizeof(word_type);
        minimize_significant_len();
        return *this;
    }

    self& operator/=(const self& x)
    {
        assert(!x.is_zero());

        _copy_on_write();
    	divide(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len, m_buffer->buf, m_significant_len, (word_type*)NULL, 0);
    	minimize_significant_len();
        return *this;
    }

    self& operator/=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);
        assert(0 != v);
    
        _copy_on_write();
        divide(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), m_buffer->buf, m_significant_len, (word_type*)NULL, 0);
        minimize_significant_len();
        return *this;
    }

    inline self& operator%=(const self& x)
    {
        assert(!x.is_zero());

        // 小幅度优化
        if (is_positive() && x.is_positive())
        {
            if (*this < x)
                return *this;
            else if (*this < (x << 1))
                return *this -= x;
        }

        _copy_on_write(x.m_significant_len);
        divide(m_buffer->buf, m_significant_len, x.m_buffer->buf, x.m_significant_len, (word_type*)NULL, 0, m_buffer->buf, x.m_significant_len);
        m_significant_len = x.m_significant_len;
        minimize_significant_len();
        return *this;
    }

    inline self& operator%=(long long v)
    {
        NUT_STATIC_ASSERT(sizeof(v) % sizeof(word_type) == 0);
        assert(0 != v);

        _copy_on_write(sizeof(v) / sizeof(word_type));
        divide(m_buffer->buf, m_significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type), (word_type*)NULL, 0, m_buffer->buf, sizeof(v) / sizeof(word_type));
        m_significant_len = sizeof(v) / sizeof(word_type);
        minimize_significant_len();
        return *this;
    }
    
    self& operator++()
    {
        ensure_significant_len(m_significant_len + 1);
        increase(m_buffer->buf, m_significant_len);
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
        decrease(m_buffer->buf, m_significant_len);
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

        self ret;
        const size_t min_sig = m_significant_len + (count - 1) / (8 * sizeof(word_type)) + 1;
        ret._copy_on_write(min_sig);
        shift_left(m_buffer->buf, m_significant_len, ret.m_buffer->buf, min_sig, count);
        ret.m_significant_len = min_sig;
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

        self ret;
        ret._copy_on_write(m_significant_len);
        shift_right(m_buffer->buf, m_significant_len, ret.m_buffer->buf, m_significant_len, count);
        ret.m_significant_len = m_significant_len;
        ret.minimize_significant_len();
        return ret;
    }

    inline self& operator<<=(size_t count)
    {
        if (0 == count)
            return *this;

        const size_t min_sig = m_significant_len + (count - 1) / (8 * sizeof(word_type)) + 1;
        _copy_on_write(min_sig);
        shift_left(m_buffer->buf, m_significant_len, m_buffer->buf, min_sig, count);
        m_significant_len = min_sig;
        minimize_significant_len();
        return *this;
    }

    inline self& operator>>=(size_t count)
    {
        if (0 == count)
            return *this;
    
        _copy_on_write();
        shift_right(m_buffer->buf, m_significant_len, m_buffer->buf, m_significant_len, count);
        minimize_significant_len();
        return *this;
    }

public:
    /**
     * 除法并返回商和余数
     */
    void divide_remainder(const self& divider, self *result, self *remainder) const
    {
        if (NULL != result)
            result->_copy_on_write(m_significant_len);
        if (NULL != remainder)
            remainder->_copy_on_write(divider->m_significant_len);

        divide(m_buffer->buf, m_significant_len, (NULL == result ? NULL : result->m_buffer->buf), m_significant_len, (NULL == remainder ? NULL : remainder->m_buffer->buf), divider->m_significant_len);

        if (NULL != result)
        {
            result->m_significant_len = m_significant_len;
            result->minimize_significant_len();
        }
        if (NULL != remainder)
        {
            remainder->m_significant_len = divider->m_significant_len;
            remainder->minimize_significant_len();
        }
    }

    inline void set_zero()
    {
         _copy_on_write();
        m_buffer->buf[0] = 0;
        m_significant_len = 1;
    }

    inline bool is_zero() const
    {
        return nut::is_zero(m_buffer->buf, m_significant_len);
    }
    
    inline bool is_positive() const
    {
        return nut::is_positive(m_buffer->buf, m_significant_len);
    }

    inline const word_type* buffer() const
    {
        return m_buffer->buf;
    }

    inline word_type* buffer()
    {
        _copy_on_write();
        return const_cast<word_type*>(static_cast<const self&>(*this).buffer());
    }

    inline void resize(size_t n)
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
    inline void limit_positive_bits_to(size_t bit_len)
    {
        assert(bit_len > 0);

#if (OPTIMIZE_LEVEL == 0)
        const size_t new_sig = bit_len / (8 * sizeof(word_type)) + 1;
        ensure_significant_len(new_sig);
        const size_t bits_shift = 8 * sizeof(word_type) - bit_len % (8 * sizeof(word_type));
        m_buffer->buf[new_sig - 1] <<= bits_shift;
        m_buffer->buf[new_sig - 1] >>= bits_shift;
        m_significant_len = new_sig;
        minimize_significant_len();
#else
        // 正数且有效位数较小，无需做任何事情
        const size_t min_sig = bit_len / (8 * sizeof(word_type));
        if (m_significant_len <= min_sig && is_positive())
            return;

        // 需要扩展符号位，或者需要截断
        const size_t bits_res = bit_len % (8 * sizeof(word_type));
        if (0 != bits_res || 0 == bit_at(bit_len - 1))
        {
            // 无需附加符号位
            const size_t new_sig = (bit_len + 8 * sizeof(word_type) - 1) / (8 * sizeof(word_type));
            ensure_significant_len(new_sig);
            if (0 != bits_res)
            {
                const size_t bits_shift = 8 * sizeof(word_type) - bits_res;
                m_buffer->buf[new_sig - 1] <<= bits_shift;
                m_buffer->buf[new_sig - 1] >>= bits_shift;
            }
            m_significant_len = new_sig;
        }
        else
        {
            // 需要附加符号位，以便保证结果是正数
            ensure_significant_len(min_sig + 1);
            m_buffer->buf[min_sig] = 0;
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
    inline void multiply_to_len(const self& a, size_t bit_len)
    {
        const size_t words_len = (bit_len + 8 * sizeof(word_type) - 1) / (8 * sizeof(word_type));
        _copy_on_write(words_len);
        multiply(m_buffer->buf, m_significant_len, a.m_buffer->buf, a.m_significant_len, m_buffer->buf, words_len);
        m_significant_len = words_len;
        limit_positive_bits_to(bit_len);
    }

    /**
     * 以word_type为单位计算有效字长度
     */
    inline size_t significant_words_length() const
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
        if (i / (8 * sizeof(word_type)) >= m_significant_len)
        {
#if (OPTIMIZE_LEVEL == 0)
            return is_positive() ? 0 : 1;
#else
            return m_buffer->buf[m_significant_len - 1] >> (8 * sizeof(word_type) - 1);
#endif
        }
        return (m_buffer->buf[i / (8 * sizeof(word_type))] >> (i % (8 * sizeof(word_type)))) & 0x01;
    }

    /**
     * 返回字值
     */
    inline word_type word_at(size_t i) const
    {
        if (i >= m_significant_len)
            return is_positive() ? 0 : ~(word_type)0;
        return m_buffer->buf[i];
    }

    /**
     * @param v 0 or 1
     */
    inline void set_bit(size_t i, int v)
    {
    	assert(v == 0 || v == 1);
    	ensure_significant_len((i + 1) / (8 * sizeof(word_type)) + 1); // 避免符号位被覆盖
    	if (0 == v)
    		m_buffer->buf[i / (8 * sizeof(word_type))] &= ~(((word_type) 1) << (i % (8 * sizeof(word_type))));
    	else
    		m_buffer->buf[i / (8 * sizeof(word_type))] |= ((word_type)1) << (i % (8 * sizeof(word_type)));
    }

    inline void set_word(size_t i, word_type v)
    {
        ensure_significant_len(i + 1 + 1); // 避免符号位被覆盖
        m_buffer->buf[i] = v;
    }
    
    inline size_t bit_length() const
    {
    	if (is_positive())
    		return nut::bit_length((uint8_t*)m_buffer->buf, sizeof(word_type) * m_significant_len);
    	else
    		return bit0_length((uint8_t*)m_buffer->buf, sizeof(word_type) * m_significant_len);
    }

    /**
     * 正数返回 bit 1 计数，负数则返回 bit 0 计数
     */
    inline size_t bit_count() const
    {
    	const size_t bc = nut::bit_count((uint8_t*)m_buffer->buf, sizeof(word_type) * m_significant_len);
    	if (is_positive())
    		return bc;
    	return 8 * sizeof(word_type) * m_significant_len - bc;
    }

    inline int lowest_bit() const
    {
        return nut::lowest_bit((uint8_t*)m_buffer->buf, sizeof(word_type) * m_significant_len);
    }

    inline long long llong_value() const
    {
        NUT_STATIC_ASSERT(sizeof(long long) % sizeof(word_type) == 0);

        long long ret = 0;
        expand(m_buffer->buf, m_significant_len, (word_type*)&ret, sizeof(ret) / sizeof(word_type));
        return ret;
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
    	ret._copy_on_write(n.m_significant_len + 1);
    	for (register size_t i = 0; i < n.m_significant_len; ++i)
        {
            for (register size_t j = 0; j < sizeof(word_type); ++j)
            {
                ret.m_buffer->buf[i] <<= 8;
    		    ret.m_buffer->buf[i] += rand() & 0xFF;
            }
        }
    	ret.m_buffer->buf[n.m_significant_len] = 0; // 保证是正数
    	ret.m_significant_len = n.m_significant_len + 1;

    	ret %= n;
    	ret += (a_is_bigger ? b : a);
    	return ret;
    }

    /**
     * 值交换
     */
    inline static void swap(self *a, self *b)
    {
        assert(NULL != a && NULL != b);
        ref<FixedBuf<word_type> > p = a->m_buffer;
        size_t s = a->m_significant_len;
        a->m_buffer = b->m_buffer;
        a->m_significant_len = b->m_significant_len;
        b->m_buffer = p;
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

#undef OPTIMIZE_LEVEL

}

#endif /* head file guarder */

