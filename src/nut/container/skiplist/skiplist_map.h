﻿
#ifndef ___HEADFILE_40DE4FAF_9BB0_4CF2_A78E_8FB1F58E09D3_
#define ___HEADFILE_40DE4FAF_9BB0_4CF2_A78E_8FB1F58E09D3_

#include <stdlib.h>
#include <new>

#include "../comparable.h"
#include "skiplist.h"


namespace nut
{

template <typename K, typename V>
class SkipListMap
{
private:
    class Node;
    typedef SkipListMap<K,V>                 self_type;
    typedef SkipList<K,Node,self_type>       algo_type;

    friend class SkipList<K,Node,self_type>;

    class Node
    {
    public:
        Node(K&& k, V&& v)
            : _key(std::forward<K>(k)), _value(std::forward<V>(v))
        {}

        Node(const K& k, V&& v)
            : _key(k), _value(std::forward<V>(v))
        {}

        Node(K&& k, const V& v)
            : _key(std::forward<K>(k)), _value(v)
        {}

        Node(const K& k, const V& v)
            : _key(k), _value(v)
        {}

        ~Node()
        {
            if (nullptr != _next)
                ::free(_next);
            _next = nullptr;
            _level = algo_type::INVALID_LEVEL;
        }

        const K& get_key() const
        {
            return _key;
        }

        const V& get_value() const
        {
            return _value;
        }

        V& get_value()
        {
            return _value;
        }

        void set_value(V&& v)
        {
            _value = std::forward<V>(v);
        }

        void set_value(const V& v)
        {
            _value = v;
        }

        int get_level() const
        {
            return _level;
        }

        void set_level(int lv)
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

        Node* get_next(int lv) const
        {
            assert(nullptr != _next && 0 <= lv && lv <= _level);
            return _next[lv];
        }

        void set_next(int lv, Node *n)
        {
            assert(nullptr != _next && 0 <= lv && lv <= _level);
            _next[lv] = n;
        }

    private:
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

    private:
        const K _key;
        V _value;
        Node **_next = nullptr;
        int _level = algo_type::INVALID_LEVEL; // 0-based
    };

public:
    SkipListMap() = default;

    SkipListMap(self_type&& x)
        : _level(x._level), _head(x._head), _size(x._size)
    {
        x._level = algo_type::INVALID_LEVEL;
        x._head = nullptr;
        x._size = 0;
    }

    SkipListMap(const self_type& x)
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
            new (c) Node(n->get_key(), n->get_value());
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

    ~SkipListMap()
    {
        clear();
        if (nullptr != _head)
            ::free(_head);
        _head = nullptr;
        _level = algo_type::INVALID_LEVEL;
    }

    self_type& operator=(self_type&& x)
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

    self_type& operator=(const self_type& x)
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
            new (c) Node(n->get_key(), n->get_value());
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

    bool operator==(const self_type& x) const
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
            if (current1->get_key() != current2->get_key() ||
                current1->get_value() != current2->get_value())
                return false;
            current1 = current1->get_next(0);
            current2 = current2->get_next(0);
        }
        assert(nullptr == current2);
        return true;
    }

    bool operator!=(const self_type& x) const
    {
        return !(*this == x);
    }

    bool operator<(const self_type& x) const
    {
        return compare(x) < 0;
    }

    bool operator>(const self_type& x) const
    {
        return x < *this;
    }

    bool operator<=(const self_type& x) const
    {
        return !(x < *this);
    }

    bool operator>=(const self_type& x) const
    {
        return !(*this < x);
    }

    V& operator[](K&& k)
    {
        if (nullptr == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(std::forward<K>(k), V());
            _head = (Node**) ::malloc(sizeof(Node*) * 1);
            _level = 0;
            _head[0] = n;
            n->set_level(0);
            n->set_next(0, nullptr);
            _size = 1;
            return n->get_value();
        }
        assert(_level >= 0);

        // Search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (_level + 1));
        Node *n = algo_type::search_node(k, *this, pre_lv);
        if (nullptr != n)
        {
            ::free(pre_lv);
            return n->get_value();
        }

        // Insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(std::forward<K>(k), V());
        algo_type::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++_size;
        return n->get_value();
    }

    V& operator[](const K& k)
    {
        if (nullptr == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(k, V());
            _head = (Node**) ::malloc(sizeof(Node*) * 1);
            _level = 0;
            _head[0] = n;
            n->set_level(0);
            n->set_next(0, nullptr);
            _size = 1;
            return n->get_value();
        }
        assert(_level >= 0);

        // Search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (_level + 1));
        Node *n = algo_type::search_node(k, *this, pre_lv);
        if (nullptr != n)
        {
            ::free(pre_lv);
            return n->get_value();
        }

        // Insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(k, V());
        algo_type::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++_size;
        return n->get_value();
    }

    int compare(const self_type& x) const
    {
        if (this == &x)
            return 0;
        else if (0 == _size || 0 == x._size)
            return 0 == _size ? (0 != x._size ? -1 : 0) : 1;
        assert(nullptr != _head && _level >= 0 && nullptr != x._head && x._level >= 0);

        Node *current1 = _head[0], current2 = x._head[0];
        while (nullptr != current1 && nullptr != current2)
        {
            const int krs = compare(current1->get_key(), current2->get_key());
            if (0 != krs)
                return krs;

            const int vrs = compare(current1->get_value(), current2->get_value());
            if (0 != vrs)
                return vrs;

            current1 = current1->get_next(0);
            current2 = current2->get_next(0);
        }
        return nullptr != current1 ? 1 : (nullptr != current2 ? -1 : 0);
    }

    size_t size() const
    {
        return _size;
    }

    void clear()
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

    bool contains_key(const K& k) const
    {
        if (0 == _size)
            return false;
        assert(nullptr != _head && _level >= 0);

        return nullptr != algo_type::search_node(k, *this, nullptr);
    }

    /**
     * @return true if insert success, else old data found
     */
    bool insert(K&& k, V&& v)
    {
        return put(std::forward<K>(k), std::forward<V>(v), false);
    }

    /**
     * @return true if insert success, else old data found
     */
    bool insert(const K& k, V&& v)
    {
        return put(k, std::forward<V>(v), false);
    }

    /**
     * @return true if insert success, else old data found
     */
    bool insert(K&& k, const V& v)
    {
        return put(std::forward<K>(k), v, false);
    }

    /**
     * @return true if insert success, else old data found
     */
    bool insert(const K& k, const V& v)
    {
        return put(k, v, false);
    }

    /**
     * @return true if new data inserted, else old data replaced
     */
    bool put(K&& k, V&& v, bool replace = true)
    {
        if (nullptr == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(std::forward<K>(k), std::forward<V>(v));
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
            if (replace)
                n->set_value(std::forward<V>(v));
            return false;
        }

        // Insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(std::forward<K>(k), std::forward<V>(v));
        algo_type::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++_size;
        return true;
    }

    /**
     * @return true if new data inserted, else old data replaced
     */
    bool put(const K& k, V&& v, bool replace = true)
    {
        if (nullptr == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(k, std::forward<V>(v));
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
            if (replace)
                n->set_value(std::forward<V>(v));
            return false;
        }

        // Insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(k, std::forward<V>(v));
        algo_type::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++_size;
        return true;
    }

    /**
     * @return true if new data inserted, else old data replaced
     */
    bool put(K&& k, const V& v, bool replace = true)
    {
        if (nullptr == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(std::forward<K>(k), v);
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
            if (replace)
                n->set_value(v);
            return false;
        }

        // Insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(std::forward<K>(k), v);
        algo_type::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++_size;
        return true;
    }

    /**
     * @return true if new data inserted, else old data replaced
     */
    bool put(const K& k, const V& v, bool replace = true)
    {
        if (nullptr == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(k, v);
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
            if (replace)
                n->set_value(v);
            return false;
        }

        // Insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(k, v);
        algo_type::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++_size;
        return true;
    }

    /**
     * @return true if data removed, else nothing happened
     */
    bool remove(const K& k)
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

    bool get(const K& k, V *v) const
    {
        assert(nullptr != v);

        if (0 == _size)
            return false;
        assert(nullptr != _head && _level >= 0);
        Node *n = algo_type::search_node(k, *this, nullptr);
        if (nullptr == n)
            return false;
        *v = n->get_value();
        return true;
    }

private:
    int get_level() const
    {
        return _level;
    }

    void set_level(int lv)
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

    Node* get_head(int lv) const
    {
        assert(nullptr != _head && 0 <= lv && lv <= _level);
        return _head[lv];
    }

    void set_head(int lv, Node *n)
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
