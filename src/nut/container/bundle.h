
#ifndef ___HEADFILE_AFDE8618_B4CF_4EE0_AD20_626B05BE65B3_
#define ___HEADFILE_AFDE8618_B4CF_4EE0_AD20_626B05BE65B3_

#include <assert.h>
#include <map>
#include <string>
#include <stdint.h>
#include <utility>

#include <nut/mem/sys_ma.h>
#include <nut/rc/rc_new.h>

namespace nut
{

class _BundleElementBase
{
    NUT_REF_COUNTABLE
};

template <typename T>
class _BundleElement : public _BundleElementBase
{
public:
    T value;

    _BundleElement(const T& v)
        : value(v)
    {}

    _BundleElement(T&& v)
        : value(std::forward<T>(v))
    {}
};

class Bundle
{
    const rc_ptr<memory_allocator> _alloc;

    typedef std::map<std::string, rc_ptr<_BundleElementBase> > map_t;
    map_t _values;

private:
    explicit Bundle(const Bundle&);
    Bundle& operator=(const Bundle&);

public:
    NUT_REF_COUNTABLE

    Bundle(memory_allocator *ma = NULL)
        : _alloc(ma)
    {}

    bool has_key(const std::string& key) const
    {
        map_t::const_iterator iter = _values.find(key);
        if (iter == _values.end() || iter->second.is_null())
            return false;
        return true;
    }

    template <typename T>
    const T& get_value(const std::string& key) const
    {
        map_t::const_iterator iter = _values.find(key);
        assert(iter != _values.end());
        _BundleElementBase *e = iter->second;
        assert(NULL != e);
        _BundleElement<T> *be = dynamic_cast<_BundleElement<T>*>(e);
        assert(NULL != be);
        return be->value;
    }

    template <typename T>
    void set_value(const std::string& key, const T& value)
    {
        _values[key] = rca_new<_BundleElement<T> >(_alloc, value);
    }

    template <typename T>
    void set_value(const std::string& key, T&& value)
    {
        _values[key] = rca_new<_BundleElement<T> >(_alloc, std::forward<T>(value));
    }

    void clear()
    {
        _values.clear();
    }
};

}

#endif
