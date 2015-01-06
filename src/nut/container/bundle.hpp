/**
 * @file -
 * @author jingqi
 * @date 2013-03-09
 * @last-edit 2014-11-15 22:20:48 jingqi
 * @brief
 */

#ifndef ___HEADFILE_AFDE8618_B4CF_4EE0_AD20_626B05BE65B3_
#define ___HEADFILE_AFDE8618_B4CF_4EE0_AD20_626B05BE65B3_

#include <assert.h>
#include <map>
#include <string>
#include <stdint.h>

#include <nut/gc/gc.hpp>

namespace nut
{

struct _BundleElementBase
{
    NUT_GC_REFERABLE
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
	NUT_GC_REFERABLE

    typedef std::map<std::string, ref<_BundleElementBase> > map_t;
    map_t m_values;

public:
    bool has_key(const std::string& key) const
    {
        map_t::const_iterator iter = m_values.find(key);
        if (iter == m_values.end())
            return false;
        ref<_BundleElementBase> v = iter->second;
        if (v.is_null())
            return false;
        return true;
    }

    template <typename T>
    const T& get_value(const std::string& key) const
    {
        map_t::const_iterator iter = m_values.find(key);
        assert(iter != m_values.end());
        ref<_BundleElementBase> v = iter->second;
        assert(v.is_not_null());
        _BundleElement<T> *p = dynamic_cast<_BundleElement<T>*>(v.pointer());
        assert(NULL != p);
        return p->value;
    }

    template <typename T>
    void set_value(const std::string& key, const T& value)
    {
        m_values[key] = gc_new<_BundleElement<T> >(value);
    }

    void clear()
    {
        m_values.clear();
    }
};

}

#endif
