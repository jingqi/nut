
#ifndef ___HEADFILE_094F11D2_4F74_47A1_A6E9_2F453DDFC214_
#define ___HEADFILE_094F11D2_4F74_47A1_A6E9_2F453DDFC214_

#include <stdint.h>
#include <string.h> // for size_t

#include <nut/nut_config.h>


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
        Fragment *next = NULL;

        // NOTE 这一部分是变长的，应该作为最后一个成员
        uint8_t buffer[1];

    private:
        Fragment(const Fragment&);
        Fragment& operator=(const Fragment&);

    public:
        Fragment(size_t cap)
            : capacity(cap)
        {}
    };

private:
    Fragment *_read_fragment = NULL;
    Fragment *_write_fragment = NULL;

    size_t _read_index = 0;
    size_t _read_available = 0;

    void enqueue(Fragment *frag);

public:
    FragmentBuffer();
    FragmentBuffer(const FragmentBuffer& x);
    ~FragmentBuffer();
    FragmentBuffer& operator=(const FragmentBuffer& x);

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
     * @return 如果内存片段被征用，则返回 NULL，否则返回传入的同一指针
     */
    Fragment* write_fragment(Fragment *frag);
};

}

#endif
