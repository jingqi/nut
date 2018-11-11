
#include <iostream>
#include <random>
#include <thread>

#include <nut/unittest/unittest.h>
#include <nut/threading/lockfree/concurrent_hash_map.h>
#include <nut/util/string/to_string.h>

using namespace std;
using namespace nut;

class TestConcurrentHashMap : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
        NUT_REGISTER_CASE(test_random);
        NUT_REGISTER_CASE(test_multi_thread);
    }

    void test_smoking()
    {
        ConcurrentHashMap<int,int> m;
        NUT_TA(m.insert(1, 2));
        NUT_TA(m.size() == 1);
        NUT_TA(m.contains_key(1));
        NUT_TA(!m.insert(1, 3));
        m.insert(2, 4);

        int v = 0;
        NUT_TA(m.get(1, &v));
        NUT_TA(v == 2);

        NUT_TA(m.remove(1));
        NUT_TA(m.size() == 1);
        m.clear();
        NUT_TA(m.size() == 0);
    }

    void test_random()
    {
        const int range = 500;
        ConcurrentHashMap<int,int> m;
        for (size_t i = 0; i < 100000; ++i)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, range * 2);
            const int r = dis(gen);
            if (r < range)
            {
                m.insert(r, r + 1);
            }
            else
            {
                int v = 0;
                bool rs = m.get(r - range, &v);
                if (rs)
                {
                    NUT_TA(m.remove(r - range));
                    NUT_TA(v == r - range + 1);
                }
            }
        }
    }

    std::atomic<bool> interrupt = ATOMIC_VAR_INIT(false);

    void product_thread(ConcurrentHashMap<int,string> *m)
    {
        assert(nullptr != m);
        cout << "consume running" << endl;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 50);
        while (!interrupt.load(std::memory_order_relaxed))
        {
            const int r = dis(gen);
            string s = to_string(r);
            m->insert(r, s);
        }
    }

    void consume_thread(ConcurrentHashMap<int,string> *m)
    {
        assert(nullptr != m);
        cout << "product running" << endl;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 50);
        while (!interrupt.load(std::memory_order_relaxed))
        {
            const int r = dis(gen);
            string s;
            const bool rs = m->get(r, &s);
            if (rs)
            {
                m->remove(r);
                if (::atoi(s.c_str()) != r)
                {
                    cout << "error " << r << " \"" << s << "\"" << endl;
                    continue;
                }
            }
        }
    }

    void test_multi_thread()
    {
        const size_t thread_count = 4;
        ConcurrentHashMap<int,string> m;
        vector<thread> threads;
        interrupt.store(false, std::memory_order_relaxed);
        for (size_t i = 0; i < thread_count; ++i)
            threads.emplace_back([=,&m] { consume_thread(&m); });
        for (size_t i = 0; i < thread_count; ++i)
            threads.emplace_back([=,&m] { product_thread(&m); });

        std::this_thread::sleep_for(std::chrono::milliseconds(5 * 1000));
        interrupt.store(true, std::memory_order_relaxed);
        cout << "interrupting..." << endl;
        for (size_t i = 0, sz = threads.size(); i < sz; ++i)
            threads.at(i).join();
        cout << "remained size " << m.size();
    }
};

NUT_REGISTER_FIXTURE(TestConcurrentHashMap, "threading, lockfree")
