
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
        void *data = NULL;
        size_t size = 0;
        Node *pre = NULL, *next = NULL;

        Node(const K& k, const void *buf, size_t cb)
            : key(k), size(cb)
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

    size_t _bytes_size = 0, _bytes_capacity = 0;
    map_t _map;
    Node *_list_head = NULL, *_list_end = NULL;
    SpinLock _lock; // 注意，linux下自旋锁不可重入

#ifndef NDEBUG
    size_t m_hit_count = 0, m_hit_size = 0, _miss_count = 0;
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
            _list_head = p->next;

        if (NULL != p->next)
            p->next->pre = p->pre;
        else
            _list_end = p->pre;
    }

    void push_list_head(Node *p)
    {
        assert(NULL != p);
        p->next = _list_head;
        p->pre = NULL;
        if (NULL != _list_head)
            _list_head->pre = p;
        else
            _list_end = p;
        _list_head = p;
    }

private:
    // invalid methods
    LRUDataCache(const LRUDataCache<K>&);
    LRUDataCache<K>& operator=(const LRUDataCache<K>&);

public:
    LRUDataCache()
        : _bytes_capacity(DEFAULT_BYTES_CAPACITY)
    {}

    LRUDataCache(size_t bytes_capacity)
        : _bytes_capacity(bytes_capacity)
    {
        assert(bytes_capacity > 0);
    }

    ~LRUDataCache()
    {
        clear();
    }

    size_t size() const
    {
        return _map.size();
    }

    size_t bytes_size() const
    {
        return _bytes_size;
    }

    size_t bytes_capacity() const
    {
        return _bytes_capacity;
    }

    void set_bytes_capacity(size_t bytes_capacity)
    {
        assert(bytes_capacity > 0);
        _bytes_capacity = bytes_capacity;
    }

    void put(const K& k, const void *buf, size_t cb)
    {
        assert(NULL != buf || 0 == cb);
        Guard<SpinLock> g(&_lock);

        typename map_t::const_iterator const n = _map.find(k);
        if (n == _map.end())
        {
            Node *const p = new_node(k, buf, cb);
            _map.insert(std::pair<K,Node*>(k,p));
            push_list_head(p);
            _bytes_size += cb;

            while (_bytes_size > _bytes_capacity)
            {
                assert(NULL != _list_end);
                typename map_t::iterator const nn = _map.find(_list_end->key);
                assert(nn != _map.end());
                Node *const pp = nn->second;
                assert(NULL != pp && _bytes_size >= pp->size);
                _bytes_size -= pp->size;
                _map.erase(nn);
                remove_from_list(pp);
                delete_node(pp);
            }
            return;
        }

        Node *const p = n->second;
        assert(NULL != p && _bytes_size >= p->size);
        _bytes_size -= p->size;
        p->copy_from(buf, cb);
        remove_from_list(p);
        push_list_head(p);
        _bytes_size += cb;
    }

    void remove(const K& k)
    {
        Guard<SpinLock> g(&_lock);

        typename map_t::iterator const n = _map.find(k);
        if (n == _map.end())
            return;

        Node *const p = n->second;
        assert(NULL != p && _bytes_size >= p->size);
        _bytes_size -= p->size;
        _map.erase(n);
        remove_from_list(p);
        delete_node(p);
    }

    bool has_key(const K& k)
    {
        return _map.find(k) != _map.end();
    }

    bool get(const K& k, const void **pdata, size_t *psize)
    {
        assert(NULL != pdata && NULL != psize);
        Guard<SpinLock> g(&_lock);

        typename map_t::const_iterator const n = _map.find(k);
        if (n == _map.end())
        {
#ifndef NDEBUG
            ++_miss_count;
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
        Guard<SpinLock> g(&_lock);

        Node *p = _list_head;
        while (NULL != p)
        {
            Node *const n = p->next;
            delete_node(p);
            p = n;
        }
        _list_head = NULL;
        _list_end = NULL;
        _map.clear();
        _bytes_size = 0;

#ifndef NDEBUG
        m_hit_count = 0;
        m_hit_size = 0;
        _miss_count = 0;
#endif
    }
};


}

#endif
