
#include <assert.h>
#include <thread>

#include "nano_lock.h"


namespace nut
{

void NanoLock::lock() noexcept
{
    unsigned trycount = 0;
    while (_locked.test_and_set(std::memory_order_acquire))
    {
        // 多次尝试失败，则放弃当前 CPU 时间片
        ++trycount;
        if (0 == (trycount & 0x0f))
            std::this_thread::yield();
    }
}

bool NanoLock::try_lock() noexcept
{
    return !_locked.test_and_set(std::memory_order_acquire);
}

void NanoLock::unlock() noexcept
{
    _locked.clear(std::memory_order_release);
}

}
