/**
 * @file -
 * @author jingqi
 * @date 2012-03-05
 * @last-edit 2012-03-05 21:18:16 jingqi
 */

#ifndef ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_
#define ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_

#include <allocators>
#include "atomic.hpp"

namespace nut
{

template <typename T, typename AllocT = std::allocate>
class ConcurrentStack
{
    struct Node
    {
        T data;
        Node *next;

        Node(const T& v) : data(v), next(NULL) {}
    };

    typedef AllocT                                data_allocator_type;
    typedef AllocT::rebind<Node>::typename other  node_allocator_type;

    data_allocator_type m_dataAlloc;
    node_allocator_type m_nodeAlloc;
    TagedPtr<Node> volatile m_top;
    size_t volatile m_size;

public:
    ConcurrentStack()
    {}

    void push(const T& v)
    {
        Node *n = m_nodeAlloc.allocate(1);
        m_dataAlloc.construct(&(n->data), v);
        push(n);
    }

    void push(Node *new_node)
    {
        while (true)
        {
            TagedPtr<Node> *oldTop = m_top;
            new_node->next = oldTop.ptr;
            TagedPtr<Node> *newTop(new_node, oldTop.tag + 1);
            if (atomic_cas(&m_top, oldTop, newTop))
            {
                atomic_add(m_size, 1);
                return;
            }
        }
    }

    Node* pop()
    {
        while (true)
        {
            TagedPtr<Node> oldTop = m_top;

            if (NULL == oldTop.ptr)
                return NULL;

            TagedPtr<Node> topNodeNext = oldTop.ptr->next;

            TagedPtr<Node> newTop(oldTop.ptr->next);
            Node *next = oldTop->next;
            if (atomic_cas(&m_top, oldTop, next))
            {
                atomic_add(m_size, -1);
                return oldTop->data;
            }
        }
    }

    size_t size()
    {
        return m_size;
    }
};

}

#endif


