
#ifndef ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_
#define ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_

#include <assert.h>
#include <string.h> // for memcpy()
#include <stdlib.h> // for rand()

#include <atomic>
#include <thread>


// 消隐数组的指针常量
#define COLLISION_EMPTY_PTR nullptr
#define COLLISION_DONE_PTR reinterpret_cast<Node*>(-1)

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
 *   [1]钱立兵，陈波等. 多线程并发访问无锁队列的算法研究[J]. 先进技术研究通报，2009-8，3(8). 50-55
 *   [2]Danny Hendler, Nir Shavit, Lena Yerushalmi. A Scalable Lock-free Stack Algorithm[J]. SPAA. 2004-06-27. 206-215
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

    typedef int stamp_type;

    struct Node;

    struct StampedPtr
    {
        Node *ptr;
        stamp_type stamp;

        StampedPtr()
            : ptr(nullptr), stamp(0)
        {}

        StampedPtr(Node *p, stamp_type s)
            : ptr(p), stamp(s)
        {}

        bool operator==(const StampedPtr& x) const
        {
            return ptr == x.ptr && stamp == x.stamp;
        }

        bool operator!=(const StampedPtr& x) const
        {
            return !(*this == x);
        }
    };

    // 节点
    struct Node
    {
        T data;
        stamp_type seg = 0; // 用于安全消隐的标记
        std::atomic<StampedPtr> prev = ATOMIC_VAR_INIT(StampedPtr());
        std::atomic<StampedPtr> next = ATOMIC_VAR_INIT(StampedPtr());

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

    std::atomic<StampedPtr> _head = ATOMIC_VAR_INIT(StampedPtr());
    std::atomic<StampedPtr> _tail = ATOMIC_VAR_INIT(StampedPtr());

    // 用于消隐的碰撞数组
    std::atomic<StampedPtr> *_collisions = nullptr;;

private:
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

public:
    ConcurrentQueue()
    {
        Node *dummy = (Node*) ::malloc(sizeof(Node));
        dummy->next = {nullptr, 0};
        dummy->prev = {nullptr, 0};
        _head = {dummy, 0};
        _tail = {dummy, 0};

        _collisions = (std::atomic<StampedPtr>*) ::malloc(sizeof(std::atomic<StampedPtr>) * COLLISIONS_ARRAY_SIZE);
        StampedPtr init_value;
        for (int i = 0; i < COLLISIONS_ARRAY_SIZE; ++i)
            new (_collisions + i) std::atomic<StampedPtr>(init_value);
    }

    ~ConcurrentQueue()
    {
        // Clear elements
        while (optimistic_dequeue(nullptr))
        {}
        assert(_head.load() == _tail.load() && nullptr != _head.load().ptr);
        ::free(_head.load().ptr);

        for (int i = 0; i < COLLISIONS_ARRAY_SIZE; ++i)
            (_collisions + i)->~atomic();
        ::free(_collisions);
        _collisions = nullptr;
    }

    bool is_empty() const
    {
        return _head.load() == _tail.load();
    }

public:
    /**
     * Optimistic算法入队
     *
     *    在满负荷并发的情况下，为了能提高吞吐量，即要减少两次成功的CAS操作之间的时间间隔，
     *    则等同于要减少(第1步)获取旧值与(第4步)CAS操作之间的耗时，因为成功操作的(第1步)获
     *    取旧值必定紧接着上次成功的CAS操作。
     *
     *    (第2步)基于旧值的操作与(第5步)收尾操作之间的操作之间的不同点在于：由于尚未插入到
     *    队列中，(第2步)无须考虑并发操作的影响，其结果是可靠的；但是(第5步)则要考虑操作延
     *    时对其他的并发操作的影响，所以有fixList()操作。
     */
    void optimistic_enqueue(const T& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(v);

        while (true)
        {
            // 获取旧值
            StampedPtr old_tail = _tail;

            // 基于旧值的操作
            new_node->next = {old_tail.ptr, old_tail.stamp + 1};

            // 尝试CAS
            if (_tail.compare_exchange_weak(old_tail, {new_node, old_tail.stamp + 1}))
            {
                // 收尾操作
                old_tail.ptr->prev = {new_node, old_tail.stamp};
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
            StampedPtr old_head = _head;
            const StampedPtr old_tail = _tail;
            const StampedPtr first_node_prev = old_head.ptr->prev;

            if (old_head == _head) // 先取head, 然后取tail和其他，再验证head是否改变，以保证取到的值是可靠的
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
                if (_head.compare_exchange_weak(old_head, {first_node_prev.ptr, old_head.stamp + 1}))
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
    void fix_list(const StampedPtr& tail, const StampedPtr& head)
    {
        StampedPtr cur_node = tail;
        while ((head == _head) && (cur_node != head))
        {
            StampedPtr cur_node_next = cur_node.ptr->next;
            cur_node_next.ptr->prev = {cur_node.ptr, cur_node.stamp - 1};
            cur_node = {cur_node_next.ptr, cur_node.stamp - 1};
        }
    }

public:
    // 采用消隐策略的入队
    void eliminate_enqueue(const T& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(v);

        const stamp_type seen_tail = _tail.load().stamp;
        while (true)
        {
            if (enqueue_attempt(new_node))
                return;
            if (seen_tail <= _head.load().stamp &&
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
            else if (DequeueAttemptResult::DequeueSuccess == rs || try_to_eliminate_dequeue(p))
                return true;
        }
    }

private:
    // 尝试入队
    bool enqueue_attempt(Node *new_node)
    {
        StampedPtr old_tail = _tail;
        new_node->next = {old_tail.ptr, old_tail.stamp + 1};

        if (_tail.compare_exchange_weak(old_tail, {new_node, old_tail.stamp + 1}))
        {
            old_tail.ptr->prev = {new_node, old_tail.stamp};
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
            StampedPtr old_head = _head;
            const StampedPtr old_tail = _tail;
            const StampedPtr first_node_prev = old_head.ptr->prev;

            if (old_head == _head)
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
                if (_head.compare_exchange_weak(old_head, {first_node_prev.ptr, old_head.stamp + 1}))
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
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        StampedPtr old_collision_to_add = _collisions[i];
        if (COLLISION_EMPTY_PTR != old_collision_to_add.ptr)
            return false;

        // 添加到碰撞数组
        if (!_collisions[i].compare_exchange_weak(
                old_collision_to_add, {new_node, old_collision_to_add.stamp + 1}))
            return false;

        // 等待一段时间
        std::this_thread::sleep_for(
            std::chrono::milliseconds(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS));

        // 检查是否消隐成功
        StampedPtr old_collision_to_remove = _collisions[i];
        if (COLLISION_DONE_PTR == old_collision_to_remove.ptr ||
            !_collisions[i].compare_exchange_weak(
                old_collision_to_remove, {COLLISION_EMPTY_PTR, old_collision_to_add.stamp + 1}))
        {
            _collisions[i] = {COLLISION_EMPTY_PTR, old_collision_to_add.stamp + 1};
            return true;
        }

        return false;
    }

    bool try_to_eliminate_dequeue(T *p)
    {
        const stamp_type seen_head = _head.load().stamp;
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        StampedPtr old_collision = _collisions[i];
        if (COLLISION_EMPTY_PTR == old_collision.ptr ||
            COLLISION_DONE_PTR == old_collision.ptr)
            return false;

        if (old_collision.ptr->seg > seen_head)
            return false;

        if (_collisions[i].compare_exchange_weak(
                old_collision,
                {COLLISION_DONE_PTR, old_collision.stamp}))
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
