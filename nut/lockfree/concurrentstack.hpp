/**
 * @file -
 * @author jingqi
 * @date 2012-03-05
 * @last-edit 2012-03-05 21:18:16 jingqi
 */

#ifndef ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_
#define ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_

#include <assert.h>
#include <allocators>

#include "atomic.hpp"

namespace nut
{

template <typename T, typename AllocT = std::allocator<T> >
class ConcurrentStack
{
    /** 这里根据具体情况配置 */
    enum
    {
        /** 消隐使用的碰撞数组的大小 */
        COLLISIONS_ARRAY_SIZE = 5,

        /** 消隐入队时等待碰撞的毫秒数 */
        ELIMINATE_ENQUEUE_DELAY_MICROSECONDS = 10,
    };

    struct Node
    {
        T data;
        Node *next;

        Node(const T& v) : data(v), next(NULL) {}
    };

    /** 尝试出栈的结果 */
    enum PopAttemptResult
    {
        POP_SUCCESS /* 成功 */,
        CONCURRENT_FAILURE /* 并发失败 */,
        EMPTY_STACK_FAILURE /* 空栈 */
    };

    /** 消隐数组的指针常量 */
    enum { COLLISION_EMPTY_PTR = NULL, COLLISION_DONE_PTR = -1 };

    typedef AllocT                                data_allocator_type;
    typedef typename AllocT::rebind<Node>::other  node_allocator_type;

    data_allocator_type m_dataAlloc;
    node_allocator_type m_nodeAlloc;
    TagedPtr<Node> volatile m_top;

    /** 用于消隐的碰撞数组 */
    TagedPtr<Node> volatile m_collisions[COLLISIONS_ARRAY_SIZE];

public:
    ConcurrentStack()
    {}

    ~ConcurrentStack()
    {
        while (pop(NULL)) {}
        assert(NULL == m_top.ptr);
    }

    bool isEmpty() const
    {
        return NULL == m_top.ptr;
    }

public:
    void push(const T& v)
    {
        Node *new_node = m_nodeAlloc.allocate(1);
        m_dataAlloc.construct(&(new_node->data), v);

        while (true)
        {
            const TagedPtr<Node> oldTop(m_top.cas);
            new_node->next = oldTop.ptr;
            const TagedPtr<Node> newTop(new_node, oldTop.tag + 1);
            if (atomic_cas(&(m_top.cas), oldTop.cas, newTop.cas))
                return;
        }
    }

    bool pop(T *p)
    {
        while (true)
        {
            const TagedPtr<Node> oldTop(m_top.cas);

            if (NULL == oldTop.ptr)
                return false;

            const TagedPtr<Node> newTop(oldTop.ptr->next, oldTop.tag + 1);
            if (atomic_cas(&(m_top.cas), oldTop.cas, newTop.cas))
            {
                if (NULL != p)
                    *p = oldTop.ptr->data;
                m_dataAlloc.destroy(&(oldTop.ptr->data));
                m_nodeAlloc.deallocate(oldTop.ptr, 1);
                return true;
            }
        }
    }

public:
    void eliminate_push(const T& v)
    {
        Node *new_node = m_nodeAlloc.allocate(1);
        m_dataAlloc.construct(&(new_node->data), v);

        while (true)
        {
            if (pushAttempt(new_node))
                return;
            if (tryToEliminatePush(new_node))
                return;
        }
    }

    bool eliminate_pop(T *p)
    {
        while (true)
        {
            const PopAttemptResult rs = popAttempt(p);
            if (rs == EMPTY_STACK_FAILURE)
                return false;
            else if (rs == POP_SUCCESS || tryToEliminatePop(p))
                return true;
        }
    }

private:
    bool pushAttempt(Node *new_node)
    {
        const TagedPtr<Node> oldTop(m_top.cas);
        new_node->next = oldTop.ptr;
        const TagedPtr<Node> newTop(new_node, oldTop.tag + 1);
        return atomic_cas(&(m_top.cas), oldTop.cas, newTop.cas);
    }

    PopAttemptResult popAttempt(T *p)
    {
        const TagedPtr<Node> oldTop(m_top.cas);

        if (NULL == oldTop.ptr)
            return EMPTY_STACK_FAILURE;

        const TagedPtr<Node> newTop(oldTop.ptr->next, oldTop.tag + 1);
        if (atomic_cas(&(m_top.cas), oldTop.cas, newTop.cas))
        {
            if (NULL != p)
                *p = oldTop.ptr->data;
            m_dataAlloc.destroy(&(oldTop.ptr->data));
            m_nodeAlloc.deallocate(oldTop.ptr, 1);
            return POP_SUCCESS;
        }
        return CONCURRENT_FAILURE;
    }

    bool tryToEliminatePush(Node *new_node)
    {
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        const TagedPtr<Node> oldCollisionToAdd(m_collisions[i].cas);
        if (oldCollisionToAdd.ptr != reinterpret_cast<Node*>(COLLISION_EMPTY_PTR))
            return false;

        // 添加到碰撞数组
        const TagedPtr<Node> newCollisionToAdd(new_node, oldCollisionToAdd.tag + 1);
        if (!atomic_cas(&(m_collisions[i].cas), oldCollisionToAdd.cas, newCollisionToAdd.cas))
            return false;

        // 等待一段时间
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::Sleep(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS);
#elif defined(NUT_PLATFORM_OS_LINUX)
        usleep(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS);
#endif

        // 检查消隐是否成功
        const TagedPtr<Node> oldCollisionToRemove(m_collisions[i].cas);
        const TagedPtr<Node> newCollisionToRemove(reinterpret_cast<Node*>(COLLISION_EMPTY_PTR), oldCollisionToAdd.tag + 1);
        if (oldCollisionToRemove.ptr == reinterpret_cast<Node*>(COLLISION_DONE_PTR) ||
            !atomic_cas(&(m_collisions[i].cas), oldCollisionToRemove.cas, newCollisionToRemove.cas))
        {
            m_collisions[i].cas = newCollisionToRemove.cas;
            return true;
        }

        return false;
    }

    bool tryToEliminatePop(T *p)
    {
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        const TagedPtr<Node> oldCollision(m_collisions[i].cas);
        if (oldCollision.ptr == reinterpret_cast<Node*>(COLLISION_EMPTY_PTR) ||
            oldCollision.ptr == reinterpret_cast<Node*>(COLLISION_DONE_PTR))
            return false;

        const TagedPtr<Node> newCollision(reinterpret_cast<Node*>(COLLISION_DONE_PTR), oldCollision.tag);
        if (atomic_cas(&(m_collisions[i].cas), oldCollision.cas, newCollision.cas))
        {
            if (NULL != p)
                *p = oldCollision.ptr->data;
            m_dataAlloc.destroy(&(oldCollision.ptr->data));
            m_nodeAlloc.deallocate(oldCollision.ptr, 1);
            return true;
        }
        return false;
    }
};

}

#endif


