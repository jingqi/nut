/**
 * @file -
 * @author jingqi
 * @date 2013-03-09
 * @last-edit 2015-01-27 13:02:09 jingqi
 * @brief
 */

#ifndef ___HEADFILE_AFDE8618_B4CF_4EE0_AD20_626B05BE65B3_
#define ___HEADFILE_AFDE8618_B4CF_4EE0_AD20_626B05BE65B3_

#include <assert.h>
#include <map>
#include <string>
#include <stdint.h>

#include <nut/mem/sys_ma.hpp>
#include <nut/rc/rc_new.hpp>

namespace nut
{

struct _BundleElementBase
{
    NUT_REF_COUNTABLE
};

template <typename T>
struct _BundleElement : public _BundleElementBase
{
    T value;

    _BundleElement(const T& v)
        : value(v)
    {}
};

class Bundle
{
    const rc_ptr<memory_allocator> m_alloc;

    typedef std::map<std::string, rc_ptr<_BundleElementBase> > map_t;
    map_t m_values;

private:
    explicit Bundle(const Bundle&);
    Bundle& operator=(const Bundle&);

public:
    NUT_REF_COUNTABLE

    Bundle(memory_allocator *ma = NULL)
        : m_alloc(ma)
    {}

    bool has_key(const std::string& key) const
    {
        map_t::const_iterator iter = m_values.find(key);
        if (iter == m_values.end() || iter->second.is_null())
            return false;
        return true;
    }

    template <typename T>
    const T& get_value(const std::string& key) const
    {
        map_t::const_iterator iter = m_values.find(key);
        assert(iter != m_values.end());
        _BundleElementBase *e = iter->second.pointer();
        assert(NULL != e);
        _BundleElement<T> *be = dynamic_cast<_BundleElement<T>*>(e);
        assert(NULL != be);
        return be->value;
    }

    template <typename T>
    void set_value(const std::string& key, const T& value)
    {
        m_values[key] = RC_NEW(m_alloc.pointer(), _BundleElement<T>, value);
    }

    void clear()
    {
        m_values.clear();
    }
};

}

#endif
