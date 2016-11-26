
#ifndef ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_
#define ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_

#include <assert.h>
#include <stdint.h>
#include <string>

#include <nut/platform/stdint_traits.h>

#include "../nut_config.h"
#include "word_array_integer.h"


namespace nut
{

/**
 * 无限大整数
 */
class NUT_API BigInteger
{
    typedef BigInteger self_type;

public:
    typedef size_t size_type;
    typedef unsigned int word_type;
    typedef StdInt<word_type>::double_unsigned_type dword_type;

private:
    word_type *_data = NULL; // 缓冲区, little-endian, 带符号
    size_type _capacity = 0;
    size_type _significant_len = 0; // 有效字长度

private:
    void ensure_cap(size_type new_size);

    /**
     * 确保有效字节长度足够长，不够长则进行符号扩展
     */
    void ensure_significant_len(size_type siglen);

    /**
     * 最小化有效字节长度
     */
    void minimize_significant_len()
    {
        _significant_len = nut::signed_significant_size(_data, _significant_len);
    }

public:
    explicit BigInteger(long long v = 0);

    template <typename U>
    BigInteger(const U *buf, size_type len, bool with_sign)
    {
        assert(NULL != buf && len > 0);

        const uint8_t fill = (with_sign ? (nut::is_positive(buf, len) ? 0 : 0xFF) : 0);
        const size_type min_sig = sizeof(U) * len / sizeof(word_type) + 1; // 保证一个空闲字节放符号位
        ensure_cap(min_sig);
        ::memcpy(_data, buf, sizeof(U) * len);
        ::memset(((U*) _data) + len, fill, min_sig * sizeof(word_type) - sizeof(U) * len);
        _significant_len = min_sig;
        minimize_significant_len();
    }

    // 上述模板函数的一个特化
    BigInteger(const word_type *buf, size_type len, bool with_sign);

    BigInteger(const self_type& x);

    BigInteger(self_type&& x);

    ~BigInteger();

public:
    self_type& operator=(const self_type& x);

    self_type& operator=(self_type&& x);

    self_type& operator=(long long v);

    bool operator==(const self_type& x) const
    {
        if (&x == this)
            return true;
        return signed_equals(_data, _significant_len, x._data, x._significant_len);
    }

    bool operator==(long long v) const
    {
        static_assert(sizeof(v) % sizeof(word_type) == 0, "整数长度对齐问题");
        return signed_equals(_data, _significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type));
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
        return signed_less_than(_data, _significant_len, x._data, x._significant_len);
    }

    bool operator<(long long v) const
    {
        static_assert(sizeof(v) % sizeof(word_type) == 0, "整数长度对齐问题");
        return signed_less_than(_data, _significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type));
    }

    bool operator>(const self_type& x) const
    {
        return x < *this;
    }

    bool operator>(long long v) const
    {
        static_assert(sizeof(v) % sizeof(word_type) == 0, "整数长度对齐问题");
        return signed_less_than((word_type*)&v, sizeof(v) / sizeof(word_type), _data, _significant_len);
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
        self_type ret(0);
        self_type::add(*this, x, &ret);
        return ret;
    }

    self_type operator+(long long v) const
    {
        self_type ret(0);
        self_type::add(*this, v, &ret);
        return ret;
    }

    self_type operator-(const self_type& x) const
    {
        self_type ret(0);
        self_type::sub(*this, x, &ret);
        return ret;
    }

    self_type operator-(long long v) const
    {
        self_type ret(0);
        self_type::sub(*this, v, &ret);
        return ret;
    }

    self_type operator-() const
    {
        self_type ret(0);
        self_type::negate(*this, &ret);
        return ret;
    }

    self_type operator*(const self_type& x) const
    {
        self_type ret(0);
        self_type::multiply(*this, x, &ret);
        return ret;
    }

    self_type operator*(long long v) const
    {
        self_type ret(0);
        self_type::multiply(*this, v, &ret);
        return ret;
    }

    self_type operator/(const self_type& x) const
    {
        self_type ret(0);
        self_type::divide(*this, x, &ret, NULL);
        return ret;
    }

    self_type operator/(long long v) const
    {
        self_type divider(v), ret(0);
        self_type::divide(*this, divider, &ret, NULL);
        return ret;
    }

    self_type operator%(const self_type& x) const;

    self_type operator%(long long v) const
    {
        static_assert(sizeof(v) % sizeof(word_type) == 0, "整数长度对齐问题");
        assert(0 != v);

        self_type divider(v), ret(0);
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
        self_type divider(v);
        self_type::divide(*this, divider, this, NULL);
        return *this;
    }

    self_type& operator%=(const self_type& x);

    self_type& operator%=(long long v)
    {
        assert(0 != v);
        self_type divider(v);
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
    void set_zero()
    {
        _data[0] = 0;
        _significant_len = 1;
    }

    bool is_zero() const
    {
        return nut::is_zero(_data, _significant_len);
    }

    bool is_positive() const
    {
        return nut::is_positive(_data, _significant_len);
    }

    void resize(size_type n)
    {
        assert(n > 0);
        ensure_significant_len(n);
        _significant_len = n;
    }

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
        return _significant_len;
    }

    const word_type* data() const
    {
        return _data;
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
    word_type word_at(size_type i) const
    {
        if (i >= _significant_len)
            return self_type::is_positive() ? 0 : ~(word_type)0;
        return _data[i];
    }

    /**
     * @param v 0 or 1
     */
    void set_bit(size_type i, int v);

    void set_word(size_type i, word_type v)
    {
        ensure_significant_len(i + 1 + 1); // NOTE 多加一个 1 是为了避免符号位被覆盖
        _data[i] = v;
    }

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
    std::string to_string(size_type radix = 10) const;
    std::wstring to_wstring(size_type radix = 10) const;

    static self_type value_of(const std::string& s, size_type radix = 10);
    static self_type value_of(const std::wstring& s, size_type radix = 10);
};

}

#endif /* head file guarder */
