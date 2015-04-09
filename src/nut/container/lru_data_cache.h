
#ifndef ___HEADFILE_B81E878E_513C_4792_A8F8_73215B57ACBE_
#define ___HEADFILE_B81E878E_513C_4792_A8F8_73215B57ACBE_

#include <assert.h>
#include <stdlib.h>
#include <map>

#include <nut/threading/sync/spinlock.h>
#include <nut/threading/sync/guard.h>

namespace nut
{

/**
 * most-recently-used data cache
 */
template <typename K>
class LRUDataCache
{
    struct Node
    {
        K key;
        void *data;
        size_t size;
        Node *pre, *next;

        Node(const K& k, const void *buf, size_t cb)
            : key(k), data(NULL), size(cb), pre(NULL), next(NULL)
        {
            assert(NULL != buf || 0 == cb);
            if (cb > 0)
            {
                data = ::malloc(cb);
                assert(NULL != data);
                ::memcpy(data, buf, cb);
            }
        }

        ~Node()
        {
            clear();
        }

        void copy_from(const void *buf, size_t cb)
        {
            assert(NULL != buf || 0 == cb);
            clear();
            if (cb > 0)
            {
                data = ::malloc(cb);
                assert(NULL != data);
                ::memcpy(data, buf, cb);
                size = cb;
            }
        }

        void fast_set(void *buf, size_t cb)
        {
            assert(NULL != buf || 0 == cb);
            clear();
            data = buf;
            size = cb;
        }

        void clear()
        {
            if (NULL != data)
                ::free(data);
            data = NULL;
            size = 0;
        }

    private:
        // invalid methods
        Node(const Node&);
        Node& operator=(const Node&);
    };

    enum { DEFAULT_BYTES_CAPACITY = 5 * 1024 * 1024 }; // 单位: 字节
    typedef std::map<K,Node*> map_t;

    size_t m_bytes_size, m_bytes_capacity;
    map_t m_map;
    Node *m_list_head, *m_list_end;
    SpinLock m_lock; // 注意，linux下自旋锁不可重入

#ifndef NDEBUG
    size_t m_hit_count, m_hit_size, m_miss_count;
#endif

    static Node* alloc_node()
    {
        return (Node*)::malloc(sizeof(Node));
    }

    static void dealloc_node(Node *p)
    {
        assert(NULL != p);
        ::free(p);
    }

    static Node* new_node(const K& k, const void *buf, size_t cb)
    {
        Node *p = alloc_node();
        assert(NULL != p);
        new (p) Node(k, buf, cb);
        return p;
    }

    static void delete_node(Node *p)
    {
        assert(NULL != p);
        p->~Node();
        dealloc_node(p);
    }

    void remove_from_list(Node *p)
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

    void push_list_head(Node *p)
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

private:
    // invalid methods
    LRUDataCache(const LRUDataCache<K>&);
    LRUDataCache<K>& operator=(const LRUDataCache<K>&);

public:
    LRUDataCache()
        : m_bytes_size(0), m_bytes_capacity(DEFAULT_BYTES_CAPACITY), m_list_head(NULL), m_list_end(NULL)
#ifndef NDEBUG
        , m_hit_count(0), m_hit_size(0), m_miss_count(0)
#endif
    {}

    LRUDataCache(size_t bytes_capacity)
        : m_bytes_size(0), m_bytes_capacity(bytes_capacity), m_list_head(NULL), m_list_end(NULL)
#ifndef NDEBUG
        , m_hit_count(0), m_hit_size(0), m_miss_count(0)
#endif
    {
        assert(bytes_capacity > 0);
    }

    ~LRUDataCache()
    {
        clear();
    }

    size_t size() const
    {
        return m_map.size();
    }

    size_t bytes_size() const
    {
        return m_bytes_size;
    }

    size_t bytes_capacity() const
    {
        return m_bytes_capacity;
    }

    void set_bytes_capacity(size_t bytes_capacity)
    {
        assert(bytes_capacity > 0);
        m_bytes_capacity = bytes_capacity;
    }

    void put(const K& k, const void *buf, size_t cb)
    {
        assert(NULL != buf || 0 == cb);
        Guard<SpinLock> g(&m_lock);

        typename map_t::const_iterator const n = m_map.find(k);
        if (n == m_map.end())
        {
            Node *const p = new_node(k, buf, cb);
            m_map.insert(std::pair<K,Node*>(k,p));
            push_list_head(p);
            m_bytes_size += cb;

            while (m_bytes_size > m_bytes_capacity)
            {
                assert(NULL != m_list_end);
                typename map_t::iterator const nn = m_map.find(m_list_end->key);
                assert(nn != m_map.end());
                Node *const pp = nn->second;
                assert(NULL != pp && m_bytes_size >= pp->size);
                m_bytes_size -= pp->size;
                m_map.erase(nn);
                remove_from_list(pp);
                delete_node(pp);
            }
            return;
        }

        Node *const p = n->second;
        assert(NULL != p && m_bytes_size >= p->size);
        m_bytes_size -= p->size;
        p->copy_from(buf, cb);
        remove_from_list(p);
        push_list_head(p);
        m_bytes_size += cb;
    }

    void remove(const K& k)
    {
        Guard<SpinLock> g(&m_lock);

        typename map_t::iterator const n = m_map.find(k);
        if (n == m_map.end())
            return;

        Node *const p = n->second;
        assert(NULL != p && m_bytes_size >= p->size);
        m_bytes_size -= p->size;
        m_map.erase(n);
        remove_from_list(p);
        delete_node(p);
    }

    bool has_key(const K& k)
    {
        return m_map.find(k) != m_map.end();
    }

    bool get(const K& k, const void **pdata, size_t *psize)
    {
        assert(NULL != pdata && NULL != psize);
        Guard<SpinLock> g(&m_lock);

        typename map_t::const_iterator const n = m_map.find(k);
        if (n == m_map.end())
        {
#ifndef NDEBUG
            ++m_miss_count;
#endif
            return false;
        }

        Node *const p = n->second;
        assert(NULL != p);
        *pdata = p->data;
        *psize = p->size;
        remove_from_list(p);
        push_list_head(p);

#ifndef NDEBUG
        ++m_hit_count;
        m_hit_size += p->size;
#endif
        return true;
    }

    void clear()
    {
        Guard<SpinLock> g(&m_lock);

        Node *p = m_list_head;
        while (NULL != p)
        {
            Node *const n = p->next;
            delete_node(p);
            p = n;
        }
        m_list_head = NULL;
        m_list_end = NULL;
        m_map.clear();
        m_bytes_size = 0;

#ifndef NDEBUG
        m_hit_count = 0;
        m_hit_size = 0;
        m_miss_count = 0;
#endif
    }
};


}

#endif
