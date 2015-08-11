
#ifndef ___HEADFILE_40DE4FAF_9BB0_4CF2_A78E_8FB1F58E09D3_
#define ___HEADFILE_40DE4FAF_9BB0_4CF2_A78E_8FB1F58E09D3_

#include <stdlib.h>
#include <new>

#include "skiplist.h"

namespace nut
{

template <typename K, typename V>
class SkipListMap
{
    // 最大 level 数, >0
    enum { MAX_LEVEL = 16 };

    class Node
    {
        K _key;
        V _value;
        Node **_next = NULL;
        int _level = -1; // 0-based

    public:
        Node(const K& k, const V& v)
            : _key(k), _value(v)
        {}

        ~Node()
        {
            if (NULL != _next)
                ::free(_next);
            _next = NULL;
            _level = -1;
        }

        const K& get_key() const
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
    typedef SkipList<K,Node,SkipListMap<K,V> > algo_t;
    friend class SkipList<K,Node,SkipListMap<K,V> >;

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
    SkipListMap()
    {}

    SkipListMap(const SkipListMap<K,V>& x)
    {
        if (x._size == 0)
            return;
        assert(NULL != x._head && x._level >= 0);

        // initial attributes
        _level = x._level;
        _head = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(_head, 0, sizeof(Node*) * (_level + 1));

        // copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (_level + 1));
        Node *n = x._head[0];
        while (NULL != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) Node(n->_key, n->_value);
            c->_level = n->_level;
            c->_next = (Node**) ::malloc(sizeof(Node*) * (c->_level + 1));
            algo_t::insert_node(c, *this, pre_lv);
            for (size_t i = 0; i <= c->_level; ++i)
                pre_lv[i] = c;

            n = n->_next[0];
        }
        ::free(pre_lv);
        _size = x._size;
    }

    ~SkipListMap()
    {
        clear();
        if (NULL != _head)
            ::free(_head);
        _head = NULL;
        _level = -1;
    }

    SkipListMap<K,V>& operator=(const SkipListMap<K,V>& x)
    {
        if (this == &x)
            return *this;

        // clear memory
        clear();
        if (x._size == 0)
            return *this;
        assert(NULL != x._head && x._level >= 0);

        // initial attributes
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

        // copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (_level + 1));
        Node *n = x._head[0];
        while (NULL != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) Node(n->_key, n->_value);
            c->_level = n->_level;
            c->_next = (Node**) ::malloc(sizeof(Node*) * (c->_level + 1));
            algo_t::insert_node(c, *this, pre_lv);
            for (size_t i = 0; i <= c->_level; ++i)
                pre_lv[i] = c;

            n = n->_next[0];
        }
        ::free(pre_lv);
        _size = x._size;

        return *this;
    }

    bool operator==(const SkipListMap<K,V>& x) const
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
            if (current1->_key != current2->_key || current1->_value != current2->_value)
                return false;
            current1 = current1->_next[0];
            current2 = current2->_next[0];
        }
        assert(NULL == current2);
        return true;
    }

    bool operator!=(const SkipListMap<K,V>& x) const
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

    bool contains_key(const K& k) const
    {
        if (0 == _size)
            return false;
        assert(NULL != _head && _level >= 0);

        return NULL != algo_t::search_node(k, *this, NULL);
    }

    bool add(const K& k, const V& v)
    {
        if (NULL == _head)
        {
            assert(_level < 0 && _size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(k,v);
            _head = (Node**) ::malloc(sizeof(Node*) * 1);
            _level = 0;
            _head[0] = n;
            n->set_level(0);
            n->set_next(0, NULL);
            _size = 1;
            return true;
        }
        assert(_level >= 0);

        // search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (_level + 1));
        Node *n = algo_t::search_node(k, *this, pre_lv);
        if (NULL != n)
        {
            ::free(pre_lv);
            return false;
        }

        // insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(k,v);
        algo_t::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++_size;
        return true;
    }

    bool remove(const K& k)
    {
        if (0 == _size)
            return false;
        assert(NULL != _head && _level >= 0);

        // search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (_level + 1));
        Node *n = algo_t::search_node(k, *this, pre_lv);
        if (NULL == n)
        {
            ::free(pre_lv);
            return false;
        }

        // remove
        algo_t::remove_node(n, *this, pre_lv);
        ::free(pre_lv);
        n->~Node();
        ::free(n);
        --_size;
        return true;
    }
};

}

#endif
