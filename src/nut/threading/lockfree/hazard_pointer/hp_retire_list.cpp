﻿
#include <assert.h>
#include <stdlib.h> // for ::free()
#include <thread>
#include <vector>

#include "hp_retire_list.h"
#include "hp_record.h"
#include "../../threading.h" // for NUT_THREAD_LOCAL


namespace nut
{

std::atomic<size_t> HPRetireList::_global_version = ATOMIC_VAR_INIT(0);

/**
 * HPRetireList 是 thread-local 的，线程结束时会析构，此时需要清理本线程资源
 */
HPRetireList::~HPRetireList()
{
    if (_retire_list.empty())
        return;

    scan_retire_list(this);

    unsigned long wait = 10;
    while (!_retire_list.empty())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(wait));
        wait = (std::min)((unsigned long) 2000, wait * 3 / 2);

        scan_retire_list(this);
    }
}

void HPRetireList::retire_memory(void *ptr)
{
    retire_any(::free, ptr);
}

void HPRetireList::retire_any(retire_func_type rfunc, void *userdata)
{
    assert(nullptr != rfunc);

    // thread local retire-list
    static NUT_THREAD_LOCAL HPRetireList retires;

    // add version to retire-list
    const size_t version = _global_version.fetch_add(1, std::memory_order_relaxed);
    retires._retire_list.emplace_back(rfunc, userdata, version);

    if (retires._retire_list.size() >= HPRecord::_list_size.load(std::memory_order_relaxed) * 2)
        scan_retire_list(&retires);
}

void HPRetireList::scan_retire_list(HPRetireList *rl)
{
    assert(nullptr != rl);

    // Find min HP version
    size_t min_version = SIZE_MAX;
    for (HPRecord *rec = HPRecord::_head.load(std::memory_order_acquire);
         nullptr != rec; rec = rec->_next)
    {
        if (!rec->_valid.load(std::memory_order_relaxed))
            continue;
        // NOTE 即使 version 是错误的(比如：'_valid' 突然被其他线程改为 false),
        //      也不影响正确性，只会影响性能
        const size_t version = rec->_version.load(std::memory_order_relaxed);
        min_version = (std::min)(min_version, version);
    }

    // do deleting
    typedef std::list<RetireRecord>::iterator iter_type;
    std::vector<iter_type> delete_later;
    for (iter_type iter = rl->_retire_list.begin(), end = rl->_retire_list.end();
         iter != end; ++iter)
    {
        const RetireRecord& rr = *iter;
        if (rr.version < min_version)
        {
            rr.retire_func(rr.userdata);
            delete_later.push_back(iter);
        }
    }
    for (size_t i = 0, sz = delete_later.size(); i < sz; ++i)
        rl->_retire_list.erase(delete_later.at(i));
}

}
