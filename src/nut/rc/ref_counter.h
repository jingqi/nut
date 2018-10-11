
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
    int operator++()
    {
        return _ref_count.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    int operator--()
    {
        return _ref_count.fetch_sub(1, std::memory_order_release) - 1;
    }

    operator int() const
    {
        return _ref_count.load(std::memory_order_relaxed);
    }

private:
    std::atomic<int> _ref_count = ATOMIC_VAR_INIT(0);
};

}

#endif
