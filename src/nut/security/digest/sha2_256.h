﻿
#ifndef ___HEADFILE_9F55FE91_EFA6_4098_A549_36E6DACA4126_
#define ___HEADFILE_9F55FE91_EFA6_4098_A549_36E6DACA4126_

#include <stdint.h>
#include <string>

#include "../../nut_config.h"


namespace nut
{

class NUT_API SHA2_256
{
public:
    // 散列结果为 256 bits (32 bytes)
    static const unsigned DIGEST_SIZE = 32;

public:
    SHA2_256();

    void reset();

    void update(uint8_t byte);
    void update(const void *data, size_t cb);

    void digest();

    /**
     * 返回二进制散列结果
     */
    const uint8_t* get_result() const;

    std::string get_hex_result() const;

private:
    void transform512bits(const void *block);

private:
    // Length of bits, max (2**64 - 1) bits
    uint64_t _bit_len = 0;

    // Block buffer, 512 bits
    uint8_t _buffer[64];

    // Digest state, 256 bits
    uint32_t _state[8];

    // Digest result, 256 bits
    uint8_t _result[DIGEST_SIZE];
};

}

#endif
