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
    struct Node
    {
        T data;
        TagedPtr<Node> volatile prev;
        TagedPtr<Node> volatile next;

        Node(const T& v) : data(v) {}
    };

    typedef AllocT                                data_allocator_type;
    typedef AllocT::rebind<Node>::typename other  node_allocator_type;

    data_allocator_type m_dataAlloc;
    node_allocator_type m_nodeAlloc;
    TagedPtr<Node> volatile m_head;
    TagedPtr<Node> volatile m_tail;

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
        clear();
        assert(NULL != m_head.ptr);
        m_nodeAlloc.deallocate(m_head.ptr, 1);
        m_head.cas = 0;
        m_end.cas = 0;
    }

public:
    void enqueue(const T& v)
    {
        Node *np = m_nodeAlloc.allocate(1);
        m_dataAlloc.construct(&(np->data), v);
        np->next.cas = 0;
        np->prev.cas = 0;
        enqueue(np);
    }

    /** 线程安全的出队 */
    bool dequeue(T *p)
    {
        assert(NULL != p);
        while(true)
        {
            //  获取旧值
            TagedPtr<Node> oldHead = m_head, oldTail = m_tail;
            TagedPtr<Node> firstNodePrev = oldHead.ptr->prev;

            if (oldHead == m_head)
            {
                if (oldTail == oldHead)
                    return false;

                if (firstNodePrev.tag != oldHead.tag)
                {
                    fixList(oldTail, oldHead);
                    continue;
                }

                // 中间操作
                T ret = (firstNodePrev.ptr)->data;

                // 构建新值
                TagedPtr<Node> newHead(firstNodePrev.ptr, oldHead.tag + 1);

                // 尝试CAS操作
                if (atomic_cas(&(m_head.cas), oldHead.cas, newHead.cas))
                {
                    // TODO 有问题，没有析构data域
                    m_nodeAlloc.deallocate(oldHead.ptr, 1);
                    *p = ret;
                    return true;
                }
            }
        }
    }
    
    void clear()
    {
        // TODO
    }

private:
    /** 线程安全的入队 */
    void enqueue(Node *new_node)
    {
        assert(NULL != new_node);
        while (true)
        {
            /**
             在满负荷并发的情况下，为了能提高吞吐量，即要减少两次成功的CAS操作之间的时间间隔，
             则等同于要减少第1步获取旧值与第4步CAS操作之间的耗时(因为第1步必定紧接着上次成功的CAS操作)
             */

            /**
             第2步中间操作与第5步收尾操作之间的操作之间的不同点在于：由于尚未插入到队列中，第2步无须
             考虑并发操作的影响，其结果是可靠的；但是第5步则要考虑操作延时对其他的并发操作的影响。
             */

            // 1. "获取旧值"
            TagedPtr<Node> oldTail = m_tail;

            // 2. "中间操作"
            new_node->next.ptr = oldTail.ptr;
            new_node->next.tag = oldTail.tag + 1;

            // 3. "构建新值"
            TagedPtr<Node> newTail(new_node, oldTail.tag + 1);

            // 4. "尝试CAS"
            if (atomic_cas(&(m_tail.cas), oldTail.cas, newTail.cas))
            {
                // 5. "收尾操作"
                oldTail.ptr->prev.ptr = new_node;
                oldTail.ptr->prev.tag = oldTail.tag;
                break;
            }
        }
    }

    void fixList(const TagedPtr<Node>& tail, const TagedPtr<Node>& head)
    {
        TagedPtr<Node> curNode = tail;
        while ((head == m_head) && (curNode != head))
        {
            TagedPtr<Node> curNodeNext = curNode.ptr->next;
            curNodeNext.ptr->prev.ptr = curNode.ptr;
            curNodeNext.ptr.prev.tag = curNode.tag - 1;
            curNode.ptr = curNodeNext.ptr;
            curNode.tag = curNode.tag - 1;
        }
    }
};

}

#endif


