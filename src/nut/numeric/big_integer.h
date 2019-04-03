
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
    typedef StdInt<word_type>::double_unsigned_type dword_type;

    static_assert(std::is_unsigned<word_type>::value, "Unexpected integer type");
    static_assert(std::is_signed<long long>::value, "Unexpected integer type");
    static_assert(sizeof(long long) % sizeof(word_type) == 0, "Unexpected integer size");

    friend bool operator==(long long a, const BigInteger& b);
    friend bool operator!=(long long a, const BigInteger& b);
    friend bool operator<(long long a, const BigInteger& b);
    friend bool operator>(long long a, const BigInteger& b);
    friend bool operator<=(long long a, const BigInteger& b);
    friend bool operator>=(long long a, const BigInteger& b);

    friend BigInteger operator+(long long a, const BigInteger& b);
    friend BigInteger operator-(long long a, const BigInteger& b);
    friend BigInteger operator*(long long a, const BigInteger& b);
    friend BigInteger operator/(long long a, const BigInteger& b);
    friend BigInteger operator%(long long a, const BigInteger& b);

public:
    explicit BigInteger(long long v = 0);
    BigInteger(const void *buf, size_type cb, bool with_sign);

    BigInteger(BigInteger&& x);
    BigInteger(const BigInteger& x);

    ~BigInteger();

    BigInteger& operator=(BigInteger&& x);
    BigInteger& operator=(const BigInteger& x);
    BigInteger& operator=(long long v);

    bool operator==(const BigInteger& x) const;
    bool operator==(long long v) const;

    bool operator!=(const BigInteger& x) const;
    bool operator!=(long long v) const;

    bool operator<(const BigInteger& x) const;
    bool operator<(long long v) const;

    bool operator>(const BigInteger& x) const;
    bool operator>(long long v) const;

    bool operator<=(const BigInteger& x) const;
    bool operator<=(long long v) const;

    bool operator>=(const BigInteger& x) const;
    bool operator>=(long long v) const;

    BigInteger operator+(const BigInteger& x) const;
    BigInteger operator+(long long v) const;

    BigInteger operator-(const BigInteger& x) const;
    BigInteger operator-(long long v) const;

    BigInteger operator-() const;

    BigInteger operator*(const BigInteger& x) const;
    BigInteger operator*(long long v) const;

    BigInteger operator/(const BigInteger& x) const;
    BigInteger operator/(long long v) const;

    BigInteger operator%(const BigInteger& x) const;
    BigInteger operator%(long long v) const;

    BigInteger& operator+=(const BigInteger& x);
    BigInteger& operator+=(long long v);

    BigInteger& operator-=(const BigInteger& x);
    BigInteger& operator-=(long long v);

    BigInteger& operator*=(const BigInteger& x);
    BigInteger& operator*=(long long v);

    BigInteger& operator/=(const BigInteger& x);
    BigInteger& operator/=(long long v);

    BigInteger& operator%=(const BigInteger& x);
    BigInteger& operator%=(long long v);

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

    int compare(const BigInteger& x) const;

    void set_zero();

    bool is_zero() const;

    /**
     * 是否为正数或者0
     */
    bool is_positive() const;

    void set(const void *buf, size_type cb, bool with_sign);

    /**
     * 截断或者符号扩充至指定字数
     *
     * @note 截断时可能导致符号翻转
     */
    void resize(size_type n);

    /**
     * 使值恒为正数(或 0)，且比特长度小于 bit_len
     *
     * @note 注意，结果为正数(或 0)
     */
    void limit_positive_bits_to(size_type bit_len);

    /**
     * 乘以a, 然后将比特长限制为小于 bit_len 的正数
     *
     * @note 注意，结果为正数(或 0)
     */
    void multiply_to_len(const BigInteger& a, size_type bit_len);

    /**
     * 以 word_type 为单位计算有效字长度
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

    ssize_t lowest_bit() const;

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
    long long to_integer() const;

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

bool operator==(long long a, const BigInteger& b);
bool operator!=(long long a, const BigInteger& b);
bool operator<(long long a, const BigInteger& b);
bool operator>(long long a, const BigInteger& b);
bool operator<=(long long a, const BigInteger& b);
bool operator>=(long long a, const BigInteger& b);

BigInteger operator+(long long a, const BigInteger& b);
BigInteger operator-(long long a, const BigInteger& b);
BigInteger operator*(long long a, const BigInteger& b);
BigInteger operator/(long long a, const BigInteger& b);
BigInteger operator%(long long a, const BigInteger& b);

}

#endif /* head file guarder */
