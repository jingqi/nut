﻿
#ifndef ___HEADFILE_7BDC0FB9_A6D2_4B2A_AA8E_8B12B6FF1E43_
#define ___HEADFILE_7BDC0FB9_A6D2_4B2A_AA8E_8B12B6FF1E43_

#include <assert.h>
#include <stdint.h>
#include <memory.h>
#include <string>

#include <nut/numeric/word_array_integer.h>

namespace nut
{

class BitStream
{
    typedef unsigned int word_type;
    word_type *m_buf; // 缓冲区
    size_t m_word_cap; // 缓冲区长度
    size_t m_bit_size; // bit 长度

private:
    void _ensure_cap(size_t new_bit_size);

public:
    BitStream();

    explicit BitStream(size_t nbits, bool setb = false);

    BitStream(const void *buf, size_t nbits);

    explicit BitStream(const std::string& s);

    explicit BitStream(const std::wstring& s);

    BitStream(const BitStream& x);

    ~BitStream();

    BitStream& operator=(const BitStream& x);

    bool operator==(const BitStream& x) const;

    bool operator!=(const BitStream& x) const
    {
        return !(*this == x);
    }

    BitStream operator+(const BitStream& x) const;

    BitStream& operator+=(const BitStream& x)
    {
        append(x);
        return *this;
    }

    bool operator[](size_t i) const
    {
        return bit_at(i);
    }

    size_t size() const
    {
        return m_bit_size;
    }

    void resize(size_t new_bit_size, bool fill_setb = false);

    void clear()
    {
        m_bit_size = 0;
    }

    bool bit_at(size_t i) const;

    void set_bit(size_t i, bool setb = true);

    void fill_bits(size_t i, size_t nbit, bool setb = true);

    /**
     * 添加一个bit
     *
     * @param b  true, 添加一个1; false, 添加一个0.
     */
    void append(bool b);

    void append(const BitStream& x);

    BitStream substeam(size_t i, size_t nbit);

public:
    size_t bit1_count();

    size_t bit0_count()
    {
        return m_bit_size - bit1_count();
    }

    void to_string(std::string *appended);

    void to_string(std::wstring *appended);
};

}

#endif
