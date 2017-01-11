
#ifndef ___HEADFILE_C230DE33_6F0F_45E8_A829_39F88DDC1A13_
#define ___HEADFILE_C230DE33_6F0F_45E8_A829_39F88DDC1A13_

#include <stdint.h>
#include <string>

#include "../../nut_config.h"


namespace nut
{

class NUT_API MD5
{
    uint32_t _state[4]; // State ABCD
    uint8_t _buffer[64];
    uint64_t _bytes_len = 0; // Length of bytes

public:
    MD5();

    void reset();

    void update(uint8_t byte);
    void update(const void *buf, size_t cb);

    void digest();

    /**
     * 返回16字节二进制散列结果
     */
    const uint8_t* get_bytes_result(void *result = nullptr) const;

    std::string get_string_result() const;

private:
    void transform512bits(const void *block);
};

}

#endif
