
#include <nut/unittest/unittest.h>

#include <nut/threading/threading.h>

using namespace nut;

class TestThreading : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoke);
    }

    void test_smoke()
    {
        // 如果不支持 thread-local 变量，则编译出错
        static NUT_THREAD_LOCAL int a;
        a = 2;
    }
};

NUT_REGISTER_FIXTURE(TestThreading, "threading, quiet")
