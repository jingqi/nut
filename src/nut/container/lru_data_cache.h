
#ifndef ___HEADFILE_B81E878E_513C_4792_A8F8_73215B57ACBE_
#define ___HEADFILE_B81E878E_513C_4792_A8F8_73215B57ACBE_

#include <assert.h>
#include <stdlib.h>
#include <unordered_map>


namespace nut
{

/**
 * most-recently-used data cache
 */
template <typename K, typename HASH = std::hash<K>>
class LRUDataCache
{
private:
    class Node
    {
    public:
        Node(K&& k, const void *buf, size_t cb) noexcept
            : key(std::forward<K>(k)), size(cb)
        {
            assert(nullptr != buf || 0 == cb);
            if (cb > 0)
            {
                data = ::malloc(cb);
                assert(nullptr != data);
                ::memcpy(data, buf, cb);
            }
        }

        Node(const K& k, const void *buf, size_t cb) noexcept
            : key(k), size(cb)
        {
            assert(nullptr != buf || 0 == cb);
            if (cb > 0)
            {
                data = ::malloc(cb);
                assert(nullptr != data);
                ::memcpy(data, buf, cb);
            }
        }

        ~Node() noexcept
        {
            clear();
        }

        void copy_from(const void *buf, size_t cb) noexcept
        {
            assert(nullptr != buf || 0 == cb);
            if (cb > 0)
            {
                data = ::realloc(data, cb);
                assert(nullptr != data);
                ::memcpy(data, buf, cb);
                size = cb;
            }
            else
            {
                clear();
            }
        }

        void fast_set(void *buf, size_t cb) noexcept
        {
            assert(nullptr != buf || 0 == cb);
            clear();
            data = buf;
            size = cb;
        }

        void clear() noexcept
        {
            if (nullptr != data)
                ::free(data);
            data = nullptr;
            size = 0;
        }

    private:
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

    public:
        K key;
        void *data = nullptr;
        size_t size = 0;
        Node *prev = nullptr;
        Node *next = nullptr;
    };

    typedef std::unordered_map<K,Node*,HASH> map_type;

public:
    explicit LRUDataCache(size_t bytes_capacity = 8 * 1024 * 1024) noexcept // 8M
        : _bytes_capacity(bytes_capacity)
    {
        assert(bytes_capacity > 0);
    }

    ~LRUDataCache() noexcept
    {
        clear();
    }

    size_t size() const noexcept
    {
        return _map.size();
    }

    size_t bytes_size() const noexcept
    {
        return _bytes_size;
    }

    size_t bytes_capacity() const noexcept
    {
        return _bytes_capacity;
    }

    void set_bytes_capacity(size_t bytes_capacity) noexcept
    {
        assert(bytes_capacity > 0);
        _bytes_capacity = bytes_capacity;
    }

    /**
     * @return -1, old data replaced
     *         1, new data inserted
     */
    int put(K&& k, const void *buf, size_t cb) noexcept
    {
        assert(nullptr != buf || 0 == cb);

        // Search and update
        typename map_type::const_iterator const iter = _map.find(k);
        if (iter != _map.end())
        {
            Node *const p = iter->second;
            assert(nullptr != p && _bytes_size >= p->size);
            _bytes_size -= p->size;
            _bytes_size += cb;
            p->copy_from(buf, cb);
            remove_from_list(p);
            push_list_head(p);
            return -1;
        }

        if (_bytes_size >= _bytes_capacity)
        {
            // Reuse last node
            Node *const p = _list_end;
            assert(nullptr != p && _bytes_size >= p->size);
            typename map_type::iterator const rm_iter = _map.find(p->key);
            assert(rm_iter != _map.end());
            _map.erase(rm_iter);

            _bytes_size -= p->size;
            _bytes_size += cb;
            p->copy_from(buf, cb);
            _map.emplace(std::forward<K>(k), p);
            remove_from_list(p);
            push_list_head(p);
        }
        else
        {
            // Add new node
            Node *const p = (Node*) ::malloc(sizeof(Node));
            assert(nullptr != p);
            new (p) Node(k, buf, cb);
            _bytes_size += cb;
            _map.emplace(std::forward<K>(k), p);
            push_list_head(p);
        }

        // Remove older nodes
        remove_older_nodes();

        return 1;
    }

    /**
     * @return -1, old data replaced
     *         1, new data inserted
     */
    int put(const K& k, const void *buf, size_t cb) noexcept
    {
        assert(nullptr != buf || 0 == cb);

        // Search and update
        typename map_type::const_iterator const iter = _map.find(k);
        if (iter != _map.end())
        {
            Node *const p = iter->second;
            assert(nullptr != p && _bytes_size >= p->size);
            _bytes_size -= p->size;
            _bytes_size += cb;
            p->copy_from(buf, cb);
            remove_from_list(p);
            push_list_head(p);
            return -1;
        }

        if (_bytes_size >= _bytes_capacity)
        {
            // Reuse last node
            Node *const p = _list_end;
            assert(nullptr != p && _bytes_size >= p->size);
            typename map_type::iterator const rm_iter = _map.find(p->key);
            assert(rm_iter != _map.end());
            _map.erase(rm_iter);

            _bytes_size -= p->size;
            _bytes_size += cb;
            p->copy_from(buf, cb);
            _map.emplace(k, p);
            remove_from_list(p);
            push_list_head(p);
        }
        else
        {
            // Add new node
            Node *const p = (Node*) ::malloc(sizeof(Node));
            assert(nullptr != p);
            new (p) Node(k, buf, cb);
            _bytes_size += cb;
            _map.emplace(k, p);
            push_list_head(p);
        }

        // Remove older nodes
        remove_older_nodes();

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
        assert(nullptr != p && _bytes_size >= p->size);
        _bytes_size -= p->size;
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

    bool get(const K& k, const void **pdata, size_t *psize) noexcept
    {
        assert(nullptr != pdata && nullptr != psize);
        typename map_type::const_iterator const iter = _map.find(k);
        if (iter == _map.end())
        {
#ifndef NDEBUG
            ++_miss_count;
#endif
            return false;
        }

        Node *const p = iter->second;
        assert(nullptr != p);
        *pdata = p->data;
        *psize = p->size;
        remove_from_list(p);
        push_list_head(p);

#ifndef NDEBUG
        ++_hit_count;
        _hit_size += p->size;
#endif
        return true;
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
        _bytes_size = 0;

#ifndef NDEBUG
        _hit_count = 0;
        _hit_size = 0;
        _miss_count = 0;
#endif
    }

private:
    LRUDataCache(const LRUDataCache<K>&) = delete;
    LRUDataCache<K>& operator=(const LRUDataCache<K>&) = delete;

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
        while (_bytes_size > _bytes_capacity)
        {
            Node *const p = _list_end;
            assert(nullptr != p && _bytes_size >= p->size);
            typename map_type::iterator const iter = _map.find(p->key);
            assert(iter != _map.end());
            _bytes_size -= p->size;
            _map.erase(iter);
            remove_from_list(p);
            p->~Node();
            ::free(p);
        }
    }

private:
    size_t _bytes_size = 0, _bytes_capacity = 0;
    map_type _map;
    Node *_list_head = nullptr, *_list_end = nullptr;

#ifndef NDEBUG
    size_t _hit_count = 0, _hit_size = 0, _miss_count = 0;
#endif
};


}

#endif
