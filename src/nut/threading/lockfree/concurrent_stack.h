
#ifndef ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_
#define ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_

#include <nut/platform/platform.h>

#include <assert.h>
#include <stdlib.h> // for rand()

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif
#if NUT_PLATFORM_CC_VC
#   include <allocators>
#endif

#include "stamped_ptr.h"


// 消隐数组的指针常量
#define COLLISION_EMPTY_PTR nullptr
#define COLLISION_DONE_PTR reinterpret_cast<Node*>(-1)

namespace nut
{

/**
 * 无锁并发栈
 *
 * 参考文献：
 *   [1]Danny Hendler, Nir Shavit, Lena Yerushalmi. A Scalable Lock-free Stack Algorithm[J]. SPAA. 2004-06-27. 206-215
 */
template <typename T, typename AllocT = std::allocator<T> >
class ConcurrentStack
{
    // 这里根据具体情况配置
    enum
    {
        // 消隐使用的碰撞数组的大小
        COLLISIONS_ARRAY_SIZE = 5,

        // 消隐入队时等待碰撞的毫秒数
        ELIMINATE_ENQUEUE_DELAY_MICROSECONDS = 10,
    };

    struct Node
    {
        T data;
        Node *next = nullptr;

        Node(const T& v)
            : data(v)
        {}
    };

    // 尝试出栈的结果
    enum PopAttemptResult
    {
        POP_SUCCESS, // 成功
        CONCURRENT_FAILURE, // 并发失败
        EMPTY_STACK_FAILURE, // 空栈
    };

    typedef AllocT                                        data_allocator_type;
    typedef typename AllocT::template rebind<Node>::other node_allocator_type;

    data_allocator_type _data_alloc;
    node_allocator_type _node_alloc;
    StampedPtr<Node> _top;

    // 用于消隐的碰撞数组
    StampedPtr<Node> _collisions[COLLISIONS_ARRAY_SIZE];

public:
    ConcurrentStack() = default;

    ~ConcurrentStack()
    {
        while (pop(nullptr))
        {}
        assert(nullptr == _top.pointer());
    }

    bool is_empty() const
    {
        return nullptr == _top.pointer();
    }

public:
    void push(const T& v)
    {
        Node *new_node = _node_alloc.allocate(1);
        _data_alloc.construct(&(new_node->data), v);

        while (true)
        {
            const StampedPtr<Node> old_top(_top);
            new_node->next = old_top.pointer();
            if (_top.compare_and_set(old_top, new_node))
                return;
        }
    }

    bool pop(T *p)
    {
        while (true)
        {
            const StampedPtr<Node> old_top(_top);

            if (nullptr == old_top.pointer())
                return false;

            if (_top.compare_and_set(old_top, old_top.pointer()->next))
            {
                if (nullptr != p)
                    *p = old_top.pointer()->data;
                _data_alloc.destroy(&(old_top.pointer()->data));
                _node_alloc.deallocate(old_top.pointer(), 1);
                return true;
            }
        }
    }

public:
    void eliminate_push(const T& v)
    {
        Node *new_node = _node_alloc.allocate(1);
        _data_alloc.construct(&(new_node->data), v);

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
            if (EMPTY_STACK_FAILURE == rs)
                return false;
            else if (POP_SUCCESS == rs || try_to_eliminate_pop(p))
                return true;
        }
    }

private:
    bool push_attempt(Node *new_node)
    {
        const StampedPtr<Node> old_top(_top);
        new_node->next = old_top.pointer();
        return _top.compare_and_set(old_top, new_node);
    }

    PopAttemptResult pop_attempt(T *p)
    {
        const StampedPtr<Node> old_top(_top);

        if (nullptr == old_top.pointer())
            return EMPTY_STACK_FAILURE;

        if (_top.compare_and_set(old_top, old_top.pointer()->next))
        {
            if (nullptr != p)
                *p = old_top.pointer()->data;
            _data_alloc.destroy(&(old_top.pointer()->data));
            _node_alloc.deallocate(old_top.pointer(), 1);
            return POP_SUCCESS;
        }
        return CONCURRENT_FAILURE;
    }

    bool try_to_eliminate_push(Node *new_node)
    {
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        const StampedPtr<Node> old_collision_to_add(_collisions[i]);
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

        // 检查消隐是否成功
        const StampedPtr<Node> old_collision_to_remove(_collisions[i]);
        if (COLLISION_DONE_PTR == old_collision_to_remove.pointer() ||
            !_collisions[i].compare_and_set(old_collision_to_remove,
                                            COLLISION_EMPTY_PTR,
                                            old_collision_to_add.stamp_value() + 1))
        {
            _collisions[i].set(COLLISION_EMPTY_PTR,
                               old_collision_to_add.stamp_value() + 1);
            return true;
        }

        return false;
    }

    bool try_to_eliminate_pop(T *p)
    {
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        const StampedPtr<Node> old_collision(_collisions[i]);
        if (COLLISION_EMPTY_PTR == old_collision.pointer() ||
            COLLISION_DONE_PTR == old_collision.pointer())
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
