/**
 * @file -
 * @author jingqi
 * @date 2012-03-05
 * @last-edit 2015-01-06 23:29:17 jingqi
 */

#ifndef ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_
#define ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_

#include <nut/platform/platform.hpp>

#include <assert.h>
#include <stdlib.h> // for rand()

#if defined(NUT_PLATFORM_CC_VS)
#   include <allocators>
#endif

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

        Node(const T& v)
            : data(v), next(NULL)
        {}
    };

    /** 尝试出栈的结果 */
    enum PopAttemptResult
    {
        POP_SUCCESS /* 成功 */,
        CONCURRENT_FAILURE /* 并发失败 */,
        EMPTY_STACK_FAILURE /* 空栈 */
    };

    /** 消隐数组的指针常量 */
    enum { COLLISION_EMPTY_PTR = (int)NULL, COLLISION_DONE_PTR = -1 };

    typedef AllocT                                data_allocator_type;
    typedef typename AllocT::template rebind<Node>::other  node_allocator_type;

    data_allocator_type m_data_alloc;
    node_allocator_type m_node_alloc;
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
        Node *new_node = m_node_alloc.allocate(1);
        m_data_alloc.construct(&(new_node->data), v);

        while (true)
        {
            const TagedPtr<Node> old_top(m_top.cas);
            new_node->next = old_top.ptr;
            const TagedPtr<Node> new_top(new_node, old_top.tag + 1);
            if (atomic_cas(&(m_top.cas), old_top.cas, new_top.cas))
                return;
        }
    }

    bool pop(T *p)
    {
        while (true)
        {
            const TagedPtr<Node> old_top(m_top.cas);

            if (NULL == old_top.ptr)
                return false;

            const TagedPtr<Node> new_top(old_top.ptr->next, old_top.tag + 1);
            if (atomic_cas(&(m_top.cas), old_top.cas, new_top.cas))
            {
                if (NULL != p)
                    *p = old_top.ptr->data;
                m_data_alloc.destroy(&(old_top.ptr->data));
                m_node_alloc.deallocate(old_top.ptr, 1);
                return true;
            }
        }
    }

public:
    void eliminate_push(const T& v)
    {
        Node *new_node = m_node_alloc.allocate(1);
        m_data_alloc.construct(&(new_node->data), v);

        while (true)
        {
            if (push_attempt(new_node))
                return;
            if (try_to_eliminate_push(new_node))
                return;
        }
    }

    bool eliminate_pop(T *p)
    {
        while (true)
        {
            const PopAttemptResult rs = pop_attempt(p);
            if (rs == EMPTY_STACK_FAILURE)
                return false;
            else if (rs == POP_SUCCESS || try_to_eliminate_pop(p))
                return true;
        }
    }

private:
    bool push_attempt(Node *new_node)
    {
        const TagedPtr<Node> old_top(m_top.cas);
        new_node->next = old_top.ptr;
        const TagedPtr<Node> new_top(new_node, old_top.tag + 1);
        return atomic_cas(&(m_top.cas), old_top.cas, new_top.cas);
    }

    PopAttemptResult pop_attempt(T *p)
    {
        const TagedPtr<Node> old_top(m_top.cas);

        if (NULL == old_top.ptr)
            return EMPTY_STACK_FAILURE;

        const TagedPtr<Node> new_top(old_top.ptr->next, old_top.tag + 1);
        if (atomic_cas(&(m_top.cas), old_top.cas, new_top.cas))
        {
            if (NULL != p)
                *p = old_top.ptr->data;
            m_data_alloc.destroy(&(old_top.ptr->data));
            m_node_alloc.deallocate(old_top.ptr, 1);
            return POP_SUCCESS;
        }
        return CONCURRENT_FAILURE;
    }

    bool try_to_eliminate_push(Node *new_node)
    {
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        const TagedPtr<Node> old_collision_to_add(m_collisions[i].cas);
        if (old_collision_to_add.ptr != reinterpret_cast<Node*>(COLLISION_EMPTY_PTR))
            return false;

        // 添加到碰撞数组
        const TagedPtr<Node> new_collision_to_add(new_node, old_collision_to_add.tag + 1);
        if (!atomic_cas(&(m_collisions[i].cas), old_collision_to_add.cas, new_collision_to_add.cas))
            return false;

        // 等待一段时间
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::Sleep(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS);
#elif defined(NUT_PLATFORM_OS_LINUX)
        usleep(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS);
#endif

        // 检查消隐是否成功
        const TagedPtr<Node> old_collision_to_remove(m_collisions[i].cas);
        const TagedPtr<Node> new_collision_to_remove(reinterpret_cast<Node*>(COLLISION_EMPTY_PTR), old_collision_to_add.tag + 1);
        if (old_collision_to_remove.ptr == reinterpret_cast<Node*>(COLLISION_DONE_PTR) ||
            !atomic_cas(&(m_collisions[i].cas), old_collision_to_remove.cas, new_collision_to_remove.cas))
        {
            m_collisions[i].cas = new_collision_to_remove.cas;
            return true;
        }

        return false;
    }

    bool try_to_eliminate_pop(T *p)
    {
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        const TagedPtr<Node> old_collision(m_collisions[i].cas);
        if (old_collision.ptr == reinterpret_cast<Node*>(COLLISION_EMPTY_PTR) ||
            old_collision.ptr == reinterpret_cast<Node*>(COLLISION_DONE_PTR))
            return false;

        const TagedPtr<Node> new_collision(reinterpret_cast<Node*>(COLLISION_DONE_PTR), old_collision.tag);
        if (atomic_cas(&(m_collisions[i].cas), old_collision.cas, new_collision.cas))
        {
            if (NULL != p)
                *p = old_collision.ptr->data;
            m_data_alloc.destroy(&(old_collision.ptr->data));
            m_node_alloc.deallocate(old_collision.ptr, 1);
            return true;
        }
        return false;
    }
};

}

#endif
