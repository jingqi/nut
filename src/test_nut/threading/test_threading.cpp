
#include <thread>

#include <nut/unittest/unittest.h>

#include <nut/threading/threading.h>

using namespace nut;


static NUT_THREAD_LOCAL int test_tl_var = 0;
static int test_gl_var = 0;
static bool error = false;

class TestThreading : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoke);
    }

    void test_smoke()
    {
        test_tl_var = 1234;
        test_gl_var = 1;
        error = false;
        std::thread t([=] {
                          if (0 != test_tl_var)
                              error = true;
                          test_tl_var = 76;
                          test_gl_var = 89;
                      });
        t.join();
        NUT_TA(!error && 1234 == test_tl_var && 89 == test_gl_var);
    }
};

NUT_REGISTER_FIXTURE(TestThreading, "threading, quiet")
