
#ifndef ___HEADFILE_AFDE8618_B4CF_4EE0_AD20_626B05BE65B3_
#define ___HEADFILE_AFDE8618_B4CF_4EE0_AD20_626B05BE65B3_

#include <assert.h>
#include <unordered_map>
#include <string>
#include <stdint.h>
#include <utility>

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
    _BundleElement(const T& v)
        : value(v)
    {}

    _BundleElement(T&& v)
        : value(std::forward<T>(v))
    {}

public:
    T value;
};

class Bundle
{
    NUT_REF_COUNTABLE

private:
    typedef std::unordered_map<std::string, rc_ptr<_BundleElementBase>> map_type;

public:
    Bundle() = default;

    bool has_key(const std::string& key) const
    {
        map_type::const_iterator iter = _values.find(key);
        if (iter == _values.end() || iter->second.is_null())
            return false;
        return true;
    }

    template <typename T>
    const T& get_value(const std::string& key) const
    {
        map_type::const_iterator iter = _values.find(key);
        assert(iter != _values.end());
        _BundleElementBase *e = iter->second;
        assert(nullptr != e);
        _BundleElement<T> *be = dynamic_cast<_BundleElement<T>*>(e);
        assert(nullptr != be);
        return be->value;
    }

    template <typename T>
    void set_value(const std::string& key, const T& value)
    {
        _values[key] = rc_new<_BundleElement<T> >(value);
    }

    template <typename T>
    void set_value(const std::string& key, T&& value)
    {
        _values[key] = rc_new<_BundleElement<T> >(std::forward<T>(value));
    }

    void clear()
    {
        _values.clear();
    }

private:
    // Non-copyable
    Bundle(const Bundle&) = delete;
    Bundle& operator=(const Bundle&) = delete;

private:
    map_type _values;
};

}

#endif
