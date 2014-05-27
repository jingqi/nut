/**
 * @file -
 * @author jingqi
 * @date 2013-03-09
 * @last-edit 2014-05-28 00:09:37 jingqi
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
    bool hasKey(const std::string& key) const
    {
        map_t::const_iterator iter = m_values.find(key);
        if (iter == m_values.end())
            return false;
        ref<_BundleElementBase> v = iter->second;
        if (v.isNull())
            return false;
        return true;
    }

    /**
     * 这个函数要想正常工作，需要开启RTTI特性
     */
    template <typename T>
    bool hasKeyT(const std::string& key) const
    {
        map_t::const_iterator iter = m_values.find(key);
        if (iter == m_values.end())
            return false;
        ref<_BundleElementBase> v = iter->second;
        if (v.isNull())
            return false;
        _BundleElement<T> *p = dynamic_cast<_BundleElement<T>*>(v.pointer()); // RTTI
        if (NULL == p)
            return false;
        return true;
    }

    template <typename T>
    const T& get(const std::string& key) const
    {
        map_t::const_iterator iter = m_values.find(key);
        assert(iter != m_values.end());
        ref<_BundleElementBase> v = iter->second;
        assert(v.isNotNull());
        _BundleElement<T> *p = dynamic_cast<_BundleElement<T>*>(v.pointer());
        assert(NULL != p);
        return p->value;
    }

    template <typename T>
    void set(const std::string& key, const T& value)
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
