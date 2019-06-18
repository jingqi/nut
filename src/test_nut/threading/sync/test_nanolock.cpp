
#include <nut/unittest/unittest.h>
#include <nut/threading/sync/nanolock.h>


using namespace nut;

class TestNanoLock : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoke);
    }

    void test_smoke()
    {
        NanoLock lock;
        lock.lock();
        NUT_TA(!lock.try_lock());
        lock.unlock();
        NUT_TA(lock.try_lock());
        lock.unlock();
    }
};

NUT_REGISTER_FIXTURE(TestNanoLock, "threading, sync, quiet")
