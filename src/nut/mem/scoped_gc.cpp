
#include <assert.h>
#include <nut/debugging/destroy_checker.h>

#include "scoped_gc.h"

namespace nut
{

scoped_gc::scoped_gc(memory_allocator *ma)
    : _alloc(ma)
{}

scoped_gc::~scoped_gc()
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    clear();
}

void* scoped_gc::raw_alloc(size_t cb)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    if (_current_block->body + cb > _end)
    {
        if (cb >= DEFAULT_BLOCK_BODY_SIZE)
        {
            Block *const new_blk = (Block*) ma_realloc(_alloc, NULL, BLOCK_HEADER_SIZE + cb);
            assert(NULL != new_blk);

            if (NULL != _current_block)
            {
                new_blk->prev = _current_block->prev;
                _current_block->prev = new_blk;
            }
            else
            {
                new_blk->prev = NULL;
                _current_block = new_blk;
                _end = _current_block->body;
            }
            return new_blk->body;
        }
        else
        {
            Block *new_blk = (Block*) ma_realloc(_alloc, NULL, DEFAULT_BLOCK_LEN);
            assert(NULL != new_blk);

            new_blk->prev = _current_block;
            _current_block = new_blk;
            _end = _current_block->body + DEFAULT_BLOCK_BODY_SIZE;
        }
    }
    _end -= cb;
    return _end;
}

void* scoped_gc::alloc(size_t cb, destruct_func_type func)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    DestructorNode *dn = (DestructorNode*) raw_alloc(sizeof(DestructorNode) + cb);
    assert(NULL != dn);
    dn->destruct_func = func;
    dn->prev = _destruct_chain;
    _destruct_chain = dn;
    return dn + 1;
}

void* scoped_gc::alloc(size_t cb, size_t count, destruct_func_type func)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    DestructorNode *dn = (DestructorNode*) raw_alloc(sizeof(DestructorNode) + sizeof(size_t) + cb * count);
    assert(NULL != dn);
    dn->destruct_func = func;
    *(size_t*)(dn + 1) = count;
    dn->prev = _destruct_chain;
    _destruct_chain = dn;
    return ((size_t*)(dn + 1)) + 1;
}

void scoped_gc::clear()
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    while (NULL != _destruct_chain)
    {
        assert(NULL != _destruct_chain->destruct_func);
        _destruct_chain->destruct_func(_destruct_chain + 1);
        _destruct_chain = _destruct_chain->prev;
    }

    while (NULL != _current_block)
    {
        Block *prev = _current_block->prev;
        ma_free(_alloc, _current_block);
        _current_block = prev;
    }
    _end = NULL;
}

void* scoped_gc::gc_alloc(size_t cb)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    void* ret = raw_alloc(cb);
    assert(NULL != ret);
    return ret;
}

}
