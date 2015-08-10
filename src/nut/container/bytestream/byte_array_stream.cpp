
#include <assert.h>

#include "byte_array_stream.h"

namespace nut
{

ByteArrayStream::ByteArrayStream(memory_allocator *ma)
{
    _data = rca_new<byte_rcarray_type>(ma, 16, ma);
}

size_t ByteArrayStream::size() const
{
    return _data->size();
}

void ByteArrayStream::resize(size_t new_size)
{
    _data->resize(new_size);
    if (_index > new_size)
        _index = new_size;
}

ByteArrayStream::byte_rcarray_type* ByteArrayStream::byte_array() const
{
    return _data;
}

bool ByteArrayStream::is_little_endian() const
{
    return _little_endian;
}

void ByteArrayStream::set_little_endian(bool le)
{
    _little_endian = le;
}

size_t ByteArrayStream::length() const
{
    return _data->size();
}

size_t ByteArrayStream::tell() const
{
    return _index;
}

size_t ByteArrayStream::available() const
{
    return _data->size() - _index;
}

void ByteArrayStream::seek(size_t index)
{
    assert(index <= _data->size());
    _index = index;
}

void ByteArrayStream::skip(size_t cb)
{
    assert(_index + cb <= _data->size());
    _index += cb;
}

uint8_t ByteArrayStream::read_uint8()
{
    assert(available() >= sizeof(uint8_t));
    const byte_rcarray_type *data = _data;
    return data->at(_index++);
}

uint16_t ByteArrayStream::read_uint16()
{
    assert(available() >= sizeof(uint16_t));
    const byte_rcarray_type *data = _data;
    uint16_t ret = 0;
    if (_little_endian)
    {
        ret = data->at(_index++);
        ret |= ((uint16_t) data->at(_index++)) << 8;
    }
    else
    {
        ret = ((uint16_t) data->at(_index++)) << 8;
        ret |= data->at(_index++);
    }
    return ret;
}

uint32_t ByteArrayStream::read_uint32()
{
    assert(available() >= sizeof(uint32_t));
    const byte_rcarray_type *data = _data;
    uint32_t ret = 0;
    if (_little_endian)
    {
        ret = data->at(_index++);
        ret |= ((uint32_t) data->at(_index++)) << 8;
        ret |= ((uint32_t) data->at(_index++)) << 16;
        ret |= ((uint32_t) data->at(_index++)) << 24;
    }
    else
    {
        ret = ((uint32_t) data->at(_index++)) << 24;
        ret |= ((uint32_t) data->at(_index++)) << 16;
        ret |= ((uint32_t) data->at(_index++)) << 8;
        ret |= data->at(_index++);
    }
    return ret;
}

uint64_t ByteArrayStream::read_uint64()
{
    assert(available() >= sizeof(uint64_t));
    const byte_rcarray_type *data = _data;
    uint64_t ret = 0;
    if (_little_endian)
    {
        ret = data->at(_index++);
        ret |= ((uint64_t) data->at(_index++)) << 8;
        ret |= ((uint64_t) data->at(_index++)) << 16;
        ret |= ((uint64_t) data->at(_index++)) << 24;
        ret |= ((uint64_t) data->at(_index++)) << 32;
        ret |= ((uint64_t) data->at(_index++)) << 40;
        ret |= ((uint64_t) data->at(_index++)) << 48;
        ret |= ((uint64_t) data->at(_index++)) << 56;
    }
    else
    {
        ret = ((uint64_t) data->at(_index++)) << 56;
        ret |= ((uint64_t) data->at(_index++)) << 48;
        ret |= ((uint64_t) data->at(_index++)) << 40;
        ret |= ((uint64_t) data->at(_index++)) << 32;
        ret |= ((uint64_t) data->at(_index++)) << 24;
        ret |= ((uint64_t) data->at(_index++)) << 16;
        ret |= ((uint64_t) data->at(_index++)) << 8;
        ret |= data->at(_index++);
    }
    return ret;
}

float ByteArrayStream::read_float()
{
    assert(available() >= sizeof(float));
    float ret = 0;
    if (_little_endian)
    {
        for (size_t i = 0; i < sizeof(float); ++i)
            reinterpret_cast<uint8_t*>(&ret)[i] = read_uint8();
    }
    else
    {
        for (size_t i = 0; i < sizeof(float); ++i)
            reinterpret_cast<uint8_t*>(&ret)[sizeof(float) - i - 1] = read_uint8();
    }
    return ret;
}

double ByteArrayStream::read_double()
{
    assert(available() >= sizeof(double));
    double ret = 0;
    if (_little_endian)
    {
        for (size_t i = 0; i < sizeof(double); ++i)
            reinterpret_cast<uint8_t*>(&ret)[i] = read_uint8();
    }
    else
    {
        for (size_t i = 0; i < sizeof(double); ++i)
            reinterpret_cast<uint8_t*>(&ret)[sizeof(double) - i - 1] = read_uint8();
    }
    return ret;
}

size_t ByteArrayStream::read(void *buf, size_t cb)
{
    assert(NULL != buf || 0 == cb);
    const byte_rcarray_type *data = _data;
    size_t ret = (cb < available() ? cb : available());
    ::memcpy(buf, data->data() + _index, ret);
    _index += ret;
    return ret;
}

void ByteArrayStream::write_uint8(uint8_t v)
{
    if (_index < _data->size())
        _data->operator[](_index) = v;
    else
        _data->push_back(v);
    ++_index;
}

void ByteArrayStream::write_uint16(uint16_t v)
{
    for (size_t i = 0; i < sizeof(uint16_t); ++i)
    {
        const uint8_t b = (uint8_t) (_little_endian ? (v >> (8 * i)) : (v >> (8 * (sizeof(uint16_t) - i - 1))));
        if (_index < _data->size())
            _data->operator[](_index) = b;
        else
            _data->push_back(b);
        ++_index;
    }
}

void ByteArrayStream::write_uint32(uint32_t v)
{
    for (size_t i = 0; i < sizeof(uint32_t); ++i)
    {
        const uint8_t b = (uint8_t) (_little_endian ? (v >> (8 * i)) : (v >> (8 * (sizeof(uint32_t) - i - 1))));
        if (_index < _data->size())
            _data->operator[](_index) = b;
        else
            _data->push_back(b);
        ++_index;
    }
}

void ByteArrayStream::write_uint64(uint64_t v)
{
    for (size_t i = 0; i < sizeof(uint64_t); ++i)
    {
        const uint8_t b = (uint8_t) (_little_endian ? (v >> (8 * i)) : (v >> (8 * (sizeof(uint64_t) - i - 1))));
        if (_index < _data->size())
            _data->operator[](_index) = b;
        else
            _data->push_back(b);
        ++_index;
    }
}

void ByteArrayStream::write_float(float v)
{
    if (_little_endian)
    {
        for (size_t i = 0; i < sizeof(float); ++i)
            write_uint8(reinterpret_cast<const uint8_t*>(&v)[i]);
    }
    else
    {
        for (size_t i = 0; i < sizeof(float); ++i)
            write_uint8(reinterpret_cast<const uint8_t*>(&v)[sizeof(float) - i - 1]);
    }
}

void ByteArrayStream::write_double(double v)
{
    if (_little_endian)
    {
        for (size_t i = 0; i < sizeof(double); ++i)
            write_uint8(reinterpret_cast<const uint8_t*>(&v)[i]);
    }
    else
    {
        for (size_t i = 0; i < sizeof(double); ++i)
            write_uint8(reinterpret_cast<const uint8_t*>(&v)[sizeof(double) - i - 1]);
    }
}

size_t ByteArrayStream::write(const void *buf, size_t cb)
{
    assert(NULL != buf || 0 == cb);
    assert(_index <= _data->size());
    const size_t copy = _data->size() - _index;
    ::memcpy(_data->data() + _index, buf, copy);
    _data->append(((const uint8_t*) buf) + copy, ((const uint8_t*) buf) + cb - copy);
    _index += cb;
    return cb;
}

size_t ByteArrayStream::write(const byte_array_type& ba)
{
    return write(ba.data(), ba.size());
}

}
