
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
    static const unsigned DIGEST_SIZE = 20;

public:
    SHA1();

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
    void transform512bits(const void *block);

private:
    // Length of bits, max (2**64 - 1) bits
    uint64_t _bits_len = 0;

    // Block, 512 bits
    uint8_t _block[64];

    // Digest state, 160 bits
    uint32_t _state[5];

    // Digest result, 160 bits
    uint8_t _result[DIGEST_SIZE];
};

}

#endif
