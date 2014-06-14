/**
 * @file -
 * @author jingqi
 * @date 2012-12-15
 * @last-edit 2012-12-15 13:58:50 jingqi
 * @brief
 */

#ifndef ___HEADFILE_CBBC3AB5_C145_43C1_AAD4_5149A52AF3DD_
#define ___HEADFILE_CBBC3AB5_C145_43C1_AAD4_5149A52AF3DD_

#include <assert.h>
#include <stdlib.h>
#include <map>

#include <nut/threading/sync/spinlock.hpp>
#include <nut/threading/sync/guard.hpp>

namespace nut
{

/**
 * most-recently-used cache
 */
template <typename K, typename V>
class MRUCache
{
    struct Node
    {
        K key;
        V value;
        Node *pre, *next;

        Node(const K& k, const V& v)
            : key(k), value(v), pre(NULL), next(NULL)
        {}
    };

    enum { DEFAULT_CAPACITY = 50 };

    const size_t m_capacity;
    std::map<K,Node*> m_map;
    Node *m_listHead, *m_listEnd;
    SpinLock m_lock; // 注意，linux下自旋锁不可重入

    static inline Node* alloc_node()
    {
        return (Node*)::malloc(sizeof(Node));
    }

    static inline void dealloc_node(Node *p)
    {
        assert(NULL != p);
        ::free(p);
    }

    static inline Node* new_node(const K& k, const V& v)
    {
        Node *p = alloc_node();
        assert(NULL != p);
        new (p) Node(k,v);
        return p;
    }

    static inline void delete_node(Node *p)
    {
        assert(NULL != p);
        p->~Node();
        dealloc_node(p);
    }

    inline void removeListNode(Node *p)
    {
        assert(NULL != p);
        if (NULL != p->pre)
            p->pre->next = p->next;
        else
            m_listHead = p->next;

        if (NULL != p->next)
            p->next->pre = p->pre;
        else
            m_listEnd = p->pre;
    }

    inline void pushListHead(Node *p)
    {
        assert(NULL != p);
        p->next = m_listHead;
        p->pre = NULL;
        if (NULL != m_listHead)
            m_listHead->pre = p;
        else
            m_listEnd = p;
        m_listHead = p;
    }

public:
    MRUCache()
        : m_capacity(DEFAULT_CAPACITY), m_listHead(NULL), m_listEnd(NULL)
    {}

    MRUCache(size_t cap)
        : m_capacity(cap), m_listHead(NULL), m_listEnd(NULL)
    {
        assert(cap > 0);
    }
    
    void put(const K& k, const V& v)
    {
        Guard<SpinLock> g(&m_lock);

        typename std::map<K,Node*>::const_iterator const n = m_map.find(k);
        if (n == m_map.end())
        {
            Node* const p = new_node(k,v);
            m_map.insert(std::pair<K,Node*>(k,p));
            while (m_map.size() > m_capacity)
            {
                assert(NULL != m_listEnd);
                typename std::map<K,Node*>::iterator const nn = m_map.find(m_listEnd->key);
                assert(nn != m_map.end());
                Node* const pp = nn->second;
                m_map.erase(nn);
                removeListNode(pp);
                delete_node(pp);
            }
            pushListHead(p);
            return;
        }

        n->second->value = v;
        removeListNode(n->second);
        pushListHead(n->second);
    }

    void remove(const K& k)
    {
        Guard<SpinLock> g(&m_lock);

        typename std::map<K,Node*>::iterator const n = m_map.find(k);
        if (n == m_map.end())
            return;
        Node* const p = n->second;
        m_map.erase(n);
        removeListNode(p);
        delete_node(p);
    }

    bool get(const K& k, V *out)
    {
        assert(NULL != out);
        Guard<SpinLock> g(&m_lock);

        typename std::map<K,Node*>::const_iterator const n = m_map.find(k);
        if (n == m_map.end())
            return false;
        *out = n->second->value;
        return true;
    }

    void clear()
    {
        Guard<SpinLock> g(&m_lock);

        Node *p = m_listHead;
        while (NULL != p)
        {
            Node *n = p->next;
            delete_node(p);
            p = n;
        }
        m_listHead = NULL;
        m_listEnd = NULL;
        m_map.clear();
    }
};

}

#endif


