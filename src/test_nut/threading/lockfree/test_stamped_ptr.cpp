
#include <type_traits>

#include <nut/unittest/unittest.h>
#include <nut/threading/lockfree/stamped_ptr.h>


using namespace std;
using namespace nut;

class TestStampedPtr : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        int a = 12, b = 24;
        AtomicStampedPtr<int> asp(&a, 1);
        NUT_TA(asp.load() == StampedPtr<int>(&a, 1));

        asp.store({&b, 2});
        NUT_TA(asp.load() == StampedPtr<int>(&b, 2));

        NUT_TA(asp.exchange({&a, 3}) == StampedPtr<int>(&b, 2));
        NUT_TA(asp.load() == StampedPtr<int>(&a, 3));

        StampedPtr<int> expected(&b, 3);
        NUT_TA(!asp.compare_exchange_weak(&expected, {nullptr, 0}));
        NUT_TA(asp.load() == StampedPtr<int>(&a, 3));
        NUT_TA(expected == StampedPtr<int>(&a, 3));
        NUT_TA(asp.compare_exchange_weak(&expected, {&b, 4}));
        NUT_TA(asp.load() == StampedPtr<int>(&b, 4));
    }
};

NUT_REGISTER_FIXTURE(TestStampedPtr, "threading, lockfree, quiet")
