
#ifndef ___HEADFILE_CD73C4E6_B2EC_409C_A37E_42DAD0C5CD14_
#define ___HEADFILE_CD73C4E6_B2EC_409C_A37E_42DAD0C5CD14_

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#include <nut/rc/rc_ptr.h>

#include "../../nut_config.h"


namespace nut
{

class NUT_API OutputStream
{
    NUT_REF_COUNTABLE

public:
    OutputStream& operator<<(uint8_t v);
    OutputStream& operator<<(int8_t v);

    OutputStream& operator<<(uint16_t v);
    OutputStream& operator<<(int16_t v);

    OutputStream& operator<<(uint32_t v);
    OutputStream& operator<<(int32_t v);

    OutputStream& operator<<(uint64_t v);
    OutputStream& operator<<(int64_t v);

    OutputStream& operator<<(float v);

    OutputStream& operator<<(double v);

public:
    virtual bool is_little_endian() const = 0;

    virtual void set_little_endian(bool le) = 0;

    virtual void write_uint8(uint8_t v) = 0;

    void write_int8(int8_t v);

    virtual void write_uint16(uint16_t v);

    void write_int16(int16_t v);

    virtual void write_uint32(uint32_t v);

    void write_int32(int32_t v);

    virtual void write_uint64(uint64_t v);

    void write_int64(int64_t v);

    virtual void write_float(float v);

    virtual void write_double(double v);

    virtual size_t write(const void *buf, size_t cb);
};

}

#endif
