
#include <assert.h>

#include "byte_array_stream.h"

namespace nut
{

ByteArrayStream::ByteArrayStream(memory_allocator *ma)
    : _data(ma)
{}

ByteArrayStream::ByteArrayStream(const self_type& data)
    : _data(data._data)
{}

size_t ByteArrayStream::size() const
{
    return _data.size();
}

size_t ByteArrayStream::tell() const
{
    return _index;
}

void ByteArrayStream::seek(size_t index)
{
    assert(index <= _data.size());
    _index = index;
}

void ByteArrayStream::resize(size_t new_size)
{
    _data.resize(new_size);
    if (_index > new_size)
        _index = new_size;
}

void ByteArrayStream::skip(size_t cb)
{
    assert(_index + cb <= _data.size());
    _index += cb;
}

uint8_t ByteArrayStream::read_uint8()
{
    assert(_index + 1 <= _data.size());
    const array_type& data = _data;
    return data.at(_index++);
}

uint16_t ByteArrayStream::read_uint16()
{
    assert(_index + 2 <= _data.size());
    const array_type& data = _data;
    uint16_t ret = 0;
    if (_little_endian)
    {
        ret = data.at(_index++);
        ret += (uint16_t) (((uint16_t) data.at(_index++)) << 8);
    }
    else
    {
        ret = (uint16_t) (((uint16_t) data.at(_index++)) << 8);
        ret += data.at(_index++);
    }
    return ret;
}

uint32_t ByteArrayStream::read_uint32()
{
    assert(_index + 4 <= _data.size());
    const array_type& data = _data;
    uint32_t ret = 0;
    if (_little_endian)
    {
        ret = data.at(_index++);
        ret += (uint32_t) (((uint32_t) data.at(_index++)) << 8);
        ret += (uint32_t) (((uint32_t) data.at(_index++)) << 16);
        ret += (uint32_t) (((uint32_t) data.at(_index++)) << 24);
    }
    else
    {
        ret = (uint32_t) (((uint32_t) data.at(_index++)) << 24);
        ret += (uint32_t) (((uint32_t) data.at(_index++)) << 16);
        ret += (uint32_t) (((uint32_t) data.at(_index++)) << 8);
        ret += data.at(_index++);
    }
    return ret;
}

uint64_t ByteArrayStream::read_uint64()
{
    assert(_index + 8 <= _data.size());
    const array_type& data = _data;
    uint64_t ret = 0;
    if (_little_endian)
    {
        ret = data.at(_index++);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 8);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 16);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 24);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 32);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 40);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 48);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 56);
    }
    else
    {
        ret = (uint64_t) (((uint64_t) data.at(_index++)) << 56);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 48);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 40);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 32);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 24);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 16);
        ret += (uint64_t) (((uint64_t) data.at(_index++)) << 8);
        ret += data.at(_index++);
    }
    return ret;
}

float ByteArrayStream::read_float()
{
    assert(_index + sizeof(float) <= _data.size());
    float ret = 0;
    for (size_t i = 0; i < sizeof(float); ++i)
        reinterpret_cast<uint8_t*>(&ret)[_little_endian ? i : (sizeof(float) - i - 1)] = read_uint8();
    return ret;
}

double ByteArrayStream::read_double()
{
    assert(_index + sizeof(double) <= _data.size());
    double ret = 0;
    for (size_t i = 0; i < sizeof(double); ++i)
        reinterpret_cast<uint8_t*>(&ret)[_little_endian ? i : (sizeof(double)-i - 1)] = read_uint8();
    return ret;
}

size_t ByteArrayStream::read_bytes(void *buf, size_t len)
{
    assert(NULL != buf || 0 == len);
    const array_type& data = _data;
    size_t ret = (len < data.size() - _index ? len : data.size() - _index);
    ::memcpy(buf, data.data() + _index, ret);
    _index += ret;
    return ret;
}

void ByteArrayStream::write_uint8(uint8_t v)
{
    if (_index < _data.size())
        _data[_index] = v;
    else
        _data.append(1, v);
    ++_index;
}

void ByteArrayStream::write_uint16(uint16_t v)
{
    for (size_t i = 0; i < sizeof(uint16_t); ++i)
    {
        const uint8_t b = (uint8_t) (_little_endian ? (v >> (8 * i)) : (v >> (8 * (2 - i - 1))));
        if (_index < _data.size())
            _data[_index] = b;
        else
            _data.append(1, b);
        ++_index;
    }
}

void ByteArrayStream::write_uint32(uint32_t v)
{
    for (size_t i = 0; i < 4; ++i)
    {
        const uint8_t b = (uint8_t) (_little_endian ? (v >> (8 * i)) : (v >> (8 * (4 - i - 1))));
        if (_index < _data.size())
            _data[_index] = b;
        else
            _data.append(1, b);
        ++_index;
    }
}

void ByteArrayStream::write_uint64(uint64_t v)
{
    for (size_t i = 0; i < 8; ++i)
    {
        const uint8_t b = (uint8_t) (_little_endian ? (v >> (8 * i)) : (v >> (8 * (8 - i - 1))));
        if (_index < _data.size())
            _data[_index] = b;
        else
            _data.append(1, b);
        ++_index;
    }
}

void ByteArrayStream::write_bytes(const void *buf, size_t len)
{
    assert(NULL != buf || 0 == len);
    assert(_index <= _data.size());
    const size_t copy = _data.size() - _index;
    ::memcpy(_data.data() + _index, buf, copy);
    _data.append(((const uint8_t*) buf) + copy, len - copy);
    _index += len;
}

void ByteArrayStream::write_bytes(const array_type& ba)
{
    if (ba.size() == 0)
        return;
    write_bytes(ba.data(), ba.size());
}

void ByteArrayStream::write_float(float v)
{
    for (size_t i = 0; i < sizeof(float); ++i)
        write_uint8(reinterpret_cast<const uint8_t*>(&v)[_little_endian ? i : (sizeof(float) - i - 1)]);
}

void ByteArrayStream::write_double(double v)
{
    for (size_t i = 0; i < sizeof(double); ++i)
        write_uint8(reinterpret_cast<const uint8_t*>(&v)[_little_endian ? i : (sizeof(double) - i - 1)]);
}

}
