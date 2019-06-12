
#ifndef ___HEADFILE_8DAEC806_FFB8_4415_A331_8D4965371079_
#define ___HEADFILE_8DAEC806_FFB8_4415_A331_8D4965371079_

#include <stdint.h>
#include <stddef.h> // for size_t

#include "../../nut_config.h"


namespace nut
{

/**
 * 环形存储
 *
 *    一般状态下
 *            |^>>>>>>>>>>>>$|
 *    |-------|==============|-----------------|
 *    0   read_index   write_index         capacity
 *
 *    环写状态下
 *    >>>>>>>>>$|                  |^>>>>>>>>>>>
 *    |=========|------------------|===========|
 *    0    write_index         read_index   capacity
 *
 * readable_size() + writable_size() = capacity - 1
 *
 */
class NUT_API RingBuffer
{
public:
    RingBuffer() = default;
    RingBuffer(RingBuffer&& x) noexcept;
    RingBuffer(const RingBuffer& x) noexcept;
    ~RingBuffer() noexcept;
    RingBuffer& operator=(RingBuffer&& x) noexcept;
    RingBuffer& operator=(const RingBuffer& x) noexcept;

    void clear() noexcept;

    /**
     * 剩余可读的大小
     */
    size_t readable_size() const noexcept;

    /**
     * 读数据
     *
     * @return 读到的字节数
     */
    size_t read(void *buf, size_t len) noexcept;

    /**
     * 读数据，但是不设置读指针
     *
     * @return 读到的字节数
     */
    size_t look_ahead(void *buf, size_t len) const noexcept;

    /**
     * 使读指针跳过一定字节数
     *
     * @return 跳过的字节数
     */
    size_t skip_read(size_t len) noexcept;

    /**
     * 返回可读指针
     *
     * @return 0 所有指针都无效, readable_size() == 0
     *         1 第一个指针有效
     *         2 第一个、第二个指针有效
     */
    size_t readable_pointers(const void **buf_ptr1, size_t *len_ptr1,
                             const void **buf_ptr2, size_t *len_ptr2) const noexcept;

    /**
     * 在不扩荣的前提下，当前可写的大小
     */
    size_t writable_size() const noexcept;

    /**
     * 确保至少有 write_size 大小的可写空间
     */
    void ensure_writable_size(size_t write_size) noexcept;

    /**
     * 写数据，如果 writable_size() 不够，则会扩容
     */
    void write(const void *buf, size_t len) noexcept;

    /**
     * 使写指针跳过一定字节数
     *
     * NOTE 被跳过的数据处于未定义状态
     *
     * @param len 不能大于 writable_size()
     * @return 跳过的字节数
     */
    size_t skip_write(size_t len) noexcept;

    /**
     * 返回可写指针
     *
     * @return 0 所有指针都无效, writable_size() == 0
     *         1 第一个指针有效
     *         2 第一个、第二个指针有效
     */
    size_t writable_pointers(void **buf_ptr1, size_t *len_ptr1,
                             void **buf_ptr2, size_t *len_ptr2) noexcept;

private:
    void *_buffer = nullptr;
    size_t _capacity = 0;
    size_t _read_index = 0, _write_index = 0;
};

}

#endif
