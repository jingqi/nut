
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

#include "atomic.h"

namespace nut
{

/**
 * 无锁并发队列
 *
 * 参考资料和关键字：
 *    MS-queue(Michael and Scott)算法, Compare-and-swap(CAS)操作,
 *    Dominique Fober算法, optimistic算法, 消隐(shavit and Touitou)
 */
template <typename T, typename AllocT = std::allocator<T> >
class ConcurrentQueue
{
    /** 这里根据具体情况配置 */
    enum
    {
        /** 消隐使用的碰撞数组的大小 */
        COLLISIONS_ARRAY_SIZE = 5,

        /** 消隐入队时等待碰撞的毫秒数 */
        ELIMINATE_ENQUEUE_DELAY_MICROSECONDS = 10,
    };

    /** 节点 */
    struct Node
    {
        T data;
        unsigned int seg = 0; // 用于安全消隐的标记
        TagedPtr<Node> volatile prev;
        TagedPtr<Node> volatile next;

        Node(const T& v)
            : data(v)
        {}
    };

    /** 尝试出队的结果 */
    enum DequeueAttemptResult
    {
        DEQUEUE_SUCCESS /* 成功 */,
        CONCURRENT_FAILURE /* 并发失败 */,
        EMPTY_QUEUE_FAILURE /* 空队列 */
    };

    /** 消隐数组的指针常量 */
    enum { COLLISION_EMPTY_PTR = (int)NULL, COLLISION_DONE_PTR = -1 };

    typedef AllocT                                data_allocator_type;
    typedef typename AllocT::template rebind<Node>::other  node_allocator_type;

    data_allocator_type _data_alloc;
    node_allocator_type _node_alloc;
    TagedPtr<Node> volatile _head;
    TagedPtr<Node> volatile _tail;

    /** 用于消隐的碰撞数组 */
    TagedPtr<Node> volatile _collisions[COLLISIONS_ARRAY_SIZE];

public:
    ConcurrentQueue()
    {
        Node *dumy = _node_alloc.allocate(1);
        dumy->next.cas = 0;
        dumy->prev.cas = 0;
        _head.ptr = dumy;
        _tail.ptr = dumy;
    }

    ~ConcurrentQueue()
    {
        // clear elements
        while (optimistic_dequeue(NULL)) {}
        assert(_head.cas == _tail.cas && NULL != _head.ptr);
        _node_alloc.deallocate(_head.ptr, 1);
    }

    bool is_empty() const
    {
        return _head.cas == _tail.cas;
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
            const TagedPtr<Node> old_tail(_tail.cas);

            // 基于旧值的操作
            new_node->next.ptr = old_tail.ptr;
            new_node->next.tag = old_tail.tag + 1;

            // 构建新值
            const TagedPtr<Node> new_tail(new_node, old_tail.tag + 1);

            // 尝试CAS
            if (atomic_cas(&(_tail.cas), old_tail.cas, new_tail.cas))
            {
                // 收尾操作
                old_tail.ptr->prev.ptr = new_node;
                old_tail.ptr->prev.tag = old_tail.tag;
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
            const TagedPtr<Node> old_head(_head.cas);
            const TagedPtr<Node> old_tail(_tail.cas);
            const TagedPtr<Node> first_node_prev(old_head.ptr->prev.cas);

            if (old_head.cas == _head.cas) // 先取head, 然后取tail和其他，再验证head是否改变，以保证取到的值是可靠的
            {
                // 队列为空
                if (old_tail.cas == old_head.cas)
                    return false;

                // 需要修正
                if (first_node_prev.tag != old_head.tag)
                {
                    fix_list(old_tail, old_head);
                    continue;
                }

                // 基于旧值的操作
                ::memcpy(tmp, &((first_node_prev.ptr)->data), sizeof(T));

                // 构建新值
                TagedPtr<Node> new_head(first_node_prev.ptr, old_head.tag + 1);

                // 尝试CAS操作
                if (atomic_cas(&(_head.cas), old_head.cas, new_head.cas))
                {
                    _node_alloc.deallocate(old_head.ptr, 1);
                    if (NULL != p)
                        *p = *reinterpret_cast<T*>(tmp);
                    _data_alloc.destroy(reinterpret_cast<T*>(tmp));
                    return true;
                }
            }
        }
    }

private:
    /** 修复 */
    void fix_list(const TagedPtr<Node>& tail, const TagedPtr<Node>& head)
    {
        TagedPtr<Node> cur_node = tail;
        while ((head.cas == _head.cas) && (cur_node.cas != head.cas))
        {
            TagedPtr<Node> cur_node_next(cur_node.ptr->next.cas);
            cur_node_next.ptr->prev.ptr = cur_node.ptr;
            cur_node_next.ptr->prev.tag = cur_node.tag - 1;
            cur_node.ptr = cur_node_next.ptr;
            cur_node.tag = cur_node.tag - 1;
        }
    }

public:
    /** 采用消隐策略的入队 */
    void eliminate_enqueue(const T& v)
    {
        Node *new_node = _node_alloc.allocate(1);
        _data_alloc.construct(&(new_node->data), v);

        const typename TagedPtr<Node>::tag_type seen_tail = _tail.tag;
        while (true)
        {
            if (enqueue_attempt(new_node))
                return;
            if (seen_tail <= _head.tag && try_to_eliminate_enqueue(new_node, (unsigned int) seen_tail))
                return;
        }
    }

    /** 采用消隐策略的出队 */
    bool eliminate_dequeue(T *p)
    {
        while (true)
        {
            const DequeueAttemptResult rs = dequeue_attempt(p);
            if (rs == EMPTY_QUEUE_FAILURE)
                return false;
            else if (rs == DEQUEUE_SUCCESS || try_to_eliminate_dequeue(p))
                return true;
        }
    }

private:
    /** 尝试入队 */
    bool enqueue_attempt(Node *new_node)
    {
        const TagedPtr<Node> old_tail(_tail.cas);
        new_node->next.ptr = old_tail.ptr;
        new_node->next.tag = old_tail.tag + 1;

        const TagedPtr<Node> new_tail(new_node, old_tail.tag + 1);
        if (atomic_cas(&(_tail.cas), old_tail.cas, new_tail.cas))
        {
            old_tail.ptr->prev.ptr = new_node;
            old_tail.ptr->prev.tag = old_tail.tag;
            return true;
        }
        return false;
    }

    /** 尝试出队 */
    DequeueAttemptResult dequeue_attempt(T *p)
    {
        uint8_t tmp[sizeof(T)];

        while (true)
        {
            // 保留旧值
            const TagedPtr<Node> old_head(_head.cas);
            const TagedPtr<Node> old_tail(_tail.cas);
            const TagedPtr<Node> first_node_prev(old_head.ptr->prev.cas);

            if (old_head.cas == _head.cas)
            {
                // 队列为空
                if (old_tail.cas == old_head.cas)
                    return EMPTY_QUEUE_FAILURE;

                // 需要修正
                if (first_node_prev.tag != old_head.tag)
                {
                    fix_list(old_tail, old_head);
                    continue;
                }

                // 基于旧值的操作
                ::memcpy(tmp, &((first_node_prev.ptr)->data), sizeof(T));

                // 构建新值
                const TagedPtr<Node> new_head(first_node_prev.ptr, old_head.tag + 1);

                // 尝试CAS操作
                if (atomic_cas(&(_head.cas), old_head.cas, new_head.cas))
                {
                    _node_alloc.deallocate(old_head.ptr, 1);
                    if (NULL != p)
                        *p = *reinterpret_cast<T*>(tmp);
                    _data_alloc.destroy(reinterpret_cast<T*>(tmp));
                    return DEQUEUE_SUCCESS;
                }
                return CONCURRENT_FAILURE;
            }
        }
    }

    bool try_to_eliminate_enqueue(Node *new_node, unsigned int seen_tail)
    {
        new_node->seg = seen_tail;
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        const TagedPtr<Node> old_collision_to_add(_collisions[i].cas);
        if (old_collision_to_add.ptr != reinterpret_cast<Node*>(COLLISION_EMPTY_PTR))
            return false;

        // 添加到碰撞数组
        const TagedPtr<Node> new_collision_to_add(new_node, old_collision_to_add.tag + 1);
        if (!atomic_cas(&(_collisions[i].cas), old_collision_to_add.cas, new_collision_to_add.cas))
            return false;

        // 等待一段时间
#if NUT_PLATFORM_OS_WINDOWS
        ::Sleep(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS);
#elif NUT_PLATFORM_OS_LINUX
        usleep(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS);
#endif

        // 检查是否消隐成功
        const TagedPtr<Node> old_collision_to_remove(_collisions[i].cas);
        const TagedPtr<Node> new_collision_to_remove(reinterpret_cast<Node*>(COLLISION_EMPTY_PTR), old_collision_to_add.tag + 1);
        if (old_collision_to_remove.ptr == reinterpret_cast<Node*>(COLLISION_DONE_PTR) ||
            !atomic_cas(&(_collisions[i].cas), old_collision_to_remove.cas, new_collision_to_remove.cas))
        {
            _collisions[i].cas = new_collision_to_remove.cas;
            return true;
        }

        return false;
    }

    bool try_to_eliminate_dequeue(T *p)
    {
        const unsigned int seen_head = (unsigned int) _head.tag;
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        const TagedPtr<Node> old_collision(_collisions[i].cas);
        if (old_collision.ptr == reinterpret_cast<Node*>(COLLISION_EMPTY_PTR) ||
            old_collision.ptr == reinterpret_cast<Node*>(COLLISION_DONE_PTR))
            return false;

        if (old_collision.ptr->seg > seen_head)
            return false;

        const TagedPtr<Node> new_collision(reinterpret_cast<Node*>(COLLISION_DONE_PTR), old_collision.tag);
        if (atomic_cas(&(_collisions[i].cas), old_collision.cas, new_collision.cas))
        {
            if (NULL != p)
                *p = old_collision.ptr->data;
            _data_alloc.destroy(&(old_collision.ptr->data));
            _node_alloc.deallocate(old_collision.ptr, 1);
            return true;
        }
        return false;
    }
};

}

#endif
