/**
 * @file -
 * @author jingqi
 * @date 2012-03-04
 * @last-edit 2012-03-05 18:27:26 jingqi
 */

#ifndef ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_
#define ___HEADFILE_C020D343_98AA_41A4_AFE8_01825671348C_

#include <nut/platform/platform.hpp>

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
template <typename T>
class ConcurentQueue
{
    struct Node
    {
        T data;
        TagedPtr<Node> pre;
        TagedPtr<Node> next;

        Node(const T& v) : data(v) {}
    };
    
    TagedPtr<Node> volatile m_head;
    TagedPtr<Node> volatile m_tail;

public:
    ConcurentQueue()
    {
        Node *dumy = (Node*) ::malloc(sizeof(Node));
        dumy->next.cas = 0;
        dumy->pre.cas = 0;
        m_head.ptr = dumy;
        m_tail.ptr = dumy;
    }

    ~ConcurentQueue()
    {
        clear();
        assert(NULL != m_head.ptr);
        ::free(m_head.ptr);
        m_head.cas = 0;
        m_end.cas = 0;
    }

public:
    void enquere(Node *new_node)
    {
        while (true)
        {
            // read the tail
            TagedPtr oldTail(m_tail);

            // set node's next ptr
            new_node->next.ptr = oldTail.ptr;
            new_node->next.tag = oldTail.tag + 1;

            // try CAS the tail
            TagedPtr newNode(new_node, oldTail.tag + 1);
            if (atomic_cas(&(m_tail.cas), oldTail.cas, newNode.cas)) {
                oldTail.ptr->pre.ptr = new_node;
                oldTail.ptr->pre.tag = oldTail.tag;
                break;
            }
        }
    }

    Node* dequeue()
    {
        // TODO
    }

private:
    /** CAS 操作 */
    static inline bool atomicCAS(long *ptr, long oldval, long newval)
    {
#if defined(NUT_PLATFORM_CC_GCC)
        return __sync_val_compare_and_swap(ptr, oldval, newval);
#elif defined(NUT_PLATFORM_CC_VC)
        return InterlockedCompareExchange(ptr, oldval, newval) == oldval;
        InterlockedCompareExchange64
#endif
    }
};

}

#endif


