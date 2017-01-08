
#ifndef ___HEADFILE_CBBC3AB5_C145_43C1_AAD4_5149A52AF3DD_
#define ___HEADFILE_CBBC3AB5_C145_43C1_AAD4_5149A52AF3DD_

#include <assert.h>
#include <stdlib.h>
#include <map>

#include <nut/threading/sync/spinlock.h>
#include <nut/threading/sync/guard.h>

namespace nut
{

/**
 * least-recently-used cache
 */
template <typename K, typename V>
class LRUCache
{
    struct Node
    {
        K key;
        V value;
        Node *pre = NULL, *next = NULL;

        Node(const K& k, V&& v)
            : key(k), value(std::forward<V>(v))
        {}
    };

    enum { DEFAULT_CAPACITY = 50 };
    typedef std::map<K,Node*> map_type;

    size_t _capacity = 0;
    map_type _map;
    Node *_list_head = NULL, *_list_end = NULL;
    SpinLock _lock; // 注意，linux下自旋锁不可重入

#ifndef NDEBUG
    size_t _hit_count = 0, _miss_count = 0;
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

    static Node* new_node(const K& k, V&& v)
    {
        Node *p = alloc_node();
        assert(NULL != p);
        new (p) Node(k,std::forward<V>(v));
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
    // Invalid methods
    LRUCache(const LRUCache<K,V>&);
    LRUCache<K,V>& operator=(const LRUCache<K,V>&);

public:
    LRUCache()
        : _capacity(DEFAULT_CAPACITY)
    {}

    LRUCache(size_t capacity)
        : _capacity(capacity)
    {
        assert(capacity > 0);
    }

    ~LRUCache()
    {
        clear();
    }

    size_t size() const
    {
        return _map.size();
    }

    size_t capacity() const
    {
        return _capacity;
    }

    void set_capacity(size_t capacity)
    {
        assert(capacity > 0);
        _capacity = capacity;
    }

    void put(const K& k, V&& v)
    {
        Guard<SpinLock> g(&_lock);

        typename map_type::const_iterator const n = _map.find(k);
        if (n == _map.end())
        {
            Node *const p = new_node(k,std::forward<V>(v));
            _map.insert(std::pair<K,Node*>(k,p));
            push_list_head(p);

            while (_map.size() > _capacity)
            {
                assert(NULL != _list_end);
                typename map_type::iterator const nn = _map.find(_list_end->key);
                assert(nn != _map.end());
                Node *const pp = nn->second;
                assert(NULL != pp);
                _map.erase(nn);
                remove_from_list(pp);
                delete_node(pp);
            }
            return;
        }

        Node *const p = n->second;
        assert(NULL != p);
        p->value = v;
        remove_from_list(p);
        push_list_head(p);
    }

    void remove(const K& k)
    {
        Guard<SpinLock> g(&_lock);

        typename map_type::iterator const n = _map.find(k);
        if (n == _map.end())
            return;

        Node *const p = n->second;
        assert(NULL != p);
        _map.erase(n);
        remove_from_list(p);
        delete_node(p);
    }

    bool has_key(const K& k)
    {
        return _map.find(k) != _map.end();
    }

    bool get(const K& k, V *v)
    {
        assert(NULL != v);
        Guard<SpinLock> g(&_lock);

        typename map_type::const_iterator const n = _map.find(k);
        if (n == _map.end())
        {
#ifndef NDEBUG
            ++_miss_count;
#endif
            return false;
        }

        Node *const p = n->second;
        assert(NULL != p);
        *v = p->value;
        remove_from_list(p);
        push_list_head(p);

#ifndef NDEBUG
        ++_hit_count;
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

#ifndef NDEBUG
        _hit_count = 0;
        _miss_count = 0;
#endif
    }
};

}

#endif
