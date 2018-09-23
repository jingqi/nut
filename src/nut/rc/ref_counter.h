
#ifndef ___HEADFILE_8713481E_5804_4D0C_ADBD_C7D1A26203FE_
#define ___HEADFILE_8713481E_5804_4D0C_ADBD_C7D1A26203FE_

#include <atomic>


namespace nut
{

/**
 * 线程安全的引用计数器
 */
class SyncRefCounter
{
    std::atomic<int> _ref_count = ATOMIC_VAR_INIT(0);

public:
    SyncRefCounter(int c)
        : _ref_count(c)
    {}

    int operator++()
    {
        return ++_ref_count;
    }

    int operator--()
    {
        return --_ref_count;
    }

    operator int() const
    {
        return _ref_count;
    }
};

}

#endif
