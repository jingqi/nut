
#ifndef ___HEADFILE_094F11D2_4F74_47A1_A6E9_2F453DDFC214_
#define ___HEADFILE_094F11D2_4F74_47A1_A6E9_2F453DDFC214_

#include <stdint.h>
#include <string.h> // for size_t

#include "../../nut_config.h"


namespace nut
{

class NUT_API FragmentBuffer
{
public:
    class Fragment
    {
    public:
        const size_t capacity = 0;
        size_t size = 0;
        Fragment *next = nullptr;

        // NOTE 这一部分是变长的，应该作为最后一个成员
        uint8_t buffer[1];

    private:
        // Non-copyable
        Fragment(const Fragment&) = delete;
        Fragment& operator=(const Fragment&) = delete;

    public:
        explicit Fragment(size_t cap)
            : capacity(cap)
        {}
    };

private:
    Fragment *_read_fragment = nullptr;
    Fragment *_write_fragment = nullptr;

    size_t _read_index = 0;
    size_t _read_available = 0;

    void enqueue(Fragment *frag);

public:
    FragmentBuffer() = default;
    FragmentBuffer(const FragmentBuffer& x);
    FragmentBuffer(FragmentBuffer&& x);
    ~FragmentBuffer();

    FragmentBuffer& operator=(const FragmentBuffer& x);
    FragmentBuffer& operator=(FragmentBuffer&& x);

    void clear();

    size_t readable_size() const;
    size_t read(void *buf, size_t len);
    size_t look_ahead(void *buf, size_t len) const;
    size_t skip_read(size_t len);

    /**
     * @return 返回指针个数
     */
    size_t readable_pointers(const void **buf_ptrs, size_t *len_ptrs,
                             size_t ptr_count) const;

    void write(const void *buf, size_t len);

    static Fragment* new_fragment(size_t capacity);
    static void delete_fragment(Fragment *frag);

    /**
     * @return 如果内存片段被征用，则返回 nullptr，否则返回传入的同一指针
     */
    Fragment* write_fragment(Fragment *frag);
};

}

#endif
