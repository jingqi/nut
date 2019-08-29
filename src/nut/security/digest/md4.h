
#ifndef ___HEADFILE_9A1D9EC9_A714_4618_A356_C048CB24C3E1_
#define ___HEADFILE_9A1D9EC9_A714_4618_A356_C048CB24C3E1_

#include <stdint.h>
#include <string>

#include "../../nut_config.h"
#include "../../platform/endian.h"


namespace nut
{

class NUT_API MD4
{
public:
    // 散列结果为 128 bits (16 bytes)
    static constexpr size_t DIGEST_SIZE = 16;

public:
    MD4() noexcept;

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
    void transform512bits(const void *block);

private:
    // Length of bits
    uint64_t _bit_len = 0;

    // Block buffer, 512 bits
    uint8_t _buffer[64];

    // Digest state ABCD, 128 bits
    uint32_t _state[4];

#if !NUT_ENDIAN_LITTLE_BYTE
    // Digest result, 128 bits
    uint8_t _result[DIGEST_SIZE];
#endif
};

}

#endif
