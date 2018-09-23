
#ifndef ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_
#define ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_

#include <atomic>
#include <thread>

#include <nut/platform/platform.h>

#include <assert.h>
#include <stdlib.h> // for rand()

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif
#if NUT_PLATFORM_CC_VC
#   include <allocators>
#endif


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

    struct StampedPtr
    {
        Node *ptr;
        int stamp;

        StampedPtr()
            : ptr(nullptr), stamp(0)
        {}

        StampedPtr(Node *p, int s)
            : ptr(p), stamp(s)
        {}
    };

    // 尝试出栈的结果
    enum class PopAttemptResult
    {
        PopSuccess,        // 成功
        ConcurrentFailure, // 并发失败
        EmptyStackFailure, // 空栈
    };

    typedef AllocT                                        data_allocator_type;
    typedef typename AllocT::template rebind<Node>::other node_allocator_type;

    data_allocator_type _data_alloc;
    node_allocator_type _node_alloc;
    std::atomic<StampedPtr> _top;

    // 用于消隐的碰撞数组
    std::atomic<StampedPtr> _collisions[COLLISIONS_ARRAY_SIZE];

private:
    ConcurrentStack(const ConcurrentStack&) = delete;
    ConcurrentStack& operator=(const ConcurrentStack&) = delete;

public:
    ConcurrentStack()
    {
        _top = {nullptr, 0};
        for (int i = 0; i < COLLISIONS_ARRAY_SIZE; ++i)
            _collisions[i] = {nullptr, 0};
    }

    ~ConcurrentStack()
    {
        while (pop(nullptr))
        {}
        assert(nullptr == _top.load().ptr);
    }

    bool is_empty() const
    {
        return nullptr == _top.load().ptr;
    }

public:
    void push(const T& v)
    {
        Node *new_node = _node_alloc.allocate(1);
        _data_alloc.construct(&(new_node->data), v);

        while (true)
        {
            StampedPtr old_top = _top;
            new_node->next = old_top.ptr;
            if (_top.compare_exchange_weak(old_top, {new_node, old_top.stamp + 1}))
                return;
        }
    }

    bool pop(T *p)
    {
        while (true)
        {
            StampedPtr old_top = _top;

            if (nullptr == old_top.ptr)
                return false;

            if (_top.compare_exchange_weak(old_top, {old_top.ptr->next, old_top.stamp + 1}))
            {
                if (nullptr != p)
                    *p = old_top.ptr->data;
                _data_alloc.destroy(&(old_top.ptr->data));
                _node_alloc.deallocate(old_top.ptr, 1);
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
            if (PopAttemptResult::EmptyStackFailure == rs)
                return false;
            else if (PopAttemptResult::PopSuccess == rs || try_to_eliminate_pop(p))
                return true;
        }
    }

private:
    bool push_attempt(Node *new_node)
    {
        StampedPtr old_top = _top;
        new_node->next = old_top.ptr;
        return _top.compare_exchange_weak(old_top, {new_node, old_top.stamp + 1});
    }

    PopAttemptResult pop_attempt(T *p)
    {
        StampedPtr old_top = _top;

        if (nullptr == old_top.ptr)
            return PopAttemptResult::EmptyStackFailure;

        if (_top.compare_exchange_weak(old_top, {old_top.ptr->next, old_top.stamp + 1}))
        {
            if (nullptr != p)
                *p = old_top.ptr->data;
            _data_alloc.destroy(&(old_top.ptr->data));
            _node_alloc.deallocate(old_top.ptr, 1);
            return PopAttemptResult::PopSuccess;
        }
        return PopAttemptResult::ConcurrentFailure;
    }

    bool try_to_eliminate_push(Node *new_node)
    {
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        StampedPtr old_collision_to_add = _collisions[i];
        if (COLLISION_EMPTY_PTR != old_collision_to_add.ptr)
            return false;

        // 添加到碰撞数组
        if (!_collisions[i].compare_exchange_weak(old_collision_to_add, {new_node, old_collision_to_add.stamp + 1}))
            return false;

        // 等待一段时间
        std::this_thread::sleep_for(
            std::chrono::milliseconds(ELIMINATE_ENQUEUE_DELAY_MICROSECONDS));

        // 检查消隐是否成功
        StampedPtr old_collision_to_remove = _collisions[i];
        if (COLLISION_DONE_PTR == old_collision_to_remove.ptr ||
            !_collisions[i].compare_exchange_weak(
                old_collision_to_remove,
                {COLLISION_EMPTY_PTR, old_collision_to_add.stamp + 1}))
        {
            _collisions[i] = {COLLISION_EMPTY_PTR, old_collision_to_add.stamp + 1};
            return true;
        }

        return false;
    }

    bool try_to_eliminate_pop(T *p)
    {
        const unsigned int i = rand() % COLLISIONS_ARRAY_SIZE;
        StampedPtr old_collision = _collisions[i];
        if (COLLISION_EMPTY_PTR == old_collision.ptr ||
            COLLISION_DONE_PTR == old_collision.ptr)
            return false;

        if (_collisions[i].compare_exchange_weak(
                old_collision, {COLLISION_DONE_PTR, old_collision.stamp}))
        {
            if (nullptr != p)
                *p = old_collision.ptr->data;
            _data_alloc.destroy(&(old_collision.ptr->data));
            _node_alloc.deallocate(old_collision.ptr, 1);
            return true;
        }
        return false;
    }
};

}

#undef COLLISION_EMPTY_PTR
#undef COLLISION_DONE_PTR

#endif
