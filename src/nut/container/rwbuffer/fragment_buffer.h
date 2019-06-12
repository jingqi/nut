
#ifndef ___HEADFILE_094F11D2_4F74_47A1_A6E9_2F453DDFC214_
#define ___HEADFILE_094F11D2_4F74_47A1_A6E9_2F453DDFC214_

#include <stdint.h>
#include <stddef.h> // for size_t

#include "../../nut_config.h"


namespace nut
{

/**
 * 多次碎片化的写数据, 再按照 FIFO 的顺序读出
 *
 *         dequeue from head
 *                ↑
 *                |
 *          +----------+         +----------+
 * read <-  | Fragment |    -    | Fragment +    .....  <- write
 *          +----------+-> next  +----------+->      ↑
 *                                                   |
 *                                            enqueue to tail
 */
class NUT_API FragmentBuffer
{
public:
    class Fragment
    {
    public:
        explicit Fragment(size_t cap) noexcept
            : capacity(cap)
        {}

    private:
        Fragment() = delete;
        Fragment(const Fragment&) = delete;
        Fragment& operator=(const Fragment&) = delete;

    public:
        const size_t capacity = 0;
        size_t size = 0;
        Fragment *next = nullptr;

        // NOTE 这一部分是变长的，应该作为最后一个成员
        uint8_t buffer[1];
    };

public:
    FragmentBuffer() = default;
    FragmentBuffer(FragmentBuffer&& x) noexcept;
    FragmentBuffer(const FragmentBuffer& x) noexcept;
    ~FragmentBuffer() noexcept;

    FragmentBuffer& operator=(FragmentBuffer&& x) noexcept;
    FragmentBuffer& operator=(const FragmentBuffer& x) noexcept;

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
     * 获取可读 buffer 列表
     *
     * @return 返回指针个数
     */
    size_t readable_pointers(const void **buf_ptrs, size_t *len_ptrs,
                             size_t ptr_count) const noexcept;

    /**
     * 写入
     */
    void write(const void *buf, size_t len) noexcept;

    /**
     * 创建 fragment
     */
    static Fragment* new_fragment(size_t capacity) noexcept;

    /**
     * 销毁 fragment
     */
    static void delete_fragment(Fragment *frag) noexcept;

    /**
     * 写入 fragment
     *
     * @return 如果传入的 fragment 被征用，则返回 nullptr，否则返回传入的同一指针
     */
    Fragment* write_fragment(Fragment *frag) noexcept;

private:
    void enqueue(Fragment *frag) noexcept;

private:
    Fragment *_read_fragment = nullptr;
    Fragment *_write_fragment = nullptr;

    size_t _read_index = 0;
    size_t _read_available = 0;
};

}

#endif
