
#include <nut/platform/portable_endian.h>

#include "input_stream.h"


namespace nut
{

InputStream& InputStream::operator>>(bool& v)
{
    v = (0 != read_uint8());
    return *this;
}

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

InputStream& InputStream::operator>>(std::string& s)
{
    s = read_string();
    return *this;
}

InputStream& InputStream::operator>>(std::wstring& s)
{
    s = read_wstring();
    return *this;
}

void InputStream::skip_read(size_t cb)
{
    assert(readable_size() >= cb);
    for (size_t i = 0; i < cb; ++i)
        read_uint8();
}

uint8_t InputStream::read_uint8()
{
    assert(readable_size() >= sizeof(uint8_t));
    uint8_t ret = 0;
    const size_t rs = read(&ret, sizeof(uint8_t));
    assert(sizeof(uint8_t) == rs);
    UNUSED(rs);
    return ret;
}

int8_t InputStream::read_int8()
{
    return (int8_t) read_uint8();
}

uint16_t InputStream::read_uint16()
{
    assert(readable_size() >= sizeof(uint16_t));
    uint16_t ret = 0;
    const size_t rs = read(&ret, sizeof(uint16_t));
    assert(sizeof(uint16_t) == rs);
    UNUSED(rs);

    if (is_little_endian())
        ret = le16toh(ret);
    else
        ret = be16toh(ret);
    return ret;
}

int16_t InputStream::read_int16()
{
    return (int16_t) read_uint16();
}

uint32_t InputStream::read_uint32()
{
    assert(readable_size() >= sizeof(uint32_t));
    uint32_t ret = 0;
    const size_t rs = read(&ret, sizeof(uint32_t));
    assert(sizeof(uint32_t) == rs);
    UNUSED(rs);

    if (is_little_endian())
        ret = le32toh(ret);
    else
        ret = be32toh(ret);
    return ret;
}

int32_t InputStream::read_int32()
{
    return (int32_t) read_uint32();
}

uint64_t InputStream::read_uint64()
{
    assert(readable_size() >= sizeof(uint64_t));
    uint64_t ret = 0;
    const size_t rs = read(&ret, sizeof(uint64_t));
    assert(sizeof(uint64_t) == rs);
    UNUSED(rs);

    if (is_little_endian())
        ret = le64toh(ret);
    else
        ret = be64toh(ret);
    return ret;
}

int64_t InputStream::read_int64()
{
    return (int64_t) read_uint64();
}

float InputStream::read_float()
{
    assert(readable_size() >= sizeof(float));
    static_assert(sizeof(float) == sizeof(uint32_t), "Unexpected float size");
    uint32_t iv = 0;
    const size_t rs = read(&iv, sizeof(float));
    assert(sizeof(float) == rs);
    UNUSED(rs);

    if (is_little_endian())
        iv = le32toh(iv);
    else
        iv = be32toh(iv);
    return *reinterpret_cast<float*>(&iv);
}

double InputStream::read_double()
{
    assert(readable_size() >= sizeof(double));
    static_assert(sizeof(double) == sizeof(uint64_t), "Unexpected double size");
    uint64_t iv = 0;
    const size_t rs = read(&iv, sizeof(double));
    assert(sizeof(double) == rs);
    UNUSED(rs);

    if (is_little_endian())
        iv = le64toh(iv);
    else
        iv = be64toh(iv);
    return *reinterpret_cast<double*>(&iv);
}

std::string InputStream::read_string()
{
    std::string ret;
    const size_t len = read_uint32();
    ret.resize(len);
    assert(readable_size() >= sizeof(char) * len);
    const size_t rs = read((char*) ret.data(), sizeof(char) * len);
    assert(rs == sizeof(char) * len);
    UNUSED(rs);
    return ret;
}

std::wstring InputStream::read_wstring()
{
    std::wstring ret;
    const size_t len = read_uint32();
    ret.resize(len);
    assert(readable_size() >= sizeof(wchar_t) * len);
    const size_t rs = read((wchar_t*) ret.data(), sizeof(wchar_t) * len);
    assert(rs == sizeof(wchar_t) * len);
    UNUSED(rs);
    return ret;
}

}
