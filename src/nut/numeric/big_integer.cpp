
#include <assert.h>
#include <string>
#include <algorithm> // for std::reverse()
#include <random>

#include "../platform/sys.h"
#include "../util/string/string_utils.h"
#include "big_integer.h"
#include "word_array_integer/word_array_integer.h"
#include "word_array_integer/mul_op.h"
#include "word_array_integer/div_op.h"
#include "word_array_integer/shift_op.h"
#include "word_array_integer/bit_op.h"
#include "numeric_algo/karatsuba.h"


namespace nut
{

#if NUT_ENDIAN_BIG_BYTE
BigInteger::BigInteger(cast_int_type v)
    : _significant_len((sizeof(cast_int_type) / sizeof(word_type)) << 1), _inner_integer(v)
{
    wswap<word_type>((const word_type*)&_inner_integer, sizeof(v) / sizeof(word_type)); // Word order to little-endian
    minimize_significant_len();
}
#endif

BigInteger::BigInteger(const void *buf, size_type cb, bool with_sign)
{
    assert(nullptr != buf && cb > 0);
    set(buf, cb, with_sign);
}

BigInteger::BigInteger(BigInteger&& x)
    : _significant_len(x._significant_len)
{
    ::memcpy(_inner_data, x._inner_data, INNER_BYTE_SIZE);

    x._significant_len = 0;
}

BigInteger::BigInteger(const BigInteger& x)
{
    const size_type x_siglen = x.significant_words_length();
    ensure_cap(x_siglen);
    ::memcpy(data(), x.data(), sizeof(word_type) * x_siglen);
    set_significant_len(x_siglen);
}

BigInteger::~BigInteger()
{
    if (is_using_heap())
        ::free(_heap_data);
    _significant_len = 0;
}

void BigInteger::ensure_cap(size_type new_size)
{
    // Small memory
    if (new_size <= INNER_CAPACITY)
    {
        assert(!is_using_heap() || _heap_capacity >= INNER_CAPACITY);
        return;
    }

    // Inner memory to heap memory
    if (!is_using_heap())
    {
        size_type new_cap = INNER_CAPACITY * 3 / 2;
        if (new_cap < new_size)
            new_cap = new_size;

        word_type *heap_data = (word_type*) ::malloc(sizeof(word_type) * new_cap);
        assert(nullptr != heap_data);
        ::memcpy(heap_data, _inner_data, sizeof(word_type) * significant_words_length());
        _significant_len |= 0x01; // Set using heap flag
        _heap_data = heap_data;
        _heap_capacity = new_cap;
        return;
    }

    // Heap memory to heap memory
    if (new_size <= _heap_capacity)
        return;

    size_type new_cap = _heap_capacity * 3 / 2;
    if (new_cap < new_size)
        new_cap = new_size;

    _heap_data = (word_type*) ::realloc(_heap_data, sizeof(word_type) * new_cap);
    assert(nullptr != _heap_data);
    _heap_capacity = new_cap;
}

/**
 * 确保有效字节长度足够长，不够长则进行符号扩展
 */
void BigInteger::ensure_significant_len(size_type new_siglen)
{
    assert(new_siglen > 0);
    const size_type old_siglen = significant_words_length();
    if (new_siglen <= old_siglen)
        return;

    ensure_cap(new_siglen);
    word_type *const raw_data = data();
    signed_expand(raw_data, old_siglen, raw_data, new_siglen);
    set_significant_len(new_siglen);
}

void BigInteger::minimize_significant_len()
{
    const size_type new_siglen = nut::signed_significant_size(data(), significant_words_length());
    set_significant_len(new_siglen);
}

BigInteger& BigInteger::operator=(BigInteger&& x)
{
    if (this == &x)
        return *this;

    if (is_using_heap())
        ::free(_heap_data);

    _significant_len = x._significant_len;
    ::memcpy(_inner_data, x._inner_data, INNER_BYTE_SIZE);

    x._significant_len = 0;

    return *this;
}

BigInteger& BigInteger::operator=(const BigInteger& x)
{
    if (this == &x)
        return *this;

    const size_type x_siglen = x.significant_words_length();
    ensure_cap(x_siglen);
    ::memcpy(data(), x.data(), sizeof(word_type) * x_siglen);
    set_significant_len(x_siglen);

    return *this;
}

BigInteger& BigInteger::operator=(cast_int_type v)
{
    const size_type v_siglen = sizeof(v) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&v, v_siglen); // Word order to little-endian
#endif

    ensure_cap(v_siglen);
    ::memcpy(data(), &v, sizeof(v));
    set_significant_len(v_siglen);
    minimize_significant_len();

    return *this;
}

bool BigInteger::operator==(const BigInteger& x) const
{
    if (this == &x)
        return true;
    return signed_compare(data(), significant_words_length(), x.data(), x.significant_words_length()) == 0;
}

bool BigInteger::operator==(cast_int_type v) const
{
    const size_type v_siglen = sizeof(v) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&v, v_siglen); // Word order to little-endian
#endif

    return signed_compare(data(), significant_words_length(), (const word_type*)&v, v_siglen) == 0;
}

bool BigInteger::operator!=(const BigInteger& x) const
{
    return !(*this == x);
}

bool BigInteger::operator!=(cast_int_type v) const
{
    return !(*this == v);
}

bool BigInteger::operator<(const BigInteger& x) const
{
    return signed_compare(data(), significant_words_length(), x.data(), x.significant_words_length()) < 0;
}

bool BigInteger::operator<(cast_int_type v) const
{
    const size_type v_siglen = sizeof(v) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&v, v_siglen); // Word order to little-endian
#endif

    return signed_compare(data(), significant_words_length(), (const word_type*)&v, v_siglen) < 0;
}

bool BigInteger::operator>(const BigInteger& x) const
{
    return x < *this;
}

bool BigInteger::operator>(cast_int_type v) const
{
    const size_type v_siglen = sizeof(v) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&v, v_siglen); // Word order to little-endian
#endif

    return signed_compare((const word_type*)&v, v_siglen, data(), significant_words_length()) < 0;
}

bool BigInteger::operator<=(const BigInteger& x) const
{
    return !(x < *this);
}

bool BigInteger::operator<=(cast_int_type v) const
{
    return !(*this > v);
}

bool BigInteger::operator>=(const BigInteger& x) const
{
    return !(*this < x);
}

bool BigInteger::operator>=(cast_int_type v) const
{
    return !(*this < v);
}

BigInteger BigInteger::operator+(const BigInteger& x) const
{
    BigInteger ret;
    const size_type siglen = significant_words_length(), x_siglen = x.significant_words_length(),
        ret_siglen = std::max(siglen, x_siglen) + 1;
    ret.ensure_cap(ret_siglen);
    signed_add(data(), siglen, x.data(), x_siglen, ret.data(), ret_siglen);
    ret.set_significant_len(ret_siglen);
    ret.minimize_significant_len();
    return ret;
}

BigInteger BigInteger::operator+(cast_int_type v) const
{
    const size_type v_siglen = sizeof(v) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&v, v_siglen); // Word order to little-endian
#endif

    BigInteger ret;
    const size_type siglen = significant_words_length(), ret_siglen = std::max(siglen, v_siglen) + 1;
    ret.ensure_cap(ret_siglen);
    signed_add(data(), siglen, (const word_type*)&v, v_siglen, ret.data(), ret_siglen);
    ret.set_significant_len(ret_siglen);
    ret.minimize_significant_len();
    return ret;
}

BigInteger BigInteger::operator-(const BigInteger& x) const
{
    BigInteger ret;
    const size_type siglen = significant_words_length(), x_siglen = x.significant_words_length(),
        ret_siglen = std::max(siglen, x_siglen) + 1;
    ret.ensure_cap(ret_siglen);
    signed_sub(data(), siglen, x.data(), x_siglen, ret.data(), ret_siglen);
    ret.set_significant_len(ret_siglen);
    ret.minimize_significant_len();
    return ret;
}

BigInteger BigInteger::operator-(cast_int_type v) const
{
    const size_type v_siglen = sizeof(v) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&v, v_siglen); // Word order to little-endian
#endif

    BigInteger ret;
    const size_type siglen = significant_words_length(), ret_siglen = std::max(siglen, v_siglen) + 1;
    ret.ensure_cap(ret_siglen);
    signed_sub(data(), siglen, (const word_type*)&v, v_siglen, ret.data(), ret_siglen);
    ret.set_significant_len(ret_siglen);
    ret.minimize_significant_len();
    return ret;
}

BigInteger BigInteger::operator-() const
{
    BigInteger ret;
    const size_type siglen = significant_words_length(), ret_siglen = siglen + 1;
    ret.ensure_cap(ret_siglen);
    signed_negate(data(), siglen, ret.data(), ret_siglen);
    ret.set_significant_len(ret_siglen);
    ret.minimize_significant_len();
    return ret;
}

BigInteger BigInteger::operator*(const BigInteger& x) const
{
    BigInteger ret;
    const size_type siglen = significant_words_length(), x_siglen = x.significant_words_length(),
        ret_siglen = siglen + x_siglen;
    ret.ensure_cap(ret_siglen);
    signed_karatsuba_multiply(data(), siglen, x.data(), x_siglen, ret.data(), ret_siglen);
    ret.set_significant_len(ret_siglen);
    ret.minimize_significant_len();
    return ret;
}

BigInteger BigInteger::operator*(cast_int_type v) const
{
    const size_type v_siglen = sizeof(v) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&v, v_siglen); // Word order to little-endian
#endif

    BigInteger ret;
    const size_type siglen = significant_words_length(), ret_siglen = siglen + v_siglen;
    ret.ensure_cap(ret_siglen);
    signed_multiply(data(), siglen, (const word_type*)&v, v_siglen, ret.data(), ret_siglen);
    ret.set_significant_len(ret_siglen);
    ret.minimize_significant_len();
    return ret;
}

BigInteger BigInteger::operator/(const BigInteger& x) const
{
    BigInteger ret;
    BigInteger::divide(*this, x, &ret, nullptr);
    return ret;
}

BigInteger BigInteger::operator/(cast_int_type v) const
{
    BigInteger ret;
    BigInteger::divide(*this, BigInteger(v), &ret, nullptr);
    return ret;
}

BigInteger BigInteger::operator%(const BigInteger& x) const
{
    assert(!x.is_zero());

    // 简单优化
    if (BigInteger::is_positive() && x.is_positive())
    {
        if (*this < x)
            return *this;
        else if (*this < (x << 1))
            return *this - x;
    }

    BigInteger ret;
    BigInteger::divide(*this, x, nullptr, &ret);
    return ret;
}

BigInteger BigInteger::operator%(cast_int_type v) const
{
    assert(0 != v);

    BigInteger ret;
    BigInteger::divide(*this, BigInteger(v), nullptr, &ret);
    return ret;
}

BigInteger& BigInteger::operator+=(const BigInteger& x)
{
    const size_type siglen = significant_words_length(), x_siglen = x.significant_words_length(),
        new_siglen = std::max(siglen, x_siglen) + 1;
    ensure_cap(new_siglen);
    word_type *const raw_data = data();
    signed_add(raw_data, siglen, x.data(), x_siglen, raw_data, new_siglen);
    set_significant_len(new_siglen);
    minimize_significant_len();
    return *this;
}

BigInteger& BigInteger::operator+=(cast_int_type v)
{
    const size_type v_siglen = sizeof(v) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&v, v_siglen); // Word order to little-endian
#endif

    const size_type siglen = significant_words_length(), new_siglen = std::max(siglen, v_siglen) + 1;
    ensure_cap(new_siglen);
    word_type *const raw_data = data();
    signed_add(raw_data, siglen, (const word_type*)&v, v_siglen, raw_data, new_siglen);
    set_significant_len(new_siglen);
    minimize_significant_len();
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& x)
{
    const size_type siglen = significant_words_length(), x_siglen = x.significant_words_length(),
        new_siglen = std::max(siglen, x_siglen) + 1;
    ensure_cap(new_siglen);
    word_type *const raw_data = data();
    signed_sub(raw_data, siglen, x.data(), x_siglen, raw_data, new_siglen);
    set_significant_len(new_siglen);
    minimize_significant_len();
    return *this;
}

BigInteger& BigInteger::operator-=(cast_int_type v)
{
    const size_type v_siglen = sizeof(v) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&v, v_siglen); // Word order to little-endian
#endif

    const size_type siglen = significant_words_length(), new_siglen = std::max(siglen, v_siglen) + 1;
    ensure_cap(new_siglen);
    word_type *const raw_data = data();
    signed_sub(raw_data, siglen, (const word_type*)&v, v_siglen, raw_data, new_siglen);
    set_significant_len(new_siglen);
    minimize_significant_len();
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& x)
{
    const size_type siglen = significant_words_length(), x_siglen = x.significant_words_length(),
        new_siglen = siglen + x_siglen;
    ensure_cap(new_siglen);
    word_type *const raw_data = data();
    signed_karatsuba_multiply(raw_data, siglen, x.data(), x_siglen, raw_data, new_siglen);
    set_significant_len(new_siglen);
    minimize_significant_len();
    return *this;
}

BigInteger& BigInteger::operator*=(cast_int_type v)
{
    const size_type v_siglen = sizeof(v) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&v, v_siglen); // Word order to little-endian
#endif

    const size_type siglen = significant_words_length(), new_siglen = siglen + v_siglen;
    ensure_cap(new_siglen);
    word_type *const raw_data = data();
    signed_multiply(raw_data, siglen, (const word_type*)&v, v_siglen, raw_data, new_siglen);
    set_significant_len(new_siglen);
    minimize_significant_len();
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& x)
{
    BigInteger::divide(*this, x, this, nullptr);
    return *this;
}

BigInteger& BigInteger::operator/=(cast_int_type v)
{
    BigInteger::divide(*this, BigInteger(v), this, nullptr);
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& x)
{
    assert(!x.is_zero());

    // 简单优化
    if (BigInteger::is_positive() && x.is_positive())
    {
        if (*this < x)
            return *this;
        else if (*this < (x << 1))
            return *this -= x;
    }

    BigInteger::divide(*this, x, nullptr, this);
    return *this;
}

BigInteger& BigInteger::operator%=(cast_int_type v)
{
    assert(0 != v);
    BigInteger::divide(*this, BigInteger(v), nullptr, this);
    return *this;
}

BigInteger& BigInteger::operator++()
{
    const size_type new_siglen = significant_words_length() + 1;
    ensure_significant_len(new_siglen);
    nut::increase(data(), new_siglen);
    minimize_significant_len();
    return *this;
}

BigInteger BigInteger::operator++(int)
{
    BigInteger ret(*this);
    ++*this;
    return ret;
}

BigInteger& BigInteger::operator--()
{
    const size_type new_siglen = significant_words_length() + 1;
    ensure_significant_len(new_siglen);
    nut::decrease(data(), new_siglen);
    minimize_significant_len();
    return *this;
}

BigInteger BigInteger::operator--(int)
{
    BigInteger ret(*this);
    --*this;
    return ret;
}

BigInteger BigInteger::operator<<(size_type count) const
{
    if (0 == count)
        return *this;

    BigInteger ret;
    const size_type siglen = significant_words_length(),
        ret_siglen = siglen + (count - 1) / (8 * sizeof(word_type)) + 1; // NOTE 额外的1个 word 防止符号位溢出
    ret.ensure_cap(ret_siglen);
    signed_shift_left(data(), siglen, ret.data(), ret_siglen, count);
    ret.set_significant_len(ret_siglen);
    ret.minimize_significant_len();
    return ret;
}

/**
 * 符号扩展的右移
 */
BigInteger BigInteger::operator>>(size_type count) const
{
    if (0 == count)
        return *this;

    BigInteger ret;
    const size_type siglen = significant_words_length();
    ret.ensure_cap(siglen);
    signed_shift_right(data(), siglen, ret.data(), siglen, count);
    ret.set_significant_len(siglen);
    ret.minimize_significant_len();
    return ret;
}

BigInteger& BigInteger::operator<<=(size_type count)
{
    if (0 == count)
        return *this;

    const size_type siglen = significant_words_length(),
        new_siglen = siglen + (count - 1) / (8 * sizeof(word_type)) + 1; // NOTE 额外的1个 word 防止符号位溢出
    ensure_cap(new_siglen);
    word_type *const raw_data = data();
    signed_shift_left(raw_data, siglen, raw_data, new_siglen, count);
    set_significant_len(new_siglen);
    minimize_significant_len();
    return *this;
}

BigInteger& BigInteger::operator>>=(size_type count)
{
    if (0 == count)
        return *this;

    const size_type siglen = significant_words_length();
    word_type *const raw_data = data();
    signed_shift_right(raw_data, siglen, raw_data, siglen, count);
    minimize_significant_len();
    return *this;
}

void BigInteger::divide(const BigInteger& a, const BigInteger& b,
                        BigInteger *result, BigInteger *remainder)
{
    assert(nullptr != result || nullptr != remainder);
    assert(!b.is_zero());

    const size_type a_siglen = a.significant_words_length(), b_siglen = b.significant_words_length();
    if (nullptr != result)
        result->ensure_cap(a_siglen);
    if (nullptr != remainder)
        remainder->ensure_cap(b_siglen);

    signed_divide(a.data(), a_siglen, b.data(), b_siglen,
                  (nullptr == result ? nullptr : result->data()), a_siglen,
                  (nullptr == remainder ? nullptr : remainder->data()), b_siglen);

    if (nullptr != result)
    {
        result->set_significant_len(a_siglen);
        result->minimize_significant_len();
    }
    if (nullptr != remainder)
    {
        remainder->set_significant_len(b_siglen);
        remainder->minimize_significant_len();
    }
}

int BigInteger::compare(const BigInteger& x) const
{
    return signed_compare(data(), significant_words_length(), x.data(), x.significant_words_length());
}

void BigInteger::set_zero()
{
    assert(significant_words_length() > 0);
    data()[0] = 0;
    set_significant_len(1);
}

bool BigInteger::is_zero() const
{
    return nut::is_zero(data(), significant_words_length());
}

bool BigInteger::is_positive() const
{
    return nut::is_positive(data(), significant_words_length());
}

bool BigInteger::is_negative() const
{
    return nut::is_negative(data(), significant_words_length());
}

void BigInteger::set(const void *buf, size_type cb, bool with_sign)
{
    assert(nullptr != buf && cb > 0);

    const size_type new_siglen = cb / sizeof(word_type) + 1; // NOTE 保证一个空闲字放符号位
    ensure_cap(new_siglen);
    word_type *const raw_data = data();
    ::memcpy(raw_data, buf, cb);
#if NUT_ENDIAN_BIG_BYTE
    bswap(raw_data, cb); // Byte order to little-endian
#endif
    const uint8_t fill = (with_sign ? (nut::is_positive((const uint8_t*) raw_data, cb) ? 0 : 0xFF) : 0);
    ::memset(((uint8_t*) raw_data) + cb, fill, sizeof(word_type) * new_siglen - cb);

#if NUT_ENDIAN_BIG_BYTE
    for (size_type i = 0; i < new_siglen; ++i)
        bswap(raw_data + i, sizeof(word_type)); // Local byte order back to big-endian
#endif

    set_significant_len(new_siglen);
    minimize_significant_len();
}

void BigInteger::resize(size_type n)
{
    assert(n > 0);
    ensure_significant_len(n);
    set_significant_len(n);
}

void BigInteger::limit_positive_bits_to(size_type bit_len)
{
    assert(bit_len > 0);

#if 0 // unoptimized
    const size_type new_siglen = bit_len / (8 * sizeof(word_type)) + 1;
    ensure_significant_len(new_siglen);
    const size_type bits_shift = 8 * sizeof(word_type) - bit_len % (8 * sizeof(word_type));
    word_type *const raw_data = data();
    raw_data[new_siglen - 1] <<= bits_shift;
    raw_data[new_siglen - 1] >>= bits_shift;
    set_significant_len(new_siglen);
    minimize_significant_len();
#else
    // 正数且有效位数较小，无需做任何事情
    const size_type min_siglen = bit_len / (8 * sizeof(word_type));
    if (significant_words_length() <= min_siglen && BigInteger::is_positive())
        return;

    // 需要扩展符号位，或者需要截断
    const size_type bits_res = bit_len % (8 * sizeof(word_type));
    if (0 != bits_res || 0 == bit_at(bit_len - 1))
    {
        // 无需附加符号位
        const size_type new_siglen = (bit_len + 8 * sizeof(word_type) - 1) / (8 * sizeof(word_type));
        ensure_significant_len(new_siglen);
        if (0 != bits_res)
        {
            const size_type bits_shift = 8 * sizeof(word_type) - bits_res;
            word_type *const raw_data = data();
            raw_data[new_siglen - 1] <<= bits_shift;
            raw_data[new_siglen - 1] >>= bits_shift;
        }
        set_significant_len(new_siglen);
    }
    else
    {
        // 需要附加符号位，以便保证结果是正数
        ensure_significant_len(min_siglen + 1);
        data()[min_siglen] = 0;
        set_significant_len(min_siglen + 1);
    }
    minimize_significant_len();
#endif
}

void BigInteger::multiply_to_len(const BigInteger& a, size_type bit_len)
{
    const size_type new_siglen = (bit_len + 8 * sizeof(word_type) - 1) / (8 * sizeof(word_type));
    ensure_cap(new_siglen);
    word_type *const raw_data = data();
    signed_karatsuba_multiply(raw_data, significant_words_length(),
                              a.data(), a.significant_words_length(),
                              raw_data, new_siglen);
    set_significant_len(new_siglen);
    limit_positive_bits_to(bit_len);
}

BigInteger::size_type BigInteger::significant_words_length() const
{
    return _significant_len >> 1;
}

bool BigInteger::is_using_heap() const
{
    return 0 != (_significant_len & 0x01);
}

void BigInteger::set_significant_len(size_type len)
{
    _significant_len = (len << 1) | (_significant_len & 0x01);
}

const BigInteger::word_type* BigInteger::data() const
{
    return is_using_heap() ? _heap_data : _inner_data;
}

BigInteger::word_type* BigInteger::data()
{
    return const_cast<word_type*>(static_cast<const BigInteger&>(*this).data());
}

int BigInteger::bit_at(size_type i) const
{
    const size_t siglen = significant_words_length();
    const word_type *const raw_data = data();
    if (i / (8 * sizeof(word_type)) >= siglen)
        return is_positive() ? 0 : 1; // 取符号位
    return (raw_data[i / (8 * sizeof(word_type))] >> (i % (8 * sizeof(word_type)))) & 0x01;
}

BigInteger::word_type BigInteger::word_at(size_type i) const
{
    if (i >= significant_words_length())
        return BigInteger::is_positive() ? 0 : ~(word_type)0;
    return data()[i];
}

/**
 * @param v 0 or 1
 */
void BigInteger::set_bit(size_type i, int v)
{
    assert(v == 0 || v == 1);
    ensure_significant_len((i + 1) / (8 * sizeof(word_type)) + 1); // 避免符号位被覆盖
    word_type *const raw_data = data();
    if (0 == v)
        raw_data[i / (8 * sizeof(word_type))] &= ~(((word_type) 1) << (i % (8 * sizeof(word_type))));
    else
        raw_data[i / (8 * sizeof(word_type))] |= ((word_type)1) << (i % (8 * sizeof(word_type)));
}

void BigInteger::set_word(size_type i, word_type v)
{
    ensure_significant_len(i + 1 + 1); // NOTE 多加一个 1 是为了避免符号位被覆盖
    data()[i] = v;
}

BigInteger::size_type BigInteger::bit_length() const
{
    if (BigInteger::is_positive())
        return nut::bit1_length(data(), significant_words_length());
    else
        return nut::bit0_length(data(), significant_words_length());
}

/**
 * 正数返回 bit 1 计数，负数则返回 bit 0 计数
 */
BigInteger::size_type BigInteger::bit_count() const
{
    const size_type siglen = significant_words_length(),
        bc = nut::bit1_count((uint8_t*) data(), sizeof(word_type) * siglen);
    if (BigInteger::is_positive())
        return bc;
    return 8 * sizeof(word_type) * siglen - bc;
}

ssize_t BigInteger::lowest_bit() const
{
    return nut::lowest_bit1(data(), significant_words_length());
}

/**
 * 取 [a, b) 范围内的随机数
 */
BigInteger BigInteger::rand_between(const BigInteger& a, const BigInteger& b)
{
    assert(a != b);

    const bool a_is_greater = (a > b);
    const BigInteger n = (a_is_greater ? a - b : b - a);
    assert(n.is_positive());

    BigInteger ret(0);
    const size_t n_siglen = n.significant_words_length();
    ret.ensure_cap(n_siglen + 1);

    word_type *const ret_data = ret.data();
    std::mt19937_64& gen = Sys::random_engine();
    std::uniform_int_distribution<word_type> dist(0, ~(word_type)0);
    for (size_type i = 0; i < n_siglen; ++i)
        ret_data[i] = dist(gen);

    ret_data[n_siglen] = 0; // 保证是正数
    ret.set_significant_len(n_siglen + 1);

    ret %= n;
    ret += (a_is_greater ? b : a);
    return ret;
}

BigInteger BigInteger::rand_positive(size_t bit_len, bool ensure_highest_bit)
{
    assert(bit_len > 0);

    const size_t word_count = (bit_len + sizeof(word_type) * 8 - 1) / (sizeof(word_type) * 8);
    const size_t redundant_bit_count = word_count * sizeof(word_type) * 8 - bit_len;
    assert(redundant_bit_count < sizeof(word_type) * 8);

    BigInteger ret(0);
    if (redundant_bit_count > 0)
    {
        ret.ensure_cap(word_count);
        ret.set_significant_len(word_count);
    }
    else
    {
        ret.ensure_cap(word_count + 1); // 多加一个 word 存储符号
        ret.data()[word_count] = 0;
        ret.set_significant_len(word_count + 1);
    }

    word_type *const ret_data = ret.data();
    std::mt19937_64& gen = Sys::random_engine();
    std::uniform_int_distribution<word_type> dist(0, ~(word_type)0);
    for (size_t i = 0; i < word_count; ++i)
        ret_data[i] = dist(gen);

    ret_data[word_count - 1] <<= redundant_bit_count;
    if (ensure_highest_bit)
        ret_data[word_count - 1] |= ((word_type) 1) << (sizeof(word_type) * 8 - 1);
    ret_data[word_count - 1] >>= redundant_bit_count;

    return ret;
}

/**
 * 值交换
 */
void BigInteger::swap(BigInteger *a, BigInteger *b)
{
    assert(nullptr != a && nullptr != b);

    const size_type tmp_siglen = a->_significant_len;
    uint8_t tmp_data[INNER_BYTE_SIZE];
    ::memcpy(tmp_data, a->_inner_data, INNER_BYTE_SIZE);

    a->_significant_len = b->_significant_len;
    ::memcpy(a->_inner_data, b->_inner_data, INNER_BYTE_SIZE);

    b->_significant_len = tmp_siglen;
    ::memcpy(b->_inner_data, tmp_data, INNER_BYTE_SIZE);
}

BigInteger::cast_int_type BigInteger::to_integer() const
{
    cast_int_type ret = 0;
    const size_t ret_siglen = sizeof(ret) / sizeof(word_type);
    signed_expand(data(), significant_words_length(), (word_type*)&ret, ret_siglen);

#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&ret, ret_siglen); // Word order to big-endian
#endif

    return ret;
}

std::vector<uint8_t> BigInteger::to_le_bytes() const
{
    const size_t siglen = significant_words_length();
    const word_type *const raw_data = data();
    std::vector<uint8_t> ret((const uint8_t*) raw_data, (const uint8_t*) (raw_data + siglen));

#if NUT_ENDIAN_BIG_BYTE
    for (size_type i = 0; i < siglen; ++i)
        bswap(ret.data() + sizeof(word_type) * i, sizeof(word_type)); // Local byte order to little-endian
#endif

    ret.resize(nut::signed_significant_size<uint8_t>(ret.data(), ret.size()));

    return ret;
}

std::vector<uint8_t> BigInteger::to_be_bytes() const
{
    std::vector<uint8_t> ret = to_le_bytes();
    std::reverse(ret.begin(), ret.end()); // Byte order to big-endian
    return ret;
}

#ifndef NDEBUG
// currently only used in DEBUG mode
static constexpr bool is_valid_radix(size_t radix)
{
    return 1 < radix && radix <= 36;
}
#endif

std::string BigInteger::to_string(size_type radix) const
{
    assert(is_valid_radix(radix));

    BigInteger tmp(*this);
    const bool negative = tmp.is_negative();
    if (negative)
        tmp = -tmp;

    std::string s;
    if (1 == nut::bit1_count((uint32_t) radix))
    {
        // 进制是 2 的幂次，可以通过移位优化
        const unsigned shift_count = nut::lowest_bit1((uint32_t) radix);
        assert(shift_count <= 8 * sizeof(word_type));
        const word_type mask = ~((~(word_type)0) << shift_count);
        do
        {
            s.push_back(int_to_char((int) (tmp.data()[0] & mask), true));
            tmp >>= shift_count;
        } while (!tmp.is_zero());
    }
    else
    {
        // 朴素算法
        const BigInteger RADIX(radix);
        do
        {
            BigInteger n;
            BigInteger::divide(tmp, RADIX, &tmp, &n);
            s.push_back(int_to_char((int) n.to_integer(), true));
        } while (!tmp.is_zero());
    }

    if (negative)
        s.push_back('-');
    std::reverse(s.begin(), s.end());
    return s;
}

std::wstring BigInteger::to_wstring(size_type radix) const
{
    assert(is_valid_radix(radix));

    BigInteger tmp(*this);
    const bool negative = tmp.is_negative();
    if (negative)
        tmp = -tmp;

    std::wstring s;
    if (1 == nut::bit1_count((uint32_t) radix))
    {
        // 进制是 2 的幂次，可以通过移位优化
        const unsigned shift_count = nut::lowest_bit1((uint32_t) radix);
        assert(shift_count <= 8 * sizeof(word_type));
        const word_type mask = ~((~(word_type)0) << shift_count);
        do
        {
            s.push_back(int_to_wchar((int) (tmp.data()[0] & mask), true));
            tmp >>= shift_count;
        } while (!tmp.is_zero());
    }
    else
    {
        // 朴素算法
        const BigInteger RADIX(radix);
        do
        {
            BigInteger n;
            BigInteger::divide(tmp, RADIX, &tmp, &n);
            s.push_back(int_to_wchar((int) n.to_integer(), true));
        } while (!tmp.is_zero());
    }

    if (negative)
        s.push_back(L'-');
    std::reverse(s.begin(), s.end());
    return s;
}

static constexpr bool is_blank(char c)
{
    return ' ' == c || '\t' == c;
}

static constexpr bool is_blank(wchar_t c)
{
    return L' ' == c || L'\t' == c;
}

static size_t skip_blank(const std::string& s, size_t start)
{
    while (start < s.length() && is_blank(s[start]))
        ++start;
    return start;
}

static size_t skip_blank(const std::wstring& s, size_t start)
{
    while (start < s.length() && is_blank(s[start]))
        ++start;
    return start;
}

static bool is_valid_char(char c, size_t radix)
{
    assert(is_valid_radix(radix));
    if (radix <= 10)
        return '0' <= c && c <= '0' + (int) radix - 1;
    if ('0' <= c && c <= '9')
        return true;
    c |= 0x20;
    return 'a' <= c && c <= 'a' + (int) radix - 10 - 1;
}

static bool is_valid_char(wchar_t c, size_t radix)
{
    assert(is_valid_radix(radix));
    if (radix <= 10)
        return L'0' <= c && c <= L'0' + (int) radix - 1;
    if (L'0' <= c && c <= L'9')
        return true;
    c |= 0x20;
    return L'a' <= c && c <= L'a' + (int) radix - 10 - 1;
}

BigInteger BigInteger::value_of(const std::string& s, size_type radix)
{
    assert(radix > 1 && radix <= 36);
    BigInteger ret;

    // 略过空白
    size_type index = skip_blank(s, 0);
    if (index >= s.length())
        return ret;

    // 正负号
    bool negative = ('-' == s[index]);
    if ('+' == s[index] || '-' == s[index])
    {
        index = skip_blank(s, index + 1);
        if (index >= s.length())
            return ret;
    }

    // 数字值
    const unsigned radix_bc = nut::bit1_count((uint32_t) radix),
        shift_count = nut::lowest_bit1((uint32_t) radix);
    assert(shift_count <= 8 * sizeof(word_type));
    while (index < s.length() && is_valid_char(s[index], radix))
    {
        if (1 == radix_bc)
        {
            ret <<= shift_count;
            ret.data()[0] |= char_to_int(s[index]);
        }
        else
        {
            ret *= radix;
            ret += char_to_int(s[index]);
        }

        index = skip_blank(s, index + 1);
    }

    if (negative)
        ret = -ret;
    return ret;
}

BigInteger BigInteger::value_of(const std::wstring& s, size_type radix)
{
    assert(radix > 1 && radix <= 36);
    BigInteger ret;

    // 略过空白
    size_type index = skip_blank(s, 0);
    if (index >= s.length())
        return ret;

    // 正负号
    bool negative = (L'-' == s[index]);
    if (L'+' == s[index] || L'-' == s[index])
    {
        index = skip_blank(s, index + 1);
        if (index >= s.length())
            return ret;
    }

    // 数字值
    const unsigned radix_bc = nut::bit1_count((uint32_t) radix),
        shift_count = nut::lowest_bit1((uint32_t) radix);
    while (index < s.length() && is_valid_char(s[index], radix))
    {
        if (1 == radix_bc)
        {
            ret <<= shift_count;
            ret.data()[0] |= char_to_int(s[index]);
        }
        else
        {
            ret *= radix;
            ret += char_to_int(s[index]);
        }

        index = skip_blank(s, index + 1);
    }

    if (negative)
        ret = -ret;
    return ret;
}

bool operator==(BigInteger::cast_int_type a, const BigInteger& b)
{
    return b == a;
}

bool operator!=(BigInteger::cast_int_type a, const BigInteger& b)
{
    return b != a;
}

bool operator<(BigInteger::cast_int_type a, const BigInteger& b)
{
    return b > a;
}

bool operator>(BigInteger::cast_int_type a, const BigInteger& b)
{
    return b < a;
}

bool operator<=(BigInteger::cast_int_type a, const BigInteger& b)
{
    return b >= a;
}

bool operator>=(BigInteger::cast_int_type a, const BigInteger& b)
{
    return b <= a;
}

BigInteger operator+(BigInteger::cast_int_type a, const BigInteger& b)
{
    return b + a;
}

BigInteger operator-(BigInteger::cast_int_type a, const BigInteger& b)
{
    typedef BigInteger::size_type size_type;
    typedef BigInteger::word_type word_type;

    const size_type a_siglen = sizeof(a) / sizeof(word_type);
#if NUT_ENDIAN_BIG_BYTE
    wswap<word_type>((word_type*)&a, a_siglen); // Word order to little-endian
#endif

    BigInteger ret;
    const size_type b_siglen = b.significant_words_length(),
        ret_siglen = std::max(a_siglen, b_siglen) + 1;
    ret.ensure_cap(ret_siglen);
    signed_sub((const word_type*)&a, a_siglen, b.data(), b_siglen, ret.data(), ret_siglen);
    ret.set_significant_len(ret_siglen);
    ret.minimize_significant_len();
    return ret;
}

BigInteger operator*(BigInteger::cast_int_type a, const BigInteger& b)
{
    return b * a;
}

BigInteger operator/(BigInteger::cast_int_type a, const BigInteger& b)
{
    return BigInteger(a) / b;
}

BigInteger operator%(BigInteger::cast_int_type a, const BigInteger& b)
{
    return BigInteger(a) % b;
}

}
