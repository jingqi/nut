
#include <assert.h>
#include <stdlib.h>
#include <string.h> // for ::memcpy()
#include <algorithm> // for std::min()

#include "ring_buffer.h"


#define VALIDATE_MEMBERS() \
    assert((nullptr == _buffer && 0 == _capacity && 0 == _read_index && 0 == _write_index) || \
           (nullptr != _buffer && _capacity > 0 && _read_index < _capacity && _write_index < _capacity))

namespace nut
{

RingBuffer::RingBuffer(RingBuffer&& x) noexcept
{
    _buffer = x._buffer;
    _capacity = x._capacity;
    _read_index = x._read_index;
    _write_index = x._write_index;

    x._buffer = nullptr;
    x._capacity = 0;
    x._read_index = 0;
    x._write_index = 0;
}

RingBuffer::RingBuffer(const RingBuffer& x) noexcept
{
    *this = x;
}

RingBuffer::~RingBuffer() noexcept
{
    if (nullptr != _buffer)
        ::free(_buffer);
    _buffer = nullptr;
    _capacity = 0;
    _read_index = 0;
    _write_index = 0;
}

RingBuffer& RingBuffer::operator=(RingBuffer&& x) noexcept
{
    if (this == &x)
        return *this;

    if (nullptr != _buffer)
        ::free(_buffer);

    _buffer = x._buffer;
    _capacity = x._capacity;
    _read_index = x._read_index;
    _write_index = x._write_index;

    x._buffer = nullptr;
    x._capacity = 0;
    x._read_index = 0;
    x._write_index = 0;

    return *this;
}

RingBuffer& RingBuffer::operator=(const RingBuffer& x) noexcept
{
    if (this == &x)
        return *this;

    clear();
    ensure_writable_size(x.readable_size());

    const void *buffers[2];
    size_t lens[2];
    const size_t n = x.readable_pointers(buffers, lens, buffers + 1, lens + 1);
    for (size_t i = 0; i < n; ++i)
        write(buffers[i], lens[i]);

    return *this;
}

void RingBuffer::clear() noexcept
{
    _read_index = 0;
    _write_index = 0;
}

size_t RingBuffer::readable_size() const noexcept
{
    VALIDATE_MEMBERS();

    if (_write_index >= _read_index)
        return _write_index - _read_index;
    return _capacity - _read_index + _write_index;
}

size_t RingBuffer::read(void *buf, size_t len) noexcept
{
    assert(nullptr != buf);
    const size_t readed = look_ahead(buf, len);
    _read_index += readed;
    if (0 != _capacity)
        _read_index %= _capacity;

    // Reset to zero, this does no harm
    if (_read_index == _write_index)
    {
        _read_index = 0;
        _write_index = 0;
    }
    return readed;
}

size_t RingBuffer::look_ahead(void *buf, size_t len) const noexcept
{
    assert(nullptr != buf);
    const size_t readed = std::min<size_t>(len, readable_size()),
        trunk_sz = _capacity - _read_index;
    if (trunk_sz >= readed)
    {
        if (0 != readed)
            ::memcpy(buf, (const uint8_t*) _buffer + _read_index, readed);
    }
    else
    {
        ::memcpy(buf, (const uint8_t*) _buffer + _read_index, trunk_sz);
        ::memcpy((uint8_t*) buf + trunk_sz, _buffer, readed - trunk_sz);
    }
    return readed;
}

size_t RingBuffer::skip_read(size_t len) noexcept
{
    const size_t skiped = std::min<size_t>(len, readable_size());
    _read_index += skiped;
    if (0 != _capacity)
        _read_index %= _capacity;

    // Reset to zero, this does no harm
    if (_read_index == _write_index)
    {
        _read_index = 0;
        _write_index = 0;
    }
    return skiped;
}

size_t RingBuffer::readable_pointers(const void **buf_ptr1, size_t *len_ptr1,
                                     const void **buf_ptr2, size_t *len_ptr2) const noexcept
{
    VALIDATE_MEMBERS();

    if (_write_index == _read_index)
        return 0;

    if (_write_index > _read_index)
    {
        if (nullptr != buf_ptr1)
            *buf_ptr1 = (const uint8_t*) _buffer + _read_index;
        if (nullptr != len_ptr1)
            *len_ptr1 = _write_index - _read_index;
        return 1;
    }

    if (nullptr != buf_ptr1)
        *buf_ptr1 = (const uint8_t*) _buffer + _read_index;
    if (nullptr != len_ptr1)
        *len_ptr1 = _capacity - _read_index;
    if (0 == _write_index)
        return 1;

    if (nullptr != buf_ptr2)
        *buf_ptr2 = _buffer;
    if (nullptr != len_ptr2)
        *len_ptr2 = _write_index;
    return 2;
}

size_t RingBuffer::writable_size() const noexcept
{
    VALIDATE_MEMBERS();

    if (0 == _capacity)
        return 0;
    if (_write_index >= _read_index)
        return _capacity - _write_index + _read_index - 1;
    return _read_index - 1 - _write_index;
}

void RingBuffer::write(const void *buf, size_t len) noexcept
{
    assert(nullptr != buf);
    ensure_writable_size(len);
    const size_t trunk_sz = _capacity - _write_index;
    if (trunk_sz >= len)
    {
        ::memcpy((uint8_t*) _buffer + _write_index, buf, len);
    }
    else
    {
        ::memcpy((uint8_t*) _buffer + _write_index, buf, trunk_sz);
        ::memcpy(_buffer, (const uint8_t*) buf + trunk_sz, len - trunk_sz);
    }
    _write_index += len;
    if (0 != _capacity)
        _write_index %= _capacity;
}

size_t RingBuffer::skip_write(size_t len) noexcept
{
    const size_t skiped = std::min<size_t>(len, writable_size());
    _write_index += len;
    if (0 != _capacity)
        _write_index %= _capacity;
    return skiped;
}

size_t RingBuffer::writable_pointers(void **buf_ptr1, size_t *len_ptr1,
                                  void **buf_ptr2, size_t *len_ptr2) noexcept
{
    VALIDATE_MEMBERS();

    if (writable_size() == 0)
        return 0;

    if (_write_index < _read_index)
    {
        if (nullptr != buf_ptr1)
            *buf_ptr1 = (uint8_t*) _buffer + _write_index;
        if (nullptr != len_ptr1)
            *len_ptr1 = _read_index - 1 - _write_index;
        return 1;
    }

    if (nullptr != buf_ptr1)
        *buf_ptr1 = (uint8_t*) _buffer + _write_index;
    if (nullptr != len_ptr1)
    {
        if (0 == _read_index)
            *len_ptr1 = _capacity - _write_index - 1;
        else
            *len_ptr1 = _capacity - _write_index;
    }
    if (_read_index < 2)
        return 1;

    if (nullptr != buf_ptr2)
        *buf_ptr2 = _buffer;
    if (nullptr != len_ptr2)
        *len_ptr2 = _read_index - 1;
    return 2;
}

void RingBuffer::ensure_writable_size(size_t write_size) noexcept
{
    VALIDATE_MEMBERS();
    if (writable_size() >= write_size)
        return;

    const size_t rd_sz = readable_size();
    size_t new_cap = _capacity * 3 / 2;
    if (new_cap < rd_sz + write_size + 1)
        new_cap = rd_sz + write_size + 1;

    if (_write_index >= _read_index)
    {
        assert(rd_sz == _write_index - _read_index);
        _buffer = ::realloc(_buffer, new_cap);
        assert(nullptr != _buffer);
        _capacity = new_cap;
    }
    else
    {
        assert(rd_sz == _capacity - _read_index + _write_index);
        void *new_buffer = ::malloc(new_cap);
        assert(nullptr != new_buffer);
        const size_t trunk_sz = _capacity - _read_index;
        ::memcpy(new_buffer, (const uint8_t*) _buffer + _read_index, trunk_sz);
        ::memcpy((uint8_t*) new_buffer + trunk_sz, _buffer, _write_index);
        ::free(_buffer);
        _buffer = new_buffer;
        _capacity = new_cap;
        _read_index = 0;
        _write_index = rd_sz;
    }
}

}
