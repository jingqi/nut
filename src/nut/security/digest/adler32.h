
#ifndef ___HEADFILE_A87EC76B_E2C1_4BCD_AF64_A177A623A874_
#define ___HEADFILE_A87EC76B_E2C1_4BCD_AF64_A177A623A874_

#include <stdint.h>
#include <stddef.h> // for size_t

#include "../../nut_config.h"


namespace nut
{

class NUT_API Adler32
{
public:
    Adler32() = default;

    void reset() noexcept;

    void update(const void *data, size_t len) noexcept;

    uint32_t digest() const noexcept;

private:
    uint32_t _result = 1;
};

/**
 * 滚动hash
 */
class NUT_API RollingAdler32
{
public:
    explicit RollingAdler32(size_t window) noexcept;
    ~RollingAdler32() noexcept;

    void initialize() noexcept;

    void update(const void *data, size_t len) noexcept;

    uint32_t get_result() const noexcept;

private:
    uint32_t _result = 1;
    const size_t _window_size = 16;
    uint8_t *_buf = nullptr;
    size_t _count = 0;
};

}

#endif
