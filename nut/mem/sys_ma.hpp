/**
 * @file -
 * @author jingqi
 * @date 2012-04-05
 * @last-edit 2012-04-05 14:55:11 jingqi
 */

#ifndef ___HEADFILE_635293EA_4990_413E_A43D_3868BE4E99C7_
#define ___HEADFILE_635293EA_4990_413E_A43D_3868BE4E99C7_

#include <assert.h>
#include <stdlib.h> // for malloc() and so on

namespace nut
{

/**
 * system memory allocator
 */
class sys_ma
{
    explicit sys_ma(const sys_ma&);
    sys_ma& operator=(const sys_ma&);

public:
    sys_ma() {}
    ~sys_ma() {}

    inline void* alloc(size_t cb)
    {
        void* ret = ::malloc(cb);
        assert(NULL != ret);
        return ret;
    }

    inline void* realloc(void *p, size_t cb, size_t new_cb)
    {
        assert(NULL != p);
        (void) cb; // unused
        void *ret = ::realloc(p, new_cb);
        assert(NULL != ret);
        return ret;
    }

    inline void free(void *p, size_t cb)
    {
        assert(NULL != p);
		(void) cb; // unused
        ::free(p);
    }
};

}

#endif /* head file guarder */

