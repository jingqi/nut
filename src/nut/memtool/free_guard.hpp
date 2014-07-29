/**
 * @file -
 * @author jingqi
 * @date 2013-09-15
 * @last-edit 2014-07-30 01:18:03 jingqi
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
    FreeGuard(void *p = NULL)
        : m_ptr(p)
    {}

    void set(void *p)
    {
        m_ptr = p;
    }

    void clear()
    {
        m_ptr = NULL;
    }

    ~FreeGuard()
    {
        if (NULL != m_ptr)
            ::free(m_ptr);
        m_ptr = NULL;
    }
};

template <typename T>
class DeleteGuard
{
    T *m_ptr;
public:
    DeleteGuard(T *p = NULL)
        : m_ptr(p)
    {}

    void set(T *p)
    {
        m_ptr = p;
    }

    void clear()
    {
        m_ptr = NULL;
    }

    ~DeleteGuard()
    {
        if (NULL != m_ptr)
            delete m_ptr;
        m_ptr = NULL;
    }
};

};

#endif
