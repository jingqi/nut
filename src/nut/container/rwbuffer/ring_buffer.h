
#ifndef ___HEADFILE_8DAEC806_FFB8_4415_A331_8D4965371079_
#define ___HEADFILE_8DAEC806_FFB8_4415_A331_8D4965371079_

#include <stdint.h>
#include <string.h> // for size_t

#include <nut/nut_config.h>


namespace nut
{

/**
 * Ring buffer
 */
class NUT_API RingBuffer
{
    /* 环形存储:
     *
      一般状态下
              |=>>>>>>>>>>>>>|
      |-------|++++++++++++++|-------------|
      0   read_index   write_index      capacity

      环写状态下
      |>>>>>>>>>|                  |=>>>>>>>>>>|
      |+++++++++|------------------|+++++++++++|
      0    write_index         read_index    capacity

     */

    void *_buffer = NULL;
    size_t _capacity = 0;
    size_t _read_index = 0, _write_index = 0;

private:
    void ensure_writable_size(size_t write_size);

public:
    RingBuffer();
    RingBuffer(const RingBuffer& x);
    ~RingBuffer();
    RingBuffer& operator=(const RingBuffer& x);

    void clear();

    size_t readable_size() const;
    size_t read(void *buf, size_t len);
    size_t look_ahead(void *buf, size_t len) const;
    size_t skip_read(size_t len);

    /**
     * 返回可读指针
     *
     * @return 0 所有指针都无效
     *         1 第一个指针有效
     *         2 第一个、第二个指针有效
     */
    size_t readable_pointers(const void **buf_ptr1, size_t *len_ptr1,
                             const void **buf_ptr2, size_t *len_ptr2) const;

    size_t writable_size() const;
    void write(const void *buf, size_t len);
    size_t skip_write(size_t len);

    /**
     * 返回可写指针
     *
     * @return 0 所有指针都无效
     *         1 第一个指针有效
     *         2 第一个、第二个指针有效
     */
    size_t writable_pointers(void **buf_ptr1, size_t *len_ptr1,
                          void **buf_ptr2, size_t *len_ptr2);
};

}

#endif
