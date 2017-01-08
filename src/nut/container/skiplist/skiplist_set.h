
#ifndef ___HEADFILE_60C4D68A_A1D8_4B2C_A488_40E9A9FFE426_
#define ___HEADFILE_60C4D68A_A1D8_4B2C_A488_40E9A9FFE426_

#include <stdlib.h>
#include <new>

#include "skiplist.h"

namespace nut
{

template <typename T>
class SkipListSet
{
    class Node
    {
        T _key;
        Node **_next = NULL;
        int _level = -1; // 0-based

    public:
        Node(const T& k)
            : _key(k)
        {}

        ~Node()
        {
            if (NULL != _next)
                ::free(_next);
            _next = NULL;
            _level = -1;
        }

        const T& get_key() const
        {
            return _key;
        }

        int get_level() const
        {
            return _level;
        }

        void set_level(int lv)
        {
            assert(lv >= 0);
            if (NULL != _next)
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
            assert(NULL != _next && 0 <= lv && lv <= _level);
            return _next[lv];
        }

        void set_next(int lv, Node *n)
        {
            assert(NULL != _next && 0 <= lv && lv <= _level);
            _next[lv] = n;
        }
    };

    int _level = -1; // 0-based
    Node **_head = NULL;
    size_t _size = 0;

private:
    typedef SkipListSet<T>             self_type;
    typedef SkipList<T,Node,self_type> algo_type;
    friend class SkipList<T,Node,self_type>;

    int get_level() const
    {
        return _level;
    }

    void set_level(int lv)
    {
        assert(lv >= 0);
        if (NULL != _head)
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
        assert(NULL != _head && 0 <= lv && lv <= _level);
        return _head[lv];
    }

    void set_head(int lv, Node *n)
    {
        assert(NULL != _head && 0 <= lv && lv <= _level);
        _head[lv] = n;
    }

public:
    SkipListSet()
    {}

    SkipListSet(const self_type& x)
    {
        if (x._size == 0)
            return;
        assert(NULL != x._head && x._level >= 0);

        // Initial attributes
        _level = x._level;
        _head = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(_head, 0, sizeof(Node*) * (_level + 1));

        // Copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (_level + 1));
        Node *n = x._head[0];
        while (NULL != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) Node(n->_key);
            c->_level = n->_level;
            c->_next = (Node**) ::malloc(sizeof(Node*) * (c->_level + 1));
            algo_type::insert_node(c, *this, pre_lv);
            for (size_t i = 0; i <= c->_level; ++i)
                pre_lv[i] = c;

            n = n->_next[0];
        }
        ::free(pre_lv);
        _size = x._size;
    }

    SkipListSet(self_type&& x)
    {
        _level = x._level;
        _head = x._head;
        _size = x._size;
        x._level = -1;
        x._head = NULL;
        x._size = 0;
    }

    ~SkipListSet()
    {
        clear();
        if (NULL != _head)
            ::free(_head);
        _head = NULL;
        _level = -1;
    }

    self_type& operator=(const self_type& x)
    {
        if (this == &x)
            return *this;

        // Clear memory
        clear();
        if (x._size == 0)
            return *this;
        assert(NULL != x._head && x._level >= 0);

        // Initial attributes
        if (NULL != _head)
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
        while (NULL != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) Node(n->_key);
            c->_level = n->_level;
            c->_next = (Node**) ::malloc(sizeof(Node*) * (c->_level + 1));
            algo_type::insert_node(c, *this, pre_lv);
            for (size_t i = 0; i <= c->_level; ++i)
                pre_lv[i] = c;

            n = n->_next[0];
        }
        ::free(pre_lv);
        _size = x._size;

        return *this;
    }

    self_type& operator=(self_type&& x)
    {
        if (this == &x)
            return *this;
        _level = x._level;
        _head = x._head;
        _size = x._size;
        x._level = -1;
        x._head = NULL;
        x._size = 0;
        return *this;
    }

    bool operator==(const self_type& x) const
    {
        if (this == &x)
            return true;
        if (_size != x._size)
            return false;
        if (0 == _size)
            return true;
        assert(NULL != _head && _level >= 0 && NULL != x._head && x._level >= 0);

        Node *current1 = _head[0], current2 = x._head[0];
        while (NULL != current1)
        {
            assert(NULL != current2);
            if (current1->_key != current2->_key)
                return false;
            current1 = current1->_next[0];
            current2 = current2->_next[0];
        }
        assert(NULL == current2);
        return true;
    }

    bool operator!=(const self_type& x) const
    {
        return !(*this == x);
    }

    size_t size() const
    {
        return _size;
    }

    void clear()
    {
        if (0 == _size)
            return;
        assert(NULL != _head && _level >= 0);

        Node *current = _head[0];
        while (NULL != current)
        {
            Node *next = current->get_next(0);
            current->~Node();
            ::free(current);
            current = next;
        }
        ::memset(_head, 0, sizeof(Node*) * (_level + 1));
        _size = 0;
    }

    bool contains(const T& k) const
    {
        if (0 == _size)
            return false;
        assert(NULL != _head && _level >= 0);

        return NULL != algo_type::search_node(k, *this, NULL);
    }

    bool add(const T& k)
    {
        if (NULL == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(k);
            _head = (Node**) ::malloc(sizeof(Node*) * 1);
            _level = 0;
            _head[0] = n;
            n->set_level(0);
            n->set_next(0, NULL);
            _size = 1;
            return true;
        }
        assert(_level >= 0);

        // Search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (_level + 1));
        Node *n = algo_type::search_node(k, *this, pre_lv);
        if (NULL != n)
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

    bool remove(const T& k)
    {
        if (0 == _size)
            return false;
        assert(NULL != _head && _level >= 0);

        // Search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (_level + 1));
        Node *n = algo_type::search_node(k, *this, pre_lv);
        if (NULL == n)
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
};

}

#endif
