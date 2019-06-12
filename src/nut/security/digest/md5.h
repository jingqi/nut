
#ifndef ___HEADFILE_C230DE33_6F0F_45E8_A829_39F88DDC1A13_
#define ___HEADFILE_C230DE33_6F0F_45E8_A829_39F88DDC1A13_

#include <stdint.h>
#include <string>

#include "../../nut_config.h"


namespace nut
{

class NUT_API MD5
{
public:
    // 散列结果为 128 bits (16 bytes)
    static constexpr size_t DIGEST_SIZE = 16;

public:
    MD5() noexcept;

    void reset() noexcept;

    void update(uint8_t byte) noexcept;
    void update(const void *data, size_t cb) noexcept;

    void digest() noexcept;

    /**
     * 返回二进制散列结果
     */
    const uint8_t* get_result() const noexcept;

    std::string get_hex_result() const noexcept;

private:
    void transform512bits(const void *block) noexcept;

private:
     // Length of bits
    uint64_t _bit_len = 0;

    // Block buffer, 512 bits
    uint8_t _buffer[64];

    // Digest state ABCD, 128 bits
    uint32_t _state[4];

    // Digest result, 128 bits
    uint8_t _result[DIGEST_SIZE];
};

}

#endif
