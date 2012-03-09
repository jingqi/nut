/**
 * @file -
 * @author jingqi
 * @date 2012-03-04
 * @last-edit 2012-03-05 18:27:26 jingqi
 */

#ifndef ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_
#define ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_

#include <allocators>
#include <nut/debugging/sourcelocation.hpp>
#include <nut/debugging/exception.hpp>
#include "atomic.hpp"

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
class ConcurentQueue
{
    /** 节点 */
    struct Node
    {
        T data;
        unsigned int seg;
        TagedPtr<Node> volatile prev;
        TagedPtr<Node> volatile next;

        Node(const T& v) : data(v), seg(0) {}
    };

    /** 消隐数组的规模，可根据线程数适量增减 */
    enum { COLLISIONS_ARRAY_SIZE = 5 };

    /** 消隐数组的指针常量 */
    enum { EMPTY = NULL, DONE = -1 };

    /** 尝试出队的结果 */
    enum DequeueAttempResult
    {
        SUCCESS /* 成功 */,
        CONCURRENT_FAILURE /* 并发失败 */,
        EMPTY_QUEUE /* 空队列 */
    };

    typedef AllocT                                data_allocator_type;
    typedef AllocT::rebind<Node>::typename other  node_allocator_type;

    data_allocator_type m_dataAlloc;
    node_allocator_type m_nodeAlloc;
    TagedPtr<Node> volatile m_head;
    TagedPtr<Node> volatile m_tail;

    /** 用于消隐的数组 */
    TagedPtr<Node> m_collisions[COLLISIONS_ARRAY_SIZE];

public:
    ConcurentQueue()
    {
        Node *dumy = m_nodeAlloc.allocate(1);
        dumy->next.cas = 0;
        dumy->prev.cas = 0;
        m_head.ptr = dumy;
        m_tail.ptr = dumy;
    }

    ~ConcurentQueue()
    {
        // clear elements
        while (optimistic_dequeue(NULL)) {}
        assert(m_head == m_tail && NULL != m_head.ptr);
        m_nodeAlloc.deallocate(m_head.ptr, 1);
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
        Node *new_node = m_nodeAlloc.allocate(1);
        m_dataAlloc.construct(&(new_node->data), v);

        while (true)
        {
            // 获取旧值
            TagedPtr<Node> oldTail = m_tail;

            // 基于旧值的操作
            new_node->next.ptr = oldTail.ptr;
            new_node->next.tag = oldTail.tag + 1;

            // 构建新值
            TagedPtr<Node> newTail(new_node, oldTail.tag + 1);

            // 尝试CAS
            if (atomic_cas(&(m_tail.cas), oldTail.cas, newTail.cas))
            {
                // 收尾操作
                oldTail.ptr->prev.ptr = new_node;
                oldTail.ptr->prev.tag = oldTail.tag;
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
            TagedPtr<Node> oldHead = m_head, oldTail = m_tail;
            TagedPtr<Node> firstNodePrev = oldHead.ptr->prev;

            if (oldHead == m_head)
            {
                // 队列为空
                if (oldTail == oldHead)
                    return false;

                // 需要修正
                if (firstNodePrev.tag != oldHead.tag)
                {
                    fixList(oldTail, oldHead);
                    continue;
                }

                // 基于旧值的操作
                ::memcpy(&((firstNodePrev.ptr)->data), tmp, sizeof(T));

                // 构建新值
                TagedPtr<Node> newHead(firstNodePrev.ptr, oldHead.tag + 1);

                // 尝试CAS操作
                if (atomic_cas(&(m_head.cas), oldHead.cas, newHead.cas))
                {
                    m_nodeAlloc.deallocate(oldHead.ptr, 1);
                    if (NULL != p)
                        *p = *reinterpret_cast<T*>(tmp);
                    m_dataAlloc.destroy(tmp);
                    m_dataAlloc.deallocate(tmp, 1);
                    return true;
                }
            }
        }
    }

private:
    /** 修复 */
    void fixList(const TagedPtr<Node>& tail, const TagedPtr<Node>& head)
    {
        TagedPtr<Node> curNode = tail;
        while ((head == m_head) && (curNode != head))
        {
            TagedPtr<Node> curNodeNext = curNode.ptr->next;
            curNodeNext.ptr->prev.ptr = curNode.ptr;
            curNodeNext.ptr->prev.tag = curNode.tag - 1;
            curNode.ptr = curNodeNext.ptr;
            curNode.tag = curNode.tag - 1;
        }
    }

public:
    /** 采用消隐策略的入队 */
    void enqueue(const T& v)
    {
        Node *new_node = m_nodeAlloc.allocate(1);
        m_dataAlloc.construct(&(new_node->data), v);

        unsigned int seen_tail = m_tail.tag;
        while (true)
        {
            if (seen_tail > m_head.tag && enqueueAttempt(new_node))
                return;
            if (tryToEliminateEnqueue(new_node, seen_tail))
                return;
        }
    }

    bool dequeue(T *p)
    {
        while (true)
        {
            if (true)
            {
                DequeueAttempResult rs = dequeueAttemp(p);
                if (rs == SUCCESS)
                    return true;
                else if (rs == EMPTY_QUEUE)
                    return false;
            }
            else
            {
                if (tryToEliminateDequeue(p))
                    return true;
            }
        }
    }

private:
    /** 尝试入队 */
    bool enqueueAttempt(Node *new_node)
    {
        TagedPtr<Node> oldTail = m_tail;
        new_node->next.ptr = oldTail.ptr;
        new_node->next.tag = oldTail.tag + 1;

        TagedPtr<Node> newTail(new_node, oldTail.tag + 1);
        if (atomic_cas(&(m_tail.cas), oldTail.cas, newTail.cas))
        {
            oldTail.ptr->prev.ptr = new_node;
            oldTail.ptr->prev.tag = oldTail.tag;
            return true;
        }
        return false;
    }

    /** 尝试出队 */
    DequeueAttempResult dequeueAttemp(T *p)
    {
        uint8_t tmp[sizeof(T)];

        while (true)
        {
            // 保留旧值
            TagedPtr<Node> oldHead = m_head, oldTail = m_tail;
            TagedPtr<Node> firstNodePrev = oldHead.ptr->prev;

            if (oldHead == m_head)
            {
                // 队列为空
                if (oldTail == oldHead)
                    return EMPTY_QUEUE;

                // 需要修正
                if (firstNodePrev.tag != oldHead.tag)
                {
                    fixList(oldTail, oldHead);
                    continue;
                }

                // 基于旧值的操作
                ::memcpy(&((firstNodePrev.ptr)->data), tmp, sizeof(T));

                // 构建新值
                TagedPtr<Node> newHead(firstNodePrev.ptr, oldHead.tag + 1);

                // 尝试CAS操作
                if (atomic_cas(&(m_head.cas), oldHead.cas, newHead.cas))
                {
                    m_nodeAlloc.deallocate(oldHead.ptr, 1);
                    *p = *reinterpret_cast<T*>(tmp);
                    m_dataAlloc.destroy(tmp);
                    m_dataAlloc.deallocate(tmp, 1);
                    return SUCCESS;
                }
                return CONCURRENT_FAILURE;
            }
        }
    }

    bool tryToEliminateEnqueue(Node *new_node, unsigned int seen_tail)
    {
        new_node->seg = seen_tail;
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        TagedPtr<Node> colnode = m_collisions[i];
        if (colnode.ptr != EMPTY)
            return false;

        TagedPtr<Node> newColnode(new_node, colnode.tag + 1);
        if (!atomic_cas(&(m_collisions[i].cas), colnode.cas, newColnode.cas))
            return false;

        delay();
        TagedPtr<Node> oldColnode = m_collisions[i];
        TagedPtr<Node> newColnode2(EMPTY, colnode.tag + 1);
        if (oldColnode.ptr == DONE ||
            !atomic_cas(&(m_collisions[i].cas), oldColnode.cas, newColnode2.cas))
        {
            m_collisions[i].tag = colnode.tag + 1;
            m_collisions[i].ptr = EMPTY;
            return true;
        }

        return false;
    }

    bool tryToEliminateDequeue(T *p)
    {
        uint8_t tmp[sizeof(T)];

        unsigned int seen_head = m_head.tag;
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        TagedPtr<Node> colnode = m_collisions[i];
        if (colnode.ptr == EMPTY || colnode.ptr == DONE)
            return false;

        if (colnode.ptr->seg > seen_head)
            return false;

        ::memcpy(&(colnode.ptr->data), tmp, sizeof(T));
        TagedPtr<Node> newColnode(DONE, colnode.tag);
        if (atomic_cas(&(m_collisions[i].cas), colnode.cas, newColnode.cas))
        {
            *p = *reinterpret_cast<T*>(tmp);
            return true;
        }
        return false;
    }
};

}

#endif


