
#ifndef ___HEADFILE_27BA84B2_4461_4A06_A9AE_5E2471B5521C_
#define ___HEADFILE_27BA84B2_4461_4A06_A9AE_5E2471B5521C_

#include <assert.h>

#include "../../nut_config.h"
#include "../array.h"
#include "input_stream.h"
#include "output_stream.h"
#include "random_access_stream.h"


namespace nut
{

/**
 * 处理字节数组流中的大端(Big-Endian)和小端(Little-Endian)字节序读写
 */
class NUT_API ByteArrayStream : public InputStream, public OutputStream, public RandomAccessStream
{
    NUT_REF_COUNTABLE_OVERRIDE

private:
    typedef Array<uint8_t> byte_array_type;
    typedef enrc<byte_array_type> byte_rcarray_type;

public:
    ByteArrayStream();
    explicit ByteArrayStream(byte_rcarray_type *arr);

    virtual bool is_little_endian() const override;
    virtual void set_little_endian(bool le) override;

    byte_rcarray_type* byte_array() const;

    virtual size_t size() const override;
    void resize(size_t new_size);
    virtual size_t tell() const override;
    virtual void seek(size_t index) override;

    virtual size_t readable_size() const override;
    virtual void skip_read(size_t cb) override;
    virtual size_t read(void *buf, size_t cb) override;

    virtual size_t write(const void *buf, size_t cb) override;

    size_t write(const byte_array_type& ba);

private:
    ByteArrayStream(const ByteArrayStream&) = delete;
    ByteArrayStream& operator=(const ByteArrayStream&) = delete;

private:
    rc_ptr<byte_rcarray_type> _data;
    size_t _index = 0;
    bool _little_endian = true;
};

}

#endif
