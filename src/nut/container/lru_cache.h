
#ifndef ___HEADFILE_CBBC3AB5_C145_43C1_AAD4_5149A52AF3DD_
#define ___HEADFILE_CBBC3AB5_C145_43C1_AAD4_5149A52AF3DD_

#include <assert.h>
#include <stdlib.h>
#include <unordered_map>


namespace nut
{

/**
 * Least-Recently-Used cache
 */
template <typename K, typename V, typename HASH = std::hash<K>>
class LRUCache
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

        Node(K&& k, const V& v) noexcept
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

    typedef std::unordered_map<K,Node*,HASH> map_type;

public:
    explicit LRUCache(size_t capacity = 50) noexcept
        : _capacity(capacity)
    {
        assert(capacity > 0);
    }

    ~LRUCache() noexcept
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

    void set_capacity(size_t capacity) noexcept
    {
        assert(capacity > 0);
        _capacity = capacity;
    }

    /**
     * @return -1, old data replaced
     *         1, new data inserted
     */
    int put(K&& k, V&& v) noexcept
    {
        // Search and update
        typename map_type::const_iterator const iter = _map.find(k);
        if (iter != _map.end())
        {
            Node *const p = iter->second;
            assert(nullptr != p);
            p->value = std::forward<V>(v);
            remove_from_list(p);
            push_list_head(p);
            return -1;
        }

        if (_map.size() >= _capacity)
        {
            // Reuse last node
            Node *const p = _list_end;
            assert(nullptr != p);
            typename map_type::iterator const rm_iter = _map.find(p->key);
            assert(rm_iter != _map.end());
            _map.erase(rm_iter);

            p->key = k;
            p->value = std::forward<V>(v);
            _map.emplace(std::forward<K>(k), p);
            remove_from_list(p);
            push_list_head(p);

            // Remove older nodes
            remove_older_nodes();
        }
        else
        {
            // Add new node
            Node *const p = (Node*) ::malloc(sizeof(Node));
            assert(nullptr != p);
            new (p) Node(k, std::forward<V>(v));
            _map.emplace(std::forward<K>(k), p);
            push_list_head(p);
        }

        return 1;
    }

    /**
     * @return -1, old data replaced
     *         1, new data inserted
     */
    int put(const K& k, V&& v) noexcept
    {
        // Search and update
        typename map_type::const_iterator const iter = _map.find(k);
        if (iter != _map.end())
        {
            Node *const p = iter->second;
            assert(nullptr != p);
            p->value = std::forward<V>(v);
            remove_from_list(p);
            push_list_head(p);
            return -1;
        }

        if (_map.size() >= _capacity)
        {
            // Reuse last node
            Node *const p = _list_end;
            assert(nullptr != p);
            typename map_type::iterator const rm_iter = _map.find(p->key);
            assert(rm_iter != _map.end());
            _map.erase(rm_iter);

            p->key = k;
            p->value = std::forward<V>(v);
            _map.emplace(k, p);
            remove_from_list(p);
            push_list_head(p);

            // Remove older nodes
            remove_older_nodes();
        }
        else
        {
            // Add new node
            Node *const p = (Node*) ::malloc(sizeof(Node));
            assert(nullptr != p);
            new (p) Node(k, std::forward<V>(v));
            _map.emplace(k, p);
            push_list_head(p);
        }

        return 1;
    }

    /**
     * @return -1, old data replaced
     *         1, new data inserted
     */
    int put(K&& k, const V& v) noexcept
    {
        // Search and update
        typename map_type::const_iterator const iter = _map.find(k);
        if (iter != _map.end())
        {
            Node *const p = iter->second;
            assert(nullptr != p);
            p->value = v;
            remove_from_list(p);
            push_list_head(p);
            return -1;
        }

        if (_map.size() >= _capacity)
        {
            // Reuse last node
            Node *const p = _list_end;
            assert(nullptr != p);
            typename map_type::iterator const rm_iter = _map.find(p->key);
            assert(rm_iter != _map.end());
            _map.erase(rm_iter);

            p->key = k;
            p->value = v;
            _map.emplace(std::forward<K>(k), p);
            remove_from_list(p);
            push_list_head(p);

            // Remove older nodes
            remove_older_nodes();
        }
        else
        {
            // Add new node
            Node *const p = (Node*) ::malloc(sizeof(Node));
            assert(nullptr != p);
            new (p) Node(k, v);
            _map.emplace(std::forward<K>(k), p);
            push_list_head(p);
        }

        return 1;
    }

    /**
     * @return -1, old data replaced
     *         1, new data inserted
     */
    int put(const K& k, const V& v) noexcept
    {
        // Search and update
        typename map_type::const_iterator const iter = _map.find(k);
        if (iter != _map.end())
        {
            Node *const p = iter->second;
            assert(nullptr != p);
            p->value = v;
            remove_from_list(p);
            push_list_head(p);
            return -1;
        }

        if (_map.size() >= _capacity)
        {
            // Reuse last node
            Node *const p = _list_end;
            assert(nullptr != p);
            typename map_type::iterator const rm_iter = _map.find(p->key);
            assert(rm_iter != _map.end());
            _map.erase(rm_iter);

            p->key = k;
            p->value = v;
            _map.emplace(k, p);
            remove_from_list(p);
            push_list_head(p);

            // Remove older nodes
            remove_older_nodes();
        }
        else
        {
            // Add new node
            Node *const p = (Node*) ::malloc(sizeof(Node));
            assert(nullptr != p);
            new (p) Node(k, v);
            _map.emplace(k, p);
            push_list_head(p);
        }

        return 1;
    }

    /**
     * @return true, remove succeeded
     *         false, no key found
     */
    bool remove(const K& k) noexcept
    {
        typename map_type::iterator const iter = _map.find(k);
        if (iter == _map.end())
            return false;

        Node *const p = iter->second;
        assert(nullptr != p);
        _map.erase(iter);
        remove_from_list(p);
        p->~Node();
        ::free(p);
        return true;
    }

    bool has_key(const K& k) noexcept
    {
        return _map.find(k) != _map.end();
    }

    const V* get(const K& k) noexcept
    {
        typename map_type::const_iterator const iter = _map.find(k);
        if (iter == _map.end())
        {
#ifndef NDEBUG
            ++_miss_count;
#endif
            return nullptr;
        }

        Node *const p = iter->second;
        assert(nullptr != p);
        const V* const ret = &p->value;
        remove_from_list(p);
        push_list_head(p);

#ifndef NDEBUG
        ++_hit_count;
#endif
        return ret;
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

#ifndef NDEBUG
        _hit_count = 0;
        _miss_count = 0;
#endif
    }

private:
    LRUCache(const LRUCache<K,V>&) = delete;
    LRUCache<K,V>& operator=(const LRUCache<K,V>&) = delete;

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
            typename map_type::iterator const iter = _map.find(p->key);
            assert(iter != _map.end());
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

#ifndef NDEBUG
    size_t _hit_count = 0, _miss_count = 0;
#endif
};

}

#endif
