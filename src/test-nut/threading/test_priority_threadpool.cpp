
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/threading/priority_thread_pool.h>


using namespace std;
using namespace nut;

class TestPriorityThreadPool : public TestFixture
{

    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoke);
    }

    std::string s;

    virtual void set_up() override
    {
        s.clear();
    }

    void push_a()
    {
        s.push_back('a');
    }

    void push_b()
    {
        s.push_back('b');
    }

    void test_smoke()
    {
        rc_ptr<PriorityThreadPool> tp = rc_new<PriorityThreadPool>(1);
        tp->add_task(
            [=] {
                tp->add_task([=] { push_b(); }, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                tp->add_task([=] { push_b(); }, 1);
                tp->add_task([=] { push_b(); }, 1);
                tp->add_task([=] { push_b(); }, 1);
                tp->add_task([=] { push_a(); }, 2);
                tp->add_task([=] { push_a(); }, 2);
                tp->add_task([=] { push_a(); }, 2);
                tp->add_task([=] { push_a(); }, 2);
            });
        tp->wait_until_all_idle();

        // cout << s << endl;
        NUT_TA(s == "aaaabbbb");
    }
};

NUT_REGISTER_FIXTURE(TestPriorityThreadPool, "threading, quiet")
