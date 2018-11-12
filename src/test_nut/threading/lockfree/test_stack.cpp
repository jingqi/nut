
#include <iostream>
#include <string>
#include <vector>

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_LINUX
#   include <unistd.h>
#endif

#include <nut/unittest/unittest.h>
#include <nut/threading/lockfree/concurrent_stack.h>
#include <nut/util/string/to_string.h>
#include <nut/util/string/string_utils.h>


using namespace std;
using namespace nut;

class TestConcurrentStack : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
        NUT_REGISTER_CASE(test_multi_thread);
    }

    void test_smoking()
    {
        ConcurrentStack<int> s;
        s.optimistic_push(1);
        s.eliminate_push(2);

        int v = 0;
        bool rs = s.eliminate_pop(&v);
        NUT_TA(rs && v == 2);

        rs = s.optimistic_pop(&v);
        NUT_TA(rs && v == 1);

        NUT_TA(s.is_empty());
    }

    std::atomic<bool> interrupt = ATOMIC_VAR_INIT(false);

    void product_thread(ConcurrentStack<string> *cs)
    {
        assert(nullptr != cs);
        cout << "producter running" << endl;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 5000);
        size_t count = 0;
        while (!interrupt.load(std::memory_order_relaxed))
        {
            const int r = dis(gen);
            if (0 == (r & 1))
                cs->eliminate_push(to_string(r) + "|" + to_string(r + 3));
            else
                cs->optimistic_push(to_string(r) + "|" + to_string(r + 3));
            ++count;
        }
        cout << "producted: " << count << endl;
    }

    void consume_thread(ConcurrentStack<string> *cs)
    {
        assert(nullptr != cs);
        cout << "consumer running" << endl;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);
        size_t success_count = 0, total_count = 0;
        while (!interrupt.load(std::memory_order_relaxed))
        {
            string s;
            bool rs = true;
            if (0 == dis(gen))
                rs = cs->eliminate_pop(&s);
            else
                rs = cs->optimistic_pop(&s);

            if (rs)
            {
                vector<string> parts = chr_split(s, '|');
                if (parts.size() != 2)
                {
                    cout << "error \"" << s << "\"" << endl;
                    continue;
                }
                if (::atoi(parts[0].c_str()) + 3 != ::atoi(parts[1].c_str()))
                {
                    cout << "error \"" << s << "\"" << endl;
                    continue;
                }
                ++success_count;
            }
            ++total_count;
        }
        cout << "consumed: " << success_count << "/" << total_count << endl;
    }

    void test_multi_thread()
    {
        ConcurrentStack<string> s;
        vector<thread> threads;
        interrupt.store(false, std::memory_order_relaxed);
        for (size_t i = 0; i < 2; ++i)
            threads.emplace_back([=,&s] { consume_thread(&s); });
        for (size_t i = 0; i < 2; ++i)
            threads.emplace_back([=,&s] { product_thread(&s); });

        std::this_thread::sleep_for(std::chrono::milliseconds(10 * 1000));
        interrupt.store(true, std::memory_order_relaxed);
        cout << "interrupting..." << endl;
        for (size_t i = 0, sz = threads.size(); i < sz; ++i)
            threads.at(i).join();
    }
};

NUT_REGISTER_FIXTURE(TestConcurrentStack, "threading, lockfree")
