
#include <assert.h>
#include <nut/debugging/destroy_checker.h>

#include "scoped_gc.h"

namespace nut
{

scoped_gc::scoped_gc(memory_allocator *ma)
    : m_alloc(ma), m_current_block(NULL), m_end(NULL), m_destruct_chain(NULL)
{}

scoped_gc::~scoped_gc()
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    clear();
}

void* scoped_gc::raw_alloc(size_t cb)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    if (m_current_block->body + cb > m_end)
    {
        if (cb >= DEFAULT_BLOCK_BODY_SIZE)
        {
            Block *const new_blk = (Block*) ma_realloc(m_alloc.pointer(), NULL, BLOCK_HEADER_SIZE + cb);
            assert(NULL != new_blk);

            if (NULL != m_current_block)
            {
                new_blk->prev = m_current_block->prev;
                m_current_block->prev = new_blk;
            }
            else
            {
                new_blk->prev = NULL;
                m_current_block = new_blk;
                m_end = m_current_block->body;
            }
            return new_blk->body;
        }
        else
        {
            Block *new_blk = (Block*) ma_realloc(m_alloc.pointer(), NULL, DEFAULT_BLOCK_LEN);
            assert(NULL != new_blk);

            new_blk->prev = m_current_block;
            m_current_block = new_blk;
            m_end = m_current_block->body + DEFAULT_BLOCK_BODY_SIZE;
        }
    }
    m_end -= cb;
    return m_end;
}

void* scoped_gc::alloc(size_t cb, destruct_func_type func)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    DestructorNode *dn = (DestructorNode*) raw_alloc(sizeof(DestructorNode) + cb);
    assert(NULL != dn);
    dn->destruct_func = func;
    dn->prev = m_destruct_chain;
    m_destruct_chain = dn;
    return dn + 1;
}

void* scoped_gc::alloc(size_t cb, size_t count, destruct_func_type func)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    DestructorNode *dn = (DestructorNode*) raw_alloc(sizeof(DestructorNode) + sizeof(size_t) + cb * count);
    assert(NULL != dn);
    dn->destruct_func = func;
    *(size_t*)(dn + 1) = count;
    dn->prev = m_destruct_chain;
    m_destruct_chain = dn;
    return ((size_t*)(dn + 1)) + 1;
}

void scoped_gc::clear()
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    while (NULL != m_destruct_chain)
    {
        assert(NULL != m_destruct_chain->destruct_func);
        m_destruct_chain->destruct_func(m_destruct_chain + 1);
        m_destruct_chain = m_destruct_chain->prev;
    }

    while (NULL != m_current_block)
    {
        Block *prev = m_current_block->prev;
        ma_free(m_alloc.pointer(), m_current_block);
        m_current_block = prev;
    }
    m_end = NULL;
}

void* scoped_gc::gc_alloc(size_t cb)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    void* ret = raw_alloc(cb);
    assert(NULL != ret);
    return ret;
}

}
