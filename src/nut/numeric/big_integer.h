
#ifndef ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_
#define ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_

#include <assert.h>
#include <stdint.h>
#include <string>

#include <nut/platform/int_type.h>

#include "../nut_config.h"
#include "word_array_integer.h"


namespace nut
{

/**
 * 无限大整数
 */
class NUT_API BigInteger
{
public:
    typedef size_t                                  size_type;
    typedef unsigned int                            word_type;
    typedef long long                               native_int_type;
    typedef StdInt<word_type>::double_unsigned_type dword_type;

    static_assert(sizeof(native_int_type) % sizeof(word_type) == 0, "Unexpected integer size");

    friend bool operator==(native_int_type a, const BigInteger& b);
    friend bool operator!=(native_int_type a, const BigInteger& b);
    friend bool operator<(native_int_type a, const BigInteger& b);
    friend bool operator>(native_int_type a, const BigInteger& b);
    friend bool operator<=(native_int_type a, const BigInteger& b);
    friend bool operator>=(native_int_type a, const BigInteger& b);

    friend BigInteger operator+(native_int_type a, const BigInteger& b);
    friend BigInteger operator-(native_int_type a, const BigInteger& b);
    friend BigInteger operator*(native_int_type a, const BigInteger& b);
    friend BigInteger operator/(native_int_type a, const BigInteger& b);
    friend BigInteger operator%(native_int_type a, const BigInteger& b);

public:
    explicit BigInteger(native_int_type v = 0);

    template <typename U>
    BigInteger(const U *buf, size_type len, bool with_sign)
    {
        assert(nullptr != buf && len > 0);

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

    BigInteger(BigInteger&& x);
    BigInteger(const BigInteger& x);

    ~BigInteger();

    BigInteger& operator=(BigInteger&& x);
    BigInteger& operator=(const BigInteger& x);
    BigInteger& operator=(native_int_type v);

    bool operator==(const BigInteger& x) const;
    bool operator==(native_int_type v) const;

    bool operator!=(const BigInteger& x) const;
    bool operator!=(native_int_type v) const;

    bool operator<(const BigInteger& x) const;
    bool operator<(native_int_type v) const;

    bool operator>(const BigInteger& x) const;
    bool operator>(native_int_type v) const;

    bool operator<=(const BigInteger& x) const;
    bool operator<=(native_int_type v) const;

    bool operator>=(const BigInteger& x) const;
    bool operator>=(native_int_type v) const;

    BigInteger operator+(const BigInteger& x) const;
    BigInteger operator+(native_int_type v) const;

    BigInteger operator-(const BigInteger& x) const;
    BigInteger operator-(native_int_type v) const;

    BigInteger operator-() const;

    BigInteger operator*(const BigInteger& x) const;
    BigInteger operator*(native_int_type v) const;

    BigInteger operator/(const BigInteger& x) const;
    BigInteger operator/(native_int_type v) const;

    BigInteger operator%(const BigInteger& x) const;
    BigInteger operator%(native_int_type v) const;

    BigInteger& operator+=(const BigInteger& x);
    BigInteger& operator+=(native_int_type v);

    BigInteger& operator-=(const BigInteger& x);
    BigInteger& operator-=(native_int_type v);

    BigInteger& operator*=(const BigInteger& x);
    BigInteger& operator*=(native_int_type v);

    BigInteger& operator/=(const BigInteger& x);
    BigInteger& operator/=(native_int_type v);

    BigInteger& operator%=(const BigInteger& x);
    BigInteger& operator%=(native_int_type v);

    BigInteger& operator++();
    BigInteger operator++(int);

    BigInteger& operator--();
    BigInteger operator--(int);

    BigInteger operator<<(size_type count) const;

    /**
     * 符号扩展的右移
     */
    BigInteger operator>>(size_type count) const;

    BigInteger& operator<<=(size_type count);
    BigInteger& operator>>=(size_type count);

    /**
     * @param result 商
     * @param remainder 余数
     */
    static void divide(const BigInteger& a, const BigInteger& b, BigInteger *result, BigInteger *remainder);

    void set_zero();

    bool is_zero() const;

    bool is_positive() const;

    void resize(size_type n);

    /**
     * 使值恒为正数，且比特长度小于 bit_len
     *
     * @note 注意，结果为正数
     */
    void limit_positive_bits_to(size_type bit_len);

    /**
     * 乘以a, 然后将比特长限制为小于 bit_len 的正数
     *
     * @note 注意，结果为正数
     */
    void multiply_to_len(const BigInteger& a, size_type bit_len);

    /**
     * 以word_type为单位计算有效字长度
     */
    size_type significant_words_length() const;

    const word_type* data() const;
    word_type* data();

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

    /**
     * 正数(负数)返回从高位到低位第一个 bit 1 (bit 0) 的位置 + 1
     */
    size_type bit_length() const;

    /**
     * 正数返回 bit 1 计数，负数则返回 bit 0 计数
     */
    size_type bit_count() const;

    int lowest_bit() const;

    /**
     * 取 [a, b) 范围内的随机数
     */
    static BigInteger rand_between(const BigInteger& a, const BigInteger& b);

    /**
     * 值交换
     */
    static void swap(BigInteger *a, BigInteger *b);

    /**
     * 如果超出返回值类型范围，返回值会被截断；否则做符号扩展
     */
    native_int_type to_integer() const;

    std::string to_string(size_type radix = 10) const;
    std::wstring to_wstring(size_type radix = 10) const;

    static BigInteger value_of(const std::string& s, size_type radix = 10);
    static BigInteger value_of(const std::wstring& s, size_type radix = 10);

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

private:
    word_type *_data = nullptr; // 缓冲区, little-endian, 带符号
    size_type _capacity = 0;
    size_type _significant_len = 0; // 有效 word 长度
};

bool operator==(BigInteger::native_int_type a, const BigInteger& b);
bool operator!=(BigInteger::native_int_type a, const BigInteger& b);
bool operator<(BigInteger::native_int_type a, const BigInteger& b);
bool operator>(BigInteger::native_int_type a, const BigInteger& b);
bool operator<=(BigInteger::native_int_type a, const BigInteger& b);
bool operator>=(BigInteger::native_int_type a, const BigInteger& b);

BigInteger operator+(BigInteger::native_int_type a, const BigInteger& b);
BigInteger operator-(BigInteger::native_int_type a, const BigInteger& b);
BigInteger operator*(BigInteger::native_int_type a, const BigInteger& b);
BigInteger operator/(BigInteger::native_int_type a, const BigInteger& b);
BigInteger operator%(BigInteger::native_int_type a, const BigInteger& b);

}

#endif /* head file guarder */
