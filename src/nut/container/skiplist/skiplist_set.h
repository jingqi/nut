
#ifndef ___HEADFILE_60C4D68A_A1D8_4B2C_A488_40E9A9FFE426_
#define ___HEADFILE_60C4D68A_A1D8_4B2C_A488_40E9A9FFE426_

#include <stdlib.h>
#include <new>

#include "../comparable.h"
#include "skiplist.h"


namespace nut
{

template <typename T>
class SkipListSet
{
private:
    class Node;
    typedef SkipListSet<T>             self_type;
    typedef SkipList<T,Node,self_type> algo_type;

    friend class SkipList<T,Node,self_type>;

    class Node
    {
    public:
        explicit Node(T&& k) noexcept
            : _key(std::forward<T>(k))
        {}

        explicit Node(const T& k) noexcept
            : _key(k)
        {}

        ~Node() noexcept
        {
            if (nullptr != _next)
                ::free(_next);
            _next = nullptr;
            _level = algo_type::INVALID_LEVEL;
        }

        const T& get_key() const noexcept
        {
            return _key;
        }

        int get_level() const noexcept
        {
            return _level;
        }

        void set_level(int lv) noexcept
        {
            assert(lv >= 0);
            if (nullptr != _next)
            {
                assert(_level >= 0);
                _next = (Node**) ::realloc(_next, sizeof(Node*) * (lv + 1));
                if (lv > _level)
                    ::memset(_next + _level + 1, 0, sizeof(Node*) * (lv - _level));
            }
            else
            {
                assert(_level < 0);
                _next = (Node**) ::malloc(sizeof(Node*) * (lv + 1));
                ::memset(_next, 0, sizeof(Node*) * (lv + 1));
            }
            _level = lv;
        }

        Node* get_next(int lv) const noexcept
        {
            assert(nullptr != _next && 0 <= lv && lv <= _level);
            return _next[lv];
        }

        void set_next(int lv, Node *n) noexcept
        {
            assert(nullptr != _next && 0 <= lv && lv <= _level);
            _next[lv] = n;
        }

    private:
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

    private:
        const T _key;
        Node **_next = nullptr;
        int _level = algo_type::INVALID_LEVEL; // 0-based
    };

public:
    SkipListSet() = default;

    SkipListSet(self_type&& x) noexcept
        : _level(x._level), _head(x._head), _size(x._size)
    {
        x._level = algo_type::INVALID_LEVEL;
        x._head = nullptr;
        x._size = 0;
    }

    SkipListSet(const self_type& x) noexcept
    {
        if (x._size == 0)
            return;
        assert(nullptr != x._head && x._level >= 0);

        // Initial attributes
        _level = x._level;
        _head = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(_head, 0, sizeof(Node*) * (_level + 1));

        // Copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (_level + 1));
        Node *n = x._head[0];
        while (nullptr != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) Node(n->get_key());
            const int level = n->get_level();
            c->set_level(level);
            algo_type::insert_node(c, *this, pre_lv);
            for (size_t i = 0; i <= level; ++i)
                pre_lv[i] = c;

            n = n->get_next(0);
        }
        ::free(pre_lv);
        _size = x._size;
    }

    ~SkipListSet() noexcept
    {
        clear();
        if (nullptr != _head)
            ::free(_head);
        _head = nullptr;
        _level = algo_type::INVALID_LEVEL;
    }

    self_type& operator=(self_type&& x) noexcept
    {
        if (this == &x)
            return *this;

        clear();
        if (nullptr != _head)
            ::free(_head);

        _level = x._level;
        _head = x._head;
        _size = x._size;

        x._level = algo_type::INVALID_LEVEL;
        x._head = nullptr;
        x._size = 0;

        return *this;
    }

    self_type& operator=(const self_type& x) noexcept
    {
        if (this == &x)
            return *this;

        // Clear memory
        clear();
        if (x._size == 0)
            return *this;
        assert(nullptr != x._head && x._level >= 0);

        // Initial attributes
        if (nullptr != _head)
        {
            assert(_level >= 0);
            _head = (Node**) ::realloc(_head, sizeof(Node*) * (x._level + 1));
        }
        else
        {
            assert(_level < 0);
            _head = (Node**) ::malloc(sizeof(Node*) * (x._level + 1));
        }
        _level = x._level;
        ::memset(_head, 0, sizeof(Node*) * (_level + 1));

        // Copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (_level + 1));
        Node *n = x._head[0];
        while (nullptr != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) Node(n->get_key());
            const int level = n->get_level();
            c->set_level(level);
            algo_type::insert_node(c, *this, pre_lv);
            for (size_t i = 0; i <= level; ++i)
                pre_lv[i] = c;

            n = n->get_next(0);
        }
        ::free(pre_lv);
        _size = x._size;

        return *this;
    }

    bool operator==(const self_type& x) const noexcept
    {
        if (this == &x)
            return true;
        else if (_size != x._size)
            return false;
        else if (0 == _size)
            return true;
        assert(nullptr != _head && _level >= 0 && nullptr != x._head && x._level >= 0);

        Node *current1 = _head[0], current2 = x._head[0];
        while (nullptr != current1)
        {
            assert(nullptr != current2);
            if (current1->get_key() != current2->get_key())
                return false;
            current1 = current1->get_next(0);
            current2 = current2->get_next(0);
        }
        assert(nullptr == current2);
        return true;
    }

    bool operator!=(const self_type& x) const noexcept
    {
        return !(*this == x);
    }

    bool operator<(const self_type& x) const noexcept
    {
        return compare(x) < 0;
    }

    bool operator>(const self_type& x) const noexcept
    {
        return x < *this;
    }

    bool operator<=(const self_type& x) const noexcept
    {
        return !(x < *this);
    }

    bool operator>=(const self_type& x) const noexcept
    {
        return !(*this < x);
    }

    int compare(const self_type& x) const noexcept
    {
        if (this == &x)
            return 0;
        else if (0 == _size || 0 == x._size)
            return 0 == _size ? (0 != x._size ? -1 : 0) : 1;
        assert(nullptr != _head && _level >= 0 && nullptr != x._head && x._level >= 0);

        Node *current1 = _head[0], current2 = x._head[0];
        while (nullptr != current1 && nullptr != current2)
        {
            const int rs = compare(current1->get_key(), current2->get_key());
            if (0 != rs)
                return rs;

            current1 = current1->get_next(0);
            current2 = current2->get_next(0);
        }
        return nullptr != current1 ? 1 : (nullptr != current2 ? -1 : 0);
    }

    size_t size() const noexcept
    {
        return _size;
    }

    void clear() noexcept
    {
        if (0 == _size)
            return;
        assert(nullptr != _head && _level >= 0);

        Node *current = _head[0];
        while (nullptr != current)
        {
            Node *next = current->get_next(0);
            current->~Node();
            ::free(current);
            current = next;
        }
        ::memset(_head, 0, sizeof(Node*) * (_level + 1));
        _size = 0;
    }

    bool contains(const T& k) const noexcept
    {
        if (0 == _size)
            return false;
        assert(nullptr != _head && _level >= 0);

        return nullptr != algo_type::search_node(k, *this, nullptr);
    }

    /**
     * @return 插入成功返回 true; 否则表示存在重复数据, 返回 false
     */
    template <typename ...Args>
    bool emplace(Args&& ...args) noexcept
    {
        if (nullptr == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(std::forward<Args>(args)...);
            _head = (Node**) ::malloc(sizeof(Node*) * 1);
            _level = 0;
            _head[0] = n;
            n->set_level(0);
            n->set_next(0, nullptr);
            _size = 1;
            return true;
        }
        assert(_level >= 0);

        // Search
        T k(std::forward<Args>(args)...);
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (_level + 1));
        Node *n = algo_type::search_node(k, *this, pre_lv);
        if (nullptr != n)
        {
            ::free(pre_lv);
            return false;
        }

        // Insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(std::move(k));
        algo_type::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++_size;
        return true;
    }

    /**
     * @return 插入成功返回 true; 否则表示存在重复数据, 返回 false
     */
    bool add(T&& k) noexcept
    {
        if (nullptr == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(std::forward<T>(k));
            _head = (Node**) ::malloc(sizeof(Node*) * 1);
            _level = 0;
            _head[0] = n;
            n->set_level(0);
            n->set_next(0, nullptr);
            _size = 1;
            return true;
        }
        assert(_level >= 0);

        // Search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (_level + 1));
        Node *n = algo_type::search_node(k, *this, pre_lv);
        if (nullptr != n)
        {
            ::free(pre_lv);
            return false;
        }

        // Insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(std::forward<T>(k));
        algo_type::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++_size;
        return true;
    }

    /**
     * @return 插入成功返回 true; 否则表示存在重复数据, 返回 false
     */
    bool add(const T& k) noexcept
    {
        if (nullptr == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(k);
            _head = (Node**) ::malloc(sizeof(Node*) * 1);
            _level = 0;
            _head[0] = n;
            n->set_level(0);
            n->set_next(0, nullptr);
            _size = 1;
            return true;
        }
        assert(_level >= 0);

        // Search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (_level + 1));
        Node *n = algo_type::search_node(k, *this, pre_lv);
        if (nullptr != n)
        {
            ::free(pre_lv);
            return false;
        }

        // Insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(k);
        algo_type::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++_size;
        return true;
    }

    /**
     * @return 删除成功返回 true; 否则表示未找到数据, 返回 false
     */
    bool remove(const T& k) noexcept
    {
        if (0 == _size)
            return false;
        assert(nullptr != _head && _level >= 0);

        // Search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (_level + 1));
        Node *n = algo_type::search_node(k, *this, pre_lv);
        if (nullptr == n)
        {
            ::free(pre_lv);
            return false;
        }

        // Remove
        algo_type::remove_node(n, *this, pre_lv);
        ::free(pre_lv);
        n->~Node();
        ::free(n);
        --_size;
        return true;
    }

private:
    int get_level() const noexcept
    {
        return _level;
    }

    void set_level(int lv) noexcept
    {
        assert(lv >= 0);
        if (nullptr != _head)
        {
            assert(_level >= 0);
            _head = (Node**) ::realloc(_head, sizeof(Node*) * (lv + 1));
            if (lv > _level)
                ::memset(_head + _level + 1, 0, sizeof(Node*) * (lv - _level));
        }
        else
        {
            assert(_level < 0);
            _head = (Node**) ::malloc(sizeof(Node*) * (lv + 1));
            ::memset(_head, 0, sizeof(Node*) * (lv + 1));
        }
        _level = lv;
    }

    Node* get_head(int lv) const noexcept
    {
        assert(nullptr != _head && 0 <= lv && lv <= _level);
        return _head[lv];
    }

    void set_head(int lv, Node *n) noexcept
    {
        assert(nullptr != _head && 0 <= lv && lv <= _level);
        _head[lv] = n;
    }

private:
    int _level = algo_type::INVALID_LEVEL; // 0-based
    Node **_head = nullptr;
    size_t _size = 0;
};

}

#endif
