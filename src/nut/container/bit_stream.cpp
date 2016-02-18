
#include <assert.h>
#include <stdint.h>
#include <memory.h>
#include <string>

#include <nut/numeric/word_array_integer.h>
#include <nut/platform/stdint_traits.h> // for ssize_t in windows VC

#include "bit_stream.h"

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

    _buf = (word_type*) ::realloc(_buf, sizeof(word_type) * new_word_cap); // XXX realloc() handle NULL
    assert(NULL != _buf);
    _word_cap = new_word_cap;
}

BitStream::BitStream()
{}

BitStream::BitStream(size_t nbits, bool setb)
{
    if (0 == nbits)
        return;

    _ensure_cap(nbits);
    ::memset(_buf, (setb ? 0xff : 0), (nbits + 7) >> 3);
    _bit_size = nbits;
}

BitStream::BitStream(const void *buf, size_t nbits)
{
    if (0 == nbits)
        return;
    _ensure_cap(nbits);
    ::memcpy(_buf, buf, (nbits + 7) >> 3);
    _bit_size = nbits;
}

BitStream::BitStream(const std::string& s)
{
    _ensure_cap(s.length());
    for (size_t i = 0, len = s.length(); i < len; ++i)
    {
        const char c = s.at(i);
        if ('1' == c)
            append(true);
        else if ('0' == c)
            append(false);
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
            append(true);
        else if (L'0' == c)
            append(false);
        else if (L' ' == c || L'\t' == c || L'\r' == c || L'\n' == c) // skip blank
            continue;
        else
            break; // error
    }
}

BitStream::BitStream(const BitStream& x)
{
    if (0 == x._bit_size)
        return;

    _ensure_cap(x._bit_size);
    ::memcpy(_buf, x._buf, (x._bit_size + 7) >> 3);
    _bit_size = x._bit_size;
}

BitStream::BitStream(BitStream&& x)
{
    _buf = x._buf;
    _word_cap = x._word_cap;
    _bit_size = x._bit_size;

    x._buf = NULL;
    x._word_cap = 0;
    x._bit_size = 0;
}

BitStream::~BitStream()
{
    if (NULL != _buf)
        ::free(_buf);
    _buf = NULL;
    _word_cap = 0;
    _bit_size = 0;
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

BitStream& BitStream::operator=(BitStream&& x)
{
    if (this == &x)
        return *this;

    if (NULL != _buf)
        ::free(_buf);

    _buf = x._buf;
    _word_cap = x._word_cap;
    _bit_size = x._bit_size;

    x._buf = NULL;
    x._word_cap = 0;
    x._bit_size = 0;

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
    if (0 != bit_remained_count)
        if ((_buf[word_count] << (sizeof(word_type) * 8 - bit_remained_count)) !=
            (x._buf[word_count] << (sizeof(word_type) * 8 - bit_remained_count)))
            return false;
    return true;
}

bool BitStream::operator!=(const BitStream& x) const
{
    return !(*this == x);
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

bool BitStream::operator[](size_t i) const
{
    return bit_at(i);
}

size_t BitStream::size() const
{
    return _bit_size;
}

void BitStream::resize(size_t new_bit_size, bool fill_setb)
{
    if (new_bit_size < _bit_size)
    {
        _bit_size = new_bit_size;
        return;
    }

    _ensure_cap(new_bit_size);
    const size_t old_bit_size = _bit_size;
    _bit_size = new_bit_size;
    fill_bits(old_bit_size, new_bit_size - old_bit_size, fill_setb);
}

void BitStream::clear()
{
    _bit_size = 0;
}

bool BitStream::bit_at(size_t i) const
{
    assert(i < _bit_size);
    return 0 != ((_buf[i / (sizeof(word_type) * 8)] >> (i % (sizeof(word_type) * 8))) & 0x01);
}

void BitStream::set_bit(size_t i, bool setb)
{
    assert(i < _bit_size);
    if (setb)
        _buf[i / (sizeof(word_type) * 8)] |= 1 << (i % (sizeof(word_type) * 8));
    else
        _buf[i / (sizeof(word_type) * 8)] &= ~(1 << (i % (sizeof(word_type) * 8)));
}

void BitStream::fill_bits(size_t i, size_t nbit, bool setb)
{
    assert(i + nbit <= _bit_size);
    const size_t end = i + nbit;
    const size_t wb = (i + sizeof(word_type) * 8 - 1) / (sizeof(word_type) * 8),
        we = end / (sizeof(word_type) * 8);
    if (wb < we)
    {
        const word_type fill = (setb ? ~(word_type)0 : 0);
        for (size_t k = wb; k < we; ++k)
            _buf[k] = fill;
        for (ssize_t k = wb * sizeof(word_type) * 8 - 1; k >= (ssize_t) i; --k)
            set_bit(k, setb);
        for (size_t k = we * sizeof(word_type) * 8; k < end; ++k)
            set_bit(k, setb);
    }
    else
    {
        for (size_t k = i; k < end; ++k)
            set_bit(k, setb);
    }
}

/**
 * 添加一个bit
 *
 * @param b  true, 添加一个1; false, 添加一个0.
 */
void BitStream::append(bool b)
{
    _ensure_cap(_bit_size + 1);
    ++_bit_size;
    set_bit(_bit_size - 1, b);
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
        ret += _bit_count(_buf[i]);
    for (size_t i = word_count * (sizeof(word_type) * 8); i < _bit_size; ++i)
        ret += (bit_at(i) ? 1 : 0);
    return ret;
}

size_t BitStream::bit0_count()
{
    return _bit_size - bit1_count();
}

void BitStream::to_string(std::string *appended)
{
    assert(NULL != appended);
    for (size_t i = 0; i < _bit_size; ++i)
        appended->push_back(bit_at(i) ? '1' : '0');
}

void BitStream::to_string(std::wstring *appended)
{
    assert(NULL != appended);
    for (size_t i = 0; i < _bit_size; ++i)
        appended->push_back(bit_at(i) ? L'1' : '0');
}

}
