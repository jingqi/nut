
#ifndef ___HEADFILE_27BA84B2_4461_4A06_A9AE_5E2471B5521C_
#define ___HEADFILE_27BA84B2_4461_4A06_A9AE_5E2471B5521C_

#include <assert.h>

#include "../array.h"
#include "random_access_input_stream.h"
#include "output_stream.h"

namespace nut
{

/**
 * 处理字节数组流中的大端(Big-Endian)和小端(Little-Endian)字节序读写
 */
class ByteArrayStream : public RandomAccessInputStream, public OutputStream
{
    NUT_REF_COUNTABLE_OVERRIDE

    typedef Array<uint8_t> byte_array_type;
    typedef enrc<byte_array_type> byte_rcarray_type;
    typedef ByteArrayStream self_type;

    rc_ptr<byte_rcarray_type> _data;
    size_t _index = 0;
    bool _little_endian = true;

public:
    ByteArrayStream(memory_allocator *ma = NULL);
    ByteArrayStream(byte_rcarray_type *arr);

    size_t size() const;
    void resize(size_t new_size);
    byte_rcarray_type* byte_array() const;

    virtual bool is_little_endian() const override;
    virtual void set_little_endian(bool le) override;

    virtual size_t length() const override;
    virtual size_t tell() const override;
    virtual size_t available() const override;
    virtual void seek(size_t index) override;
    virtual void skip(size_t cb) override;

    virtual uint8_t read_uint8() override;
    virtual uint16_t read_uint16() override;
    virtual uint32_t read_uint32() override;
    virtual uint64_t read_uint64() override;
    virtual float read_float() override;
    virtual double read_double() override;
    virtual size_t read(void *buf, size_t cb) override;

    virtual void write_uint8(uint8_t v) override;
    virtual void write_uint16(uint16_t v) override;
    virtual void write_uint32(uint32_t v) override;
    virtual void write_uint64(uint64_t v) override;
    virtual void write_float(float v) override;
    virtual void write_double(double v) override;
    virtual size_t write(const void *buf, size_t cb) override;

    size_t write(const byte_array_type& ba);
};

}

#endif
