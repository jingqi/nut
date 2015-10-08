
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
    word_type *_buf = NULL; // 缓冲区
    size_t _word_cap = 0; // 缓冲区长度
    size_t _bit_size = 0; // bit 长度

private:
    void _ensure_cap(size_t new_bit_size);

public:
    BitStream();

    explicit BitStream(size_t nbits, bool setb = false);

    BitStream(const void *buf, size_t nbits);

    explicit BitStream(const std::string& s);

    explicit BitStream(const std::wstring& s);

    BitStream(const BitStream& x);
    BitStream(BitStream&& x);

    ~BitStream();

    BitStream& operator=(const BitStream& x);
    BitStream& operator=(BitStream&& x);

    bool operator==(const BitStream& x) const;

    bool operator!=(const BitStream& x) const;

    BitStream operator+(const BitStream& x) const;

    BitStream& operator+=(const BitStream& x);

    bool operator[](size_t i) const;

    size_t size() const;

    void resize(size_t new_bit_size, bool fill_setb = false);

    void clear();

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

    size_t bit0_count();

    void to_string(std::string *appended);

    void to_string(std::wstring *appended);
};

}

#endif
