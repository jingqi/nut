
#ifndef ___HEADFILE_70C13D94_A7E7_4EE0_A2F6_2FDD1FB846AB_
#define ___HEADFILE_70C13D94_A7E7_4EE0_A2F6_2FDD1FB846AB_

namespace nut
{

class DummyLock
{
public:
    DummyLock() = default;

    void lock() noexcept
    {}

    bool trylock() noexcept
    {
        return true;
    }

    void unlock() noexcept
    {}

private:
    DummyLock(const DummyLock&) = delete;
    DummyLock& operator=(const DummyLock&) = delete;
};

}

#endif
