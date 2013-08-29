/**
 * @file -
 * @author jingqi
 * @date 2013-08-29
 * @last-edit 2013-08-29 14:34:59 jingqi
 * @brief
 */

#ifndef ___HEADFILE_40DE4FAF_9BB0_4CF2_A78E_8FB1F58E09D3_
#define ___HEADFILE_40DE4FAF_9BB0_4CF2_A78E_8FB1F58E09D3_

#include <stdlib.h>
#include <new>

#include "skiplist.hpp"

namespace nut
{

template <typename K, typename V>
class SkipListMap
{
    // 最大 level 数, >0
    enum { MAX_LEVEL = 16 };
    
    class Node
    {
        K m_key;
        V m_value;
        Node **m_next;
        int m_level; // 0-based
        
    public:
        Node(const K& k, const V& v)
            : m_key(k), m_value(v), m_next(NULL), m_level(-1)
        {}
        
        ~Node()
        {
            if (NULL != m_next)
                ::free(m_next);
            m_next = NULL;
            m_level = -1;
        }

        inline const K& getKey() const
        {
            return m_key;
        }

        inline int getLevel() const
        {
            return m_level;
        }

        void setLevel(int lv)
        {
            assert(lv >= 0);
            if (NULL != m_next)
            {
                assert(m_level >= 0);
                m_next = (Node**) ::realloc(m_next, sizeof(Node*) * (lv + 1));
                if (lv > m_level)
                    ::memset(m_next + m_level + 1, 0, sizeof(Node*) * (lv - m_level));
            }
            else
            {
                assert(m_level < 0);
                m_next = (Node**) ::malloc(sizeof(Node*) * (lv + 1));
                ::memset(m_next, 0, sizeof(Node*) * (lv + 1));
            }
            m_level = lv;
        }
        
        inline Node* getNext(int lv) const
        {
            assert(NULL != m_next && 0 <= lv && lv <= m_level);
            return m_next[lv];
        }

        inline void setNext(int lv, Node *n)
        {
            assert(NULL != m_next && 0 <= lv && lv <= m_level);
            m_next[lv] = n;
        }
    };
    
    int m_level; // 0-based
    Node **m_head;
    size_t m_size;
    
private:
    typedef SkipList<K,Node,SkipListMap<K,V> > algo_t;
    friend class SkipList<K,Node,SkipListMap<K,V> >;

    inline int getLevel() const
    {
        return m_level;
    }

    void setLevel(int lv)
    {
        assert(lv >= 0);
        if (NULL != m_head)
        {
            assert(m_level >= 0);
            m_head = (Node**) ::realloc(m_head, sizeof(Node*) * (lv + 1));
            if (lv > m_level)
                ::memset(m_head + m_level + 1, 0, sizeof(Node*) * (lv - m_level));
        }
        else
        {
            assert(m_level < 0);
            m_head = (Node**) ::malloc(sizeof(Node*) * (lv + 1));
            ::memset(m_head, 0, sizeof(Node*) * (lv + 1));
        }
        m_level = lv;
    }
    
    inline Node* getHead(int lv) const
    {
        assert(NULL != m_head && 0 <= lv && lv <= m_level);
        return m_head[lv];
    }
    
    inline void setHead(int lv, Node *n)
    {
        assert(NULL != m_head && 0 <= lv && lv <= m_level);
        m_head[lv] = n;
    }

public:
    SkipListMap()
        : m_level(-1), m_head(NULL), m_size(0)
    {}

    SkipListMap(const SkipListMap<K,V>& x)
        : m_level(-1), m_head(NULL), m_size(0)
    {
        if (x.m_size == 0)
            return;
        assert(NULL != x.m_head && x.m_level >= 0);

        // initial attributes
        m_level = x.m_level;
        m_head = (Node**) ::malloc(sizeof(Node*) * (m_level + 1));
        ::memset(m_head, 0, sizeof(Node*) * (m_level + 1));

        // copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (m_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (m_level + 1));
        Node *n = x.m_head[0];
        while (NULL != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) Node(n->m_key, n->m_value);
            c->m_level = n->m_level;
            c->m_next = (Node**) ::malloc(sizeof(Node*) * (c->m_level + 1));
            algo_t::insertNode(c, *this, pre_lv);
            for (register size_t i = 0; i <= c->m_level; ++i)
                pre_lv[i] = c;

            n = n->m_next[0];
        }
        ::free(pre_lv);
        m_size = x.m_size;
    }

    ~SkipListMap()
    {
        clear();
        if (NULL != m_head)
            ::free(m_head);
        m_head = NULL;
        m_level = -1;
    }

    SkipListMap<K,V>& operator=(const SkipListMap<K,V>& x)
    {
        if (this == &x)
            return *this;

        // clear memory
        clear();
        if (x.m_size == 0)
            return *this;
        assert(NULL != x.m_head && x.m_level >= 0);

        // initial attributes
        if (NULL != m_head)
        {
            assert(m_level >= 0);
            m_head = (Node**) ::realloc(m_head, sizeof(Node*) * (x.m_level + 1));
        }
        else
        {
            assert(m_level < 0);
            m_head = (Node**) ::malloc(sizeof(Node*) * (x.m_level + 1));
        }
        m_level = x.m_level;
        ::memset(m_head, 0, sizeof(Node*) * (m_level + 1));

        // copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (m_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (m_level + 1));
        Node *n = x.m_head[0];
        while (NULL != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) Node(n->m_key, n->m_value);
            c->m_level = n->m_level;
            c->m_next = (Node**) ::malloc(sizeof(Node*) * (c->m_level + 1));
            algo_t::insertNode(c, *this, pre_lv);
            for (register size_t i = 0; i <= c->m_level; ++i)
                pre_lv[i] = c;

            n = n->m_next[0];
        }
        ::free(pre_lv);
        m_size = x.m_size;

        return *this;
    }

    bool operator==(const SkipListMap<K,V>& x) const
    {
        if (this == &x)
            return true;
        if (m_size != x.m_size)
            return false;
        if (0 == m_size)
            return true;
        assert(NULL != m_head && m_level >= 0 && NULL != x.m_head && x.m_level >= 0);

        Node *current1 = m_head[0], current2 = x.m_head[0];
        while (NULL != current1)
        {
            assert(NULL != current2);
            if (current1->m_key != current2->m_key || current1->m_value != current2->m_value)
                return false;
            current1 = current1->m_next[0];
            current2 = current2->m_next[0];
        }
        assert(NULL == current2);
        return true;
    }

    bool operator!=(const SkipListMap<K,V>& x) const
    {
        return !(*this == x);
    }

    inline size_t size() const
    {
        return m_size;
    }

    void clear()
    {
        if (0 == m_size)
            return;
        assert(NULL != m_head && m_level >= 0);

        Node *current = m_head[0];
        while (NULL != current)
        {
            Node *next = current->getNext(0);
            current->~Node();
            ::free(current);
            current = next;
        }
        ::memset(m_head, 0, sizeof(Node*) * (m_level + 1));
        m_size = 0;
    }

    bool containsKey(const K& k) const
    {
        if (0 == m_size)
            return false;
        assert(NULL != m_head && m_level >= 0);

        return NULL != algo_t::searchNode(k, *this, NULL);
    }

    bool add(const K& k, const V& v)
    {
        if (NULL == m_head)
        {
            assert(m_level < 0 && m_size == 0);
            Node *n = (Node*) ::malloc(sizeof(Node));
            new (n) Node(k,v);
            m_head = (Node**) ::malloc(sizeof(Node*) * 1);
            m_level = 0;
            m_head[0] = n;
            n->setLevel(0);
            n->setNext(0, NULL);
            m_size = 1;
            return true;
        }
        assert(m_level >= 0);

        // search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (m_level + 1));
        Node *n = algo_t::searchNode(k, *this, pre_lv);
        if (NULL != n)
        {
            ::free(pre_lv);
            return false;
        }

        // insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) Node(k,v);
        algo_t::insertNode(n, *this, pre_lv);
        ::free(pre_lv);
        ++m_size;
        return true;
    }

    bool remove(const K& k)
    {
        if (0 == m_size)
            return false;
        assert(NULL != m_head && m_level >= 0);

        // search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (m_level + 1));
        Node *n = algo_t::searchNode(k, *this, pre_lv);
        if (NULL == n)
        {
            ::free(pre_lv);
            return false;
        }

        // remove
        algo_t::removeNode(n, *this, pre_lv);
        ::free(pre_lv);
        n->~Node();
        ::free(n);
        --m_size;
        return true;
    }
};

}

#endif
