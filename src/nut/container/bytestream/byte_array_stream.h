
#ifndef ___HEADFILE_27BA84B2_4461_4A06_A9AE_5E2471B5521C_
#define ___HEADFILE_27BA84B2_4461_4A06_A9AE_5E2471B5521C_

#include <assert.h>
#include <stdint.h>
#include <vector>

#include "../../nut_config.h"
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

public:
    ByteArrayStream() = default;
    explicit ByteArrayStream(std::vector<uint8_t>&& arr) noexcept;
    explicit ByteArrayStream(const std::vector<uint8_t>& arr) noexcept;
    ByteArrayStream(const void *data, size_t cb) noexcept;

    virtual bool is_little_endian() const noexcept override;
    virtual void set_little_endian(bool le) noexcept override;

    const std::vector<uint8_t>& byte_array() const noexcept;
    std::vector<uint8_t>& byte_array() noexcept;

    virtual size_t size() const noexcept override;
    void resize(size_t new_size) noexcept;
    virtual size_t tell() const noexcept override;
    virtual void seek(size_t index) noexcept override;

    virtual size_t readable_size() const noexcept override;
    virtual void skip_read(size_t cb) noexcept override;
    virtual size_t read(void *buf, size_t cb) noexcept override;

    virtual size_t write(const void *buf, size_t cb) noexcept override;

    size_t write(const std::vector<uint8_t>& ba) noexcept;

private:
    ByteArrayStream(const ByteArrayStream&) = delete;
    ByteArrayStream& operator=(const ByteArrayStream&) = delete;

private:
    std::vector<uint8_t> _data;
    size_t _index = 0;
    bool _little_endian = true;
};

}

#endif
