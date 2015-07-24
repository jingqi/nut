
#ifndef ___HEADFILE_A87EC76B_E2C1_4BCD_AF64_A177A623A874_
#define ___HEADFILE_A87EC76B_E2C1_4BCD_AF64_A177A623A874_

#include <stdint.h>
#include <string.h> /* for size_t */

namespace nut
{

class Adler32
{
    uint32_t _result = 1;

public:
    Adler32()
    {}

    void reset()
    {
        _result = 1;
    }

    void update(const void *buf, size_t len);

    uint32_t digest() const
    {
        return _result;
    }
};

/**
 * 滚动hash
 */
class RollingAdler32
{
    uint32_t _result = 1;
    const size_t _window_size = 16;
    uint8_t *_buf = NULL;
    size_t _count = 0;

public:
    RollingAdler32(size_t window);
    ~RollingAdler32();

    void initialize();

    void update(const void *buf, size_t len);

    uint32_t get_result() const
    {
        return _result;
    }
};

}

#endif
