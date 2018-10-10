
#ifndef ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_
#define ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_

#include <assert.h>
#include <string.h> // for memcpy()
#include <stdlib.h> // for rand()

#define _ENABLE_ATOMIC_ALIGNMENT_FIX // VS2015 SP2 BUG
#include <atomic>
#include <thread>

#include "stamped_ptr.h"

// 消隐数组的指针常量
#define COLLISION_EMPTY_PTR nullptr
#define COLLISION_DONE_PTR (reinterpret_cast<Node*>(-1))

namespace nut
{

/**
 * 无锁并发队列
 *
 * 关键字：
 *   CAS(Compare-And-Swap)
 *   MS-queue(Michael and Scott)算法
 *   Dominique Fober算法
 *   optimistic算法
 *   消隐(shavit and Touitou)
 *
 * 参考文献：
 *   [1] 钱立兵，陈波等. 多线程并发访问无锁队列的算法研究[J]. 先进技术研究通报，
 *       2009-8，3(8). 50-55
 *   [2] Danny Hendler, Nir Shavit, Lena Yerushalmi. A Scalable Lock-free Stack
 *       Algorithm[J]. SPAA. 2004-06-27. 206-215
 */
template <typename T>
class ConcurrentQueue
{
    // 这里根据具体情况配置
    enum
    {
        // 消隐使用的碰撞数组的大小
        COLLISIONS_ARRAY_SIZE = 5,

        // 消隐入队时等待碰撞的毫秒数
        ELIMINATE_ENQUEUE_DELAY_MICROSECONDS = 10,
    };

    typedef typename StampedPtr<void>::stamp_type stamp_type;

    // 节点
    struct Node
    {
        T data;
        stamp_type seg = 0; // 用于安全消隐的标记
        std::atomic<StampedPtr<Node>> prev = ATOMIC_VAR_INIT(StampedPtr<Node>());
        std::atomic<StampedPtr<Node>> next = ATOMIC_VAR_INIT(StampedPtr<Node>());

        Node(const T& v)
            : data(v)
        {}
    };

    // 尝试出队的结果
    enum class DequeueAttemptResult
    {
        DequeueSuccess,    // 成功
        ConcurrentFailure, // 并发失败
        EmptyQueueFailure, // 空队列
    };

    std::atomic<StampedPtr<Node>> _head = ATOMIC_VAR_INIT(StampedPtr<Node>());
    std::atomic<StampedPtr<Node>> _tail = ATOMIC_VAR_INIT(StampedPtr<Node>());

    // 用于消隐的碰撞数组
    std::atomic<StampedPtr<Node>> *_collisions = nullptr;;

private:
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

public:
    ConcurrentQueue()
    {
        Node *dummy = (Node*) ::malloc(sizeof(Node));
        dummy->next.store({nullptr, 0}, std::memory_order_relaxed);
        dummy->prev.store({nullptr, 0}, std::memory_order_relaxed);
        _head.store({dummy, 0}, std::memory_order_relaxed);
        _tail.store({dummy, 0}, std::memory_order_relaxed);

        _collisions = (std::atomic<StampedPtr<Node>>*) ::malloc(
            sizeof(std::atomic<StampedPtr<Node>>) * COLLISIONS_ARRAY_SIZE);
        StampedPtr<Node> init_value;
        for (int i = 0; i < COLLISIONS_ARRAY_SIZE; ++i)
            new (_collisions + i) std::atomic<StampedPtr<Node>>(init_value);
    }

    ~ConcurrentQueue()
    {
        // Clear elements
        while (optimistic_dequeue(nullptr))
        {}
        assert(is_empty());

        assert(nullptr != _head.load(std::memory_order_relaxed).ptr);
        ::free(_head.load(std::memory_order_relaxed).ptr);

        for (int i = 0; i < COLLISIONS_ARRAY_SIZE; ++i)
            (_collisions + i)->~atomic();
        ::free(_collisions);
        _collisions = nullptr;
    }

    bool is_empty() const
    {
        return _head.load(std::memory_order_relaxed) == _tail.load(std::memory_order_relaxed);
    }

public:
    /**
     * Optimistic算法入队
     *
     *   在满负荷并发的情况下，为了能提高吞吐量，即要减少两次成功的CAS操作之间的
     *   时间间隔，则等同于要减少(第1步)获取旧值与(第4步)CAS操作之间的耗时，因为
     *   成功操作的(第1步)获取旧值必定紧接着上次成功的CAS操作。
     *
     *   (第2步)基于旧值的操作与(第5步)收尾操作之间的操作之间的不同点在于：由于
     *   尚未插入到队列中，(第2步)无须考虑并发操作的影响，其结果是可靠的；但是(
     *   第5步)则要考虑操作延时对其他的并发操作的影响，所以有fixList()操作。
     */
    void optimistic_enqueue(const T& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(v);

        // 获取旧值
        StampedPtr<Node> old_tail = _tail.load(std::memory_order_relaxed);
        while (true)
        {
            // 基于旧值的操作
            new_node->next.store({old_tail.ptr, old_tail.stamp + 1},
                                 std::memory_order_relaxed);

            // 尝试CAS，失败则自动获取旧值到 old_tail
            if (_tail.compare_exchange_weak(
                    old_tail, {new_node, old_tail.stamp + 1},
                    std::memory_order_release, std::memory_order_relaxed))
            {
                // 收尾操作
                old_tail.ptr->prev.store({new_node, old_tail.stamp},
                                         std::memory_order_relaxed);
                break;
            }
        }
    }

    /**
     * Optimistic算法出队
     */
    bool optimistic_dequeue(T *p)
    {
        uint8_t tmp[sizeof(T)];

        while (true)
        {
            // 保留旧值
            // NOTE 先取 head, 然后取 tail 和其他，再验证 head 是否改变，以保证
            //      取到的值是可靠的
            StampedPtr<Node> old_head = _head.load(std::memory_order_relaxed);
            const StampedPtr<Node> old_tail = _tail.load(std::memory_order_relaxed);
            const StampedPtr<Node> first_node_prev = old_head.ptr->prev.load(std::memory_order_relaxed);

            if (old_head == _head.load(std::memory_order_relaxed))
            {
                // 队列为空
                if (old_tail == old_head)
                    return false;

                // 需要修正
                if (first_node_prev.stamp != old_head.stamp)
                {
                    fix_list(old_tail, old_head);
                    continue;
                }

                // 基于旧值的操作
                ::memcpy(tmp, &(first_node_prev.ptr->data), sizeof(T));

                // 尝试CAS操作
                if (_head.compare_exchange_weak(
                        old_head, {first_node_prev.ptr, old_head.stamp + 1},
                        std::memory_order_release, std::memory_order_relaxed))
                {
                    ::free(old_head.ptr);
                    if (nullptr != p)
                        *p = *reinterpret_cast<T*>(tmp);
                    ((T*) tmp)->~T();
                    return true;
                }
            }
        }
    }

private:
    // 修复
    void fix_list(const StampedPtr<Node>& tail, const StampedPtr<Node>& head)
    {
        StampedPtr<Node> cur_node = tail;
        while ((head == _head.load(std::memory_order_relaxed)) && (cur_node != head))
        {
            StampedPtr<Node> cur_node_next = cur_node.ptr->next.load(std::memory_order_relaxed);
            cur_node_next.ptr->prev.store({cur_node.ptr, cur_node.stamp - 1},
                                          std::memory_order_relaxed);
            cur_node.set(cur_node_next.ptr, cur_node.stamp - 1);
        }
    }

public:
    // 采用消隐策略的入队
    void eliminate_enqueue(const T& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(v);

        const stamp_type seen_tail = _tail.load(std::memory_order_relaxed).stamp;
        while (true)
        {
            if (enqueue_attempt(new_node))
                return;
            if (seen_tail <= _head.load(std::memory_order_relaxed).stamp &&
                try_to_eliminate_enqueue(new_node, seen_tail))
                return;
        }
    }

    // 采用消隐策略的出队
    bool eliminate_dequeue(T *p)
    {
        while (true)
        {
            const DequeueAttemptResult rs = dequeue_attempt(p);
            if (DequeueAttemptResult::EmptyQueueFailure == rs)
                return false;
            else if (DequeueAttemptResult::DequeueSuccess == rs ||
                     try_to_eliminate_dequeue(p))
                return true;
        }
    }

private:
    // 尝试入队
    bool enqueue_attempt(Node *new_node)
    {
        StampedPtr<Node> old_tail = _tail.load(std::memory_order_relaxed);
        new_node->next.store({old_tail.ptr, old_tail.stamp + 1},
                             std::memory_order_relaxed);

        if (_tail.compare_exchange_weak(
                old_tail, {new_node, old_tail.stamp + 1},
                std::memory_order_release, std::memory_order_relaxed))
        {
            old_tail.ptr->prev.store({new_node, old_tail.stamp},
                                     std::memory_order_relaxed);
            return true;
        }
        return false;
    }

    // 尝试出队
    DequeueAttemptResult dequeue_attempt(T *p)
    {
        uint8_t tmp[sizeof(T)];

        while (true)
        {
            // 保留旧值
            StampedPtr<Node> old_head = _head.load(std::memory_order_relaxed);
            const StampedPtr<Node> old_tail = _tail.load(std::memory_order_relaxed);
            const StampedPtr<Node> first_node_prev = old_head.ptr->prev.load(std::memory_order_relaxed);

            if (old_head == _head.load(std::memory_order_relaxed))
            {
                // 队列为空
                if (old_tail == old_head)
                    return DequeueAttemptResult::EmptyQueueFailure;

                // 需要修正
                if (first_node_prev.stamp != old_head.stamp)
                {
                    fix_list(old_tail, old_head);
                    continue;
                }

                // 基于旧值的操作
                ::memcpy(tmp, &(first_node_prev.ptr->data), sizeof(T));

                // 尝试CAS操作
                if (_head.compare_exchange_weak(
                        old_head, {first_node_prev.ptr, old_head.stamp + 1},
                        std::memory_order_release, std::memory_order_relaxed))
                {
                    ::free(old_head.ptr);
                    if (nullptr != p)
                        *p = *reinterpret_cast<T*>(tmp);
                    ((T*) tmp)->~T();
                    return DequeueAttemptResult::DequeueSuccess;
                }
                return DequeueAttemptResult::ConcurrentFailure;
            }
        }
    }

    bool try_to_eliminate_enqueue(Node *new_node, stamp_type seen_tail)
    {
        new_node->seg = seen_tail;
        const unsigned int i = ::rand() % COLLISIONS_ARRAY_SIZE;
        StampedPtr<Node> old_collision_to_add = _collisions[i].load(std::memory_order_relaxed);
        if (COLLISION_EMPTY_PTR != old_collision_to_add.ptr)
            return false;

        // 添加到碰撞数组
        if (!_collisions[i].compare_exchange_weak(
                old_collision_to_add, {new_node, old_collision_to_add.stamp + 1},
                std::memory_order_release, std::memory_order_relaxed))
            return false;

        // 等待一段时间
        std::this_thread::sleep_for(
            std::chrono::milliseconds(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS));

        // 检查是否消隐成功
        StampedPtr<Node> old_collision_to_remove = _collisions[i].load(std::memory_order_relaxed);
        if (COLLISION_DONE_PTR == old_collision_to_remove.ptr ||
            !_collisions[i].compare_exchange_weak(
                old_collision_to_remove, {COLLISION_EMPTY_PTR, old_collision_to_add.stamp + 1},
                std::memory_order_release, std::memory_order_relaxed))
        {
            _collisions[i].store({COLLISION_EMPTY_PTR, old_collision_to_add.stamp + 1},
                                 std::memory_order_relaxed);
            return true;
        }

        return false;
    }

    bool try_to_eliminate_dequeue(T *p)
    {
        const stamp_type seen_head = _head.load(std::memory_order_relaxed).stamp;
        const unsigned int i = ::rand() % COLLISIONS_ARRAY_SIZE;
        StampedPtr<Node> old_collision = _collisions[i].load(std::memory_order_relaxed);
        if (COLLISION_EMPTY_PTR == old_collision.ptr ||
            COLLISION_DONE_PTR == old_collision.ptr)
            return false;

        if (old_collision.ptr->seg > seen_head)
            return false;

        if (_collisions[i].compare_exchange_weak(
                old_collision, {COLLISION_DONE_PTR, old_collision.stamp},
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
};

}

#undef COLLISION_EMPTY_PTR
#undef COLLISION_DONE_PTR

#endif
