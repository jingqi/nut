/**
 * @file -
 * @author jingqi
 * @date 2011-11-12 11:52
 * @brief
 *  引用计数的计数器
 */

#ifndef ___HEADFILE___9D7DF0EC_0FB5_48E2_8771_6F52D286DE11_
#define ___HEADFILE___9D7DF0EC_0FB5_48E2_8771_6F52D286DE11_

namespace nut
{

/**
 * 一般的引用计数器
 */
struct RefCounter
{
    int m_value;

    RefCounter(int v) : m_value(v) {}
    inline int operator++ () { return ++m_value; }
    inline int operator-- () { return --m_value; }
};

/**
 * 多线程环境下的引用计数器
 */
struct RefCounterSync
{
    volatile int m_value;

    RefCounterSync(int v) : m_value(v) {}
    inline int operator++ () { return ++m_value; }
    inline int operator-- () { return --m_value; }
};

}

#endif /* head file guarder */