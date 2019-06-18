
#ifndef ___HEADFILE_A87EC76B_E2C1_4BCD_AF64_A177A623A874_
#define ___HEADFILE_A87EC76B_E2C1_4BCD_AF64_A177A623A874_

#include <stdint.h>
#include <stddef.h> // for size_t

#include "../../nut_config.h"


namespace nut
{

class NUT_API Adler32
{
public:
    Adler32() = default;

    void reset() noexcept;

    void update(uint8_t byte) noexcept;
    void update(const void *data, size_t len) noexcept;

    uint32_t digest() const noexcept;

private:
    uint32_t _result = 1;
};

/**
 * 滚动hash
 */
class NUT_API RollingAdler32
{
public:
    explicit RollingAdler32(size_t window) noexcept;
    ~RollingAdler32() noexcept;

    void initialize() noexcept;

    void update(uint8_t byte) noexcept;
    void update(const void *data, size_t len) noexcept;

    uint32_t get_result() const noexcept;

    /**
     * 总数据数
     */
    size_t get_data_size() const noexcept;

    /**
     * 获取滚动窗口中的数据
     *
     * @param data_ptr1 第一段数据
     * @param len_ptr1  第一段数据长度
     * @param data_ptr2 第二段数据
     * @param len_ptr2  第二段数据长度
     * @return 0 没有数据
     *         1 仅第一个段数据有效
     *         2 第一段、第二段数据都有效
     */
    size_t get_window_data(const void **data_ptr1, size_t *len_ptr1,
                           const void **data_ptr2, size_t *len_ptr2) const noexcept;

private:
    uint32_t _result = 1;
    const size_t _window_size = 16;
    uint8_t *_buf = nullptr;
    size_t _count = 0;
};

}

#endif
