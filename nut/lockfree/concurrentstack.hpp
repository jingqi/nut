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

template <typename T, typename AllocT = std::allocator<T> >
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
        : m_size(0)
    {}

    void push(const T& v)
    {
        Node *n = m_nodeAlloc.allocate(1);
        m_dataAlloc.construct(&(n->data), v);
        push(n);
        atomic_add(m_size, 1);
    }

    bool pop(T *p)
    {
        assert(NULL != p);
        Node *pn = pop();
        if (NULL == pn)
            return false;

        *p = pn->data;
        m_dataAlloc.destroy(&pn->data);
        m_nodeAlloc.deallocate(pn, 1);
        atomic_add(m_size, -1);
        return true;
    }

    size_t size()
    {
        return m_size;
    }

private:
    void push(Node *new_node)
    {
        assert(NULL != new_node);
        while (true)
        {
            TagedPtr<Node> oldTop = m_top;
            new_node->next = oldTop.ptr;
            TagedPtr<Node> *newTop(new_node, oldTop.tag + 1);
            if (atomic_cas(&m_top, oldTop, newTop))
                return;
        }
    }

    Node* pop()
    {
        while (true)
        {
            TagedPtr<Node> oldTop = m_top;

            if (NULL == oldTop.ptr)
                return NULL;

            TagedPtr<Node> newTop(oldTop.ptr->next, oldTop.tag + 1);
            if (atomic_cas(&m_top, oldTop, newTop))
                return oldTop.ptr;
        }
    }
};

}

#endif


