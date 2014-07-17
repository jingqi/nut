/**
 * @file -
 * @author jingqi
 * @date 2012-12-15
 * @last-edit 2014-07-17 21:14:18 jingqi
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
class LRUCache
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
    typedef std::map<K,Node*> map_t;

    const size_t m_capacity;
    map_t m_map;
    Node *m_list_head, *m_list_end;
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

    inline void removeFromList(Node *p)
    {
        assert(NULL != p);
        if (NULL != p->pre)
            p->pre->next = p->next;
        else
            m_list_head = p->next;

        if (NULL != p->next)
            p->next->pre = p->pre;
        else
            m_list_end = p->pre;
    }

    inline void pushListHead(Node *p)
    {
        assert(NULL != p);
        p->next = m_list_head;
        p->pre = NULL;
        if (NULL != m_list_head)
            m_list_head->pre = p;
        else
            m_list_end = p;
        m_list_head = p;
    }

    LRUCache(const LRUCache<K,V>&);
    LRUCache<K,V>& operator=(const LRUCache<K,V>&);

public:
    LRUCache()
        : m_capacity(DEFAULT_CAPACITY), m_list_head(NULL), m_list_end(NULL)
    {}

    LRUCache(size_t capacity)
        : m_capacity(capacity), m_list_head(NULL), m_list_end(NULL)
    {
        assert(capacity > 0);
    }

    ~LRUCache()
    {
        clear();
    }

    void put(const K& k, const V& v)
    {
        Guard<SpinLock> g(&m_lock);

        typename map_t::const_iterator const n = m_map.find(k);
        if (n == m_map.end())
        {
            Node *const p = new_node(k,v);
            m_map.insert(std::pair<K,Node*>(k,p));
            pushListHead(p);

            while (m_map.size() > m_capacity)
            {
                assert(NULL != m_list_end);
                typename map_t::iterator const nn = m_map.find(m_list_end->key);
                assert(nn != m_map.end());
                Node *const pp = nn->second;
                m_map.erase(nn);
                removeFromList(pp);
                delete_node(pp);
            }
            return;
        }

        Node *const p = n->second;
        p->value = v;
        removeFromList(p);
        pushListHead(p);
    }

    void remove(const K& k)
    {
        Guard<SpinLock> g(&m_lock);

        typename map_t::iterator const n = m_map.find(k);
        if (n == m_map.end())
            return;
        Node *const p = n->second;
        m_map.erase(n);
        removeFromList(p);
        delete_node(p);
    }

    bool get(const K& k, V *out)
    {
        assert(NULL != out);
        Guard<SpinLock> g(&m_lock);

        typename map_t::const_iterator const n = m_map.find(k);
        if (n == m_map.end())
            return false;
        *out = n->second->value;
        return true;
    }

    void clear()
    {
        Guard<SpinLock> g(&m_lock);

        Node *p = m_list_head;
        while (NULL != p)
        {
            Node *n = p->next;
            delete_node(p);
            p = n;
        }
        m_list_head = NULL;
        m_list_end = NULL;
        m_map.clear();
    }
};

}

#endif
