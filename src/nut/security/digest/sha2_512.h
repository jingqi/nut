
#ifndef ___HEADFILE_8CB94427_107F_4213_ACB0_52FE90F9A142_
#define ___HEADFILE_8CB94427_107F_4213_ACB0_52FE90F9A142_

#include <string>

#include <nut/platform/int_type.h>

#include "../../nut_config.h"


namespace nut
{

class NUT_API SHA2_512
{
public:
    // 散列结果为 512 bits (64 bytes)
    static const unsigned DIGEST_SIZE = 64;

public:
    SHA2_512();

    void reset();

    void update(uint8_t byte);
    void update(const void *buf, size_t cb);

    void digest();

    /**
     * 返回二进制散列结果
     */
    const uint8_t* get_result() const;

    std::string get_hex_result() const;

private:
    void transform1024bits(const void *block);

private:
    // Length of bits, max (2**128 - 1) bits
#if NUT_HAS_INT128
    uint128_t _bits_len = 0;
#else
    uint64_t _bits_len_low = 0, _bits_len_high = 0;
#endif

    // Block, 1024 bits
    uint8_t _block[128];

    // Digest state, 512 bits
    uint64_t _state[8];

    // Digest result, 512 bits
    uint8_t _result[DIGEST_SIZE];
};

}

#endif
