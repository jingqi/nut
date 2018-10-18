
#ifndef ___HEADFILE_70C13D94_A7E7_4EE0_A2F6_2FDD1FB846AB_
#define ___HEADFILE_70C13D94_A7E7_4EE0_A2F6_2FDD1FB846AB_

namespace nut
{

class DummyLock
{
public:
    DummyLock() = default;

    void lock()
    {}

    bool trylock()
    {
        return true;
    }

    void unlock()
    {}

private:
    // Non-copyable
    DummyLock(const DummyLock&) = delete;
    DummyLock& operator=(const DummyLock&) = delete;
};

}

#endif
