
#include <nut/platform/portable_endian.h>

#include "output_stream.h"


namespace nut
{

OutputStream& OutputStream::operator<<(bool v)
{
    write_uint8(v ? 1 : 0);
    return *this;
}

OutputStream& OutputStream::operator<<(uint8_t v)
{
    write_uint8(v);
    return *this;
}

OutputStream& OutputStream::operator<<(int8_t v)
{
    write_int8(v);
    return *this;
}

OutputStream& OutputStream::operator<<(uint16_t v)
{
    write_uint16(v);
    return *this;
}

OutputStream& OutputStream::operator<<(int16_t v)
{
    write_int16(v);
    return *this;
}

OutputStream& OutputStream::operator<<(uint32_t v)
{
    write_uint32(v);
    return *this;
}

OutputStream& OutputStream::operator<<(int32_t v)
{
    write_int32(v);
    return *this;
}

OutputStream& OutputStream::operator<<(uint64_t v)
{
    write_uint64(v);
    return *this;
}

OutputStream& OutputStream::operator<<(int64_t v)
{
    write_int64(v);
    return *this;
}

OutputStream& OutputStream::operator<<(float v)
{
    write_float(v);
    return *this;
}

OutputStream& OutputStream::operator<<(double v)
{
    write_double(v);
    return *this;
}

OutputStream& OutputStream::operator<<(const char *s)
{
    assert(nullptr != s);
    write_string(s);
    return *this;
}

OutputStream& OutputStream::operator<<(const std::string& s)
{
    write_string(s);
    return *this;
}

OutputStream& OutputStream::operator<<(const wchar_t* s)
{
    write_wstring(s);
    return *this;
}

OutputStream& OutputStream::operator<<(const std::wstring& s)
{
    write_wstring(s);
    return *this;
}

void OutputStream::write_int8(int8_t v)
{
    write_uint8((uint8_t) v);
}

void OutputStream::write_uint16(uint16_t v)
{
    if (is_little_endian())
        v = htole16(v);
    else
        v = htobe16(v);
    
    const size_t rs = write(&v, sizeof(uint16_t));
    assert(sizeof(uint16_t) == rs);
    UNUSED(rs);
}

void OutputStream::write_int16(int16_t v)
{
    write_uint16((uint16_t) v);
}

void OutputStream::write_uint32(uint32_t v)
{
    if (is_little_endian())
        v = htole32(v);
    else
        v = htobe32(v);
    
    const size_t rs = write(&v, sizeof(uint32_t));
    assert(sizeof(uint32_t) == rs);
    UNUSED(rs);
}

void OutputStream::write_int32(int32_t v)
{
    write_uint32((uint32_t) v);
}

void OutputStream::write_uint64(uint64_t v)
{
    if (is_little_endian())
        v = htole64(v);
    else
        v = htobe64(v);
    
    const size_t rs = write(&v, sizeof(uint64_t));
    assert(sizeof(uint64_t) == rs);
    UNUSED(rs);
}

void OutputStream::write_int64(int64_t v)
{
    write_uint64((uint64_t) v);
}

void OutputStream::write_float(float v)
{
    static_assert(sizeof(float) == sizeof(uint32_t), "Unexpected float size");
    uint32_t iv = *reinterpret_cast<uint32_t*>(&v);
    if (is_little_endian())
        iv = htole32(iv);
    else
        iv = htobe32(iv);
    
    const size_t rs = write(&iv, sizeof(float));
    assert(sizeof(float) == rs);
    UNUSED(rs);
}

void OutputStream::write_double(double v)
{
    static_assert(sizeof(double) == sizeof(uint64_t), "Unexpected double size");
    uint64_t iv = *reinterpret_cast<uint64_t*>(&v);
    if (is_little_endian())
        iv = htole64(iv);
    else
        iv = htobe64(iv);
    
    const size_t rs = write(&iv, sizeof(double));
    assert(sizeof(double) == rs);
    UNUSED(rs);
}

void OutputStream::write_string(const char *s, ssize_t len)
{
    assert(nullptr != s);
    if (len < 0)
        write_string(std::string(s));
    else
        write_string(std::string(s, len));
}

void OutputStream::write_string(const std::string& s)
{
    const size_t len = s.length();
    write_uint32((uint32_t) len);
    const size_t rs = write(s.data(), sizeof(char) * len);
    assert(rs == sizeof(char) * len);
    UNUSED(rs);
}

void OutputStream::write_wstring(const wchar_t* s, ssize_t len)
{
    assert(nullptr != s);
    if (len < 0)
        write_wstring(std::wstring(s));
    else
        write_wstring(std::wstring(s, len));
}

void OutputStream::write_wstring(const std::wstring& s)
{
    const size_t len = s.length();
    write_uint32((uint32_t) len);
    const size_t rs = write(s.data(), sizeof(wchar_t) * len);
    assert(rs == sizeof(wchar_t) * len);
    UNUSED(rs);
}

size_t OutputStream::write(const void *buf, size_t cb)
{
    assert(nullptr != buf || 0 == cb);
    for (size_t i = 0; i < cb; ++i)
        write_uint8(((const uint8_t*) buf)[i]);
    return cb;
}

}
