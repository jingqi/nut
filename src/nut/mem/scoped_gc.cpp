
#include <assert.h>
#include <stdlib.h> // for ::malloc() and so on

#include <nut/debugging/destroy_checker.h>

#include "scoped_gc.h"

namespace nut
{

scoped_gc::~scoped_gc()
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    clear();
}

void* scoped_gc::raw_alloc(size_t sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    if (_current_block->body + sz > _end)
    {
        if (sz >= DEFAULT_BLOCK_BODY_SIZE)
        {
            Block *const new_blk = (Block*) ::malloc(BLOCK_HEADER_SIZE + sz);
            assert(nullptr != new_blk);

            if (nullptr != _current_block)
            {
                new_blk->prev = _current_block->prev;
                _current_block->prev = new_blk;
            }
            else
            {
                new_blk->prev = nullptr;
                _current_block = new_blk;
                _end = _current_block->body;
            }
            return new_blk->body;
        }
        else
        {
            Block *new_blk = (Block*) ::malloc(DEFAULT_BLOCK_LEN);
            assert(nullptr != new_blk);

            new_blk->prev = _current_block;
            _current_block = new_blk;
            _end = _current_block->body + DEFAULT_BLOCK_BODY_SIZE;
        }
    }
    _end -= sz;
    return _end;
}

void* scoped_gc::alloc(size_t sz, destruct_func_type func)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    DestructorNode *dn = (DestructorNode*) raw_alloc(sizeof(DestructorNode) + sz);
    assert(nullptr != dn);
    dn->destruct_func = func;
    dn->prev = _destruct_chain;
    _destruct_chain = dn;
    return dn + 1;
}

void* scoped_gc::alloc(size_t sz, size_t count, destruct_func_type func)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    DestructorNode *dn = (DestructorNode*) raw_alloc(sizeof(DestructorNode) + sizeof(size_t) + sz * count);
    assert(nullptr != dn);
    dn->destruct_func = func;
    *(size_t*)(dn + 1) = count;
    dn->prev = _destruct_chain;
    _destruct_chain = dn;
    return ((size_t*)(dn + 1)) + 1;
}

void scoped_gc::clear()
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    while (nullptr != _destruct_chain)
    {
        assert(nullptr != _destruct_chain->destruct_func);
        _destruct_chain->destruct_func(_destruct_chain + 1);
        _destruct_chain = _destruct_chain->prev;
    }

    while (nullptr != _current_block)
    {
        Block *prev = _current_block->prev;
        ::free(_current_block);
        _current_block = prev;
    }
    _end = nullptr;
}

void* scoped_gc::gc_alloc(size_t sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    void* ret = raw_alloc(sz);
    assert(nullptr != ret);
    return ret;
}

}
