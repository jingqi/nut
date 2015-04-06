
#include <assert.h>
#include <stdlib.h> /* for malloc() */

#include "adler32.h"

/* 65521 是小于 2^16 的最大素数 */
#define MOD_ADLER 65521

namespace nut
{

Adler32::Adler32()
    : m_result(1)
{}

void Adler32::reset()
{
    m_result = 1;
}

void Adler32::update(const void *buf, size_t len)
{
    assert(NULL != buf || len <= 0);
    uint16_t *a = reinterpret_cast<uint16_t*>(&m_result);
    uint16_t *b = reinterpret_cast<uint16_t*>(&m_result) + 1;
    for (size_t i = 0; i < len; ++i)
    {
        const uint8_t d = static_cast<const uint8_t*>(buf)[i];
        *a = (((uint32_t) *a) + d) % MOD_ADLER;
        *b = (((uint32_t) *b) + *a) % MOD_ADLER;
    }
}

RollingAdler32::RollingAdler32(size_t window)
    : m_result(1), m_window_size(window), m_buf(NULL), m_count(0)
{
    assert(window > 0);
    m_buf = (uint8_t*) ::malloc(m_window_size);
}

RollingAdler32::~RollingAdler32()
{
    ::free(m_buf);
}

void RollingAdler32::initialize()
{
    m_result = 1;
    m_count = 0;
}

void RollingAdler32::update(const void *buf, size_t len)
{
    assert(NULL != buf || len <= 0);
    uint16_t& a = reinterpret_cast<uint16_t*>(&m_result)[0];
    uint16_t& b = reinterpret_cast<uint16_t*>(&m_result)[1];
    for (size_t i = 0; i < len; ++i)
    {
        if (m_count >= m_window_size)
        {
            const uint32_t dd = MOD_ADLER - m_buf[m_count % m_window_size];
            a = (a + dd) % MOD_ADLER;
            b = (b + dd * m_window_size - 1) % MOD_ADLER;
        }

        const uint8_t d = static_cast<const uint8_t*>(buf)[i];
        a = (((uint32_t) a) + d) % MOD_ADLER;
        b = (((uint32_t) b) + a) % MOD_ADLER;

        m_buf[m_count % m_window_size] = d;
        ++m_count;
    }
}

}
