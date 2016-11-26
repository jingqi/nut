
#ifndef ___HEADFILE_A3BF9913_72C8_41A5_AF8D_57F83CAFC7AD_
#define ___HEADFILE_A3BF9913_72C8_41A5_AF8D_57F83CAFC7AD_

#include <assert.h>
#include <stdint.h>
#include <stddef.h> // for size_t and so on

#include <nut/rc/rc_ptr.h>

#include "../../nut_config.h"


namespace nut
{

class NUT_API InputStream
{
    NUT_REF_COUNTABLE

public:
    InputStream& operator>>(uint8_t& v);
    InputStream& operator>>(int8_t& v);

    InputStream& operator>>(uint16_t& v);
    InputStream& operator>>(int16_t& v);

    InputStream& operator>>(uint32_t& v);
    InputStream& operator>>(int32_t& v);

    InputStream& operator>>(uint64_t& v);
    InputStream& operator>>(int64_t& v);

    InputStream& operator>>(float& v);

    InputStream& operator>>(double& v);

public:
    virtual bool is_little_endian() const = 0;
    virtual void set_little_endian(bool le) = 0;

    virtual size_t available() const = 0;

    virtual void skip(size_t cb);

    virtual uint8_t read_uint8() = 0;
    int8_t read_int8();

    virtual uint16_t read_uint16();
    int16_t read_int16();

    virtual uint32_t read_uint32();
    int32_t read_int32();

    virtual uint64_t read_uint64();
    int64_t read_int64();

    // IEEE-754 浮点数
    virtual float read_float();

    // IEEE-754 双精度浮点数
    virtual double read_double();

    virtual size_t read(void *buf, size_t cb);
};

}

#endif
