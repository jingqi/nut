
#ifndef ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_
#define ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_

#include <assert.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <type_traits>

#include "../nut_config.h"
#include "../platform/endian.h"
#include "../platform/int_type.h"


namespace nut
{

/**
 * 带符号无限大整数
 *
 * NOTE
 * - 由 '字(word)' 的数组来存储.
 * - word 之间按照 little-endian 顺序存储, 高位 word 在数组末尾; 最高位 word 的
 *   最高位 bit 表示整个 BitInteger 的符号.
 * - word 内部的字节序与本地字节序相同; 即在 big-endian 机器下是 big-endian, 在
 *   little-endian 机器下是 little-endian.
 */
class NUT_API BigInteger
{
public:
    typedef unsigned int                            word_type;
    typedef StdInt<word_type>::double_unsigned_type dword_type;
    typedef long long                               cast_int_type;
    typedef size_t                                  size_type;

    static_assert(std::is_unsigned<word_type>::value, "Unexpected integer type");
    static_assert(std::is_unsigned<dword_type>::value, "Unexpected integer type");
    static_assert(std::is_signed<cast_int_type>::value, "Unexpected integer type");
    static_assert(sizeof(cast_int_type) % sizeof(word_type) == 0, "Unexpected integer size");

    friend bool operator==(cast_int_type a, const BigInteger& b) noexcept;
    friend bool operator!=(cast_int_type a, const BigInteger& b) noexcept;
    friend bool operator<(cast_int_type a, const BigInteger& b) noexcept;
    friend bool operator>(cast_int_type a, const BigInteger& b) noexcept;
    friend bool operator<=(cast_int_type a, const BigInteger& b) noexcept;
    friend bool operator>=(cast_int_type a, const BigInteger& b) noexcept;

    friend BigInteger operator+(cast_int_type a, const BigInteger& b) noexcept;
    friend BigInteger operator-(cast_int_type a, const BigInteger& b) noexcept;
    friend BigInteger operator*(cast_int_type a, const BigInteger& b) noexcept;
    friend BigInteger operator/(cast_int_type a, const BigInteger& b) noexcept;
    friend BigInteger operator%(cast_int_type a, const BigInteger& b) noexcept;

public:
    constexpr BigInteger() noexcept
        : _significant_len(1 << 1), _inner_data{0} // only the first word is valid
    {}

#if NUT_ENDIAN_LITTLE_BYTE
    constexpr explicit BigInteger(cast_int_type v) noexcept
        : _significant_len((sizeof(v) / sizeof(word_type)) << 1), _inner_integer(v)
    {}
#else
    explicit BigInteger(cast_int_type v) noexcept;
#endif

    /**
     * @param buf 字节序应与本地一致
     */
    BigInteger(const void *buf, size_type cb, bool with_sign) noexcept;

    BigInteger(BigInteger&& x) noexcept;
    BigInteger(const BigInteger& x) noexcept;

    ~BigInteger() noexcept;

    BigInteger& operator=(BigInteger&& x) noexcept;
    BigInteger& operator=(const BigInteger& x) noexcept;
    BigInteger& operator=(cast_int_type v) noexcept;

    bool operator==(const BigInteger& x) const noexcept;
    bool operator==(cast_int_type v) const noexcept;

    bool operator!=(const BigInteger& x) const noexcept;
    bool operator!=(cast_int_type v) const noexcept;

    bool operator<(const BigInteger& x) const noexcept;
    bool operator<(cast_int_type v) const noexcept;

    bool operator>(const BigInteger& x) const noexcept;
    bool operator>(cast_int_type v) const noexcept;

    bool operator<=(const BigInteger& x) const noexcept;
    bool operator<=(cast_int_type v) const noexcept;

    bool operator>=(const BigInteger& x) const noexcept;
    bool operator>=(cast_int_type v) const noexcept;

    BigInteger operator+(const BigInteger& x) const noexcept;
    BigInteger operator+(cast_int_type v) const noexcept;

    BigInteger operator-(const BigInteger& x) const noexcept;
    BigInteger operator-(cast_int_type v) const noexcept;

    BigInteger operator-() const noexcept;

    BigInteger operator*(const BigInteger& x) const noexcept;
    BigInteger operator*(cast_int_type v) const noexcept;

    BigInteger operator/(const BigInteger& x) const noexcept;
    BigInteger operator/(cast_int_type v) const noexcept;

    BigInteger operator%(const BigInteger& x) const noexcept;
    BigInteger operator%(cast_int_type v) const noexcept;

    BigInteger& operator+=(const BigInteger& x) noexcept;
    BigInteger& operator+=(cast_int_type v) noexcept;

    BigInteger& operator-=(const BigInteger& x) noexcept;
    BigInteger& operator-=(cast_int_type v) noexcept;

    BigInteger& operator*=(const BigInteger& x) noexcept;
    BigInteger& operator*=(cast_int_type v) noexcept;

    BigInteger& operator/=(const BigInteger& x) noexcept;
    BigInteger& operator/=(cast_int_type v) noexcept;

    BigInteger& operator%=(const BigInteger& x) noexcept;
    BigInteger& operator%=(cast_int_type v) noexcept;

    BigInteger& operator++() noexcept;
    BigInteger operator++(int) noexcept;

    BigInteger& operator--() noexcept;
    BigInteger operator--(int) noexcept;

    BigInteger operator<<(size_type count) const noexcept;

    /**
     * 符号扩展的右移
     */
    BigInteger operator>>(size_type count) const noexcept;

    BigInteger& operator<<=(size_type count) noexcept;
    BigInteger& operator>>=(size_type count) noexcept;

    /**
     * @param result 商
     * @param remainder 余数
     */
    static void divide(const BigInteger& a, const BigInteger& b, BigInteger *result,
                       BigInteger *remainder) noexcept;

    int compare(const BigInteger& x) const noexcept;

    void set_zero() noexcept;

    bool is_zero() const noexcept;

    /**
     * 是否为正数或者0
     */
    bool is_positive() const noexcept;

    /**
     * 是否为负数
     */
    bool is_negative() const noexcept;

    /**
     * @param buf 字节序应与本地一致
     */
    void set(const void *buf, size_type cb, bool with_sign) noexcept;

    /**
     * 截断或者符号扩充至指定字数
     *
     * @note 截断时可能导致符号翻转
     */
    void resize(size_type n) noexcept;

    /**
     * 使值恒为正数(或 0)，且比特长度小于 bit_len
     *
     * @note 注意，结果为正数(或 0)
     */
    void limit_positive_bits_to(size_type bit_len) noexcept;

    /**
     * 乘以a, 然后将比特长限制为小于 bit_len 的正数
     *
     * @note 注意，结果为正数(或 0)
     */
    void multiply_to_len(const BigInteger& a, size_type bit_len) noexcept;

    /**
     * 以 word_type 为单位计算有效字长度
     */
    size_type significant_words_length() const noexcept;

    const word_type* data() const noexcept;
    word_type* data() noexcept;

    /**
     * 返回比特位
     *
     * @return 0 or 1
     */
    int bit_at(size_type i) const noexcept;

    /**
     * 返回字值
     */
    word_type word_at(size_type i) const noexcept;

    /**
     * @param v 0 or 1
     */
    void set_bit(size_type i, int v) noexcept;

    void set_word(size_type i, word_type v) noexcept;

    /**
     * 正数(负数)返回从高位到低位第一个 bit 1 (bit 0) 的位置 + 1
     */
    size_type bit_length() const noexcept;

    /**
     * 正数返回 bit 1 计数，负数则返回 bit 0 计数
     */
    size_type bit_count() const noexcept;

    ssize_t lowest_bit() const noexcept;

    /**
     * 取 [a, b) 范围内的随机数
     */
    static BigInteger rand_between(const BigInteger& a, const BigInteger& b) noexcept;

    /**
     * 随机一个 bit_len 位的正整数
     *
     * @param ensure_highest_bit 确保第 bit_len - 1 位一定是 1
     */
    static BigInteger rand_positive(size_t bit_len, bool ensure_highest_bit = false) noexcept;

    /**
     * 值交换
     */
    static void swap(BigInteger *a, BigInteger *b) noexcept;

    /**
     * 如果超出返回值类型范围，返回值会被截断；否则做符号扩展
     */
    cast_int_type to_integer() const noexcept;

    /**
     * 转为 little-endian(or big-endian) 字节数组
     */
    std::vector<uint8_t> to_le_bytes() const noexcept;
    std::vector<uint8_t> to_be_bytes() const noexcept;

    std::string to_string(size_type radix = 10) const noexcept;
    std::wstring to_wstring(size_type radix = 10) const noexcept;

    static BigInteger value_of(const std::string& s, size_type radix = 10) noexcept;
    static BigInteger value_of(const std::wstring& s, size_type radix = 10) noexcept;

private:
    /**
     * 当前是使用内部存储空间(小对象优化)还是堆上存储空间
     */
    bool is_using_heap() const noexcept;

    /**
     * 设置有效字节长度数值
     */
    void set_significant_len(size_type len) noexcept;

    /**
     * 确保有足够的存储空间
     */
    void ensure_cap(size_type new_size) noexcept;

    /**
     * 确保有效字节长度足够长，不够长则进行符号扩展
     */
    void ensure_significant_len(size_type new_siglen) noexcept;

    /**
     * 最小化有效字节长度
     */
    void minimize_significant_len() noexcept;

private:
    size_type _significant_len = 0; // big0: 是否使用 heap; other bits: 有效 word 长度

    struct HeapInfo
    {
        word_type *data = nullptr;
        size_type capacity = 0;
    };

    static constexpr size_t INNER_BYTE_SIZE = (
        sizeof(HeapInfo) > sizeof(cast_int_type) ? sizeof(HeapInfo) : sizeof(cast_int_type));
    static constexpr size_t INNER_CAPACITY = INNER_BYTE_SIZE / sizeof(word_type);

    union
    {
        HeapInfo _heap; // 堆上缓冲区, little-endian, 带符号
        cast_int_type _inner_integer;
        word_type _inner_data[INNER_CAPACITY]; // 内部缓冲区, little-endian, 带符号
    };
};


bool operator==(BigInteger::cast_int_type a, const BigInteger& b) noexcept;
bool operator!=(BigInteger::cast_int_type a, const BigInteger& b) noexcept;
bool operator<(BigInteger::cast_int_type a, const BigInteger& b) noexcept;
bool operator>(BigInteger::cast_int_type a, const BigInteger& b) noexcept;
bool operator<=(BigInteger::cast_int_type a, const BigInteger& b) noexcept;
bool operator>=(BigInteger::cast_int_type a, const BigInteger& b) noexcept;

BigInteger operator+(BigInteger::cast_int_type a, const BigInteger& b) noexcept;
BigInteger operator-(BigInteger::cast_int_type a, const BigInteger& b) noexcept;
BigInteger operator*(BigInteger::cast_int_type a, const BigInteger& b) noexcept;
BigInteger operator/(BigInteger::cast_int_type a, const BigInteger& b) noexcept;
BigInteger operator%(BigInteger::cast_int_type a, const BigInteger& b) noexcept;

}

#endif /* head file guarder */
