
#ifndef ___HEADFILE_C230DE33_6F0F_45E8_A829_39F88DDC1A13_
#define ___HEADFILE_C230DE33_6F0F_45E8_A829_39F88DDC1A13_

#include <stdint.h>
#include <string>

namespace nut
{

class MD5
{
    uint32_t _state[4]; // state ABCD
    uint8_t _buffer[64];
    uint64_t _byteslen = 0; // len of bytes

public:
    MD5();

    void reset();

    void update(uint8_t byte);

    void update(const void *buf, size_t cb);

    void digest();

    std::string get_string_result() const;

    void get_bytes_result(uint8_t *ret);

private:
    void transform512bits(const void *block);
};

}

#endif
