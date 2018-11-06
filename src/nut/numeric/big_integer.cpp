
#include <assert.h>
#include <string>
#include <algorithm> // for std::reverse()

#include "big_integer.h"
#include "word_array_integer.h"

#define OPTIMIZE_LEVEL 1000

namespace nut
{

BigInteger::BigInteger(long long v)
{
    static_assert(sizeof(v) % sizeof(word_type) == 0, "Unexpected integer size");

    ensure_cap(sizeof(v) / sizeof(word_type));
    ::memcpy(_data, &v, sizeof(v));
    _significant_len = sizeof(v) / sizeof(word_type);
    minimize_significant_len();
}

BigInteger::BigInteger(const word_type *buf, size_type len, bool with_sign)
{
    assert(nullptr != buf && len > 0);

    if (with_sign || nut::is_positive(buf, len))
    {
        ensure_cap(len);
        ::memcpy(_data, buf, sizeof(word_type) * len);
        _significant_len = len;
    }
    else
    {
        ensure_cap(len + 1);
        ::memcpy(_data, buf, sizeof(word_type) * len);
        _data[len] = 0;
        _significant_len = len + 1;
    }
    minimize_significant_len();
}

BigInteger::BigInteger(BigInteger&& x)
    : _data(x._data), _capacity(x._capacity), _significant_len(x._significant_len)
{
    x._data = nullptr;
    x._capacity = 0;
    x._significant_len = 0;
}

BigInteger::BigInteger(const BigInteger& x)
{
    ensure_cap(x._significant_len);
    ::memcpy(_data, x._data, sizeof(word_type) * x._significant_len);
    _significant_len = x._significant_len;
}

BigInteger::~BigInteger()
{
    if (nullptr != _data)
        ::free(_data);
    _data = nullptr;
    _capacity = 0;
    _significant_len = 0;
}

void BigInteger::ensure_cap(size_type new_size)
{
    if (new_size <= _capacity)
        return;

    size_type new_cap = _capacity * 3 / 2;
    if (new_cap < new_size)
        new_cap = new_size;

    _data = (word_type*) ::realloc(_data, sizeof(word_type) * new_cap);
    assert(nullptr != _data);
    _capacity = new_cap;
}

/**
 * 确保有效字节长度足够长，不够长则进行符号扩展
 */
void BigInteger::ensure_significant_len(size_type siglen)
{
    assert(siglen > 0);
    if (siglen <= _significant_len)
        return;

    ensure_cap(siglen);
    signed_expand(_data, _significant_len, _data, siglen);
    _significant_len = siglen;
}

void BigInteger::minimize_significant_len()
{
    _significant_len = nut::signed_significant_size(_data, _significant_len);
}

BigInteger& BigInteger::operator=(BigInteger&& x)
{
    if (this == &x)
        return *this;

    if (nullptr != _data)
        ::free(_data);

    _data = x._data;
    _capacity = x._capacity;
    _significant_len = x._significant_len;

    x._data = nullptr;
    x._capacity = 0;
    x._significant_len = 0;

    return *this;
}

BigInteger& BigInteger::operator=(const BigInteger& x)
{
    if (this == &x)
        return *this;

    ensure_cap(x._significant_len);
    ::memcpy(_data, x._data, sizeof(word_type) * x._significant_len);
    _significant_len = x._significant_len;
    return *this;
}

BigInteger& BigInteger::operator=(long long v)
{
    static_assert(sizeof(v) % sizeof(word_type) == 0, "Unexpected integer size");

    ensure_cap(sizeof(v) / sizeof(word_type));
    ::memcpy(_data, &v, sizeof(v));
    _significant_len = sizeof(v) / sizeof(word_type);
    minimize_significant_len();

    return *this;
}

bool BigInteger::operator==(const BigInteger& x) const
{
    if (&x == this)
        return true;
    return signed_equals(_data, _significant_len, x._data, x._significant_len);
}

bool BigInteger::operator==(long long v) const
{
    static_assert(sizeof(v) % sizeof(word_type) == 0, "整数长度对齐问题");
    return signed_equals(_data, _significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type));
}

bool BigInteger::operator!=(const BigInteger& x) const
{
    return !(*this == x);
}

bool BigInteger::operator!=(long long v) const
{
    return !(*this == v);
}

bool BigInteger::operator<(const BigInteger& x) const
{
    return signed_less_than(_data, _significant_len, x._data, x._significant_len);
}

bool BigInteger::operator<(long long v) const
{
    static_assert(sizeof(v) % sizeof(word_type) == 0, "整数长度对齐问题");
    return signed_less_than(_data, _significant_len, (word_type*)&v, sizeof(v) / sizeof(word_type));
}

bool BigInteger::operator>(const BigInteger& x) const
{
    return x < *this;
}

bool BigInteger::operator>(long long v) const
{
    static_assert(sizeof(v) % sizeof(word_type) == 0, "整数长度对齐问题");
    return signed_less_than((word_type*)&v, sizeof(v) / sizeof(word_type), _data, _significant_len);
}

bool BigInteger::operator<=(const BigInteger& x) const
{
    return !(x < *this);
}

bool BigInteger::operator<=(long long v) const
{
    return !(*this > v);
}

bool BigInteger::operator>=(const BigInteger& x) const
{
    return !(*this < x);
}

bool BigInteger::operator>=(long long v) const
{
    return !(*this < v);
}

BigInteger BigInteger::operator+(const BigInteger& x) const
{
    BigInteger ret(0);
    BigInteger::add(*this, x, &ret);
    return ret;
}

BigInteger BigInteger::operator+(long long v) const
{
    BigInteger ret(0);
    BigInteger::add(*this, v, &ret);
    return ret;
}

BigInteger BigInteger::operator-(const BigInteger& x) const
{
    BigInteger ret(0);
    BigInteger::sub(*this, x, &ret);
    return ret;
}

BigInteger BigInteger::operator-(long long v) const
{
    BigInteger ret(0);
    BigInteger::sub(*this, v, &ret);
    return ret;
}

BigInteger BigInteger::operator-() const
{
    BigInteger ret(0);
    BigInteger::negate(*this, &ret);
    return ret;
}

BigInteger BigInteger::operator*(const BigInteger& x) const
{
    BigInteger ret(0);
    BigInteger::multiply(*this, x, &ret);
    return ret;
}

BigInteger BigInteger::operator*(long long v) const
{
    BigInteger ret(0);
    BigInteger::multiply(*this, v, &ret);
    return ret;
}

BigInteger BigInteger::operator/(const BigInteger& x) const
{
    BigInteger ret(0);
    BigInteger::divide(*this, x, &ret, nullptr);
    return ret;
}

BigInteger BigInteger::operator/(long long v) const
{
    BigInteger divider(v), ret(0);
    BigInteger::divide(*this, divider, &ret, nullptr);
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

    BigInteger ret(0);
    BigInteger::divide(*this, x, nullptr, &ret);
    return ret;
}

BigInteger BigInteger::operator%(long long v) const
{
    static_assert(sizeof(v) % sizeof(word_type) == 0, "整数长度对齐问题");
    assert(0 != v);

    BigInteger divider(v), ret(0);
    BigInteger::divide(*this, divider, nullptr, &ret);
    return ret;
}

BigInteger& BigInteger::operator+=(const BigInteger& x)
{
    BigInteger::add(*this, x, this);
    return *this;
}

BigInteger& BigInteger::operator+=(long long v)
{
    BigInteger::add(*this, v, this);
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& x)
{
    BigInteger::sub(*this, x, this);
    return *this;
}

BigInteger& BigInteger::operator-=(long long v)
{
    BigInteger::sub(*this, v, this);
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& x)
{
    BigInteger::multiply(*this, x, this);
    return *this;
}

BigInteger& BigInteger::operator*=(long long v)
{
    BigInteger::multiply(*this, v, this);
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& x)
{
    BigInteger::divide(*this, x, this, nullptr);
    return *this;
}

BigInteger& BigInteger::operator/=(long long v)
{
    BigInteger divider(v);
    BigInteger::divide(*this, divider, this, nullptr);
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

BigInteger& BigInteger::operator%=(long long v)
{
    assert(0 != v);
    BigInteger divider(v);
    BigInteger::divide(*this, divider, nullptr, this);
    return *this;
}

BigInteger& BigInteger::operator++()
{
    BigInteger::increase(this);
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
    BigInteger::decrease(this);
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
    BigInteger::shift_left(*this, count, &ret);
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
    BigInteger::shift_right(*this, count, &ret);
    return ret;
}

BigInteger& BigInteger::operator<<=(size_type count)
{
    if (0 == count)
        return *this;

    BigInteger::shift_left(*this, count, this);
    return *this;
}

BigInteger& BigInteger::operator>>=(size_type count)
{
    if (0 == count)
        return *this;

    BigInteger::shift_right(*this, count, this);
    return *this;
}

void BigInteger::add(const BigInteger& a, const BigInteger& b, BigInteger *x)
{
    assert(nullptr != x);

    const size_type max_len = (a._significant_len > b._significant_len ? a._significant_len : b._significant_len);
    x->ensure_cap(max_len + 1);
    signed_add(a._data, a._significant_len, b._data, b._significant_len, x->_data, max_len + 1);
    x->_significant_len = max_len + 1;
    x->minimize_significant_len();
}

void BigInteger::add(const BigInteger& a, long long b, BigInteger *x)
{
    static_assert(sizeof(b) % sizeof(word_type) == 0, "Unexpected integer size");
    assert(nullptr != x);

    const size_type max_len = (a._significant_len > sizeof(b) / sizeof(word_type) ? a._significant_len : sizeof(b) / sizeof(word_type));
    x->ensure_cap(max_len + 1);
    signed_add(a._data, a._significant_len, (word_type*)&b, sizeof(b) / sizeof(word_type), x->_data, max_len + 1);
    x->_significant_len = max_len + 1;
    x->minimize_significant_len();
}

void BigInteger::add(long long a, const BigInteger& b, BigInteger *x)
{
    static_assert(sizeof(a) % sizeof(word_type) == 0, "Unexpected integer size");
    assert(nullptr != x);

    const size_type max_len = (sizeof(a) / sizeof(word_type) > b._significant_len ? sizeof(a) / sizeof(word_type) : b._significant_len);
    x->ensure_cap(max_len + 1);
    signed_add((word_type*)&a, sizeof(a) / sizeof(word_type), b._data, b._significant_len, x->_data, max_len + 1);
    x->_significant_len = max_len + 1;
    x->minimize_significant_len();
}

void BigInteger::sub(const BigInteger& a, const BigInteger& b, BigInteger *x)
{
    assert(nullptr != x);

    const size_type max_len = (a._significant_len > b._significant_len ? a._significant_len : b._significant_len);
    x->ensure_cap(max_len + 1);
    signed_sub(a._data, a._significant_len, b._data, b._significant_len, x->_data, max_len + 1);
    x->_significant_len = max_len + 1;
    x->minimize_significant_len();
}

void BigInteger::sub(const BigInteger& a, long long b, BigInteger *x)
{
    static_assert(sizeof(b) % sizeof(word_type) == 0, "Unexpected integer size");
    assert(nullptr != x);

    const size_type max_len = (a._significant_len > sizeof(b) / sizeof(word_type) ? a._significant_len : sizeof(b) / sizeof(word_type));
    x->ensure_cap(max_len + 1);
    signed_sub(a._data, a._significant_len, (word_type*)&b, sizeof(b) / sizeof(word_type), x->_data, max_len + 1);
    x->_significant_len = max_len + 1;
    x->minimize_significant_len();
}

void BigInteger::sub(long long a, const BigInteger& b, BigInteger *x)
{
    static_assert(sizeof(a) % sizeof(word_type) == 0, "Unexpected integer size");
    assert(nullptr != x);

    const size_type max_len = (sizeof(a) / sizeof(word_type) > b._significant_len ? sizeof(a) / sizeof(word_type) : b._significant_len);
    x->ensure_cap(max_len + 1);
    signed_sub((word_type*)&a, sizeof(a) / sizeof(word_type), b._data, b._significant_len, x->_data, max_len + 1);
    x->_significant_len = max_len + 1;
    x->minimize_significant_len();
}

void BigInteger::negate(const BigInteger &a, BigInteger *x)
{
    assert(nullptr != x);

    x->ensure_cap(a._significant_len + 1);
    signed_negate(a._data, a._significant_len, x->_data, a._significant_len + 1);
    x->_significant_len = a._significant_len + 1;
    x->minimize_significant_len();
}

void BigInteger::increase(BigInteger *x)
{
    assert(nullptr != x);
    x->ensure_significant_len(x->_significant_len + 1);
    nut::increase(x->_data, x->_significant_len);
    x->minimize_significant_len();
}

void BigInteger::decrease(BigInteger *x)
{
    assert(nullptr != x);
    x->ensure_significant_len(x->_significant_len + 1);
    nut::decrease(x->_data, x->_significant_len);
    x->minimize_significant_len();
}

void BigInteger::multiply(const BigInteger& a, const BigInteger& b, BigInteger *x)
{
    assert(nullptr != x);

    x->ensure_cap(a._significant_len + b._significant_len);
    signed_multiply(a._data, a._significant_len, b._data, b._significant_len,
        x->_data, a._significant_len + b._significant_len);
    x->_significant_len = a._significant_len + b._significant_len;
    x->minimize_significant_len();
}

void BigInteger::multiply(const BigInteger& a, long long b, BigInteger *x)
{
    static_assert(sizeof(b) % sizeof(word_type) == 0, "Unexpected integer size");
    assert(nullptr != x);

    x->ensure_cap(a._significant_len + sizeof(b) / sizeof(word_type));
    signed_multiply(a._data, a._significant_len, (word_type*)&b, sizeof(b) / sizeof(word_type),
        x->_data, a._significant_len + sizeof(b) / sizeof(word_type));
    x->_significant_len = a._significant_len + sizeof(b) / sizeof(word_type);
    x->minimize_significant_len();
}

void BigInteger::multiply(long long a, const BigInteger& b, BigInteger *x)
{
    static_assert(sizeof(a) % sizeof(word_type) == 0, "Unexpected integer size");
    assert(nullptr != x);

    x->ensure_cap(sizeof(a) / sizeof(word_type) + b._significant_len);
    signed_multiply((word_type*)&a, sizeof(a) / sizeof(word_type), b._data, b._significant_len,
        x->_data, sizeof(a) / sizeof(word_type) + b._significant_len);
    x->_significant_len = sizeof(a) / sizeof(word_type) + b._significant_len;
    x->minimize_significant_len();
}

/**
 * @param result 商
 * @param remainder 余数
 */
void BigInteger::divide(const BigInteger& a, const BigInteger& b, BigInteger *result, BigInteger *remainder)
{
    assert(nullptr != result || nullptr != remainder);
    assert(!b.is_zero());

    if (nullptr != result)
        result->ensure_cap(a._significant_len);
    if (nullptr != remainder)
        remainder->ensure_cap(b._significant_len);

    signed_divide(a._data, a._significant_len, b._data, b._significant_len,
           (nullptr == result ? nullptr : result->_data), a._significant_len,
           (nullptr == remainder ? nullptr : remainder->_data), b._significant_len);

    if (nullptr != result)
    {
        result->_significant_len = a._significant_len;
        result->minimize_significant_len();
    }
    if (nullptr != remainder)
    {
        remainder->_significant_len = b._significant_len;
        remainder->minimize_significant_len();
    }
}

void BigInteger::shift_left(const BigInteger& a, size_type count, BigInteger *x)
{
    const size_type min_sig = a._significant_len + (count - 1) / (8 * sizeof(word_type)) + 1;
    x->ensure_cap(min_sig);
    signed_shift_left(a._data, a._significant_len, x->_data, min_sig, count);
    x->_significant_len = min_sig;
    x->minimize_significant_len();
}

void BigInteger::shift_right(const BigInteger& a, size_type count, BigInteger *x)
{
    x->ensure_cap(a._significant_len);
    signed_shift_right(a._data, a._significant_len, x->_data, a._significant_len, count);
    x->_significant_len = a._significant_len;
    x->minimize_significant_len();
}

void BigInteger::set_zero()
{
    _data[0] = 0;
    _significant_len = 1;
}

bool BigInteger::is_zero() const
{
    return nut::is_zero(_data, _significant_len);
}

bool BigInteger::is_positive() const
{
    return nut::is_positive(_data, _significant_len);
}

void BigInteger::resize(size_type n)
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
void BigInteger::limit_positive_bits_to(size_type bit_len)
{
    assert(bit_len > 0);

#if (OPTIMIZE_LEVEL == 0)
    const size_type new_sig = bit_len / (8 * sizeof(word_type)) + 1;
    ensure_significant_len(new_sig);
    const size_type bits_shift = 8 * sizeof(word_type) - bit_len % (8 * sizeof(word_type));
    _data[new_sig - 1] <<= bits_shift;
    _data[new_sig - 1] >>= bits_shift;
    _significant_len = new_sig;
    minimize_significant_len();
#else
    // 正数且有效位数较小，无需做任何事情
    const size_type min_sig = bit_len / (8 * sizeof(word_type));
    if (_significant_len <= min_sig && BigInteger::is_positive())
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
            _data[new_sig - 1] <<= bits_shift;
            _data[new_sig - 1] >>= bits_shift;
        }
        _significant_len = new_sig;
    }
    else
    {
        // 需要附加符号位，以便保证结果是正数
        ensure_significant_len(min_sig + 1);
        _data[min_sig] = 0;
        _significant_len = min_sig + 1;
    }
    minimize_significant_len();
#endif
}

/**
 * 乘以a, 然后将比特长限制为小于 bit_len 的正数
 *
 * @return 注意，返回为正数
 */
void BigInteger::multiply_to_len(const BigInteger& a, size_type bit_len)
{
    const size_type words_len = (bit_len + 8 * sizeof(word_type) - 1) / (8 * sizeof(word_type));
    ensure_cap(words_len);
    signed_multiply(_data, _significant_len, a._data, a._significant_len, _data, words_len);
    _significant_len = words_len;
    limit_positive_bits_to(bit_len);
}

BigInteger::size_type BigInteger::significant_words_length() const
{
    return _significant_len;
}

const BigInteger::word_type* BigInteger::data() const
{
    return _data;
}

BigInteger::word_type* BigInteger::data()
{
    return const_cast<word_type*>(static_cast<const BigInteger&>(*this).data());
}

/**
 * 返回比特位
 *
 * @return 0 or 1
 */
int BigInteger::bit_at(size_type i) const
{
    if (i / (8 * sizeof(word_type)) >= _significant_len)
    {
#if (OPTIMIZE_LEVEL == 0)
        return BigInteger::is_positive() ? 0 : 1;
#else
        return _data[_significant_len - 1] >> (8 * sizeof(word_type) - 1);
#endif
    }
    return (_data[i / (8 * sizeof(word_type))] >> (i % (8 * sizeof(word_type)))) & 0x01;
}

BigInteger::word_type BigInteger::word_at(size_type i) const
{
    if (i >= _significant_len)
        return BigInteger::is_positive() ? 0 : ~(word_type)0;
    return _data[i];
}

/**
 * @param v 0 or 1
 */
void BigInteger::set_bit(size_type i, int v)
{
    assert(v == 0 || v == 1);
    ensure_significant_len((i + 1) / (8 * sizeof(word_type)) + 1); // 避免符号位被覆盖
    if (0 == v)
        _data[i / (8 * sizeof(word_type))] &= ~(((word_type) 1) << (i % (8 * sizeof(word_type))));
    else
        _data[i / (8 * sizeof(word_type))] |= ((word_type)1) << (i % (8 * sizeof(word_type)));
}

void BigInteger::set_word(size_type i, word_type v)
{
    ensure_significant_len(i + 1 + 1); // NOTE 多加一个 1 是为了避免符号位被覆盖
    _data[i] = v;
}

BigInteger::size_type BigInteger::bit_length() const
{
    if (BigInteger::is_positive())
        return nut::bit1_length((uint8_t*)_data, sizeof(word_type) * _significant_len);
    else
        return nut::bit0_length((uint8_t*)_data, sizeof(word_type) * _significant_len);
}

/**
 * 正数返回 bit 1 计数，负数则返回 bit 0 计数
 */
BigInteger::size_type BigInteger::bit_count() const
{
    const size_type bc = nut::bit1_count((uint8_t*)_data, sizeof(word_type) * _significant_len);
    if (BigInteger::is_positive())
        return bc;
    return 8 * sizeof(word_type) * _significant_len - bc;
}

int BigInteger::lowest_bit() const
{
    return nut::lowest_bit1((uint8_t*)_data, sizeof(word_type) * _significant_len);
}

long long BigInteger::llong_value() const
{
    static_assert(sizeof(long long) % sizeof(word_type) == 0, "Unexpected integer size");

    long long ret = 0;
    signed_expand(_data, _significant_len, (word_type*)&ret, sizeof(ret) / sizeof(word_type));
    return ret;
}

/**
 * 取 [a, b) 范围内的随机数
 */
BigInteger BigInteger::rand_between(const BigInteger& a, const BigInteger& b)
{
    assert(a != b);

    const bool a_is_bigger = (a > b);
    const BigInteger n = (a_is_bigger ? a - b : b - a);
    assert(n.is_positive());

    BigInteger ret(0);
    ret.ensure_cap(n._significant_len + 1);
    for (size_type i = 0; i < n._significant_len; ++i)
    {
        for (size_type j = 0; j < sizeof(word_type); ++j)
        {
            ret._data[i] <<= 8;
            ret._data[i] += ::rand() & 0xFF;
        }
    }
    ret._data[n._significant_len] = 0; // 保证是正数
    ret._significant_len = n._significant_len + 1;

    ret %= n;
    ret += (a_is_bigger ? b : a);
    return ret;
}

/**
 * 值交换
 */
void BigInteger::swap(BigInteger *a, BigInteger *b)
{
    assert(nullptr != a && nullptr != b);
    word_type *tmp = a->_data;
    const size_type tmp_sig = a->_significant_len;
    a->_data = nullptr;
    a->_capacity = 0;

    a->ensure_cap(b->_significant_len);
    ::memcpy(a->_data, b->_data, sizeof(word_type) * b->_significant_len);
    a->_significant_len = b->_significant_len;

    b->ensure_cap(tmp_sig);
    ::memcpy(b->_data, tmp, sizeof(word_type) * tmp_sig);
    b->_significant_len = tmp_sig;

    ::free(tmp);
}

#ifndef NDEBUG
// currently only used in DEBUG mode
static bool is_valid_radix(size_t radix)
{
    return 1 < radix && radix <= 36;
}
#endif

static char num2char(size_t n)
{
    assert(n < 36);
    return (char) (n < 10 ? '0' + n : 'A' + n - 10);
}

static wchar_t num2wchar(size_t n)
{
    assert(n < 36);
    return (wchar_t) (n < 10 ? L'0' + n : L'A' + n - 10);
}

std::string BigInteger::to_string(size_type radix) const
{
    assert(is_valid_radix(radix));

    BigInteger tmp(*this);
    const bool positive = tmp.is_positive();
    if (!positive)
        BigInteger::negate(tmp, &tmp);

    std::string s;
    const BigInteger RADIX(radix);
    do
    {
        const size_type n = (size_t) (tmp % RADIX).llong_value();
        s.push_back(num2char(n));

        tmp /= RADIX;
    } while (!tmp.is_zero());
    if (!positive)
        s.push_back('-');
    std::reverse(s.begin(), s.end());
    return s;
}

std::wstring BigInteger::to_wstring(size_type radix) const
{
    assert(is_valid_radix(radix));

    BigInteger tmp(*this);
    const bool positive = tmp.is_positive();
    if (!positive)
        BigInteger::negate(tmp, &tmp);

    std::wstring s;
    const BigInteger RADIX(radix);
    do
    {
        const size_type n = (size_t) (tmp % RADIX).llong_value();
        s.push_back(num2wchar(n));

        tmp /= RADIX;
    } while (!tmp.is_zero());
    if (!positive)
        s.push_back(L'-');
    std::reverse(s.begin(), s.end());
    return s;
}

static bool is_blank(char c)
{
    return ' ' == c || '\t' == c;
}

static bool is_blank(wchar_t c)
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

static size_t char2num(char c)
{
    assert(is_valid_char(c, 36));
    if ('0' <= c && c <= '9')
        return c - '0';
    return (c | 0x20) - 'a' + 10;
}

static size_t char2num(wchar_t c)
{
    assert(is_valid_char(c, 36));
    if (L'0' <= c && c <= L'9')
        return c - L'0';
    return (c | 0x20) - L'a' + 10;
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
        BigInteger::negate(ret, &ret);
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
        BigInteger::negate(ret, &ret);
    return ret;
}

}
