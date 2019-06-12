
#ifndef ___HEADFILE_EBEFB8BB_FC0F_4EDB_A849_F0B53C882C75_
#define ___HEADFILE_EBEFB8BB_FC0F_4EDB_A849_F0B53C882C75_

#include <stdint.h>
#include <string>

#include "../../nut_config.h"


namespace nut
{

class NUT_API SHA1
{
public:
    // 散列结果为 160 bits (20 bytes)
    static constexpr size_t DIGEST_SIZE = 20;

public:
    SHA1() noexcept;

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
    // Length of bits, max (2**64 - 1) bits
    uint64_t _bit_len = 0;

    // Block buffer, 512 bits
    uint8_t _buffer[64];

    // Digest state, 160 bits
    uint32_t _state[5];

    // Digest result, 160 bits
    uint8_t _result[DIGEST_SIZE];
};

}

#endif
