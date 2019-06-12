
#ifndef ___HEADFILE_E7849DBE_E176_427D_A0C1_60B0E4C3A1D0_
#define ___HEADFILE_E7849DBE_E176_427D_A0C1_60B0E4C3A1D0_

#include <assert.h>
#include <atomic>
#include <algorithm>

#include "../../numeric/word_array_integer/bit_op.h"
#include "../../container/comparable.h"
#include "../sync/spinlock.h"
#include "../sync/lock_guard.h"
#include "stamped_ptr.h"
#include "hazard_pointer/hp_record.h"
#include "hazard_pointer/hp_retire_list.h"


// #define EXTRACT_TAG(stamp) ((stamp) >> 1)
// #define EXTRACT_RETIRED(stamp) ((stamp) & 0x01)

#define MARK_RETIRED(stamp) ((stamp) | 0x01)
#define CLEAR_RETIRED(stamp) ((stamp) & ~1LL)
#define IS_RETIRED(stamp) (1 == ((stamp) & 0x01))

#define INCREASE_TAG(stamp) ((stamp) + 0x02)

namespace nut
{

/**
 * trunk0                                 trunk1     ...
 *   |                                      |
 * bucket0              bucket1   ...    bucket16    ...
 *   |                     |                |
 * [dummy] -> 1 -> 2 -> [dummy] -> ... -> [dummy] -> ...
 *
 * @see https://coolshell.cn/articles/9703.html
 * @see http://ifeve.com/lock-free-linked-list/
 */
template <typename K, typename V, typename HASH = std::hash<K>>
class ConcurrentHashMap
{
private:
    static constexpr size_t FIRST_TRUNK_SIZE_SHIFT = 4; // first trunk size (initial bucket size) is 2**4 = 16
    static constexpr size_t TRUNK_COUNT = 16; // make max bucket size is 2**(3+16) = 524288

    // FIXME 单独 'typedef size_t hash_type' 匹配 reverse_bits(uint64_t) 函数时
    //       会出现歧义，找不到最佳匹配
    typedef typename std::conditional<
        sizeof(size_t) == sizeof(uint8_t), uint8_t,
        typename std::conditional<
            sizeof(size_t) == sizeof(uint16_t), uint16_t,
            typename std::conditional<
                sizeof(size_t) == sizeof(uint32_t), uint32_t,
                uint64_t>::type>::type>::type hash_type;

    /**
     * NOTE 这个类不能有任何虚函数
     */
    struct Entry
    {
    public:
        void construct_plump(K&& k, V&& v, hash_type rh) noexcept
        {
            assert(0 != (rh & 0x01)); // Not dummy

            new (&next) AtomicStampedPtr<Entry>;
            new (const_cast<K*>(&key)) K(std::forward<K>(k));
            new (const_cast<V*>(&value)) V(std::forward<V>(v));
            const_cast<hash_type&>(reversed_hash) = rh;
        }

        void construct_plump(const K& k, V&& v, hash_type rh) noexcept
        {
            assert(0 != (rh & 0x01)); // Not dummy

            new (&next) AtomicStampedPtr<Entry>;
            new (const_cast<K*>(&key)) K(k);
            new (const_cast<V*>(&value)) V(std::forward<V>(v));
            const_cast<hash_type&>(reversed_hash) = rh;
        }

        void construct_plump(K& k, const V& v, hash_type rh) noexcept
        {
            assert(0 != (rh & 0x01)); // Not dummy

            new (&next) AtomicStampedPtr<Entry>;
            new (const_cast<K*>(&key)) K(std::forward<K>(k));
            new (const_cast<V*>(&value)) V(v);
            const_cast<hash_type&>(reversed_hash) = rh;
        }

        void construct_plump(const K& k, const V& v, hash_type rh) noexcept
        {
            assert(0 != (rh & 0x01)); // Not dummy

            new (&next) AtomicStampedPtr<Entry>;
            new (const_cast<K*>(&key)) K(k);
            new (const_cast<V*>(&value)) V(v);
            const_cast<hash_type&>(reversed_hash) = rh;
        }

        void construct_dummy(hash_type rh, Entry *n) noexcept
        {
            assert(0 == (rh & 0x01)); // Dummy

            new (&next) AtomicStampedPtr<Entry>(n, 0);
            const_cast<hash_type&>(reversed_hash) = rh;
        }

        void destruct() noexcept
        {
            (&next)->~AtomicStampedPtr();
            if (!is_dummy())
            {
                (&key)->~K();
                (&value)->~V();
            }
        }

        // Only used by 'HPRetireList'
        static void delete_entry(void *n) noexcept
        {
            assert(nullptr != n);
            ((Entry*) n)->destruct();
            ::free(n);
        }

        bool is_dummy() const noexcept
        {
            return 0 == (reversed_hash & 0x01);
        }

        bool is_retired() const noexcept
        {
            return IS_RETIRED(next.load(std::memory_order_relaxed).stamp);
        }

    private:
        Entry() = delete;
        Entry(const Entry&) = delete;
        ~Entry() = delete;
        Entry& operator=(const Entry&) = delete;

    public:
        AtomicStampedPtr<Entry> next;

        const K key;
        const V value;
        const hash_type reversed_hash; // bits reversed hash value
    };

public:
    ConcurrentHashMap() noexcept
    {
        ::memset(_trunks, 0, sizeof(Entry**) * TRUNK_COUNT);

        const size_t trunk_size = ((size_t) 1) << FIRST_TRUNK_SIZE_SHIFT;
        Entry *dummies = (Entry*) ::malloc(sizeof(Entry) * trunk_size);
        _trunks[0] = dummies;

        const hash_type inc_rh = ((hash_type) 1) << (sizeof(hash_type) * 8 - FIRST_TRUNK_SIZE_SHIFT); // 0x10000000
        for (size_t i = 0; i < trunk_size; ++i)
        {
            const hash_type rh = reverse_bits((hash_type) i);
            const hash_type next_pos = reverse_bits((hash_type) (rh + inc_rh)); // NOTE 0xF0000000 will overflow to 0
            assert(next_pos < trunk_size);
            Entry *const next = (0 == next_pos ? nullptr : dummies + next_pos);
            dummies[i].construct_dummy(rh, next);
        }
    }

    ~ConcurrentHashMap() noexcept
    {
        Entry *p = _trunks[0]; // Head of link
        while (nullptr != p)
        {
            Entry *next = p->next.load(std::memory_order_acquire).ptr;

            const bool dummy = p->is_dummy();
            p->destruct();
            if (!dummy)
                ::free(p);

            p = next;
        }

        const size_t bss = _bucket_size_shift.load(std::memory_order_relaxed);
        for (size_t i = 0; i < bss + 1 - FIRST_TRUNK_SIZE_SHIFT; ++i)
            ::free(_trunks[i]);
    }

    size_t size() const noexcept
    {
        return _size.load(std::memory_order_relaxed);
    }

    bool contains_key(const K& k) const noexcept
    {
        return get(k, nullptr);
    }

    bool get(const K& k, V *v) const noexcept
    {
        // Locate bucket
        const hash_type h = (hash_type) _hash(k);
        Entry *bucket = get_bucket(h);
        assert(nullptr != bucket);

        // Search key
        const hash_type rh = reverse_bits(h) | 0x01;
        return search_link(bucket, &k, rh, nullptr, nullptr, v);
    }

    /**
     * @return true if insert success, else old data found
     */
    bool insert(const K& k, V&& v) noexcept
    {
        // Locate bucket
        const hash_type h = (hash_type) _hash(k);
        Entry *bucket = get_bucket(h);
        assert(nullptr != bucket);

        const hash_type rh = reverse_bits(h) | 0x01;
        Entry *new_item = nullptr;
        while (true)
        {
            // Search key
            Entry *prev = nullptr;
            StampedPtr<Entry> item;
            if (search_link(bucket, &k, rh, &prev, &item))
            {
                // Delete temperory new item
                if (nullptr != new_item)
                {
                    new_item->destruct();
                    ::free(new_item);
                }

                return false;
            }
            assert(nullptr != prev);
            if (IS_RETIRED(item.stamp))
                continue; // 'prev' deleted by some other thread, retry

            // New item
            if (nullptr == new_item)
            {
                // NOTE 'k' 在构建 'new_item' 之后，由于 while 循环还可能会在
                //      search_link() 调用时使用，故不能用右值引用传入
                new_item = (Entry*) ::malloc(sizeof(Entry));
                new_item->construct_plump(k, std::forward<V>(v), rh);
            }

            // Do insert
            // NOTE 这里 CAS 失败的可能原因：
            // - prev 节点被删除，导致 EXTRACT_RETIRED(prev->next.stamp) 标记改
            //   变
            // - prev 后面插入了新节点，或者删除了节点(item)，导致 prev->next.ptr
            //   指针改变或者 EXTRACT_TAG(prev->next.stamp) 标记改变
            new_item->next.store(StampedPtr<Entry>(item.ptr, 0), std::memory_order_relaxed);
            assert(!IS_RETIRED(item.stamp));
            if (prev->next.compare_exchange_weak(
                    &item, {new_item, INCREASE_TAG(item.stamp)},
                    std::memory_order_release, std::memory_order_relaxed))
            {
                const size_t sz = _size.fetch_add(1, std::memory_order_relaxed) + 1;
                const size_t bss = _bucket_size_shift.load(std::memory_order_relaxed);
                if (sz >= (((size_t) 1) << bss) * 0.75)
                    rehash(bss + 1);
                return true;
            }
        }

        // dead code
        assert(false);
        return false;
    }

    bool insert(const K& k, const V& v) noexcept
    {
        // Locate bucket
        const hash_type h = (hash_type) _hash(k);
        Entry *bucket = get_bucket(h);
        assert(nullptr != bucket);

        const hash_type rh = reverse_bits(h) | 0x01;
        Entry *new_item = nullptr;
        while (true)
        {
            // Search key
            Entry *prev = nullptr;
            StampedPtr<Entry> item;
            if (search_link(bucket, &k, rh, &prev, &item))
            {
                // Delete temperory new item
                if (nullptr != new_item)
                {
                    new_item->destruct();
                    ::free(new_item);
                }

                return false;
            }
            assert(nullptr != prev);
            if (IS_RETIRED(item.stamp))
                continue; // 'prev' deleted by some other thread, retry

            // New item
            if (nullptr == new_item)
            {
                // NOTE 'k' 在构建 'new_item' 之后，由于 while 循环还可能会在
                //      search_link() 调用时使用，故不能用右值引用传入
                new_item = (Entry*) ::malloc(sizeof(Entry));
                new_item->construct_plump(k, v, rh);
            }

            // Do insert
            // NOTE 这里 CAS 失败的可能原因：
            // - prev 节点被删除，导致 EXTRACT_RETIRED(prev->next.stamp) 标记改
            //   变
            // - prev 后面插入了新节点，或者删除了节点(item)，导致 prev->next.ptr
            //   指针改变或者 EXTRACT_TAG(prev->next.stamp) 标记改变
            new_item->next.store(StampedPtr<Entry>(item.ptr, 0), std::memory_order_relaxed);
            assert(!IS_RETIRED(item.stamp));
            if (prev->next.compare_exchange_weak(
                    &item, {new_item, INCREASE_TAG(item.stamp)},
                    std::memory_order_release, std::memory_order_relaxed))
            {
                const size_t sz = _size.fetch_add(1, std::memory_order_relaxed) + 1;
                const size_t bss = _bucket_size_shift.load(std::memory_order_relaxed);
                if (sz >= (((size_t) 1) << bss) * 0.75)
                    rehash(bss + 1);
                return true;
            }
        }

        // dead code
        assert(false);
        return false;
    }

    /**
     * @return true if data removed, else nothing happened
     */
    bool remove(const K& k, V *v = nullptr) noexcept
    {
        // Locate bucket
        const hash_type h = (hash_type) _hash(k);
        Entry *bucket = get_bucket(h);
        assert(nullptr != bucket);

        const hash_type rh = reverse_bits(h) | 0x01;
        while (true)
        {
            // Search key
            Entry *prev = nullptr;
            StampedPtr<Entry> item;
            if (!search_link(bucket, &k, rh, &prev, &item))
                return false;
            assert(nullptr != prev && nullptr != item.ptr);
            if (IS_RETIRED(item.stamp))
                continue; // 'prev' deleted by some other thread, retry

            // Remove item
            const int rs = remove_item(prev, item, v);
            if (rs >= 0) // 1 success, 0 failed, -1 retry
                return rs > 0;
        }

        // dead code
        assert(false);
        return false;
    }

    void clear() noexcept
    {
        Entry *head = _trunks[0]; // Head of link
        while (true)
        {
            // Find first removeable item
            Entry *prev = nullptr;
            StampedPtr<Entry> item;
            if (!get_first_removeable(head, &head, &prev, &item))
                return;
            assert(nullptr != prev && nullptr != item.ptr);
            if (IS_RETIRED(item.stamp))
                continue; // 'prev' deleted by some other thread, retry

            // Remove item, whatever failed or not
            remove_item(prev, item);
        }
    }

private:
    ConcurrentHashMap(const ConcurrentHashMap&) = delete;
    ConcurrentHashMap& operator=(const ConcurrentHashMap&) = delete;

    Entry *get_bucket(hash_type h) const noexcept
    {
        const size_t bss = _bucket_size_shift.load(std::memory_order_acquire);
        const hash_type mask = ~(~((hash_type) 0) << bss);// Lower bits mask, eg. 0x0f
        const hash_type bucket_index = h & mask;
        const int trunk_index = std::max<int>(0, highest_bit1(bucket_index) - FIRST_TRUNK_SIZE_SHIFT + 1);
        hash_type local_bucket_index = bucket_index;
        if (0 != trunk_index)
            local_bucket_index -= ((hash_type) 1) << (trunk_index + FIRST_TRUNK_SIZE_SHIFT - 1);
        return _trunks[trunk_index] + local_bucket_index;
    }

    /**
     * @param head 必须是 dummy 节点，不会被删除
     * @param key 如果搜索 dummy 节点，需要设置为 nullptr
     * @param pvalue 如果搜索 dummy 节点，需要设置为 nullptr
     * @param pitem 回传参数。
     *        NOTE item 本身必定不是 retired, 但是 pitem 可能返回带 retired 标记
     *        的值，表明 prev 是 retired
     */
    bool search_link(Entry *head, const K *key, hash_type rh, Entry **pprev = nullptr,
                     StampedPtr<Entry> *pitem = nullptr, V *pvalue = nullptr) const noexcept
    {
        assert(nullptr != head && head->is_dummy());
        assert(nullptr != key || nullptr == pvalue);

        HPGuard guard_item;
        while (true)
        {
            bool retry = false;
            Entry *prev = head;
            StampedPtr<Entry> item = prev->next.load(std::memory_order_acquire);
            while (nullptr != item.ptr)
            {
                // Hold 'item'
                guard_item.reacquire();
                if (item.ptr->is_retired())
                {
                    // 如果已被标记，那么紧接着 item 可能被移除链表甚至释放，所
                    // 以需要重新查找
                    retry = true;
                    break;
                }

                // Visite 'item': compare hash value and key
                if (item.ptr->reversed_hash >= rh)
                {
                    if (nullptr != pprev)
                        *pprev = prev;
                    if (nullptr != pitem)
                        *pitem = item;

                    if (item.ptr->reversed_hash == rh)
                    {
                        if (nullptr == key) // search dummy item
                            return true;
                        if (item.ptr->key == *key) // search data item
                        {
                            if (nullptr != pvalue)
                                *pvalue = item.ptr->value;
                            return true;
                        }
                    }
                    return false;
                }

                prev = item.ptr;
                item = prev->next.load(std::memory_order_acquire);
            }
            if (retry)
                continue;

            if (nullptr != pprev)
                *pprev = prev;
            if (nullptr != pitem)
                *pitem = item; // {nullptr, {tag, retired}}
            return false;
        }

        // dead code
        assert(false);
        return false;
    }

    /**
     * 找到第一个可删除 Entry
     *
     * @param pitem 回传参数。
     *        NOTE item 本身必定不是 retired, 但是 pitem 可能返回带 retired 标记
     *        的值，表明 prev 是 retired
     */
    bool get_first_removeable(Entry *head, Entry **phead, Entry **pprev,
                              StampedPtr<Entry> *pitem) const noexcept
    {
        assert(nullptr != head && head->is_dummy());
        assert(nullptr != phead && nullptr != pprev && nullptr != pitem);

        HPGuard guard_item;
        while (true)
        {
            bool retry = false;
            Entry *prev = head;
            StampedPtr<Entry> item = prev->next.load(std::memory_order_acquire);
            while (nullptr != item.ptr)
            {
                // Hold 'item'
                guard_item.reacquire();
                if (item.ptr->is_retired())
                {
                    // 如果已被标记，那么紧接着 item 可能被移除链表甚至释放，所
                    // 以需要重新查找
                    retry = true;
                    break;
                }

                if (item.ptr->is_dummy())
                {
                    head = item.ptr; // new head found
                }
                else
                {
                    *phead = head;
                    *pprev = prev;
                    *pitem = item;
                    return true;
                }

                prev = item.ptr;
                item = prev->next.load(std::memory_order_acquire);
            }
            if (retry)
                continue;

            *phead = head;
            *pprev = prev;
            *pitem = item; // {nullptr, {tag, retired}}
            return false;
        }

        // dead code
        assert(false);
        return false;
    }

    /**
     * @return 1, success
     *         0, failed
     *         -1, retry
     */
    int remove_item(Entry *prev, const StampedPtr<Entry>& item, V *pvalue = nullptr) noexcept
    {
        if (IS_RETIRED(item.stamp))
            return -1; // 'prev' is deleted by some other thread, please retry

        // Mark retired
        // NOTE 这里 CAS 失败的可能原因：
        // - 其他线程也在尝试删除 item, 导致 EXTRACT_RETIRED(item.ptr->next.stamp)
        //   标记改变
        // - item 后面插入了新节点，或者删除了节点，导致 item.ptr->next.ptr
        //   指针改变或者 EXTRACT_TAG(item.ptr->next.stamp) 标记改变
        StampedPtr<Entry> inext = item.ptr->next.load(std::memory_order_relaxed);
        do
        {
            if (IS_RETIRED(inext.stamp))
                return 0; // 'item' already deleted by some other thread, delete failed
        } while (!item.ptr->next.compare_exchange_weak(
                     &inext, {inext.ptr, MARK_RETIRED(inext.stamp)},
                     std::memory_order_relaxed, std::memory_order_relaxed));
        assert(!IS_RETIRED(inext.stamp));

        // Remove from map, and add to retire list
        // NOTE 这里 CAS 失败的可能原因：
        // - prev 节点被删除，导致 EXTRACT_RETIRED(prev->next.stamp) 标记改变
        // - prev 后面插入了新节点，导致 prev->next.ptr 指针改变或者
        //   EXTRACT_TAG(prev->next.stamp) 标记改变
        StampedPtr<Entry> old_item = item;
        assert(!IS_RETIRED(old_item.stamp));
        if (prev->next.compare_exchange_weak(
                &old_item, {inext.ptr, INCREASE_TAG(old_item.stamp)},
                std::memory_order_relaxed, std::memory_order_relaxed))
        {
            if (nullptr != pvalue)
                *pvalue = std::move(item.ptr->value);
            HPRetireList::retire_any(Entry::delete_entry, item.ptr);
            _size.fetch_sub(1, std::memory_order_relaxed);
            return 1;
        }
        else
        {
            // 还原 retire 标记
            // NOTE
            // - 'old_item.ptr' 可能在上个 CAS 操作中被改动了，这里不能用了，用回 'item.ptr'
            // - 由于 'item' 被已经标记 retired, 所以其后面是不可能插入或者删除节点的
            assert(!IS_RETIRED(inext.stamp));
            assert(item.ptr->next.load(std::memory_order_relaxed) ==
                StampedPtr<Entry>(inext.ptr, MARK_RETIRED(inext.stamp)));
            item.ptr->next.store(inext, std::memory_order_relaxed);
            return -1;
        }
    }

    bool insert_dummy_entry(Entry *head, Entry *new_item) noexcept
    {
        assert(nullptr != head && head->is_dummy());
        assert(nullptr != new_item && new_item->is_dummy());

        const hash_type rh = new_item->reversed_hash;
        while (true)
        {
            // Search key
            Entry *prev = nullptr;
            StampedPtr<Entry> item;
            if (search_link(head, nullptr, rh, &prev, &item))
                return false;
            assert(nullptr != prev);
            if (IS_RETIRED(item.stamp))
                continue; // 'prev' deleted by some other thread, retry

            // Do insert
            // NOTE 这里 CAS 失败的可能原因：
            // - prev 节点被删除，导致 EXTRACT_RETIRED(prev->next.stamp) 标记改
            //   变
            // - prev 后面插入了新节点，或者删除了节点(item)，导致 prev->next.ptr
            //   指针改变或者 EXTRACT_TAG(prev->next.stamp) 标记改变
            bool retry = false;
            new_item->next.store(StampedPtr<Entry>(item.ptr, 0), std::memory_order_relaxed);
            assert(!IS_RETIRED(item.stamp));
            while (!prev->next.compare_exchange_weak(
                       &item, {new_item, INCREASE_TAG(item.stamp)},
                       std::memory_order_release, std::memory_order_relaxed))
            {
                if (IS_RETIRED(item.stamp))
                {
                    retry = true; // 'prev' deleted by some other thread
                    break;
                }
                new_item->next.store(StampedPtr<Entry>(item.ptr, 0), std::memory_order_relaxed);
            }
            if (retry)
                continue;
            return true;
        }

        // dead code
        assert(false);
        return false;
    }

    void rehash(size_t expect_bss) noexcept
    {
        if (!_rehash_lock.trylock())
            return;
        LockGuard<SpinLock> g(&_rehash_lock, false);

        const size_t bss = _bucket_size_shift.load(std::memory_order_relaxed);
        if (bss >= expect_bss)
            return;

        const size_t trunk_size = ((size_t) 1) << bss;
        Entry *dummies = (Entry*) ::malloc(sizeof(Entry) * trunk_size);
        _trunks[bss - FIRST_TRUNK_SIZE_SHIFT + 1] = dummies;

        for (size_t i = 0; i < trunk_size; ++i)
        {
            const hash_type rh = reverse_bits((hash_type) (trunk_size + i));
            dummies[i].construct_dummy(rh, nullptr);

            // Find old bucket
            const int trunk_index = std::max<int>(0, highest_bit1((hash_type) i) -
                                                  FIRST_TRUNK_SIZE_SHIFT + 1);
            size_t local_bucket_index = i;
            if (0 != trunk_index)
                local_bucket_index -= ((size_t) 1) << (trunk_index + FIRST_TRUNK_SIZE_SHIFT - 1);
            Entry *bucket = _trunks[trunk_index] + local_bucket_index;

            // Insert into link
            const bool rs = insert_dummy_entry(bucket, dummies + i);
            assert(rs);
            UNUSED(rs);
        }
        _bucket_size_shift.fetch_add(1, std::memory_order_release);
    }

private:
    HASH _hash;
    Entry *_trunks[TRUNK_COUNT];

    // total bucket size will be (1 << _bucket_size_shift), eg. 16, 32, 64 ....
    std::atomic<size_t> _bucket_size_shift = ATOMIC_VAR_INIT(FIRST_TRUNK_SIZE_SHIFT);
    std::atomic<size_t> _size = ATOMIC_VAR_INIT(0);

    SpinLock _rehash_lock;
};

}


// #undef EXTRACT_TAG
// #undef EXTRACT_RETIRED

#undef MARK_RETIRED
#undef CLEAR_RETIRED
#undef IS_RETIRED

#undef INCREASE_TAG

#endif
