
#ifndef ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_
#define ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_

#include <assert.h>
#include <stdint.h>
#include <string>

#include <nut/platform/stdint.hpp>
#include <nut/debugging/static_assert.h>
#include <nut/mem/sys_ma.h>

#include "word_array_integer.hpp"

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
    void ensure_cap(size_type new_size);

    /**
     * 确保有效字节长度足够长，不够长则进行符号扩展
     */
    void ensure_significant_len(size_type siglen);

    /**
     * 最小化有效字节长度
     */
    void minimize_significant_len();

public:
    explicit BigInteger(long long v = 0, memory_allocator *ma = NULL);

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
    BigInteger(const word_type *buf, size_type len, bool with_sign, memory_allocator *ma = NULL);

    BigInteger(const self_type& x);

    ~BigInteger();

public:
    self_type& operator=(const self_type& x);

    self_type& operator=(long long v);

    bool operator==(const self_type& x) const;

    bool operator==(long long v) const;

    bool operator!=(const self_type& x) const
    {
        return !(*this == x);
    }

    bool operator!=(long long v) const
    {
        return !(*this == v);
    }

    bool operator<(const self_type& x) const;

    bool operator<(long long v) const;

    bool operator>(const self_type& x) const
    {
        return x < *this;
    }

    bool operator>(long long v) const;

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

    self_type operator%(const self_type& x) const;

    self_type operator%(long long v) const;

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

    self_type& operator%=(const self_type& x);

    self_type& operator%=(long long v);

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

    self_type operator<<(size_type count) const;

    /**
     * 符号扩展的右移
     */
    self_type operator>>(size_type count) const;

    self_type& operator<<=(size_type count);

    self_type& operator>>=(size_type count);

public:
    static void add(const self_type& a, const self_type& b, self_type *x);

    static void add(const self_type& a, long long b, self_type *x);

    static void add(long long a, const self_type& b, self_type *x);

    static void sub(const self_type& a, const self_type& b, self_type *x);

    static void sub(const self_type& a, long long b, self_type *x);

    static void sub(long long a, const self_type& b, self_type *x);

    static void negate(const self_type &a, self_type *x);

    static void increase(self_type *x);

    static void decrease(self_type *x);

    static void multiply(const self_type& a, const self_type& b, self_type *x);

    static void multiply(const self_type& a, long long b, self_type *x);

    static void multiply(long long a, const self_type& b, self_type *x);

    /**
     * @param result 商
     * @param remainder 余数
     */
    static void divide(const self_type& a, const self_type& b, self_type *result, self_type *remainder);

    static void shift_left(const self_type& a, size_type count, self_type *x);

    static void shift_right(const self_type& a, size_type count, self_type *x);

public:
    void set_zero();

    bool is_zero() const;

    bool is_positive() const;

    void resize(size_type n);

    /**
     * 使值恒为正数，且比特长度小于 bit_len
     *
     * @return 注意，返回为正数
     */
    void limit_positive_bits_to(size_type bit_len);

    /**
     * 乘以a, 然后将比特长限制为小于 bit_len 的正数
     *
     * @return 注意，返回为正数
     */
    void multiply_to_len(const self_type& a, size_type bit_len);

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
    int bit_at(size_type i) const;

    /**
     * 返回字值
     */
    word_type word_at(size_type i) const;

    /**
     * @param v 0 or 1
     */
    void set_bit(size_type i, int v);

    void set_word(size_type i, word_type v);

    size_type bit_length() const;

    /**
     * 正数返回 bit 1 计数，负数则返回 bit 0 计数
     */
    size_type bit_count() const;

    int lowest_bit() const;

    long long llong_value() const;

    /**
     * 取 [a, b) 范围内的随机数
     */
    static self_type rand_between(const self_type& a, const self_type& b);

    /**
     * 值交换
     */
    static void swap(self_type *a, self_type *b);

public:
    void to_string(std::string *appended, size_type radix = 10) const;
    void to_string(std::wstring *appended, size_type radix = 10) const;

    static self_type value_of(const std::string& s, size_type radix = 10);
    static self_type value_of(const std::wstring& s, size_type radix = 10);
};

#undef OPTIMIZE_LEVEL

}

#endif /* head file guarder */
