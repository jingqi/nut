
#ifndef ___HEADFILE_7BDC0FB9_A6D2_4B2A_AA8E_8B12B6FF1E43_
#define ___HEADFILE_7BDC0FB9_A6D2_4B2A_AA8E_8B12B6FF1E43_

#include <assert.h>
#include <stdint.h>
#include <memory.h>
#include <string>

#include <nut/numeric/word_array_integer.h>

#include "../nut_config.h"


namespace nut
{

class NUT_API BitStream
{
private:
    typedef unsigned int word_type;

public:
    BitStream() = default;

    /**
     * @param fill_bit 填充比特值，只能为 1 或者 0
     */
    explicit BitStream(size_t bit_size, int fill_bit = 0);

    BitStream(const void *buf, size_t bit_size);

    explicit BitStream(const std::string& s);

    explicit BitStream(const std::wstring& s);

    BitStream(BitStream&& x);
    BitStream(const BitStream& x);

    ~BitStream();

    BitStream& operator=(BitStream&& x);
    BitStream& operator=(const BitStream& x);

    bool operator==(const BitStream& x) const;
    bool operator!=(const BitStream& x) const;

    BitStream operator+(const BitStream& x) const;
    BitStream& operator+=(const BitStream& x);

    BitStream operator&(const BitStream& x) const;
    BitStream& operator&=(const BitStream& x);

    BitStream operator|(const BitStream& x) const;
    BitStream& operator|=(const BitStream& x);

    BitStream operator^(const BitStream& x) const;
    BitStream& operator^=(const BitStream& x);

    /**
     * @return 1 或者 0
     */
    int operator[](size_t i) const;

    size_t size() const;

    /**
     * @param fill_bit 填充比特值，只能是 1 或者 0
     */
    void resize(size_t new_bit_size, int fill_bit = 0);

    void clear();

    /**
     * @return 1 或者 0
     */
    int bit_at(size_t i) const;

    /**
     * @param bit 比特值，只能是 1 或者 0
     */
    void set_bit(size_t i, int bit = 1);

    /**
     * @param bit 比特值，只能是 1 或者 0
     */
    void fill_bits(size_t i, size_t nbit, int bit = 1);

    /**
     * 添加一个bit
     *
     * @param bit 比特值，只能是 1 或者 0
     */
    void append_bit(int bit);

    void append(const BitStream& x);

    BitStream substream(size_t i, size_t nbit);

    size_t bit1_count();
    size_t bit0_count();

    std::string to_string();
    std::wstring to_wstring();

private:
    void _ensure_cap(size_t new_bit_size);

    size_t _word_size() const;

    // 使最后一个有效的 word 空位为 0
    void _normalize_tail();

private:
    word_type *_buf = nullptr; // 缓冲区
    size_t _word_cap = 0; // 缓冲区长度
    size_t _bit_size = 0; // bit 长度
};

}

#endif
