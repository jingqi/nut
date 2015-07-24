
#ifndef ___HEADFILE_27BA84B2_4461_4A06_A9AE_5E2471B5521C_
#define ___HEADFILE_27BA84B2_4461_4A06_A9AE_5E2471B5521C_

#include <assert.h>

#include "byte_array.h"

namespace nut
{

/**
 * 处理字节数组流中的大端(Big-Endian)和小端(Little-Endian)字节序读写
 */
class ByteArrayStream
{
    typedef ByteArrayStream self_type;
    typedef ByteArray array_type;

    array_type _data;
    size_t _index = 0;
    bool _little_endian = true;

public:
    ByteArrayStream(memory_allocator *ma = NULL);
    ByteArrayStream(const self_type& data);

    bool is_little_endian() const
    {
        return _little_endian;
    }

    void set_little_endian(bool little_endian = true)
    {
        _little_endian = little_endian;
    }

    size_t size() const;
    size_t tell() const;
    void seek(size_t index);
    void resize(size_t new_size);
    void skip(size_t cb);

    uint8_t read_uint8();
    uint16_t read_uint16();
    uint32_t read_uint32();
    uint64_t read_uint64();
    float read_float();
    double read_double();
    size_t read_bytes(void *buf, size_t len);

    void write_uint8(uint8_t v);
    void write_uint16(uint16_t v);
    void write_uint32(uint32_t v);
    void write_uint64(uint64_t v);
    void write_bytes(const void *buf, size_t len);
    void write_bytes(const array_type& ba);
    void write_float(float v);
    void write_double(double v);

    const array_type& byte_array() const
    {
        return _data;
    }
};

}

#endif
