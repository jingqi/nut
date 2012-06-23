/**
 * @file -
 * @author jingqi
 * @date 2012-05-20
 * @last-edit 2012-05-20 14:51:45 jingqi
 */

#ifndef ___HEADFILE_4BB72A81_6B6A_4D6C_A44E_DAB190957A2F_
#define ___HEADFILE_4BB72A81_6B6A_4D6C_A44E_DAB190957A2F_

#include <stack.h>

namespace nut
{

class RTreeIterator
{
    Rect m_rect;
    std::stack<> m_stack;

public:
    RTreeIterator()
    {
        // TODO
    }

    RTreeIterator& operator++()
    {
        // TODO
    }

    RTreeIterator operator++(int)
    {
        RTreeIterator ret(*this);
        ++(*this)
        return ret;
    }

    bool operator==() const
    {
        // TODO
    }

    bool operator!=(const RTreeIterator& x) const
    {
        return !(*this == x);
    }
};

}

#endif

