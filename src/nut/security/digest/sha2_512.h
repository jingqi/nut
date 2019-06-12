
#ifndef ___HEADFILE_8CB94427_107F_4213_ACB0_52FE90F9A142_
#define ___HEADFILE_8CB94427_107F_4213_ACB0_52FE90F9A142_

#include <string>

#include "../../nut_config.h"
#include "../../platform/int_type.h"


namespace nut
{

class NUT_API SHA2_512
{
public:
    // 散列结果为 512 bits (64 bytes)
    static constexpr size_t DIGEST_SIZE = 64;

public:
    SHA2_512() noexcept;

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
    void transform1024bits(const void *block) noexcept;

private:
    // Length of bits, max (2**128 - 1) bits
#if NUT_HAS_INT128
    uint128_t _bit_len = 0;
#else
    uint64_t _bit_len_low = 0, _bit_len_high = 0;
#endif

    // Block buffer, 1024 bits
    uint8_t _buffer[128];

    // Digest state, 512 bits
    uint64_t _state[8];

    // Digest result, 512 bits
    uint8_t _result[DIGEST_SIZE];
};

}

#endif
