
#ifndef ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_
#define ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_

#include <assert.h>

#define _ENABLE_ATOMIC_ALIGNMENT_FIX // VS2015 SP2 BUG
#include <atomic>
#include <thread>
#include <random>

#include "stamped_ptr.h"
#include "../threading.h"


// 消隐数组的指针常量
#define COLLISION_EMPTY_PTR nullptr
#define COLLISION_DONE_PTR (reinterpret_cast<Node*>(-1))

namespace nut
{

/**
 * 无锁并发栈
 *
 *        +------+-> next
 * top -> | node |   ->   .....
 *        +------+
 *
 * 参考文献：
 *   [1] Danny Hendler, Nir Shavit, Lena Yerushalmi. A Scalable Lock-free Stack
 *       Algorithm[J]. SPAA. 2004-06-27. 206-215
 */
template <typename T>
class ConcurrentStack
{
private:
    // 这里根据具体情况配置
    enum
    {
        // 消隐使用的碰撞数组的大小
        COLLISIONS_ARRAY_SIZE = 5,

        // 消隐入队时等待碰撞的毫秒数
        ELIMINATE_ENQUEUE_DELAY_MICROSECONDS = 10,
    };

    class Node
    {
    public:
        Node(T&& v)
            : data(std::forward<T>(v))
        {}

        Node(const T& v)
            : data(v)
        {}

    public:
        T data;
        Node *next = nullptr;
    };

    // 尝试出栈的结果
    enum class PopAttemptResult
    {
        PopSuccess,        // 成功
        ConcurrentFailure, // 并发失败
        EmptyStackFailure, // 空栈
    };

public:
    ConcurrentStack()
    {
        _collisions = (AtomicStampedPtr<Node>*) ::malloc(
            sizeof(AtomicStampedPtr<Node>) * COLLISIONS_ARRAY_SIZE);
        for (int i = 0; i < COLLISIONS_ARRAY_SIZE; ++i)
            new (_collisions + i) AtomicStampedPtr<Node>();
    }

    ~ConcurrentStack()
    {
        while (optimistic_pop(nullptr))
        {}
        assert(is_empty());

        for (int i = 0; i < COLLISIONS_ARRAY_SIZE; ++i)
            (_collisions + i)->~AtomicStampedPtr();
        ::free(_collisions);
        _collisions = nullptr;
    }

    bool is_empty() const
    {
        return nullptr == _top.load(std::memory_order_relaxed).ptr;
    }

    void optimistic_push(T&& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(std::forward<T>(v));

        StampedPtr<Node> old_top = _top.load(std::memory_order_relaxed);
        do
        {
            new_node->next = old_top.ptr;
        } while(!_top.compare_exchange_weak(
                    &old_top, {new_node, old_top.stamp + 1},
                    std::memory_order_release, std::memory_order_relaxed));
    }

    void optimistic_push(const T& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(v);

        StampedPtr<Node> old_top = _top.load(std::memory_order_relaxed);
        do
        {
            new_node->next = old_top.ptr;
        } while (!_top.compare_exchange_weak(
                     &old_top, {new_node, old_top.stamp + 1},
                     std::memory_order_release, std::memory_order_relaxed));
    }

    bool optimistic_pop(T *p)
    {
        StampedPtr<Node> old_top = _top.load(std::memory_order_relaxed);
        while (true)
        {
            if (nullptr == old_top.ptr)
                return false;

            if (_top.compare_exchange_weak(
                    &old_top, {old_top.ptr->next, old_top.stamp + 1},
                    std::memory_order_release, std::memory_order_relaxed))
            {
                if (nullptr != p)
                    *p = old_top.ptr->data;
                (&(old_top.ptr->data))->~T();
                ::free(old_top.ptr);
                return true;
            }
        }
    }

    void eliminate_push(T&& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(std::forward<T>(v));

        while (true)
        {
            if (push_attempt(new_node))
                return;
            if (try_to_eliminate_push(new_node))
                return;
        }
    }

    void eliminate_push(const T& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(v);

        while (true)
        {
            if (push_attempt(new_node))
                return;
            if (try_to_eliminate_push(new_node))
                return;
        }
    }

    bool eliminate_pop(T *p)
    {
        while (true)
        {
            const PopAttemptResult rs = pop_attempt(p);
            if (PopAttemptResult::EmptyStackFailure == rs)
                return false;
            else if (PopAttemptResult::PopSuccess == rs || try_to_eliminate_pop(p))
                return true;
        }
    }

private:
    ConcurrentStack(const ConcurrentStack&) = delete;
    ConcurrentStack& operator=(const ConcurrentStack&) = delete;

    bool push_attempt(Node *new_node)
    {
        StampedPtr<Node> old_top = _top.load(std::memory_order_relaxed);
        new_node->next = old_top.ptr;
        return _top.compare_exchange_weak(
            &old_top, {new_node, old_top.stamp + 1},
            std::memory_order_release, std::memory_order_relaxed);
    }

    PopAttemptResult pop_attempt(T *p)
    {
        StampedPtr<Node> old_top = _top.load(std::memory_order_relaxed);

        if (nullptr == old_top.ptr)
            return PopAttemptResult::EmptyStackFailure;

        if (_top.compare_exchange_weak(
                &old_top, {old_top.ptr->next, old_top.stamp + 1},
                std::memory_order_release, std::memory_order_relaxed))
        {
            if (nullptr != p)
                *p = old_top.ptr->data;
            (&(old_top.ptr->data))->~T();
            ::free(old_top.ptr);
            return PopAttemptResult::PopSuccess;
        }
        return PopAttemptResult::ConcurrentFailure;
    }

    bool try_to_eliminate_push(Node *new_node)
    {
        const unsigned r = rand_pos();
        StampedPtr<Node> old_collision_to_add = _collisions[r].load(std::memory_order_relaxed);
        if (COLLISION_EMPTY_PTR != old_collision_to_add.ptr)
            return false;

        // 添加到碰撞数组
        if (!_collisions[r].compare_exchange_weak(
                &old_collision_to_add, {new_node, old_collision_to_add.stamp + 1},
                std::memory_order_release, std::memory_order_relaxed))
            return false;

        // 等待一段时间
        std::this_thread::sleep_for(
            std::chrono::milliseconds(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS));

        // 检查消隐是否成功
        StampedPtr<Node> old_collision_to_remove = _collisions[r].load(std::memory_order_relaxed);
        if (COLLISION_DONE_PTR == old_collision_to_remove.ptr ||
            !_collisions[r].compare_exchange_weak(
                &old_collision_to_remove, {COLLISION_EMPTY_PTR, old_collision_to_add.stamp + 1},
                std::memory_order_release, std::memory_order_relaxed))
        {
            _collisions[r].store({COLLISION_EMPTY_PTR, old_collision_to_add.stamp + 1},
                                 std::memory_order_release);
            return true;
        }

        return false;
    }

    bool try_to_eliminate_pop(T *p)
    {
        const unsigned r = rand_pos();
        StampedPtr<Node> old_collision = _collisions[r].load(std::memory_order_relaxed);
        if (COLLISION_EMPTY_PTR == old_collision.ptr ||
            COLLISION_DONE_PTR == old_collision.ptr)
            return false;

        if (_collisions[r].compare_exchange_weak(
                &old_collision, {COLLISION_DONE_PTR, old_collision.stamp},
                std::memory_order_release, std::memory_order_relaxed))
        {
            if (nullptr != p)
                *p = old_collision.ptr->data;
            (&(old_collision.ptr->data))->~T();
            ::free(old_collision.ptr);
            return true;
        }
        return false;
    }

    /**
     * Fast, thread safely random integer in [0, COLLISIONS_ARRAY_SIZE - 1]
     */
    static unsigned rand_pos()
    {
        static NUT_THREAD_LOCAL std::random_device rd;
        static NUT_THREAD_LOCAL std::mt19937 gen(rd());
        static NUT_THREAD_LOCAL std::uniform_int_distribution<unsigned> dist(0, COLLISIONS_ARRAY_SIZE - 1);
        return dist(gen);
    }

private:
    alignas(sizeof(AtomicStampedPtr<Node>)) AtomicStampedPtr<Node> _top;

    // 用于消隐的碰撞数组
    AtomicStampedPtr<Node> *_collisions = nullptr;
};

}

#undef COLLISION_EMPTY_PTR
#undef COLLISION_DONE_PTR

#endif
