
#ifndef ___HEADFILE_8713481E_5804_4D0C_ADBD_C7D1A26203FE_
#define ___HEADFILE_8713481E_5804_4D0C_ADBD_C7D1A26203FE_

#include <atomic>


namespace nut
{

/**
 * 线程安全的引用计数器
 */
class RefCounter
{
public:
    int operator++() noexcept
    {
        return _ref_count.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    int operator--() noexcept
    {
        const int ret = _ref_count.fetch_sub(1, std::memory_order_release) - 1;
        if (0 == ret)
        {
            // NOTE Object will be deleted, ensure all modificiations in other
            //      threads of the object is visible in current thread
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return ret;
    }

    operator int() const noexcept
    {
        return _ref_count.load(std::memory_order_relaxed);
    }

private:
    std::atomic<int> _ref_count = ATOMIC_VAR_INIT(0);
};

}

#endif
