/**
 * @file -
 * @author jingqi
 * @date 2011-11-11 18:49
 */

#ifndef ___HEADFILE___31A42BCF_A2BF_4E7C_8387_751B0648CFBA_
#define ___HEADFILE___31A42BCF_A2BF_4E7C_8387_751B0648CFBA_

#include <assert.h>
#include <stdint.h>

namespace nut
{

#ifndef NDEBUG
class DestroyChecker
{
    enum { CONSTRUCTED = 0x12344321, DESTRUCTED = 0xFEDCCDEF};
    int32_t m_tag;

public:
    DestroyChecker() : m_tag(CONSTRUCTED) {}

    ~DestroyChecker()
    {
        assert(CONSTRUCTED == m_tag);
        m_tag = DESTRUCTED;
    }

    void checkDestroy() const { assert(CONSTRUCTED == m_tag); }
};
#endif /* NDEBUG */

}

#endif /* head file guarder */
