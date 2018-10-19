
#include <iostream>

#include <nut/unittest/unittest.h>

#include <nut/container/rwbuffer/ring_buffer.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestRingBuffer)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoke)
    NUT_CASE(test_wrap_write);
    NUT_CASES_END()

    void test_smoke()
    {
        RingBuffer rb;
        NUT_TA(rb.readable_size() == 0);
        NUT_TA(rb.writable_size() == 0);

        const void *rbufs[2];
        ::memset(rbufs, 0, sizeof(const void*) * 2);
        size_t lens[2];
        ::memset(lens, 0, sizeof(size_t) * 2);
        NUT_TA(rb.readable_pointers(rbufs, lens, rbufs + 1, lens + 1) == 0);

        void *wbufs[2];
        ::memset(wbufs, 0, sizeof(void*) * 2);
        NUT_TA(rb.writable_pointers(wbufs, lens, wbufs + 1, lens + 1) == 0);

        uint8_t buf[10];
        for (size_t i = 0; i < 10; ++i)
            buf[i] = (uint8_t) i;
        rb.write(buf, 10);
        NUT_TA(rb.readable_size() == 10);
        size_t wsize = rb.writable_size();
        NUT_TA(rb.readable_pointers(rbufs, lens, rbufs + 1, lens + 1) == 1);
        NUT_TA(lens[0] == 10 && 0 == ::memcmp(rbufs[0], buf, 10));

        NUT_TA(rb.read(buf, 2) == 2);
        NUT_TA(rb.readable_size() == 8);
        NUT_TA(rb.writable_size() == wsize + 2);
        NUT_TA(rb.readable_pointers(rbufs, lens, rbufs + 1, lens + 1) == 1);
        NUT_TA(lens[0] == 8 && 0 == ::memcmp(rbufs[0], buf + 2, 8));
    }

    void test_wrap_write()
    {
        // test half wrap
        RingBuffer rb;
        uint8_t buf[6];
        for (size_t i = 0; i < 6; ++i)
            buf[i] = (uint8_t) i;
        rb.write(buf, 6);                 // [====]-
        NUT_TA(rb.writable_size() == 0);
        rb.skip_read(2);                  // --[==]-
        NUT_TA(rb.writable_size() == 2);
        buf[0] = 0x12;
        buf[1] = 0x34;
        rb.write(buf, 2);                 // ]-[====
        NUT_TA(rb.writable_size() == 0);
        const void *rbufs[2];
        size_t lens[2];
        NUT_TA(rb.readable_pointers(rbufs, lens, rbufs + 1, lens + 1) == 2);
        NUT_TA(lens[0] == 5 && lens[1] == 1);

        uint16_t v = 0;
        rb.skip_read(4);                  // ]-----[
        NUT_TA(rb.look_ahead(&v, 2) == 2);
        NUT_TA(v == 0x3412);
        NUT_TA(rb.read(&v, 2) == 2);      // |------
        NUT_TA(v == 0x3412);

        // test completely wrap
        NUT_TA(rb.readable_size() == 0);
        NUT_TA(rb.writable_size() == 6);
        rb.skip_write(6);                 // [====]-
        rb.skip_read(4);                  // ----[]-
        rb.skip_write(1);                 // ----[=]
        NUT_TA(rb.readable_size() == 3);
        NUT_TA(rb.writable_size() == 3);
        void *wbufs[2];
        NUT_TA(rb.writable_pointers(wbufs, lens, wbufs + 1, lens + 1) == 1);
        NUT_TA(lens[0] == 3);
        buf[0] = 0x56;
        buf[1] = 0x78;
        rb.write(buf, 2);                 // =]--[==
        rb.skip_read(3);                  // []-----
        NUT_TA(rb.look_ahead(&v, 2) == 2);
        NUT_TA(v == 0x7856);
        NUT_TA(rb.read(&v, 2) == 2);      // |------
        NUT_TA(v == 0x7856);
    }
};

NUT_REGISTER_FIXTURE(TestRingBuffer, "container, quiet")
