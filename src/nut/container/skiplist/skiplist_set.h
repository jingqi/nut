
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
        Node **_next;
        int _level; // 0-based

    public:
        Node(const T& k)
            : _key(k), _next(NULL), _level(-1)
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

    int _level; // 0-based
    Node **m_head;
    size_t m_size;

private:
    typedef SkipList<T,Node,SkipListSet<T> > algo_t;
    friend class SkipList<T,Node,SkipListSet<T> >;

    int get_level() const
    {
        return _level;
    }

    void set_level(int lv)
    {
        assert(lv >= 0);
        if (NULL != m_head)
        {
            assert(_level >= 0);
            m_head = (Node**) ::realloc(m_head, sizeof(Node*) * (lv + 1));
            if (lv > _level)
                ::memset(m_head + _level + 1, 0, sizeof(Node*) * (lv - _level));
        }
        else
        {
            assert(_level < 0);
            m_head = (Node**) ::malloc(sizeof(Node*) * (lv + 1));
            ::memset(m_head, 0, sizeof(Node*) * (lv + 1));
        }
        _level = lv;
    }

    Node* get_head(int lv) const
    {
        assert(NULL != m_head && 0 <= lv && lv <= _level);
        return m_head[lv];
    }

    void set_head(int lv, Node *n)
    {
        assert(NULL != m_head && 0 <= lv && lv <= _level);
        m_head[lv] = n;
    }

public:
    SkipListSet()
        : _level(-1), m_head(NULL), m_size(0)
    {}

    SkipListSet(const SkipListSet<T>& x)
        : _level(-1), m_head(NULL), m_size(0)
    {
        if (x.m_size == 0)
            return;
        assert(NULL != x.m_head && x._level >= 0);

        // initial attributes
        _level = x._level;
        m_head = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(m_head, 0, sizeof(Node*) * (_level + 1));

        // copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (_level + 1));
        Node *n = x.m_head[0];
        while (NULL != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) Node(n->_key);
            c->_level = n->_level;
            c->_next = (Node**) ::malloc(sizeof(Node*) * (c->_level + 1));
            algo_t::insert_node(c, *this, pre_lv);
            for (size_t i = 0; i <= c->_level; ++i)
                pre_lv[i] = c;

            n = n->_next[0];
        }
        ::free(pre_lv);
        m_size = x.m_size;
    }

    ~SkipListSet()
    {
        clear();
        if (NULL != m_head)
            ::free(m_head);
        m_head = NULL;
        _level = -1;
    }

    SkipListSet<T>& operator=(const SkipListSet<T>& x)
    {
        if (this == &x)
            return *this;

        // clear memory
        clear();
        if (x.m_size == 0)
            return *this;
        assert(NULL != x.m_head && x._level >= 0);

        // initial attributes
        if (NULL != m_head)
        {
            assert(_level >= 0);
            m_head = (Node**) ::realloc(m_head, sizeof(Node*) * (x._level + 1));
        }
        else
        {
            assert(_level < 0);
            m_head = (Node**) ::malloc(sizeof(Node*) * (x._level + 1));
        }
        _level = x._level;
        ::memset(m_head, 0, sizeof(Node*) * (_level + 1));

        // copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (_level + 1));
        Node *n = x.m_head[0];
        while (NULL != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) Node(n->_key);
            c->_level = n->_level;
            c->_next = (Node**) ::malloc(sizeof(Node*) * (c->_level + 1));
            algo_t::insert_node(c, *this, pre_lv);
            for (size_t i = 0; i <= c->_level; ++i)
                pre_lv[i] = c;

            n = n->_next[0];
        }
        ::free(pre_lv);
        m_size = x.m_size;

        return *this;
    }

    bool operator==(const SkipListSet<T>& x) const
    {
        if (this == &x)
            return true;
        if (m_size != x.m_size)
            return false;
        if (0 == m_size)
            return true;
        assert(NULL != m_head && _level >= 0 && NULL != x.m_head && x._level >= 0);

        Node *current1 = m_head[0], current2 = x.m_head[0];
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

    bool operator!=(const SkipListSet<T>& x) const
    {
        return !(*this == x);
    }

    size_t size() const
    {
        return m_size;
    }

    void clear()
    {
        if (0 == m_size)
            return;
        assert(NULL != m_head && _level >= 0);

        Node *current = m_head[0];
        while (NULL != current)
        {
            Node *next = current->get_next(0);
            current->~Node();
            ::free(current);
            current = next;
        }
        ::memset(m_head, 0, sizeof(Node*) * (_level + 1));
        m_size = 0;
    }

    bool contains(const T& k) const
    {
        if (0 == m_size)
            return false;
        assert(NULL != m_head && _level >= 0);

        return NULL != algo_t::search_node(k, *this, NULL);
    }

    bool add(const T& k)
    {
        if (NULL == m_head)
        {
            assert(_level < 0 && m_size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(k);
            m_head = (Node**) ::malloc(sizeof(Node*) * 1);
            _level = 0;
            m_head[0] = n;
            n->set_level(0);
            n->set_next(0, NULL);
            m_size = 1;
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
        new (n) Node(k);
        algo_t::insert_node(n, *this, pre_lv);
        ::free(pre_lv);
        ++m_size;
        return true;
    }

    bool remove(const T& k)
    {
        if (0 == m_size)
            return false;
        assert(NULL != m_head && _level >= 0);

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
        --m_size;
        return true;
    }
};

}

#endif
