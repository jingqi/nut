
#ifndef ___HEADFILE_9EE89D8A_0520_4E5A_A36B_1AE21C03C8CD_
#define ___HEADFILE_9EE89D8A_0520_4E5A_A36B_1AE21C03C8CD_

#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <map>


namespace nut
{

/**
 * Keyed Least-Recently-Used pool
 */
template <typename K, typename V, typename HASH = std::hash<K>>
class LRUPool
{
private:
    class Node
    {
    public:
        Node(K&& k, V&& v) noexcept
            : key(std::forward<K>(k)), value(std::forward<V>(v))
        {}

        Node(const K& k, V&& v) noexcept
            : key(k), value(std::forward<V>(v))
        {}

        Node(K&& k, const V&& v) noexcept
            : key(std::forward<K>(k)), value(v)
        {}

        Node(const K& k, const V& v) noexcept
            : key(k), value(v)
        {}

    public:
        K key;
        V value;
        Node *prev = nullptr;
        Node *next = nullptr;
    };

    typedef std::multimap<K,Node*> map_type;

public:
    explicit LRUPool(size_t capacity = 50) noexcept
        : _capacity(capacity)
    {
        assert(capacity > 0);
    }

    ~LRUPool() noexcept
    {
        clear();
    }

    size_t size() const noexcept
    {
        return _map.size();
    }

    size_t capacity() const noexcept
    {
        return _capacity;
    }

    void set_capacity(size_t cap) noexcept
    {
        assert(cap > 0);
        _capacity = cap;
    }

    bool obtain_object(const K& k, V *v)
    {
        assert(nullptr != v);

        // NOTE 在 multimap 中, 相同 key 的 value 是按插入顺序排列的, 这里取最近
        //      插入的记录
        typename map_type::const_iterator iter = _map.upper_bound(k);
        if (iter == _map.begin())
            return false;
        --iter;
        if (iter->first != k)
            return false;

        Node *const p = iter->second;
        assert(nullptr != p);
        *v = std::move(p->value);
        _map.erase(iter);
        remove_from_list(p);
        p->~Node();
        ::free(p);
        return true;
    }

    void release_object(K&& k, V&& v) noexcept
    {
        // Add new node
        Node *const p = (Node*) ::malloc(sizeof(Node));
        assert(nullptr != p);
        new (p) Node(k, std::forward<V>(v));
        _map.emplace(std::forward<K>(k), p);
        push_list_head(p);

        // Remove older nodes
        remove_older_nodes();
    }

    void release_object(const K& k, V&& v) noexcept
    {
        // Add new node
        Node *const p = (Node*) ::malloc(sizeof(Node));
        assert(nullptr != p);
        new (p) Node(k, std::forward<V>(v));
        _map.emplace(k, p);
        push_list_head(p);

        // Remove older nodes
        remove_older_nodes();
    }

    void release_object(K&& k, const V& v) noexcept
    {
        // Add new node
        Node *const p = (Node*) ::malloc(sizeof(Node));
        assert(nullptr != p);
        new (p) Node(k, v);
        _map.emplace(std::forward<K>(k), p);
        push_list_head(p);

        // Remove older nodes
        remove_older_nodes();
    }

    void release_object(const K& k, const V& v) noexcept
    {
        // Add new node
        Node *const p = (Node*) ::malloc(sizeof(Node));
        assert(nullptr != p);
        new (p) Node(k, v);
        _map.emplace(k, p);
        push_list_head(p);

        // Remove older nodes
        remove_older_nodes();
    }

    void clear() noexcept
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
    }

private:
    LRUPool(const LRUPool<K,V,HASH>&) = delete;
    LRUPool<K,V,HASH>& operator=(const LRUPool<K,V,HASH>&) = delete;

    void remove_from_list(Node *p) noexcept
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

    void push_list_head(Node *p) noexcept
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

    void remove_older_nodes() noexcept
    {
        while (_map.size() > _capacity)
        {
            Node *const p = _list_end;
            assert(nullptr != p);
            typename map_type::iterator const iter = _map.lower_bound(p->key);
            assert(iter != _map.end() && iter->first == p->key && iter->second == p);
            _map.erase(iter);
            remove_from_list(p);
            p->~Node();
            ::free(p);
        }
    }

private:
    size_t _capacity = 0;
    map_type _map;
    Node *_list_head = nullptr, *_list_end = nullptr;
};

}

#endif
