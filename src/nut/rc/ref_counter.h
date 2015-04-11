
#ifndef ___HEADFILE_8713481E_5804_4D0C_ADBD_C7D1A26203FE_
#define ___HEADFILE_8713481E_5804_4D0C_ADBD_C7D1A26203FE_

#include <nut/threading/lockfree/atomic.h>

namespace nut
{

/**
 * 线程安全的引用计数器
 */
class SyncRefCounter
{
    int volatile m_ref_count;

public:
    SyncRefCounter(int c)
        : m_ref_count(c)
    {}

    int operator++()
    {
        return atomic_add(&m_ref_count, 1) + 1;
    }

    int operator--()
    {
        return atomic_add(&m_ref_count, -1) - 1;
    }

    operator int() const
    {
        return m_ref_count;
    }
};

}

#endif
