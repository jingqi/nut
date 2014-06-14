/**
 * @file -
 * @author jingqi
 * @date 2013-09-15
 * @last-edit 2013-09-15 22:24:44 jingqi
 * @brief
 */

#ifndef ___HEADFILE_49E4EA4F_3D1E_4E4E_A406_0186EBB688D9_
#define ___HEADFILE_49E4EA4F_3D1E_4E4E_A406_0186EBB688D9_

#include <assert.h>
#include <stdlib.h>

namespace nut
{

class FreeGuard
{
    void *m_ptr;
public:
    FreeGuard(void *p)
        : m_ptr(p)
    {
        assert(NULL != p);
    }

    ~FreeGuard()
    {
        assert(NULL != m_ptr);
        ::free(m_ptr);
        m_ptr = NULL;
    }
};

template <typename T>
class DeleteGuard
{
    T *m_ptr;
public:
    DeleteGuard(T *p)
        : m_ptr(p)
    {
        assert(NULL != p);
    }
    
    ~DeleteGuard()
    {
        assert(NULL != m_ptr);
        delete m_ptr;
        m_ptr = NULL;
    }
};

};

#endif
