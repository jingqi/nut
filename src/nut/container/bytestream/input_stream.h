
#ifndef ___HEADFILE_A3BF9913_72C8_41A5_AF8D_57F83CAFC7AD_
#define ___HEADFILE_A3BF9913_72C8_41A5_AF8D_57F83CAFC7AD_

#include <assert.h>
#include <stdint.h>
#include <stddef.h> // for size_t and so on
#include <string>

#include "../../nut_config.h"
#include "../../rc/rc_ptr.h"


namespace nut
{

class NUT_API InputStream
{
    NUT_REF_COUNTABLE

public:
    InputStream& operator>>(bool& v);

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

    InputStream& operator>>(std::string& s);
    InputStream& operator>>(std::wstring& s);

    virtual bool is_little_endian() const noexcept = 0;
    virtual void set_little_endian(bool le) noexcept = 0;

    virtual size_t readable_size() const noexcept = 0;

    virtual size_t read(void *buf, size_t cb) = 0;

    virtual void skip_read(size_t cb);

    uint8_t read_uint8();
    int8_t read_int8();

    uint16_t read_uint16();
    int16_t read_int16();

    uint32_t read_uint32();
    int32_t read_int32();

    uint64_t read_uint64();
    int64_t read_int64();

    // IEEE-754 单精度浮点数
    float read_float();

    // IEEE-754 双精度浮点数
    double read_double();

    // Read 4 byte length + string data
    virtual std::string read_string();
    virtual std::wstring read_wstring();
};

}

#endif
