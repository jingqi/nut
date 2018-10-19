
#include <nut/unittest/unittest.h>

#include <nut/threading/threading.h>

using namespace nut;

NUT_FIXTURE(TestThreading)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoke)
    NUT_CASES_END()

    void test_smoke()
    {
        // 如果不支持 thread-local 变量，则编译出错
        static NUT_THREAD_LOCAL int a;
        a = 2;
    }
};

NUT_REGISTER_FIXTURE(TestThreading, "threading, quiet")
