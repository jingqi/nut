
#ifndef ___HEADFILE_A87EC76B_E2C1_4BCD_AF64_A177A623A874_
#define ___HEADFILE_A87EC76B_E2C1_4BCD_AF64_A177A623A874_

#include <stdint.h>
#include <string.h> /* for size_t */

namespace nut
{

class Adler32
{
    uint32_t m_result;

public:
    Adler32();

    void reset();

    void update(const void *buf, size_t len);

    uint32_t digest() const
    {
        return m_result;
    }
};

/**
 * 滚动hash
 */
class RollingAdler32
{
    uint32_t m_result;
    const size_t m_window_size;
    uint8_t *m_buf;
    size_t m_count;

public:
    RollingAdler32(size_t window);
    ~RollingAdler32();

    void initialize();

    void update(const void *buf, size_t len);

    uint32_t get_result() const
    {
        return m_result;
    }
};

}

#endif
