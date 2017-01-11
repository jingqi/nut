
#include "input_stream.h"

namespace nut
{

InputStream& InputStream::operator>>(uint8_t& v)
{
    v = read_uint8();
    return *this;
}

InputStream& InputStream::operator>>(int8_t& v)
{
    v = read_int8();
    return *this;
}

InputStream& InputStream::operator>>(uint16_t& v)
{
    v = read_uint16();
    return *this;
}

InputStream& InputStream::operator>>(int16_t& v)
{
    v = read_int16();
    return *this;
}

InputStream& InputStream::operator>>(uint32_t& v)
{
    v = read_uint32();
    return *this;
}

InputStream& InputStream::operator>>(int32_t& v)
{
    v = read_int32();
    return *this;
}

InputStream& InputStream::operator>>(uint64_t& v)
{
    v = read_uint64();
    return *this;
}

InputStream& InputStream::operator>>(int64_t& v)
{
    v = read_int64();
    return *this;
}

InputStream& InputStream::operator>>(float& v)
{
    v = read_float();
    return *this;
}

InputStream& InputStream::operator>>(double& v)
{
    v = read_double();
    return *this;
}

void InputStream::skip(size_t cb)
{
    assert(available() >= cb);
    for (size_t i = 0; i < cb; ++i)
        read_uint8();
}

int8_t InputStream::read_int8()
{
    return (int8_t) read_uint8();
}

uint16_t InputStream::read_uint16()
{
    assert(available() >= sizeof(uint16_t));
    uint16_t ret = 0;
    if (is_little_endian())
    {
        ret |= (uint16_t) read_uint8();
        ret |= ((uint16_t) read_uint8()) << 8;
    }
    else
    {
        ret |= ((uint16_t) read_uint8()) << 8;
        ret |= (uint16_t) read_uint8();
    }
    return ret;
}

int16_t InputStream::read_int16()
{
    return (int16_t) read_uint16();
}

uint32_t InputStream::read_uint32()
{
    assert(available() >= sizeof(uint32_t));
    uint32_t ret = 0;
    if (is_little_endian())
    {
        ret |= (uint32_t) read_uint8();
        ret |= ((uint32_t) read_uint8()) << 8;
        ret |= ((uint32_t) read_uint8()) << 16;
        ret |= ((uint32_t) read_uint8()) << 24;
    }
    else
    {
        ret |= ((uint32_t) read_uint8()) << 24;
        ret |= ((uint32_t) read_uint8()) << 16;
        ret |= ((uint32_t) read_uint8()) << 8;
        ret |= (uint32_t) read_uint8();
    }
    return ret;
}

int32_t InputStream::read_int32()
{
    return (int32_t) read_uint32();
}

uint64_t InputStream::read_uint64()
{
    assert(available() >= sizeof(uint64_t));
    uint64_t ret = 0;
    if (is_little_endian())
    {
        ret |= (uint64_t) read_uint8();
        ret |= ((uint64_t) read_uint8()) << 8;
        ret |= ((uint64_t) read_uint8()) << 16;
        ret |= ((uint64_t) read_uint8()) << 24;
        ret |= ((uint64_t) read_uint8()) << 32;
        ret |= ((uint64_t) read_uint8()) << 40;
        ret |= ((uint64_t) read_uint8()) << 48;
        ret |= ((uint64_t) read_uint8()) << 56;
    }
    else
    {
        ret |= ((uint64_t) read_uint8()) << 56;
        ret |= ((uint64_t) read_uint8()) << 48;
        ret |= ((uint64_t) read_uint8()) << 40;
        ret |= ((uint64_t) read_uint8()) << 32;
        ret |= ((uint64_t) read_uint8()) << 24;
        ret |= ((uint64_t) read_uint8()) << 16;
        ret |= ((uint64_t) read_uint8()) << 8;
        ret |= (uint64_t) read_uint8();
    }
    return ret;
}

int64_t InputStream::read_int64()
{
    return (int64_t) read_uint64();
}

float InputStream::read_float()
{
    assert(available() >= sizeof(float));
    float ret = 0.0;
    if (is_little_endian())
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

double InputStream::read_double()
{
    assert(available() >= sizeof(double));
    double ret = 0.0;
    if (is_little_endian())
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

size_t InputStream::read(void *buf, size_t cb)
{
    assert(nullptr != buf || 0 == cb);
    assert(available() >= cb);
    for (size_t i = 0; i < cb; ++i)
        static_cast<uint8_t*>(buf)[i] = read_uint8();
    return cb;
}

}
