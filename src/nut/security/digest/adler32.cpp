
#include <assert.h>
#include <stdlib.h> /* for malloc() */

#include "adler32.h"

/* 65521 是小于 2^16 的最大素数 */
#define MOD_ADLER 65521

namespace nut
{

void Adler32::reset()
{
    _result = 1;
}

void Adler32::update(const void *buf, size_t len)
{
    assert(nullptr != buf || len <= 0);
    uint16_t *a = reinterpret_cast<uint16_t*>(&_result);
    uint16_t *b = reinterpret_cast<uint16_t*>(&_result) + 1;
    for (size_t i = 0; i < len; ++i)
    {
        const uint8_t d = static_cast<const uint8_t*>(buf)[i];
        *a = (((uint32_t) *a) + d) % MOD_ADLER;
        *b = (((uint32_t) *b) + *a) % MOD_ADLER;
    }
}

uint32_t Adler32::digest() const
{
    return _result;
}

// -----------------------------------------------------------------------------

RollingAdler32::RollingAdler32(size_t window)
    : _window_size(window)
{
    assert(window > 0);
    _buf = (uint8_t*) ::malloc(_window_size);
}

RollingAdler32::~RollingAdler32()
{
    ::free(_buf);
}

void RollingAdler32::initialize()
{
    _result = 1;
    _count = 0;
}

void RollingAdler32::update(const void *buf, size_t len)
{
    assert(nullptr != buf || len <= 0);
    uint16_t& a = reinterpret_cast<uint16_t*>(&_result)[0];
    uint16_t& b = reinterpret_cast<uint16_t*>(&_result)[1];
    for (size_t i = 0; i < len; ++i)
    {
        if (_count >= _window_size)
        {
            const uint32_t dd = MOD_ADLER - _buf[_count % _window_size];
            a = (a + dd) % MOD_ADLER;
            b = (b + dd * _window_size - 1) % MOD_ADLER;
        }

        const uint8_t d = static_cast<const uint8_t*>(buf)[i];
        a = (((uint32_t) a) + d) % MOD_ADLER;
        b = (((uint32_t) b) + a) % MOD_ADLER;

        _buf[_count % _window_size] = d;
        ++_count;
    }
}

uint32_t RollingAdler32::get_result() const
{
    return _result;
}

}
