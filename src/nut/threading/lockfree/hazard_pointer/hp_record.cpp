
#include <assert.h>
#include <stdlib.h> // for ::malloc()
#include <new> // for placement new operator

#include "hp_record.h"
#include "hp_retire_list.h"


namespace nut
{

std::atomic<HPRecord*> HPRecord::_head = ATOMIC_VAR_INIT(nullptr);
std::atomic<size_t> HPRecord::_list_size = ATOMIC_VAR_INIT(0);

HPRecord::HPRecord(size_t v)
{
    _version.store(v, std::memory_order_relaxed);
}

HPRecord* HPRecord::acquire()
{
    // Try to reuse a retired HP record
    const size_t version = HPRetireList::_global_version.load(std::memory_order_relaxed);
    for (HPRecord *rec = _head.load(std::memory_order_relaxed);
         nullptr != rec; rec = rec->_next)
    {
        bool false_value = false;
        if (rec->_valid.load(std::memory_order_relaxed) ||
            !rec->_valid.compare_exchange_weak(
                false_value, true, std::memory_order_relaxed, std::memory_order_relaxed))
            continue;

        rec->_version.store(version, std::memory_order_release);
        return rec;
    }

    // Allocate a new one
    HPRecord *new_rec = (HPRecord*) ::malloc(sizeof(HPRecord));
    new (new_rec) HPRecord(version);

    HPRecord *old_head = _head.load(std::memory_order_relaxed);
    do
    {
        new_rec->_next = old_head;
    } while (!_head.compare_exchange_weak(
                 old_head, new_rec, std::memory_order_release, std::memory_order_relaxed));
    _list_size.fetch_add(1, std::memory_order_relaxed);
    return new_rec;
}

void HPRecord::release(HPRecord *rec)
{
    assert(nullptr != rec);

    // NOTE '_valid' 是关键变量，需要放到函数最后修改
    rec->_valid.store(false, std::memory_order_relaxed);
}

void HPRecord::reacquire()
{
    assert(_valid.load(std::memory_order_relaxed));
    const size_t version = HPRetireList::_global_version.load(std::memory_order_relaxed);
    _version.store(version, std::memory_order_release);
}

void HPRecord::clear()
{
    HPRecord *rec = _head.exchange(nullptr, std::memory_order_relaxed);
    while (nullptr != rec)
    {
        assert(!rec->_valid.load(std::memory_order_relaxed));
        HPRecord *next = rec->_next;
        rec->~HPRecord();
        ::free(rec);
        rec = next;
    }
}

HPGuard::HPGuard(HPRecord *rec)
    : _record(rec)
{
    if (nullptr == rec)
        _record = HPRecord::acquire();
}

HPGuard::~HPGuard()
{
    assert(nullptr != _record);
    HPRecord::release(_record);
    _record = nullptr;
}

void HPGuard::reacquire()
{
    _record->reacquire();
}

}
