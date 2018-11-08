
#ifndef ___HEADFILE_CBBC3AB5_C145_43C1_AAD4_5149A52AF3DD_
#define ___HEADFILE_CBBC3AB5_C145_43C1_AAD4_5149A52AF3DD_

#include <assert.h>
#include <stdlib.h>
#include <unordered_map>

#include <nut/threading/sync/dummy_lock.h>
#include <nut/threading/sync/guard.h>


namespace nut
{

/**
 * least-recently-used cache
 */
template <typename K, typename V, typename HASH = std::hash<K>>
class LRUCache
{
private:
    class Node
    {
    public:
        Node(const K& k, V&& v)
            : key(k), value(std::forward<V>(v))
        {}

        Node(const K& k, const V& v)
            : key(k), value(v)
        {}

    public:
        const K key;
        V value;
        Node *prev = nullptr;
        Node *next = nullptr;
    };

    typedef std::unordered_map<K,Node*,HASH> map_type;

public:
    explicit LRUCache(size_t capacity = 50)
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

    /**
     * @return true if new data inserted, else old data replaced
     */
    bool put(const K& k, V&& v)
    {
        typename map_type::const_iterator const n = _map.find(k);
        if (n == _map.end())
        {
            Node *const p = (Node*) ::malloc(sizeof(Node));
            assert(nullptr != p);
            new (p) Node(k, std::forward<V>(v));
            _map.insert(std::pair<K,Node*>(k,p));
            push_list_head(p);

            while (_map.size() > _capacity)
            {
                assert(nullptr != _list_end);
                typename map_type::iterator const nn = _map.find(_list_end->key);
                assert(nn != _map.end());
                Node *const pp = nn->second;
                assert(nullptr != pp);
                _map.erase(nn);
                remove_from_list(pp);
                pp->~Node();
                ::free(pp);
            }
            return true;
        }

        Node *const p = n->second;
        assert(nullptr != p);
        p->value = std::forward<V>(v);
        remove_from_list(p);
        push_list_head(p);
        return false;
    }

    /**
     * @return true if new data inserted, else old data replaced
     */
    bool put(const K& k, const V& v)
    {
        typename map_type::const_iterator const n = _map.find(k);
        if (n == _map.end())
        {
            Node *const p = (Node*) ::malloc(sizeof(Node));
            assert(nullptr != p);
            new (p) Node(k, v);
            _map.insert(std::pair<K,Node*>(k,p));
            push_list_head(p);

            while (_map.size() > _capacity)
            {
                assert(nullptr != _list_end);
                typename map_type::iterator const nn = _map.find(_list_end->key);
                assert(nn != _map.end());
                Node *const pp = nn->second;
                assert(nullptr != pp);
                _map.erase(nn);
                remove_from_list(pp);
                pp->~Node();
                ::free(pp);
            }
            return true;
        }

        Node *const p = n->second;
        assert(nullptr != p);
        p->value = v;
        remove_from_list(p);
        push_list_head(p);
        return false;
    }

    /**
     * @return true if remove succeeded
     */
    bool remove(const K& k)
    {
        typename map_type::iterator const n = _map.find(k);
        if (n == _map.end())
            return false;

        Node *const p = n->second;
        assert(nullptr != p);
        _map.erase(n);
        remove_from_list(p);
        p->~Node();
        ::free(p);
        return true;
    }

    bool has_key(const K& k)
    {
        return _map.find(k) != _map.end();
    }

    bool get(const K& k, V *v)
    {
        assert(nullptr != v);
        typename map_type::const_iterator const n = _map.find(k);
        if (n == _map.end())
        {
#ifndef NDEBUG
            ++_miss_count;
#endif
            return false;
        }

        Node *const p = n->second;
        assert(nullptr != p);
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
        Node *p = _list_head;
        while (nullptr != p)
        {
            Node *const n = p->next;
            p->~Node();
            ::free(p);
            p = n;
        }
        _list_head = nullptr;
        _list_end = nullptr;
        _map.clear();

#ifndef NDEBUG
        _hit_count = 0;
        _miss_count = 0;
#endif
    }

private:
    // Non-copyable
    LRUCache(const LRUCache<K,V>&) = delete;
    LRUCache<K,V>& operator=(const LRUCache<K,V>&) = delete;

    void remove_from_list(Node *p)
    {
        assert(nullptr != p);
        if (nullptr != p->prev)
            p->prev->next = p->next;
        else
            _list_head = p->next;

        if (nullptr != p->next)
            p->next->prev = p->prev;
        else
            _list_end = p->prev;
    }

    void push_list_head(Node *p)
    {
        assert(nullptr != p);
        p->next = _list_head;
        p->prev = nullptr;
        if (nullptr != _list_head)
            _list_head->prev = p;
        else
            _list_end = p;
        _list_head = p;
    }

private:
    size_t _capacity = 0;
    map_type _map;
    Node *_list_head = nullptr, *_list_end = nullptr;

#ifndef NDEBUG
    size_t _hit_count = 0, _miss_count = 0;
#endif
};

}

#endif
