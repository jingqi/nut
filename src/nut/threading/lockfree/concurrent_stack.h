
#ifndef ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_
#define ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_

#include <assert.h>

#define _ENABLE_ATOMIC_ALIGNMENT_FIX // VS2015 SP2 BUG
#include <atomic>

#include "stamped_ptr.h"


namespace nut
{

/**
 * 无锁并发栈
 *
 *        +------+-> next
 * top -> | node |   ->   .....
 *        +------+
 *
 * 参考文献：
 *   [1] Danny Hendler, Nir Shavit, Lena Yerushalmi. A Scalable Lock-free Stack
 *       Algorithm[J]. SPAA. 2004-06-27. 206-215
 */
template <typename T>
class ConcurrentStack
{
private:
    class Node
    {
    public:
        Node(T&& v)
            : data(std::forward<T>(v))
        {}

        Node(const T& v)
            : data(v)
        {}

    public:
        T data;
        Node *next = nullptr;
    };

public:
    ConcurrentStack() = default;

    ~ConcurrentStack()
    {
        Node *p = _top.load(std::memory_order_acquire).ptr;
        while (nullptr != p)
        {
            Node *next = p->next;
            (&(p->data))->~T();
            ::free(p);
            p = next;
        }
    }

    size_t size() const
    {
        return _size.load(std::memory_order_relaxed);
    }

    bool is_empty() const
    {
        return nullptr == _top.load(std::memory_order_relaxed).ptr;
    }

    void push(T&& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(std::forward<T>(v));

        StampedPtr<Node> old_top = _top.load(std::memory_order_relaxed);
        do
        {
            new_node->next = old_top.ptr;
        } while(!_top.compare_exchange_weak(
                    &old_top, {new_node, old_top.stamp + 1},
                    std::memory_order_release, std::memory_order_relaxed));
        _size.fetch_add(1, std::memory_order_relaxed);
    }

    void push(const T& v)
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(v);

        StampedPtr<Node> old_top = _top.load(std::memory_order_relaxed);
        do
        {
            new_node->next = old_top.ptr;
        } while (!_top.compare_exchange_weak(
                     &old_top, {new_node, old_top.stamp + 1},
                     std::memory_order_release, std::memory_order_relaxed));
        _size.fetch_add(1, std::memory_order_relaxed);
    }

    bool pop(T *p)
    {
        StampedPtr<Node> old_top = _top.load(std::memory_order_acquire);
        while (true)
        {
            if (nullptr == old_top.ptr)
                return false;

            if (_top.compare_exchange_weak(
                    &old_top, {old_top.ptr->next, old_top.stamp + 1},
                    std::memory_order_relaxed, std::memory_order_acquire))
            {
                if (nullptr != p)
                    *p = std::move(old_top.ptr->data);
                (&(old_top.ptr->data))->~T();
                ::free(old_top.ptr);
                _size.fetch_sub(1, std::memory_order_relaxed);
                return true;
            }
        }
    }

private:
    ConcurrentStack(const ConcurrentStack&) = delete;
    ConcurrentStack& operator=(const ConcurrentStack&) = delete;

private:
    AtomicStampedPtr<Node> _top;
    std::atomic<size_t> _size = ATOMIC_VAR_INIT(0);
};

}

#endif
