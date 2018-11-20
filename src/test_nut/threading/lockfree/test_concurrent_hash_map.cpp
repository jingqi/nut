
#include <iostream>
#include <random>
#include <thread>

#include <nut/unittest/unittest.h>
#include <nut/threading/lockfree/concurrent_hash_map.h>
#include <nut/threading/sync/spinlock.h>
#include <nut/threading/sync/lock_guard.h>
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

        v = 0;
        NUT_TA(m.remove(1, &v));
        NUT_TA(m.size() == 1 && v == 2);
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
                    NUT_TA(v == r - range + 1);
                    v = 0;
                    NUT_TA(m.remove(r - range, &v));
                }
            }
        }
    }

    std::atomic<bool> interrupt = ATOMIC_VAR_INIT(false);
    SpinLock msglock;

    void product_thread(ConcurrentHashMap<int,string> *m)
    {
        assert(nullptr != m);

        {
            LockGuard<SpinLock> g(&msglock);
            cout << "producter running" << endl;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 50);
        size_t success_count = 0, total_count = 0;
        while (!interrupt.load(std::memory_order_relaxed))
        {
            const int r = dis(gen);
            string s = to_string(r);
            if (m->insert(r, s))
                ++success_count;
            ++total_count;
        }

        LockGuard<SpinLock> g(&msglock);
        cout << "producted: " << success_count << "/" << total_count << endl;
    }

    void consume_thread(ConcurrentHashMap<int,string> *m)
    {
        assert(nullptr != m);
        {
            LockGuard<SpinLock> g(&msglock);
            cout << "consumer running" << endl;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 50);
        size_t success_count = 0, total_count = 0;
        while (!interrupt.load(std::memory_order_relaxed))
        {
            const int r = dis(gen);
            string s;
            bool rs = m->get(r, &s);
            if (rs)
            {
                if (m->remove(r))
                    ++success_count;
                if (::atoi(s.c_str()) != r)
                {
                    LockGuard<SpinLock> g(&msglock);
                    cout << "error " << r << " \"" << s << "\"" << endl;
                    continue;
                }
            }
            ++total_count;
        }

        LockGuard<SpinLock> g(&msglock);
        cout << "consumed: " << success_count << "/" << total_count << endl;
    }

    void test_multi_thread()
    {
        ConcurrentHashMap<int,string> m;
        vector<thread> threads;
        interrupt.store(false, std::memory_order_relaxed);
        for (size_t i = 0; i < 2; ++i)
            threads.emplace_back([=,&m] { consume_thread(&m); });
        for (size_t i = 0; i < 2; ++i)
            threads.emplace_back([=,&m] { product_thread(&m); });

        std::this_thread::sleep_for(std::chrono::milliseconds(10 * 1000));

        {
            LockGuard<SpinLock> g(&msglock);
            cout << "interrupting..." << endl;
        }
        interrupt.store(true, std::memory_order_relaxed);

        for (size_t i = 0, sz = threads.size(); i < sz; ++i)
            threads.at(i).join();

        cout << "remained size " << m.size();
    }
};

NUT_REGISTER_FIXTURE(TestConcurrentHashMap, "threading, lockfree")
