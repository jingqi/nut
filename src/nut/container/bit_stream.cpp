
#include <assert.h>
#include <stdint.h>
#include <memory.h>
#include <string>

#include <nut/numeric/word_array_integer.h>

#include "bit_stream.h"

namespace nut
{

void BitStream::_ensure_cap(size_t new_bit_size)
{
    // XXX 常量运算留给编译器来优化
    const size_t new_word_size = (new_bit_size + sizeof(word_type) * 8 - 1) / (sizeof(word_type) * 8);
    if (new_word_size <= m_word_cap)
        return;

    size_t new_word_cap = m_word_cap * 3 / 2;
    if (new_word_cap < new_word_size)
        new_word_cap = new_word_size;
    m_buf = (word_type*) ::realloc(m_buf, sizeof(word_type) * new_word_cap); // XXX realloc() handle NULL
    assert(NULL != m_buf);
    m_word_cap = new_word_cap;
}

BitStream::BitStream()
    : m_buf(NULL), m_word_cap(0), m_bit_size(0)
{}

BitStream::BitStream(size_t nbits, bool setb)
    : m_buf(NULL), m_word_cap(0), m_bit_size(0)
{
    if (0 == nbits)
        return;
    _ensure_cap(nbits);
    ::memset(m_buf, (setb ? 0xff : 0), (nbits + 7) >> 3);
    m_bit_size = nbits;
}

BitStream::BitStream(const void *buf, size_t nbits)
    : m_buf(NULL), m_word_cap(0), m_bit_size(0)
{
    if (0 == nbits)
        return;
    _ensure_cap(nbits);
    ::memcpy(m_buf, buf, (nbits + 7) >> 3);
    m_bit_size = nbits;
}

BitStream::BitStream(const std::string& s)
    : m_buf(NULL), m_word_cap(0), m_bit_size(0)
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
    : m_buf(NULL), m_word_cap(0), m_bit_size(0)
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
    : m_buf(NULL), m_word_cap(0), m_bit_size(0)
{
    if (0 == x.m_bit_size)
        return;
    _ensure_cap(x.m_bit_size);
    ::memcpy(m_buf, x.m_buf, (x.m_bit_size + 7) >> 3);
    m_bit_size = x.m_bit_size;
}

BitStream::~BitStream()
{
    if (NULL != m_buf)
        ::free(m_buf);
    m_buf = NULL;
    m_word_cap = 0;
    m_bit_size = 0;
}

BitStream& BitStream::operator=(const BitStream& x)
{
    if (this == &x)
        return *this;

    _ensure_cap(x.m_bit_size);
    ::memcpy(m_buf, x.m_buf, (x.m_bit_size + 7) >> 3);
    m_bit_size = x.m_bit_size;
    return *this;
}

bool BitStream::operator==(const BitStream& x) const
{
    if (this == &x)
        return true;
    if (m_bit_size != x.m_bit_size)
        return false;

    const size_t word_count = m_bit_size / (sizeof(word_type) * 8),
        bit_remained_count = m_bit_size % (sizeof(word_type) * 8);
    if (0 != ::memcmp(m_buf, x.m_buf, sizeof(word_type) * word_count))
        return false;
    if (0 != bit_remained_count)
        if ((m_buf[word_count] << (sizeof(word_type) * 8 - bit_remained_count)) !=
            (x.m_buf[word_count] << (sizeof(word_type) * 8 - bit_remained_count)))
            return false;
    return true;
}

BitStream BitStream::operator+(const BitStream& x) const
{
    BitStream ret;
    ret._ensure_cap(m_bit_size + x.m_bit_size);
    ret.append(*this);
    ret.append(x);
    return ret;
}

void BitStream::resize(size_t new_bit_size, bool fill_setb)
{
    if (new_bit_size < m_bit_size)
    {
        m_bit_size = new_bit_size;
        return;
    }

    _ensure_cap(new_bit_size);
    const size_t old_bit_size = m_bit_size;
    m_bit_size = new_bit_size;
    fill_bits(old_bit_size, new_bit_size - old_bit_size, fill_setb);
}

bool BitStream::bit_at(size_t i) const
{
    assert(i < m_bit_size);
    return 0 != ((m_buf[i / (sizeof(word_type) * 8)] >> (i % (sizeof(word_type) * 8))) & 0x01);
}

void BitStream::set_bit(size_t i, bool setb)
{
    assert(i < m_bit_size);
    if (setb)
        m_buf[i / (sizeof(word_type) * 8)] |= 1 << (i % (sizeof(word_type) * 8));
    else
        m_buf[i / (sizeof(word_type) * 8)] &= ~(1 << (i % (sizeof(word_type) * 8)));
}

void BitStream::fill_bits(size_t i, size_t nbit, bool setb)
{
    assert(i + nbit < m_bit_size);
    const size_t end = i + nbit;
    const size_t fw = (i + sizeof(word_type) * 8 - 1) / (sizeof(word_type) * 8),
        le = end / (sizeof(word_type) * 8);
    const word_type fill = (setb ? ~(word_type)0 : 0);
    for (size_t wi = fw; wi < le; ++wi)
        m_buf[wi] = fill;
    for (size_t k = (fw + 1) * sizeof(word_type) * 8 - 1; k >= i; --k)
        set_bit(k, setb);
    for (size_t k = le * sizeof(word_type) * 8; k < end; ++k)
        set_bit(k, setb);
}

/**
 * 添加一个bit
 *
 * @param b  true, 添加一个1; false, 添加一个0.
 */
void BitStream::append(bool b)
{
    _ensure_cap(m_bit_size + 1);
    ++m_bit_size;
    set_bit(m_bit_size - 1, b);
}

void BitStream::append(const BitStream& x)
{
    _ensure_cap(m_bit_size + x.m_bit_size);
    const size_t old_bit_size = m_bit_size; // XXX 这里需要保证及时是 this==&x 也是正确的
    m_bit_size += x.m_bit_size;
    for (size_t i = old_bit_size; i < m_bit_size; ++i)
        set_bit(i, x.bit_at(i - old_bit_size));
}

BitStream BitStream::substeam(size_t i, size_t nbit)
{
    assert(i + nbit < m_bit_size);
    BitStream ret;
    ret._ensure_cap(nbit);
    ret.m_bit_size = nbit;
    for (size_t k = 0; k < nbit; ++k)
        ret.set_bit(k, bit_at(k + nbit));
    return ret;
}

size_t BitStream::bit1_count()
{
    const size_t word_count = m_bit_size / (sizeof(word_type) * 8);
    size_t ret = 0;
    for (size_t i = 0; i < word_count; ++i)
        ret += _bit_count(m_buf[i]);
    for (size_t i = word_count * (sizeof(word_type) * 8); i < m_bit_size; ++i)
        ret += (bit_at(i) ? 1 : 0);
    return ret;
}

void BitStream::to_string(std::string *appended)
{
    assert(NULL != appended);
    for (size_t i = 0; i < m_bit_size; ++i)
        appended->push_back(bit_at(i) ? '1' : '0');
}

void BitStream::to_string(std::wstring *appended)
{
    assert(NULL != appended);
    for (size_t i = 0; i < m_bit_size; ++i)
        appended->push_back(bit_at(i) ? L'1' : '0');
}

}
