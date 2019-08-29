
#ifndef ___HEADFILE_695CC7B7_8429_4634_A119_BFB7D2B6AD91_
#define ___HEADFILE_695CC7B7_8429_4634_A119_BFB7D2B6AD91_

#include <stdint.h>
#include <string>

#include "../../nut_config.h"


namespace nut
{

class NUT_API MD2
{
public:
    // 散列结果为 128 bits (16 bytes)
    static constexpr size_t DIGEST_SIZE = 16;

public:
    MD2() noexcept;

    void reset() noexcept;
    void update(uint8_t byte) noexcept;
    void update(const void *data, size_t cb) noexcept;

    void digest() noexcept;

    const uint8_t* get_result() const noexcept;
    std::string get_hex_result() const noexcept;

private:
    void transform128bits(const void *block);

private:
   uint8_t _buffer[16];
   size_t _buffer_size = 0;

   uint8_t _state[48];
   uint8_t _checksum[16];
};

}

#endif
