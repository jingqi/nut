
#ifndef ___HEADFILE_CD73C4E6_B2EC_409C_A37E_42DAD0C5CD14_
#define ___HEADFILE_CD73C4E6_B2EC_409C_A37E_42DAD0C5CD14_

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string>

#include "../../nut_config.h"
#include "../../rc/rc_ptr.h"
#include "../../platform/int_type.h" // for ssize_t


namespace nut
{

class NUT_API OutputStream
{
    NUT_REF_COUNTABLE

public:
    OutputStream& operator<<(bool v);

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

    OutputStream& operator<<(const char *s);
    OutputStream& operator<<(const std::string& s);
    OutputStream& operator<<(const wchar_t *s);
    OutputStream& operator<<(const std::wstring& s);

    virtual bool is_little_endian() const noexcept = 0;
    virtual void set_little_endian(bool le) noexcept = 0;

    virtual size_t write(const void *buf, size_t cb) = 0;

    void write_uint8(uint8_t v);
    void write_int8(int8_t v);

    void write_uint16(uint16_t v);
    void write_int16(int16_t v);

    void write_uint32(uint32_t v);
    void write_int32(int32_t v);

    void write_uint64(uint64_t v);
    void write_int64(int64_t v);

    // IEEE-754 单精度浮点数
    void write_float(float v);

    // IEEE-754 双精度浮点数
    void write_double(double v);

    // Write 4 byte length + string data
    void write_string(const char* s, ssize_t len = -1);
    void write_wstring(const wchar_t* s, ssize_t len = -1);
    virtual void write_string(const std::string& s);
    virtual void write_wstring(const std::wstring& s);
};

}

#endif
