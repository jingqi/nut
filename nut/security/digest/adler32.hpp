/**
 * @file -
 * @author jingqi
 * @date 2012-11-19
 * @last-edit 2012-11-19 10:53:01 jingqi
 */

#ifndef ___HEADFILE_A87EC76B_E2C1_4BCD_AF64_A177A623A874_
#define ___HEADFILE_A87EC76B_E2C1_4BCD_AF64_A177A623A874_

#include <assert.h>
#include <stdint.h>

namespace nut
{

class Adler32
{
    /** 65521是小于2^16的最大素数 */
    enum { MOD_ADLER = 65521 };

    uint32_t m_result;

public:
    Adler32()
        : m_result(1)
    {}

    void initialize()
    {
        m_result = 1;
    }

    void update(const void *buf, size_t len)
    {
        assert(NULL != buf || len <= 0);
        uint16_t *a = reinterpret_cast<uint16_t*>(&m_result);
        uint16_t *b = reinterpret_cast<uint16_t*>(&m_result) + 1;
        for (register size_t i = 0; i < len; ++i)
        {
            const uint8_t d = static_cast<const uint8_t*>(buf)[i];
            *a = (((uint32_t) *a) + d) % MOD_ADLER;
            *b = (((uint32_t) *b) + *a) % MOD_ADLER;
        }
    }

    uint32_t getResult() const
    {
        return m_result;
    }
};

/**
 * 滚动hash
 */
class RollingAdler32
{
    /** 65521是小于2^16的最大素数 */
    enum { MOD_ADLER = 65521 };

    uint32_t m_result;
    const size_t m_window;
    uint8_t *m_buf;
    size_t m_count;

public:
    RollingAdler32(size_t window)
        : m_result(1), m_window(window), m_buf(NULL), m_count(0)
    {
        assert(window > 0);
        m_buf = new uint8_t[m_window];
    }

    ~RollingAdler32()
    {
        delete[] m_buf;
    }

    void initialize()
    {
        m_result = 1;
        m_count = 0;
    }

    void update(const void *buf, size_t len)
    {
        assert(NULL != buf || len <= 0);
        uint16_t& a = reinterpret_cast<uint16_t*>(&m_result)[0];
        uint16_t& b = reinterpret_cast<uint16_t*>(&m_result)[1];
        for (register size_t i = 0; i < len; ++i)
        {
            if (m_count >= m_window)
            {
                const uint32_t dd = MOD_ADLER - m_buf[m_count % m_window];
                a = (a + dd) % MOD_ADLER;
                b = (b + dd * m_window - 1) % MOD_ADLER;
            }

            const uint8_t d = static_cast<const uint8_t*>(buf)[i];
            a = (((uint32_t) a) + d) % MOD_ADLER;
            b = (((uint32_t) b) + a) % MOD_ADLER;

            m_buf[m_count % m_window] = d;
            ++m_count;
        }
    }

    uint32_t getResult() const
    {
        return m_result;
    }
};

}

#endif

