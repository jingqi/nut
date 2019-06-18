
#ifndef ___HEADFILE_707FB9B0_2FE8_47E8_AFB4_31C79BB68D16_
#define ___HEADFILE_707FB9B0_2FE8_47E8_AFB4_31C79BB68D16_

#include <atomic>

#include "../../nut_config.h"


namespace nut
{

/**
 * 及其轻量级的自旋锁, 不支持重入
 */
class NUT_API NanoLock
{
public:
    NanoLock() = default;

    void lock() noexcept;
    bool try_lock() noexcept;
    void unlock() noexcept;

private:
    NanoLock(const NanoLock&) = delete;
    NanoLock& operator=(const NanoLock&) = delete;

private:
    std::atomic_flag _locked = ATOMIC_FLAG_INIT;
};

}

#endif
