
#ifndef ___HEADFILE_7BDC0FB9_A6D2_4B2A_AA8E_8B12B6FF1E43_
#define ___HEADFILE_7BDC0FB9_A6D2_4B2A_AA8E_8B12B6FF1E43_

#include <assert.h>
#include <stdint.h>
#include <memory.h>
#include <string>

#include "../nut_config.h"


namespace nut
{

class NUT_API BitStream
{
private:
    typedef unsigned int word_type;

    static_assert(std::is_unsigned<word_type>::value, "Unexpected integer type");

public:
    BitStream() = default;

    /**
     * @param fill_bit 填充比特值，只能为 1 或者 0
     */
    explicit BitStream(size_t bit_size, int fill_bit = 0) noexcept;

    BitStream(const void *buf, size_t bit_size) noexcept;

    explicit BitStream(const std::string& s) noexcept;
    explicit BitStream(const std::wstring& s) noexcept;

    BitStream(BitStream&& x) noexcept;
    BitStream(const BitStream& x) noexcept;

    ~BitStream() noexcept;

    BitStream& operator=(BitStream&& x) noexcept;
    BitStream& operator=(const BitStream& x) noexcept;

    bool operator==(const BitStream& x) const noexcept;
    bool operator!=(const BitStream& x) const noexcept;

    /**
     * 字典序比较，不是二进制数字比较
     */
    bool operator<(const BitStream& x) const noexcept;
    bool operator>(const BitStream& x) const noexcept;
    bool operator<=(const BitStream& x) const noexcept;
    bool operator>=(const BitStream& x) const noexcept;

    BitStream operator+(const BitStream& x) const noexcept;
    BitStream& operator+=(const BitStream& x) noexcept;

    BitStream operator&(const BitStream& x) const noexcept;
    BitStream& operator&=(const BitStream& x) noexcept;

    BitStream operator|(const BitStream& x) const noexcept;
    BitStream& operator|=(const BitStream& x) noexcept;

    BitStream operator^(const BitStream& x) const noexcept;
    BitStream& operator^=(const BitStream& x) noexcept;

    /**
     * @return 1 或者 0
     */
    int operator[](size_t i) const noexcept;

    int compare(const BitStream& x) const noexcept;

    size_t size() const noexcept;

    /**
     * @param fill_bit 填充比特值，只能是 1 或者 0
     */
    void resize(size_t new_bit_size, int fill_bit = 0) noexcept;

    void clear() noexcept;

    /**
     * @return 1 或者 0
     */
    int bit_at(size_t i) const noexcept;

    /**
     * @param bit 比特值，只能是 1 或者 0
     */
    void set_bit(size_t i, int bit = 1) noexcept;

    /**
     * @param bit 比特值，只能是 1 或者 0
     */
    void fill_bits(size_t i, size_t nbit, int bit = 1) noexcept;

    /**
     * 添加一个bit
     *
     * @param bit 比特值，只能是 1 或者 0
     */
    void append_bit(int bit) noexcept;

    void append(const BitStream& x) noexcept;

    BitStream substream(size_t i, size_t nbit) noexcept;

    size_t bit1_count() noexcept;
    size_t bit0_count() noexcept;

    std::string to_string() noexcept;
    std::wstring to_wstring() noexcept;

private:
    void _ensure_cap(size_t new_bit_size) noexcept;

    size_t _word_size() const noexcept;

    // 使最后一个有效的 word 空位为 0
    void _normalize_tail() noexcept;

private:
    word_type *_buf = nullptr; // 缓冲区
    size_t _word_cap = 0; // 缓冲区长度
    size_t _bit_size = 0; // bit 长度
};

}

#endif
