/**
 * @file -
 * @author jingqi
 * @date 2012-03-11
 * @last-edit 2012-03-11 17:36:58 jingqi
 */

#ifndef ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_
#define ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_

namespace nut
{

template <typename AllocT = std::allocator<uint8_t> >
class scoped_gc
{
    enum { DEFAULT_BLOCK_LEN = 2048 };

    struct MemBlock
    {
        MemBlock *prev;
        
    };
    
    typedef void (*destruct_func_type)(void*);
    
    struct DestructorNode
    {
        DestructorNode *prev;
        destruct_func_type destruct_func;
    };

    uint8_t *m_begin, *m_end;
    DestructorNode *m_destructChain;

private:
    explicit scoped_gc(const scoped_gc&);
    scoped_gc& operator=(const scoped_gc&);

    MemBlock
    
public:
    explicit scoped_alloc()
        : m_begin(NULL), m_end(NULL), m_destructChain(NULL)
    {}

    ~scoped_alloc()
    {}

public:
    void* alloc(size_t cb)
    {}
    
    void clear()
    {
        while (NULL != m_destructChain)
        {
            assert(NULL != m_destructChain->destruct_func;
            m_destructChain->destruct_func(m_destructChain + 1);
            m_destructChain = m_destructChain->prev;
        }
        
        while ()
        {
        }
    }

    template <typename T>
    T* gc_new()
    {}

    template <typename T, typename A1>
    T* gc_new(A1 a1)
    {}

    template <typename T, typename A1, typename A2>
    T* gc_new(A1 a1, A2 a2)
    {}
};

}

#endif /* head file guarder */

