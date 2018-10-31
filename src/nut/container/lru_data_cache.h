
#ifndef ___HEADFILE_B81E878E_513C_4792_A8F8_73215B57ACBE_
#define ___HEADFILE_B81E878E_513C_4792_A8F8_73215B57ACBE_

#include <assert.h>
#include <stdlib.h>
#include <unordered_map>

#include <nut/threading/sync/dummy_lock.h>
#include <nut/threading/sync/guard.h>

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
        Node(const K& k, const void *buf, size_t cb)
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

        ~Node()
        {
            clear();
        }

        void copy_from(const void *buf, size_t cb)
        {
            assert(nullptr != buf || 0 == cb);
            clear();
            if (cb > 0)
            {
                data = ::malloc(cb);
                assert(nullptr != data);
                ::memcpy(data, buf, cb);
                size = cb;
            }
        }

        void fast_set(void *buf, size_t cb)
        {
            assert(nullptr != buf || 0 == cb);
            clear();
            data = buf;
            size = cb;
        }

        void clear()
        {
            if (nullptr != data)
                ::free(data);
            data = nullptr;
            size = 0;
        }

    private:
        // Non-copyable
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
    explicit LRUDataCache(size_t bytes_capacity = 2 * 1024 * 1024) // 2M
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

    /**
     * @return true if new data inserted, else old data replaced
     */
    bool put(const K& k, const void *buf, size_t cb)
    {
        assert(nullptr != buf || 0 == cb);

        typename map_type::const_iterator const n = _map.find(k);
        if (n == _map.end())
        {
            Node *const p = (Node*) ::malloc(sizeof(Node));
            assert(nullptr != p);
            new (p) Node(k, buf, cb);
            _map.insert(std::pair<K,Node*>(k,p));
            push_list_head(p);
            _bytes_size += cb;

            while (_bytes_size > _bytes_capacity)
            {
                assert(nullptr != _list_end);
                typename map_type::iterator const nn = _map.find(_list_end->key);
                assert(nn != _map.end());
                Node *const pp = nn->second;
                assert(nullptr != pp && _bytes_size >= pp->size);
                _bytes_size -= pp->size;
                _map.erase(nn);
                remove_from_list(pp);
                pp->~Node();
                ::free(pp);
            }
            return true;
        }

        Node *const p = n->second;
        assert(nullptr != p && _bytes_size >= p->size);
        _bytes_size -= p->size;
        p->copy_from(buf, cb);
        remove_from_list(p);
        push_list_head(p);
        _bytes_size += cb;
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
        assert(nullptr != p && _bytes_size >= p->size);
        _bytes_size -= p->size;
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

    bool get(const K& k, const void **pdata, size_t *psize)
    {
        assert(nullptr != pdata && nullptr != psize);

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
        _bytes_size = 0;

#ifndef NDEBUG
        _hit_count = 0;
        _hit_size = 0;
        _miss_count = 0;
#endif
    }

private:
    // Non-copyable
    LRUDataCache(const LRUDataCache<K>&) = delete;
    LRUDataCache<K>& operator=(const LRUDataCache<K>&) = delete;

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
    size_t _bytes_size = 0, _bytes_capacity = 0;
    map_type _map;
    Node *_list_head = nullptr, *_list_end = nullptr;

#ifndef NDEBUG
    size_t _hit_count = 0, _hit_size = 0, _miss_count = 0;
#endif
};


}

#endif
