
#include <assert.h>
#include <stdlib.h> /* for malloc() */

#include "adler32.h"


/* 65521 是小于 2^16 的最大素数 */
#define MOD_ADLER 65521

namespace nut
{

void Adler32::reset() noexcept
{
    _result = 1;
}

void Adler32::update(uint8_t byte) noexcept
{
    update(&byte, 1);
}

void Adler32::update(const void *data, size_t len) noexcept
{
    assert(nullptr != data || len <= 0);
    uint16_t *a = reinterpret_cast<uint16_t*>(&_result);
    uint16_t *b = reinterpret_cast<uint16_t*>(&_result) + 1;
    for (size_t i = 0; i < len; ++i)
    {
        const uint8_t d = static_cast<const uint8_t*>(data)[i];
        *a = (((uint32_t) *a) + d) % MOD_ADLER;
        *b = (((uint32_t) *b) + *a) % MOD_ADLER;
    }
}

uint32_t Adler32::digest() const noexcept
{
    return _result;
}

// -----------------------------------------------------------------------------

RollingAdler32::RollingAdler32(size_t window) noexcept
    : _window_size(window)
{
    assert(window > 0);
    _buf = (uint8_t*) ::malloc(_window_size);
}

RollingAdler32::~RollingAdler32() noexcept
{
    ::free(_buf);
}

void RollingAdler32::initialize() noexcept
{
    _result = 1;
    _count = 0;
}

void RollingAdler32::update(uint8_t byte) noexcept
{
    update(&byte, 1);
}

void RollingAdler32::update(const void *data, size_t len) noexcept
{
    assert(nullptr != data || len <= 0);
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

        const uint8_t d = static_cast<const uint8_t*>(data)[i];
        a = (((uint32_t) a) + d) % MOD_ADLER;
        b = (((uint32_t) b) + a) % MOD_ADLER;

        _buf[_count % _window_size] = d;
        ++_count;
    }
}

uint32_t RollingAdler32::get_result() const noexcept
{
    return _result;
}

size_t RollingAdler32::get_data_size() const noexcept
{
    return _count;
}

size_t RollingAdler32::get_window_data(const void **data_ptr1, size_t *len_ptr1,
                                       const void **data_ptr2, size_t *len_ptr2) const noexcept
{
    if (nullptr != data_ptr1)
        *data_ptr1 = nullptr;
    if (nullptr != len_ptr1)
        *len_ptr1 = 0;
    if (nullptr != data_ptr2)
        *data_ptr2 = nullptr;
    if (nullptr != len_ptr2)
        *len_ptr2 = 0;

    if (0 == _count)
        return 0;

    if (_count <= _window_size)
    {
        if (nullptr != data_ptr1)
            *data_ptr1 = _buf;
        if (nullptr != len_ptr1)
            *len_ptr1 = _count;
        return 1;
    }

    const size_t rpos = _count % _window_size;
    if (nullptr != data_ptr1)
        *data_ptr1 = _buf + rpos;
    if (nullptr != len_ptr1)
        *len_ptr1 = _window_size - rpos;
    if (0 == rpos)
        return 1;

    if (nullptr != data_ptr2)
        *data_ptr2 = _buf;
    if (nullptr != len_ptr2)
        *len_ptr2 = rpos;
    return 2;
}

}
