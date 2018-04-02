﻿
#ifndef ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_
#define ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_

#include <nut/platform/platform.h>

#include <assert.h>
#include <string.h> // for memcpy()
#include <stdlib.h> // for rand()

#if NUT_PLATFORM_CC_VC
#   include <allocators>
#endif

#include <nut/debugging/source_location.h>
#include <nut/debugging/exception.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "stamped_ptr.h"


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
template <typename T, typename AllocT = std::allocator<T> >
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

    typedef typename stamped_ptr<void>::stamp_type stamp_type;

    // 节点
    struct Node
    {
        T data;
        stamp_type seg = 0; // 用于安全消隐的标记
        stamped_ptr<Node> prev;
        stamped_ptr<Node> next;

        Node(const T& v)
            : data(v)
        {}
    };

    // 尝试出队的结果
    enum class DequeueAttemptResult
    {
        DEQUEUE_SUCCESS, // 成功
        CONCURRENT_FAILURE, // 并发失败
        EMPTY_QUEUE_FAILURE, // 空队列
    };

    typedef AllocT                                        data_allocator_type;
    typedef typename AllocT::template rebind<Node>::other node_allocator_type;

    data_allocator_type _data_alloc;
    node_allocator_type _node_alloc;
    stamped_ptr<Node> _head;
    stamped_ptr<Node> _tail;

    // 用于消隐的碰撞数组
    stamped_ptr<Node> _collisions[COLLISIONS_ARRAY_SIZE];
    
private:
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

public:
    ConcurrentQueue()
    {
        Node *dummy = _node_alloc.allocate(1);
        dummy->next.clear();
        dummy->prev.clear();
        _head.set_pointer(dummy);
        _tail.set_pointer(dummy);
    }

    ~ConcurrentQueue()
    {
        // Clear elements
        while (optimistic_dequeue(nullptr))
        {}
        assert(_head == _tail && nullptr != _head.pointer());
        _node_alloc.deallocate(_head.pointer(), 1);
    }

    bool is_empty() const
    {
        return _head == _tail;
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
        Node *new_node = _node_alloc.allocate(1);
        _data_alloc.construct(&(new_node->data), v);

        while (true)
        {
            // 获取旧值
            const stamped_ptr<Node> old_tail(_tail);

            // 基于旧值的操作
            new_node->next.set(old_tail.pointer(), old_tail.stamp_value() + 1);

            // 尝试CAS
            if (_tail.compare_and_set(old_tail, new_node))
            {
                // 收尾操作
                old_tail.pointer()->prev.set(new_node, old_tail.stamp_value());
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
            const stamped_ptr<Node> old_head(_head);
            const stamped_ptr<Node> old_tail(_tail);
            const stamped_ptr<Node> first_node_prev(old_head.pointer()->prev);

            if (old_head == _head) // 先取head, 然后取tail和其他，再验证head是否改变，以保证取到的值是可靠的
            {
                // 队列为空
                if (old_tail == old_head)
                    return false;

                // 需要修正
                if (first_node_prev.stamp_value() != old_head.stamp_value())
                {
                    fix_list(old_tail, old_head);
                    continue;
                }

                // 基于旧值的操作
                ::memcpy(tmp, &(first_node_prev.pointer()->data), sizeof(T));

                // 尝试CAS操作
                if (_head.compare_and_set(old_head, first_node_prev.pointer()))
                {
                    _node_alloc.deallocate(old_head.pointer(), 1);
                    if (nullptr != p)
                        *p = *reinterpret_cast<T*>(tmp);
                    _data_alloc.destroy(reinterpret_cast<T*>(tmp));
                    return true;
                }
            }
        }
    }

private:
    // 修复
    void fix_list(const stamped_ptr<Node>& tail, const stamped_ptr<Node>& head)
    {
        stamped_ptr<Node> cur_node = tail;
        while ((head == _head) && (cur_node != head))
        {
            stamped_ptr<Node> cur_node_next(cur_node.pointer()->next);
            cur_node_next.pointer()->prev.set(cur_node.pointer(), cur_node.stamp_value() - 1);
            cur_node.set(cur_node_next.pointer(), cur_node.stamp_value() - 1);
        }
    }

public:
    // 采用消隐策略的入队
    void eliminate_enqueue(const T& v)
    {
        Node *new_node = _node_alloc.allocate(1);
        _data_alloc.construct(&(new_node->data), v);

        const stamp_type seen_tail = _tail.stamp_value();
        while (true)
        {
            if (enqueue_attempt(new_node))
                return;
            if (seen_tail <= _head.stamp_value() &&
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
            if (DequeueAttemptResult::EMPTY_QUEUE_FAILURE == rs)
                return false;
            else if (DequeueAttemptResult::DEQUEUE_SUCCESS == rs || try_to_eliminate_dequeue(p))
                return true;
        }
    }

private:
    // 尝试入队
    bool enqueue_attempt(Node *new_node)
    {
        const stamped_ptr<Node> old_tail(_tail);
        new_node->next.set(old_tail.pointer(), old_tail.stamp_value() + 1);

        if (_tail.compare_and_set(old_tail, new_node))
        {
            old_tail.pointer()->prev.set(new_node, old_tail.stamp_value());
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
            const stamped_ptr<Node> old_head(_head);
            const stamped_ptr<Node> old_tail(_tail);
            const stamped_ptr<Node> first_node_prev(old_head.pointer()->prev);

            if (old_head == _head)
            {
                // 队列为空
                if (old_tail == old_head)
                    return DequeueAttemptResult::EMPTY_QUEUE_FAILURE;

                // 需要修正
                if (first_node_prev.stamp_value() != old_head.stamp_value())
                {
                    fix_list(old_tail, old_head);
                    continue;
                }

                // 基于旧值的操作
                ::memcpy(tmp, &(first_node_prev.pointer()->data), sizeof(T));

                // 尝试CAS操作
                if (_head.compare_and_set(old_head, first_node_prev.pointer()))
                {
                    _node_alloc.deallocate(old_head.pointer(), 1);
                    if (nullptr != p)
                        *p = *reinterpret_cast<T*>(tmp);
                    _data_alloc.destroy(reinterpret_cast<T*>(tmp));
                    return DequeueAttemptResult::DEQUEUE_SUCCESS;
                }
                return DequeueAttemptResult::CONCURRENT_FAILURE;
            }
        }
    }

    bool try_to_eliminate_enqueue(Node *new_node, stamp_type seen_tail)
    {
        new_node->seg = seen_tail;
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        const stamped_ptr<Node> old_collision_to_add(_collisions[i]);
        if (COLLISION_EMPTY_PTR != old_collision_to_add.pointer())
            return false;

        // 添加到碰撞数组
        if (!_collisions[i].compare_and_set(old_collision_to_add, new_node))
            return false;

        // 等待一段时间
#if NUT_PLATFORM_OS_WINDOWS
        ::Sleep(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS);
#elif NUT_PLATFORM_OS_LINUX
        ::usleep(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS * 1000);
#endif

        // 检查是否消隐成功
        const stamped_ptr<Node> old_collision_to_remove(_collisions[i]);
        if (COLLISION_DONE_PTR == old_collision_to_remove.pointer() ||
            !_collisions[i].compare_and_set(old_collision_to_remove,
                                            COLLISION_EMPTY_PTR,
                                            old_collision_to_add.stamp_value() + 1))
        {
            _collisions[i].set(COLLISION_EMPTY_PTR, old_collision_to_add.stamp_value() + 1);
            return true;
        }

        return false;
    }

    bool try_to_eliminate_dequeue(T *p)
    {
        const stamp_type seen_head = _head.stamp_value();
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        const stamped_ptr<Node> old_collision(_collisions[i]);
        if (COLLISION_EMPTY_PTR == old_collision.pointer() ||
            COLLISION_DONE_PTR == old_collision.pointer())
            return false;

        if (old_collision.pointer()->seg > seen_head)
            return false;

        if (_collisions[i].compare_and_set(old_collision,
                                           COLLISION_DONE_PTR,
                                           old_collision.stamp_value()))
        {
            if (nullptr != p)
                *p = old_collision.pointer()->data;
            _data_alloc.destroy(&(old_collision.pointer()->data));
            _node_alloc.deallocate(old_collision.pointer(), 1);
            return true;
        }
        return false;
    }
};

}

#undef COLLISION_EMPTY_PTR
#undef COLLISION_DONE_PTR

#endif
