
#ifndef ___HEADFILE_A3BF9913_72C8_41A5_AF8D_57F83CAFC7AD_
#define ___HEADFILE_A3BF9913_72C8_41A5_AF8D_57F83CAFC7AD_

#include <assert.h>
#include <stdint.h>
#include <stddef.h> // for size_t and so on

#include <nut/rc/rc_ptr.h>

namespace nut
{

class InputStream
{
    NUT_REF_COUNTABLE

public:
    virtual bool is_little_endian() const = 0;

    virtual void set_little_endian(bool le) = 0;

    virtual size_t available() const = 0;

    virtual void skip(size_t cb)
    {
        assert(available() >= cb);
        for (size_t i = 0; i < cb; ++i)
            read_uint8();
    }

    virtual uint8_t read_uint8() = 0;

    int8_t read_int8()
    {
        return (int8_t) read_uint8();
    }

    virtual uint16_t read_uint16()
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

    int16_t read_int16()
    {
        return (int16_t) read_uint16();
    }

    virtual uint32_t read_uint32()
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

    int32_t read_int32()
    {
        return (int32_t) read_uint32();
    }

    virtual uint64_t read_uint64()
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

    int64_t read_int64()
    {
        return (int64_t) read_uint64();
    }

    // IEEE-754 浮点数
    virtual float read_float()
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

    virtual double read_double()
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

    virtual size_t read(void *buf, size_t cb)
    {
        assert(NULL != buf || 0 == cb);
        assert(available() >= cb);
        for (size_t i = 0; i < cb; ++i)
            static_cast<uint8_t*>(buf)[i] = read_uint8();
        return cb;
    }
};

}

#endif
