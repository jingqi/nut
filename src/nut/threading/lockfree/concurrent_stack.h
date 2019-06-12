
#ifndef ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_
#define ___HEADFILE_039EC871_866B_4C6A_AF26_747D92A9ADA7_

#include <assert.h>
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
        explicit Node(T&& v) noexcept
            : data(std::forward<T>(v))
        {}

        explicit Node(const T& v) noexcept
            : data(v)
        {}

    public:
        T data;
        Node *next = nullptr;
    };

public:
    ConcurrentStack() = default;

    ~ConcurrentStack() noexcept
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

    size_t size() const noexcept
    {
        return _size.load(std::memory_order_relaxed);
    }

    bool is_empty() const noexcept
    {
        return nullptr == _top.load(std::memory_order_relaxed).ptr;
    }

    template <typename ...Args>
    void emplace(Args ...args) noexcept
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(std::forward<Args>(args)...);
        push(new_node);
    }

    void push(T&& v) noexcept
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(std::forward<T>(v));
        push(new_node);
    }

    void push(const T& v) noexcept
    {
        Node *new_node = (Node*) ::malloc(sizeof(Node));
        new (&(new_node->data)) T(v);
        push(new_node);
    }

    bool pop(T *p = nullptr) noexcept
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

    void clear() noexcept
    {
        while (pop(nullptr))
        {}
    }

private:
    ConcurrentStack(const ConcurrentStack&) = delete;
    ConcurrentStack& operator=(const ConcurrentStack&) = delete;

    void push(Node *new_node) noexcept
    {
        assert(nullptr != new_node);

        StampedPtr<Node> old_top = _top.load(std::memory_order_relaxed);
        do
        {
            new_node->next = old_top.ptr;
        } while (!_top.compare_exchange_weak(
                     &old_top, {new_node, old_top.stamp + 1},
                     std::memory_order_release, std::memory_order_relaxed));
        _size.fetch_add(1, std::memory_order_relaxed);
    }

private:
    AtomicStampedPtr<Node> _top;
    std::atomic<size_t> _size = ATOMIC_VAR_INIT(0);
};

}

#endif
