
#ifndef ___HEADFILE_EBEFB8BB_FC0F_4EDB_A849_F0B53C882C75_
#define ___HEADFILE_EBEFB8BB_FC0F_4EDB_A849_F0B53C882C75_

#include <stdint.h>
#include <string>

#include "../../nut_config.h"


namespace nut
{

class NUT_API SHA1
{
    uint32_t _state[5];
    uint8_t _buffer[64];
    uint32_t _bytes_len = 0;

public:
    SHA1();

    void reset();

    void update(uint8_t byte);
    void update(const void *buf, size_t cb);

    void digest();

    /**
     * 返回20字节二进制散列结果
     */
    const uint8_t* get_bytes_result(void *result = nullptr) const;

    std::string get_string_result() const;

private:
    void transform512bits(const void *block);
};

}

#endif
