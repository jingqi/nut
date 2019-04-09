
#include <assert.h>
#include <stdint.h>
#include <memory.h>
#include <string>

#include "../numeric/word_array_integer.h"
#include "../platform/int_type.h" // for ssize_t in windows VC
#include "bit_stream.h"
#include "comparable.h"


namespace nut
{

void BitStream::_ensure_cap(size_t new_bit_size)
{
    // XXX 常量运算留给编译器来优化
    const size_t new_word_size = (new_bit_size + sizeof(word_type) * 8 - 1) / (sizeof(word_type) * 8);
    if (new_word_size <= _word_cap)
        return;

    size_t new_word_cap = _word_cap * 3 / 2;
    if (new_word_cap < new_word_size)
        new_word_cap = new_word_size;

    _buf = (word_type*) ::realloc(_buf, sizeof(word_type) * new_word_cap); // XXX realloc() handle nullptr
    assert(nullptr != _buf);
    _word_cap = new_word_cap;
}

size_t BitStream::_word_size() const
{
    return (_bit_size + sizeof(word_type) * 8 - 1) / (sizeof(word_type) * 8);
}

BitStream::BitStream(size_t bit_size, int fill_bit)
{
    assert(0 == fill_bit || 1 == fill_bit);
    if (0 == bit_size)
        return;

    _ensure_cap(bit_size);
    ::memset(_buf, (0 == fill_bit ? 0 : 0xff), (bit_size + 7) >> 3);
    _bit_size = bit_size;
}

BitStream::BitStream(const void *buf, size_t bit_size)
{
    assert(nullptr != buf || 0 == bit_size);
    if (0 == bit_size)
        return;

    _ensure_cap(bit_size);
    ::memcpy(_buf, buf, (bit_size + 7) >> 3);
    _bit_size = bit_size;
}

BitStream::BitStream(const std::string& s)
{
    _ensure_cap(s.length());
    for (size_t i = 0, len = s.length(); i < len; ++i)
    {
        const char c = s.at(i);
        if ('1' == c)
            append_bit(1);
        else if ('0' == c)
            append_bit(0);
        else if (' ' == c || '\t' == c || '\r' == c || '\n' == c) // skip blank
            continue;
        else
            break; // error
    }
}

BitStream::BitStream(const std::wstring& s)
{
    _ensure_cap(s.length());
    for (size_t i = 0, len = s.length(); i < len; ++i)
    {
        const wchar_t c = s.at(i);
        if (L'1' == c)
            append_bit(1);
        else if (L'0' == c)
            append_bit(0);
        else if (L' ' == c || L'\t' == c || L'\r' == c || L'\n' == c) // skip blank
            continue;
        else
            break; // error
    }
}

BitStream::BitStream(BitStream&& x)
    : _buf(x._buf), _word_cap(x._word_cap), _bit_size(x._bit_size)
{
    x._buf = nullptr;
    x._word_cap = 0;
    x._bit_size = 0;
}

BitStream::BitStream(const BitStream& x)
{
    if (0 == x._bit_size)
        return;

    _ensure_cap(x._bit_size);
    ::memcpy(_buf, x._buf, (x._bit_size + 7) >> 3);
    _bit_size = x._bit_size;
}

BitStream::~BitStream()
{
    if (nullptr != _buf)
        ::free(_buf);
    _buf = nullptr;
    _word_cap = 0;
    _bit_size = 0;
}

void BitStream::_normalize_tail()
{
    const size_t tail_bitlen = _bit_size % (sizeof(word_type) * 8);
    if (0 != tail_bitlen)
    {
        const size_t last_index = _word_size() - 1;
        _buf[last_index] &= ~((~(word_type)0) << tail_bitlen);
    }
}

BitStream& BitStream::operator=(BitStream&& x)
{
    if (this == &x)
        return *this;

    if (nullptr != _buf)
        ::free(_buf);

    _buf = x._buf;
    _word_cap = x._word_cap;
    _bit_size = x._bit_size;

    x._buf = nullptr;
    x._word_cap = 0;
    x._bit_size = 0;

    return *this;
}

BitStream& BitStream::operator=(const BitStream& x)
{
    if (this == &x)
        return *this;

    _ensure_cap(x._bit_size);
    ::memcpy(_buf, x._buf, (x._bit_size + 7) >> 3);
    _bit_size = x._bit_size;
    return *this;
}

bool BitStream::operator==(const BitStream& x) const
{
    if (this == &x)
        return true;
    if (_bit_size != x._bit_size)
        return false;

    const size_t word_count = _bit_size / (sizeof(word_type) * 8),
        bit_remained_count = _bit_size % (sizeof(word_type) * 8);
    if (0 != ::memcmp(_buf, x._buf, sizeof(word_type) * word_count))
        return false;
    if (bit_remained_count > 0)
    {
        const word_type l = _buf[word_count] << (sizeof(word_type) * 8 - bit_remained_count),
            r = x._buf[word_count] << (sizeof(word_type) * 8 - bit_remained_count);
        if (l != r)
            return false;
    }
    return true;
}

bool BitStream::operator!=(const BitStream& x) const
{
    return !(*this == x);
}

bool BitStream::operator<(const BitStream& x) const
{
    return compare(x) < 0;
}

bool BitStream::operator>(const BitStream& x) const
{
    return x < *this;
}

bool BitStream::operator<=(const BitStream& x) const
{
    return !(x < *this);
}

bool BitStream::operator>=(const BitStream& x) const
{
    return !(*this < x);
}

BitStream BitStream::operator+(const BitStream& x) const
{
    BitStream ret;
    ret._ensure_cap(_bit_size + x._bit_size);
    ret.append(*this);
    ret.append(x);
    return ret;
}

BitStream& BitStream::operator+=(const BitStream& x)
{
    append(x);
    return *this;
}

BitStream BitStream::operator&(const BitStream& x) const
{
    BitStream rs;
    const size_t new_bit_size = std::max(_bit_size, x._bit_size);
    rs._ensure_cap(new_bit_size);

    const size_t word_size1 = _word_size(), word_size2 = x._word_size(),
        new_word_size = std::max(word_size1, word_size2),
        tail_bitlen1 = _bit_size % (sizeof(word_type) * 8),
        tail_bitlen2 = x._bit_size % (sizeof(word_type) * 8);
    for (size_t i = 0; i < new_word_size; ++i)
    {
        word_type w1 = (i < word_size1 ? _buf[i] : 0);
        if (0 != tail_bitlen1 && i + 1 == word_size1)
            w1 &= ~((~(word_type)0) << tail_bitlen1);

        word_type w2 = (i < word_size2 ? x._buf[i] : 0);
        if (0 != tail_bitlen2 && i + 1 == word_size2)
            w2 &= ~((~(word_type)0) << tail_bitlen2);

        rs._buf[i] = w1 & w2;
    }
    rs._bit_size = new_bit_size;
    return rs;
}

BitStream& BitStream::operator&=(const BitStream& x)
{
    const size_t new_bit_size = std::max(_bit_size, x._bit_size);
    _ensure_cap(new_bit_size);

    const size_t word_size1 = _word_size(), word_size2 = x._word_size(),
        new_word_size = std::max(word_size1, word_size2),
        tail_bitlen1 = _bit_size % (sizeof(word_type) * 8),
        tail_bitlen2 = x._bit_size % (sizeof(word_type) * 8);
    for (size_t i = 0; i < new_word_size; ++i)
    {
        word_type w1 = (i < word_size1 ? _buf[i] : 0);
        if (0 != tail_bitlen1 && i + 1 == word_size1)
            w1 &= ~((~(word_type)0) << tail_bitlen1);

        word_type w2 = (i < word_size2 ? x._buf[i] : 0);
        if (0 != tail_bitlen2 && i + 1 == word_size2)
            w2 &= ~((~(word_type)0) << tail_bitlen2);

        _buf[i] = w1 & w2;
    }
    _bit_size = new_bit_size;
    return *this;
}

BitStream BitStream::operator|(const BitStream& x) const
{
    BitStream rs;
    const size_t new_bit_size = std::max(_bit_size, x._bit_size);
    rs._ensure_cap(new_bit_size);

    const size_t word_size1 = _word_size(), word_size2 = x._word_size(),
        new_word_size = std::max(word_size1, word_size2),
        tail_bitlen1 = _bit_size % (sizeof(word_type) * 8),
        tail_bitlen2 = x._bit_size % (sizeof(word_type) * 8);
    for (size_t i = 0; i < new_word_size; ++i)
    {
        word_type w1 = (i < word_size1 ? _buf[i] : 0);
        if (0 != tail_bitlen1 && i + 1 == word_size1)
            w1 &= ~((~(word_type)0) << tail_bitlen1);

        word_type w2 = (i < word_size2 ? x._buf[i] : 0);
        if (0 != tail_bitlen2 && i + 1 == word_size2)
            w2 &= ~((~(word_type)0) << tail_bitlen2);

        rs._buf[i] = w1 | w2;
    }
    rs._bit_size = new_bit_size;
    return rs;
}

BitStream& BitStream::operator|=(const BitStream& x)
{
    const size_t new_bit_size = std::max(_bit_size, x._bit_size);
    _ensure_cap(new_bit_size);

    const size_t word_size1 = _word_size(), word_size2 = x._word_size(),
        new_word_size = std::max(word_size1, word_size2),
        tail_bitlen1 = _bit_size % (sizeof(word_type) * 8),
        tail_bitlen2 = x._bit_size % (sizeof(word_type) * 8);
    for (size_t i = 0; i < new_word_size; ++i)
    {
        word_type w1 = (i < word_size1 ? _buf[i] : 0);
        if (0 != tail_bitlen1 && i + 1 == word_size1)
            w1 &= ~((~(word_type)0) << tail_bitlen1);

        word_type w2 = (i < word_size2 ? x._buf[i] : 0);
        if (0 != tail_bitlen2 && i + 1 == word_size2)
            w2 &= ~((~(word_type)0) << tail_bitlen2);

        _buf[i] = w1 | w2;
    }
    _bit_size = new_bit_size;
    return *this;
}

BitStream BitStream::operator^(const BitStream& x) const
{
    BitStream rs;
    const size_t new_bit_size = std::max(_bit_size, x._bit_size);
    rs._ensure_cap(new_bit_size);

    const size_t word_size1 = _word_size(), word_size2 = x._word_size(),
        new_word_size = std::max(word_size1, word_size2),
        tail_bitlen1 = _bit_size % (sizeof(word_type) * 8),
        tail_bitlen2 = x._bit_size % (sizeof(word_type) * 8);
    for (size_t i = 0; i < new_word_size; ++i)
    {
        word_type w1 = (i < word_size1 ? _buf[i] : 0);
        if (0 != tail_bitlen1 && i + 1 == word_size1)
            w1 &= ~((~(word_type)0) << tail_bitlen1);

        word_type w2 = (i < word_size2 ? x._buf[i] : 0);
        if (0 != tail_bitlen2 && i + 1 == word_size2)
            w2 &= ~((~(word_type)0) << tail_bitlen2);

        rs._buf[i] = w1 ^ w2;
    }
    rs._bit_size = new_bit_size;
    return rs;
}

BitStream& BitStream::operator^=(const BitStream& x)
{
    const size_t new_bit_size = std::max(_bit_size, x._bit_size);
    _ensure_cap(new_bit_size);

    const size_t word_size1 = _word_size(), word_size2 = x._word_size(),
        new_word_size = std::max(word_size1, word_size2),
        tail_bitlen1 = _bit_size % (sizeof(word_type) * 8),
        tail_bitlen2 = x._bit_size % (sizeof(word_type) * 8);
    for (size_t i = 0; i < new_word_size; ++i)
    {
        word_type w1 = (i < word_size1 ? _buf[i] : 0);
        if (0 != tail_bitlen1 && i + 1 == word_size1)
            w1 &= ~((~(word_type)0) << tail_bitlen1);

        word_type w2 = (i < word_size2 ? x._buf[i] : 0);
        if (0 != tail_bitlen2 && i + 1 == word_size2)
            w2 &= ~((~(word_type)0) << tail_bitlen2);

        _buf[i] = w1 ^ w2;
    }
    _bit_size = new_bit_size;
    return *this;
}

int BitStream::operator[](size_t i) const
{
    return bit_at(i);
}

int BitStream::compare(const BitStream& x) const
{
    if (this == &x)
        return 0;

    const size_t min_bitsz = std::min(_bit_size, x._bit_size);
    const size_t word_count = min_bitsz / (sizeof(word_type) * 8);
    for (size_t i = 0; i < word_count; ++i)
    {
        if (_buf[i] != x._buf[i])
            return nut::compare(reverse_bits(_buf[i]), reverse_bits(x._buf[i])); // NOTE 比较字典序而不是整数值
    }
    const size_t bit_remained_count = min_bitsz % (sizeof(word_type) * 8);
    if (bit_remained_count > 0)
    {
        const word_type l = _buf[word_count] << (sizeof(word_type) * 8 - bit_remained_count),
            r = x._buf[word_count] << (sizeof(word_type) * 8 - bit_remained_count);
        if (l != r)
            return nut::compare(reverse_bits(l), reverse_bits(r));
    }
    return _bit_size < x._bit_size ? -1 : (_bit_size > x._bit_size ? 1 : 0);
}

size_t BitStream::size() const
{
    return _bit_size;
}

void BitStream::resize(size_t new_bit_size, int fill_bit)
{
    assert(0 == fill_bit || 1 == fill_bit);

    if (new_bit_size < _bit_size)
    {
        _bit_size = new_bit_size;
        return;
    }

    _ensure_cap(new_bit_size);
    const size_t old_bit_size = _bit_size;
    _bit_size = new_bit_size;
    fill_bits(old_bit_size, new_bit_size - old_bit_size, fill_bit);
}

void BitStream::clear()
{
    _bit_size = 0;
}

int BitStream::bit_at(size_t i) const
{
    assert(i < _bit_size);
    return (_buf[i / (sizeof(word_type) * 8)] >> (i % (sizeof(word_type) * 8))) & 0x01;
}

void BitStream::set_bit(size_t i, int bit)
{
    assert(i < _bit_size && (0 == bit || 1 == bit));
    if (0 == bit)
        _buf[i / (sizeof(word_type) * 8)] &= ~(1 << (i % (sizeof(word_type) * 8)));
    else
        _buf[i / (sizeof(word_type) * 8)] |= 1 << (i % (sizeof(word_type) * 8));
}

void BitStream::fill_bits(size_t i, size_t nbit, int bit)
{
    assert(i + nbit <= _bit_size && (0 == bit || 1 == bit));
    const size_t end = i + nbit;
    const size_t wb = (i + sizeof(word_type) * 8 - 1) / (sizeof(word_type) * 8),
        we = end / (sizeof(word_type) * 8);
    if (wb < we)
    {
        const uint8_t fill = (0 == bit ? 0 : 0xff);
        ::memset(_buf + wb, fill, (we - wb) * sizeof(word_type));
        for (ssize_t k = wb * sizeof(word_type) * 8 - 1; k >= (ssize_t) i; --k)
            set_bit(k, bit);
        for (size_t k = we * sizeof(word_type) * 8; k < end; ++k)
            set_bit(k, bit);
    }
    else
    {
        for (size_t k = i; k < end; ++k)
            set_bit(k, bit);
    }
}

void BitStream::append_bit(int bit)
{
    assert(0 == bit || 1 == bit);
    _ensure_cap(_bit_size + 1);
    ++_bit_size;
    set_bit(_bit_size - 1, bit);
}

void BitStream::append(const BitStream& x)
{
    _ensure_cap(_bit_size + x._bit_size);
    const size_t old_bit_size = _bit_size; // XXX 这里需要保证及时是 this==&x 也是正确的
    _bit_size += x._bit_size;
    for (size_t i = old_bit_size; i < _bit_size; ++i)
        set_bit(i, x.bit_at(i - old_bit_size));
}

BitStream BitStream::substream(size_t i, size_t nbit)
{
    assert(i + nbit < _bit_size);
    BitStream ret;
    ret._ensure_cap(nbit);
    ret._bit_size = nbit;
    for (size_t k = 0; k < nbit; ++k)
        ret.set_bit(k, bit_at(i + k));
    return ret;
}

size_t BitStream::bit1_count()
{
    const size_t word_count = _bit_size / (sizeof(word_type) * 8);
    size_t ret = 0;
    for (size_t i = 0; i < word_count; ++i)
        ret += nut::bit1_count(_buf[i]);
    for (size_t i = word_count * (sizeof(word_type) * 8); i < _bit_size; ++i)
        ret += (0 == bit_at(i) ? 0 : 1);
    return ret;
}

size_t BitStream::bit0_count()
{
    return _bit_size - bit1_count();
}

std::string BitStream::to_string()
{
    std::string s;
    for (size_t i = 0; i < _bit_size; ++i)
        s.push_back(0 == bit_at(i) ? '0' : '1');
    return s;
}

std::wstring BitStream::to_wstring()
{
    std::wstring s;
    for (size_t i = 0; i < _bit_size; ++i)
        s.push_back(0 == bit_at(i) ? L'0' : L'1');
    return s;
}

}
