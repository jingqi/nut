
#include <iostream>
#include <string>
#include <vector>

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_LINUX
#   include <unistd.h>
#endif

#include <nut/unittest/unittest.h>
#include <nut/threading/lockfree/concurrent_queue.h>
#include <nut/threading/sync/spinlock.h>
#include <nut/threading/sync/lock_guard.h>
#include <nut/util/string/to_string.h>
#include <nut/util/string/string_utils.h>


using namespace std;
using namespace nut;

class TestConcurrentQueue : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_trivially_copy_elem);
        NUT_REGISTER_CASE(test_none_trivially_copy_elem);
        NUT_REGISTER_CASE(test_multi_thread);
    }

    void test_trivially_copy_elem()
    {
        ConcurrentQueue<int> q;
        q.optimistic_enqueue(1);
        q.eliminate_enqueue(2);

        int v = 0;
        bool rs = q.eliminate_dequeue(&v);
        NUT_TA(rs && v == 1);

        rs = q.optimistic_dequeue(&v);
        NUT_TA(rs && v == 2);

        NUT_TA(q.is_empty());
    }

    void test_none_trivially_copy_elem()
    {
        ConcurrentQueue<string> q;
        q.optimistic_enqueue("abc");
        q.eliminate_enqueue("bcdefghi");

        string v;
        bool rs = q.eliminate_dequeue(&v);
        NUT_TA(rs && v == "abc");

        rs = q.optimistic_dequeue(&v);
        NUT_TA(rs && v == "bcdefghi");

        NUT_TA(q.is_empty());
    }

    std::atomic<bool> interrupt = ATOMIC_VAR_INIT(false);
    SpinLock msglock;

    void product_thread(ConcurrentQueue<string> *q)
    {
        assert(nullptr != q);
        {
            LockGuard<SpinLock> g(&msglock);
            cout << "producter running" << endl;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 5000);
        size_t count = 0;
        while (!interrupt.load(std::memory_order_relaxed))
        {
            const int r = dis(gen);
            if (0 == (r & 1))
                q->eliminate_enqueue(to_string(r) + "|" + to_string(r + 3));
            else
                q->optimistic_enqueue(to_string(r) + "|" + to_string(r + 3));
            ++count;
        }
        LockGuard<SpinLock> g(&msglock);
        cout << "producted: " << count << endl;
    }

    void consume_thread(ConcurrentQueue<string> *q)
    {
        assert(nullptr != q);
        {
            LockGuard<SpinLock> g(&msglock);
            cout << "consumer running" << endl;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);
        size_t success_count = 0, total_count = 0;
        while (!interrupt.load(std::memory_order_relaxed))
        {
            string s;
            bool rs = true;
            if (0 == dis(gen))
                rs = q->eliminate_dequeue(&s);
            else
                rs = q->optimistic_dequeue(&s);

            if (rs)
            {
                vector<string> parts = chr_split(s, '|');
                if (parts.size() != 2)
                {
                    LockGuard<SpinLock> g(&msglock);
                    cout << "error \"" << s << "\"" << endl;
                    continue;
                }
                if (::atoi(parts[0].c_str()) + 3 != ::atoi(parts[1].c_str()))
                {
                    LockGuard<SpinLock> g(&msglock);
                    cout << "error \"" << s << "\"" << endl;
                    continue;
                }
                ++success_count;
            }
            ++total_count;
        }
        LockGuard<SpinLock> g(&msglock);
        cout << "consumed: " << success_count << "/" << total_count << endl;
    }

    void test_multi_thread()
    {
        ConcurrentQueue<string> q;
        vector<thread> threads;
        interrupt.store(false, std::memory_order_relaxed);
        for (size_t i = 0; i < 2; ++i)
            threads.emplace_back([=,&q] { consume_thread(&q); });
        for (size_t i = 0; i < 2; ++i)
            threads.emplace_back([=,&q] { product_thread(&q); });

        std::this_thread::sleep_for(std::chrono::milliseconds(10 * 1000));

        {
            LockGuard<SpinLock> g(&msglock);
            cout << "interrupting..." << endl;
        }
        interrupt.store(true, std::memory_order_relaxed);

        for (size_t i = 0, sz = threads.size(); i < sz; ++i)
            threads.at(i).join();

        cout << "remained size " << q.size();
    }
};

NUT_REGISTER_FIXTURE(TestConcurrentQueue, "threading, lockfree")
