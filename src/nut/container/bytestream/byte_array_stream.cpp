
#include <assert.h>
#include <string.h> // for ::memcpy()
#include <algorithm> // for std::min()

#include "byte_array_stream.h"


namespace nut
{

ByteArrayStream::ByteArrayStream(std::vector<uint8_t>&& arr) noexcept
    : _data(std::forward<std::vector<uint8_t>>(arr))
{}

ByteArrayStream::ByteArrayStream(const std::vector<uint8_t>& arr) noexcept
    : _data(arr)
{}

ByteArrayStream::ByteArrayStream(const void *data, size_t cb) noexcept
    : _data(reinterpret_cast<const uint8_t*>(data),
            reinterpret_cast<const uint8_t*>(data) + cb)
{
    assert(nullptr != data || 0 == cb);
}

bool ByteArrayStream::is_little_endian() const noexcept
{
    return _little_endian;
}

void ByteArrayStream::set_little_endian(bool le) noexcept
{
    _little_endian = le;
}

const std::vector<uint8_t>& ByteArrayStream::byte_array() const noexcept
{
    return _data;
}

std::vector<uint8_t>& ByteArrayStream::byte_array() noexcept
{
    return const_cast<std::vector<uint8_t>&>(
        static_cast<const ByteArrayStream&>(*this).byte_array());
}

size_t ByteArrayStream::size() const noexcept
{
    return _data.size();
}

void ByteArrayStream::resize(size_t new_size) noexcept
{
    _data.resize(new_size);
    if (_index > new_size)
        _index = new_size;
}

size_t ByteArrayStream::tell() const noexcept
{
    return _index;
}

void ByteArrayStream::seek(size_t index) noexcept
{
    assert(index <= _data.size());
    _index = index;
}

size_t ByteArrayStream::readable_size() const noexcept
{
    return _data.size() - _index;
}

void ByteArrayStream::skip_read(size_t cb) noexcept
{
    assert(_index + cb <= _data.size());
    _index += cb;
}

size_t ByteArrayStream::read(void *buf, size_t cb) noexcept
{
    assert(nullptr != buf || 0 == cb);
    size_t ret = std::min(cb, readable_size());
    ::memcpy(buf, _data.data() + _index, ret);
    _index += ret;
    return ret;
}

size_t ByteArrayStream::write(const void *buf, size_t cb) noexcept
{
    assert(nullptr != buf || 0 == cb);
    assert(_index <= _data.size());
    const size_t copy = std::min(cb, _data.size() - _index);
    ::memcpy(_data.data() + _index, buf, copy);
    _data.insert(_data.end(), reinterpret_cast<const uint8_t*>(buf) + copy,
                 reinterpret_cast<const uint8_t*>(buf) + cb);
    _index += cb;
    return cb;
}

size_t ByteArrayStream::write(const std::vector<uint8_t>& ba) noexcept
{
    return write(ba.data(), ba.size());
}

}
