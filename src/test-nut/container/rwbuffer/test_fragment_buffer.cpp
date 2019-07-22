
#include <nut/unittest/unittest.h>

#include <nut/container/rwbuffer/fragment_buffer.h>

using namespace std;
using namespace nut;

class TestFragmentBuffer : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoke);
    }

    void test_smoke()
    {
        FragmentBuffer fb;
        NUT_TA(fb.readable_size() == 0);
        const void* bufs[2];
        size_t lens[2];
        NUT_TA(fb.readable_pointers(bufs, lens, 2) == 0);

        FragmentBuffer::Fragment *f = FragmentBuffer::new_fragment(10);
        f->size = 2;
        f->buffer[1] = 0x12;
        f = fb.write_fragment(f);
        NUT_TA(nullptr == f);
        NUT_TA(fb.readable_size() == 2);
        NUT_TA(fb.readable_pointers(bufs, lens, 2) == 1);
        NUT_TA(lens[0] == 2);

        f = FragmentBuffer::new_fragment(10);
        f->size = 2;
        f->buffer[0] = 0x34;
        f->buffer[1] = 0x56;
        f = fb.write_fragment(f);
        NUT_TA(nullptr != f);
        NUT_TA(fb.readable_size() == 4);
        NUT_TA(fb.readable_pointers(bufs, lens, 2) == 1);
        NUT_TA(lens[0] == 4);

        f->size = 7;
        f->buffer[0] = 0x78;
        f = fb.write_fragment(f);
        NUT_TA(nullptr == f);
        NUT_TA(fb.readable_size() == 11);
        NUT_TA(fb.readable_pointers(bufs, lens, 2) == 2);
        NUT_TA(lens[0] == 4 && lens[1] == 7);

        fb.skip_read(1);
        NUT_TA(fb.readable_pointers(bufs, lens, 2) == 2);
        NUT_TA(lens[0] == 3 && lens[1] == 7);
        uint16_t v = 0;
        fb.read(&v, 2);
        NUT_TA(v == 0x3412);

        fb.read(&v, 2);
        NUT_TA(v == 0x7856);
        NUT_TA(fb.readable_size() == 6);
    }
};

NUT_REGISTER_FIXTURE(TestFragmentBuffer, "container, quiet")
