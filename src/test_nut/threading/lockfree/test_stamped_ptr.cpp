
#include <type_traits>

#include <nut/unittest/unittest.h>
#include <nut/threading/lockfree/stamped_ptr.h>


using namespace std;
using namespace nut;

class TestStampedPtr : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        // alignas(16) int64_t volatile dest[2];
        // int64_t expect[2];
        // ::InterlockedCompareExchange128(dest, 0, 0, expect);

        // Windows VC 下需要 align 16
        alignas(16) AtomicStampedPtr<int> asp;
        // AtomicStampedPtr<int> asp;
        int i = 12;
        StampedPtr<int> sp(&i, 13);
        asp.store(sp);
        asp.load();
    }
};

NUT_REGISTER_FIXTURE(TestStampedPtr, "threading, lockfree, quiet")
