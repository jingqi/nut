
#ifndef ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_
#define ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_

#include <assert.h>
#include <string.h> // for memcpy()

#define _ENABLE_ATOMIC_ALIGNMENT_FIX // VS2015 SP2 BUG
#include <atomic>
#include <thread>
#include <random>

#include "stamped_ptr.h"
#include "hazard_pointer/hp_record.h"
#include "hazard_pointer/hp_retire_list.h"
#include "../threading.h" // for NUT_THREAD_LOCAL

// 隐消数组的指针常量
#define COLLISION_EMPTY_PTR nullptr
#define COLLISION_DONE_PTR (reinterpret_cast<Node*>(-1))

namespace nut
{

/**
 * 无锁并发队列
 *
 * 从队尾入队，从队头出队：
 *                           dequeue
 *                              ↑
 *      prev <-+-------+        |
 * head    -   | dummy |   -    < .............. <  tail
 *             +-------+-> next                  ↑
 *                                               |
 *                                            enqueue
 *
 * 关键字：
 *   CAS(Compare-And-Swap)
 *   MS-queue(Michael and Scott)算法
 *   Dominique Fober算法
 *   optimistic算法
 *   隐消(shavit and Touitou)
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
private:
    enum
    {
        // 隐消使用的碰撞数组的大小
        COLLISIONS_ARRAY_SIZE = 5,

        // 隐消入队时等待碰撞的毫秒数
        ELIMINATE_ENQUEUE_DELAY_MICROSECONDS = 10,
    };

    typedef typename StampedPtr<void>::stamp_type stamp_type;

    // 节点
    class Node
    {
    public:
        typedef typename std::conditional<
            std::is_trivially_copyable<T>::value, T, T*>::type data_store_type;

    public:
        template <typename U>
        explicit Node(U&& v,
                      typename std::enable_if<std::is_trivially_copyable<U>::value,int>::type _ = 0)
            : data(std::forward<T>(v))
        {}

        template <typename U>
        explicit Node(const U& v,
                      typename std::enable_if<std::is_trivially_copyable<U>::value,int>::type _ = 0)
            : data(v)
        {}

        template <typename U>
        explicit Node(U&& v,
                      typename std::enable_if<!std::is_trivially_copyable<U>::value,int>::type _ = 0)
            : data(nullptr)
        {
            data = (T*) ::malloc(sizeof(T));
            new (data) T(std::forward<T>(v));
        }

        template <typename U>
        explicit Node(const U& v,
                      typename std::enable_if<!std::is_trivially_copyable<U>::value,int>::type _ = 0)
            : data(nullptr)
        {
            data = (T*) ::malloc(sizeof(T));
            new (data) T(v);
        }

        ~Node()
        {
            delete_data<T>();
        }

        /**
         * 构造 dummy 节点，初始化除了 data 以外的其他字段
         *
         */
        void construct_dummy()
        {
            seg = 0;

            // NOTE
            // - 初始有效 stamp 为 1，故 prev.stamp 置 1
            // - next.stamp 需要置 0, 使得 _head.stamp != next.stamp 以表明
            //   next.ptr 尚未指向新增节点
            new (&prev) AtomicStampedPtr<Node>(nullptr, 1);
            new (&next) AtomicStampedPtr<Node>(nullptr, 0);

            new (&retired) std::atomic<bool>(false);
        }

        // 析构 dummy 节点
        void destruct_dummy()
        {
            (&prev)->~AtomicStampedPtr();
            (&next)->~AtomicStampedPtr();
            (&retired)->~atomic();
        }

        static void delete_dummy(void *n)
        {
            assert(nullptr != n);
            ((Node*) n)->destruct_dummy();
            ::free(n);
        }

        template <typename U>
        static typename std::enable_if<std::is_trivially_copyable<U>::value, void>::type
        move_data(data_store_type *ds, T *p)
        {
            assert(nullptr != ds);
            if (nullptr != p)
                *p = *ds;
        }

        template <typename U>
        static typename std::enable_if<!std::is_trivially_copyable<U>::value, void>::type
        move_data(data_store_type *ds, T *p)
        {
            assert(nullptr != ds);
            if (nullptr != p)
                *p = std::move(**ds);
        }

        template <typename U>
        static typename std::enable_if<std::is_trivially_copyable<U>::value, void>::type
        move_and_destroy_data(data_store_type *ds, T *p)
        {
            assert(nullptr != ds);
            if (nullptr != p)
                *p = *ds;
            ds->~T();
        }

        template <typename U>
        static typename std::enable_if<!std::is_trivially_copyable<U>::value, void>::type
        move_and_destroy_data(data_store_type *ds, T *p)
        {
            assert(nullptr != ds);
            if (nullptr != p)
                *p = std::move(**ds);
            (*ds)->~T();
            ::free(*ds);
        }

    private:
        template <typename U>
        typename std::enable_if<std::is_trivially_copyable<U>::value, void>::type
        delete_data()
        {}

        template <typename U>
        typename std::enable_if<!std::is_trivially_copyable<U>::value, void>::type
        delete_data()
        {
            data->~T();
            ::free(data);
        }

    public:
        AtomicStampedPtr<Node> prev;
        AtomicStampedPtr<Node> next;

        data_store_type data;
        stamp_type seg = 0; // 用于安全隐消的标记
        std::atomic<bool> retired = ATOMIC_VAR_INIT(false);
    };

    // 尝试出队的结果
    enum class DequeueAttemptResult
    {
        DequeueSuccess,    // 成功
        ConcurrentFailure, // 并发失败
        EmptyQueueFailure, // 空队列
    };

public:
    ConcurrentQueue()
    {
        Node *dummy = (Node*) ::malloc(sizeof(Node));
        dummy->construct_dummy();

        // NOTE 初始 stamp 从 1 开始，参见 construct_dummy()
        _head.store({dummy, 1}, std::memory_order_relaxed);
        _tail.store({dummy, 1}, std::memory_order_relaxed);

        StampedPtr<Node> init_value(COLLISION_EMPTY_PTR, 0);
        for (int i = 0; i < COLLISIONS_ARRAY_SIZE; ++i)
            _collisions[i].store(init_value, std::memory_order_relaxed);
    }

    ~ConcurrentQueue()
    {
        // Clear elements
        Node *const dummy = _head.load(std::memory_order_acquire).ptr;
        Node *p = dummy->next.load(std::memory_order_relaxed).ptr;
        while (nullptr != p)
        {
            Node *next = p->next.load(std::memory_order_relaxed).ptr;
            p->~Node();
            ::free(p);
            p = next;
        }

        assert(nullptr != dummy);
        dummy->destruct_dummy();
        ::free(dummy);
    }

    /**
     * NOTE 由于有两次取值，并发状态下计算的结果可能偏大
     */
    size_t size() const
    {
        const stamp_type head_stamp = _head.load(std::memory_order_relaxed).stamp;
        return _tail.load(std::memory_order_relaxed).stamp - head_stamp;
    }

    bool is_empty() const
    {
        return _head.load(std::memory_order_relaxed) == _tail.load(std::memory_order_relaxed);
    }

    /**
     * 乐观算法入队
     */
    template <typename ...Args>
    void optimistic_emplace(Args&& ...args)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (new_node) Node(std::forward<Args>(args)...);
        optimistic_enqueue(new_node);
    }

    void optimistic_enqueue(T&& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (new_node) Node(std::forward<T>(v));
        optimistic_enqueue(new_node);
    }

    void optimistic_enqueue(const T& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (new_node) Node(v);
        optimistic_enqueue(new_node);
    }

    /**
     * 乐观算法出队
     */
    bool optimistic_dequeue(T *p)
    {
        uint8_t tmp[sizeof(typename Node::data_store_type)];

        while (true)
        {
            // 保留旧值
            // NOTE 先取 head, 然后取 tail 和其他，再验证 head 是否改变，以保证
            //      取到的值是一致的
            StampedPtr<Node> old_head = _head.load(std::memory_order_acquire);
            const StampedPtr<Node> old_tail = _tail.load(std::memory_order_relaxed);
            const StampedPtr<Node> first_node_next = old_head.ptr->next.load(std::memory_order_relaxed);

            if (old_head != _head.load(std::memory_order_relaxed))
                continue; // 上述取值过程被其他线程打断，重试

            // 队列为空
            if (old_head == old_tail)
                return false;

            // 需要修正
            if (first_node_next.stamp != old_head.stamp)
            {
                fix_list(old_head, old_tail);
                continue;
            }

            // 基于旧值的操作
            ::memcpy(tmp, &(first_node_next.ptr->data), sizeof(typename Node::data_store_type));

            // 尝试CAS操作
            if (_head.compare_exchange_weak(
                    &old_head, {first_node_next.ptr, old_head.stamp + 1},
                    std::memory_order_relaxed, std::memory_order_relaxed))
            {
                // NOTE first_node_next.ptr 成为新的 dummy 节点
                old_head.ptr->retired.store(true, std::memory_order_relaxed);
                HPRetireList::retire_any(Node::delete_dummy, old_head.ptr);
                Node::template move_and_destroy_data<T>((typename Node::data_store_type*) tmp, p);
                return true;
            }
        }
    }

    /**
     * 采用隐消策略的入队
     */
    template <typename ...Args>
    void eliminate_emplace(Args ...args)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (new_node) Node(std::forward<Args>(args)...);
        eliminate_enqueue(new_node);
    }

    void eliminate_enqueue(T&& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (new_node) Node(std::forward<T>(v));
        eliminate_enqueue(new_node);
    }

    void eliminate_enqueue(const T& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (new_node) Node(v);
        eliminate_enqueue(new_node);
    }

    /**
     * 采用隐消策略的出队
     */
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
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;
    
    void optimistic_enqueue(Node *new_node)
    {
        assert(nullptr != new_node);

        // 获取旧值
        StampedPtr<Node> old_tail = _tail.load(std::memory_order_relaxed);
        while (true)
        {
            // 基于旧值的操作
            new_node->prev.store({old_tail.ptr, old_tail.stamp + 1},
                                 std::memory_order_relaxed);

            // 尝试CAS，失败则自动获取旧值到 old_tail
            if (_tail.compare_exchange_weak(
                    &old_tail, {new_node, old_tail.stamp + 1},
                    std::memory_order_release, std::memory_order_relaxed))
            {
                HPGuard guard_item; // Hold 'old_tail' if not deleted
                if (old_tail.ptr->retired.load(std::memory_order_relaxed))
                    break; // 'old_tail' already deleted by some other thread

                // 收尾操作
                // NOTE 这个操作可能不能即时完成，其他线程如果要删除本节点时，会
                //      通过 fix_list() 来修复
                old_tail.ptr->next.store({new_node, old_tail.stamp},
                                         std::memory_order_relaxed);
                break;
            }
        }
    }
    
    void eliminate_enqueue(Node *new_node)
    {
        assert(nullptr != new_node);

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

    // 修复
    void fix_list(const StampedPtr<Node>& head, const StampedPtr<Node>& tail)
    {
        StampedPtr<Node> cur_node = tail;
        while ((head == _head.load(std::memory_order_relaxed)) && (cur_node != head))
        {
            StampedPtr<Node> cur_node_prev = cur_node.ptr->prev.load(std::memory_order_relaxed);
            cur_node_prev.ptr->next.store({cur_node.ptr, cur_node.stamp - 1},
                                          std::memory_order_relaxed);
            cur_node.set(cur_node_prev.ptr, cur_node.stamp - 1);
        }
    }

    // 尝试入队
    bool enqueue_attempt(Node *new_node)
    {
        StampedPtr<Node> old_tail = _tail.load(std::memory_order_relaxed);
        new_node->prev.store({old_tail.ptr, old_tail.stamp + 1},
                             std::memory_order_relaxed);

        if (_tail.compare_exchange_weak(
                &old_tail, {new_node, old_tail.stamp + 1},
                std::memory_order_release, std::memory_order_relaxed))
        {
            HPGuard guard_item; // Hold 'old_tail' if not deleted
            if (old_tail.ptr->retired.load(std::memory_order_relaxed))
                return true; // 'old_tail' already deleted by some other thread

            // NOTE 这个操作可能不能即时完成，其他线程如果要删除本节点时，会通过
            //      fix_list() 来修复
            old_tail.ptr->next.store({new_node, old_tail.stamp},
                                     std::memory_order_relaxed);
            return true;
        }
        return false;
    }

    // 尝试出队
    DequeueAttemptResult dequeue_attempt(T *p)
    {
        uint8_t tmp[sizeof(typename Node::data_store_type)];

        while (true)
        {
            // 保留旧值
            StampedPtr<Node> old_head = _head.load(std::memory_order_acquire);
            const StampedPtr<Node> old_tail = _tail.load(std::memory_order_relaxed);
            const StampedPtr<Node> first_node_next = old_head.ptr->next.load(std::memory_order_relaxed);

            if (old_head != _head.load(std::memory_order_relaxed))
                continue; // 上述取值过程被其他线程打断，重试

            // 队列为空
            if (old_head == old_tail)
                return DequeueAttemptResult::EmptyQueueFailure;

            // 需要修正
            if (first_node_next.stamp != old_head.stamp)
            {
                fix_list(old_head, old_tail);
                continue;
            }

            // 基于旧值的操作
            ::memcpy(tmp, &(first_node_next.ptr->data), sizeof(typename Node::data_store_type));

            // 尝试CAS操作
            if (_head.compare_exchange_weak(
                    &old_head, {first_node_next.ptr, old_head.stamp + 1},
                    std::memory_order_relaxed, std::memory_order_relaxed))
            {
                // NOTE first_node_next.ptr 成为新的 dummy 节点
                old_head.ptr->retired.store(true, std::memory_order_relaxed);
                HPRetireList::retire_any(Node::delete_dummy, old_head.ptr);
                Node::template move_and_destroy_data<T>((typename Node::data_store_type*) tmp, p);
                return DequeueAttemptResult::DequeueSuccess;
            }
            return DequeueAttemptResult::ConcurrentFailure;
        }
    }

    bool try_to_eliminate_enqueue(Node *new_node, stamp_type seen_tail)
    {
        const unsigned r = rand_pos();
        StampedPtr<Node> old_collision_to_add = _collisions[r].load(std::memory_order_relaxed);
        if (COLLISION_EMPTY_PTR != old_collision_to_add.ptr)
            return false;

        // 添加到碰撞数组
        new_node->seg = seen_tail;
        if (!_collisions[r].compare_exchange_weak(
                &old_collision_to_add, {new_node, old_collision_to_add.stamp + 1},
                std::memory_order_release, std::memory_order_relaxed))
            return false;

        // 等待一段时间
        std::this_thread::sleep_for(
            std::chrono::milliseconds(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS));

        // 检查是否隐消成功
        StampedPtr<Node> old_collision_to_remove = _collisions[r].load(std::memory_order_relaxed);
        if (COLLISION_DONE_PTR == old_collision_to_remove.ptr ||
            !_collisions[r].compare_exchange_weak(
                &old_collision_to_remove, {COLLISION_EMPTY_PTR, old_collision_to_add.stamp + 1},
                std::memory_order_relaxed, std::memory_order_relaxed))
        {
            _collisions[r].store({COLLISION_EMPTY_PTR, old_collision_to_add.stamp + 1},
                                 std::memory_order_relaxed);
            return true;
        }

        return false;
    }

    bool try_to_eliminate_dequeue(T *p)
    {
        const unsigned r = rand_pos();
        StampedPtr<Node> old_collision = _collisions[r].load(std::memory_order_acquire);
        if (COLLISION_EMPTY_PTR == old_collision.ptr ||
            COLLISION_DONE_PTR == old_collision.ptr)
            return false;

        const stamp_type seen_head = _head.load(std::memory_order_relaxed).stamp;
        if (old_collision.ptr->seg > seen_head)
            return false;

        if (_collisions[r].compare_exchange_weak(
                &old_collision, {COLLISION_DONE_PTR, old_collision.stamp},
                std::memory_order_relaxed, std::memory_order_relaxed))
        {
            Node::template move_data<T>(&(old_collision.ptr->data), p);
            old_collision.ptr->~Node();
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
    AtomicStampedPtr<Node> _head;
    AtomicStampedPtr<Node> _tail;

    // 用于隐消的碰撞数组
    AtomicStampedPtr<Node> _collisions[COLLISIONS_ARRAY_SIZE];
};

}

#undef COLLISION_EMPTY_PTR
#undef COLLISION_DONE_PTR

#endif
