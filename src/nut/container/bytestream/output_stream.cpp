
#include "output_stream.h"

namespace nut
{

void OutputStream::write_int8(int8_t v)
{
    write_uint8((uint8_t) v);
}

void OutputStream::write_uint16(uint16_t v)
{
    if (is_little_endian())
    {
        write_uint8((uint8_t) v);
        write_uint8((uint8_t) (v >> 8));
    }
    else
    {
        write_uint8((uint8_t) (v >> 8));
        write_uint8((uint8_t) v);
    }
}

void OutputStream::write_int16(int16_t v)
{
    write_uint16((uint16_t) v);
}

void OutputStream::write_uint32(uint32_t v)
{
    if (is_little_endian())
    {
        write_uint8((uint8_t) v);
        write_uint8((uint8_t) (v >> 8));
        write_uint8((uint8_t) (v >> 16));
        write_uint8((uint8_t) (v >> 24));
    }
    else
    {
        write_uint8((uint8_t) (v >> 24));
        write_uint8((uint8_t) (v >> 16));
        write_uint8((uint8_t) (v >> 8));
        write_uint8((uint8_t) v);
    }
}

void OutputStream::write_int32(int32_t v)
{
    write_uint32((uint32_t) v);
}

void OutputStream::write_uint64(uint64_t v)
{
    if (is_little_endian())
    {
        write_uint8((uint8_t) v);
        write_uint8((uint8_t) (v >> 8));
        write_uint8((uint8_t) (v >> 16));
        write_uint8((uint8_t) (v >> 24));
        write_uint8((uint8_t) (v >> 32));
        write_uint8((uint8_t) (v >> 40));
        write_uint8((uint8_t) (v >> 48));
        write_uint8((uint8_t) (v >> 56));
    }
    else
    {
        write_uint8((uint8_t) (v >> 56));
        write_uint8((uint8_t) (v >> 48));
        write_uint8((uint8_t) (v >> 40));
        write_uint8((uint8_t) (v >> 32));
        write_uint8((uint8_t) (v >> 24));
        write_uint8((uint8_t) (v >> 16));
        write_uint8((uint8_t) (v >> 8));
        write_uint8((uint8_t) v);
    }
}

void OutputStream::write_int64(int64_t v)
{
    write_uint64((uint64_t) v);
}

void OutputStream::write_float(float v)
{
    if (is_little_endian())
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

void OutputStream::write_double(double v)
{
    if (is_little_endian())
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

size_t OutputStream::write(const void *buf, size_t cb)
{
    assert(NULL != buf || 0 == cb);
    for (size_t i = 0; i < cb; ++i)
        write_uint8(((const uint8_t*) buf)[i]);
    return cb;
}

}
