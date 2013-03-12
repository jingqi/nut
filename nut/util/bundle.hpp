/**
 * @file -
 * @author jingqi
 * @date 2013-03-09
 * @last-edit 2013-03-12 10:35:19 jingqi
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

enum BundleValueType {
    BOOLEAN,
    INTEGER,
    STRING,
    DOUBLE,
    BUNDLE
};

struct _BundleValue {
    NUT_GC_REFERABLE
    BundleValueType type;
    _BundleValue(BundleValueType t)
        : type(t)
    {}
};

struct _BoolValue : public _BundleValue {
    bool value;
    _BoolValue(bool v)
        : _BundleValue(BOOLEAN), value(v)
    {}
};

struct _IntValue : public _BundleValue {
    int64_t value;
    _IntValue(int64_t v)
        : _BundleValue(INTEGER), value(v)
    {}
};

struct _DoubleValue : public _BundleValue {
    double value;
    _DoubleValue(double v)
        : _BundleValue(DOUBLE), value(v)
    {}
};

struct _StringValue : public _BundleValue {
    std::string value;
    _StringValue(const std::string& v)
        : _BundleValue(STRING), value(v)
    {}
};

class Bundle : public _BundleValue
{
    typedef std::map<std::string, ref<_BundleValue> > map_t;
    map_t m_values;

public:
    Bundle()
        : _BundleValue(BUNDLE)
    {}

    bool hasKey(const std::string& key, BundleValueType type) const
    {
        map_t::const_iterator iter = m_values.find(key);
        if (iter == m_values.end())
            return false;
        ref<_BundleValue> v = iter->second;
        if (v.isNull())
            return false;
        if (v->type != type)
            return false;
        return true;
    }

    bool getBool(const std::string& key) const
    {
        assert(hasKey(key, BOOLEAN));
        map_t::const_iterator iter = m_values.find(key);
        ref<_BoolValue> v = dynamic_ref_cast<_BoolValue>(iter->second);
        return v->value;
    }

    void setBool(const std::string& key, bool value)
    {
        m_values[key] = gc_new<_BoolValue>(value);
    }

    int64_t getInt(const std::string& key) const
    {
        assert(hasKey(key, INTEGER));
        map_t::const_iterator iter = m_values.find(key);
        ref<_IntValue> v = dynamic_ref_cast<_IntValue>(iter->second);
        return v->value;
    }

    void setInt(const std::string& key, int64_t value)
    {
        m_values[key] = gc_new<_IntValue>(value);
    }

    const std::string& getStr(const std::string& key) const
    {
        assert(hasKey(key, STRING));
        map_t::const_iterator iter = m_values.find(key);
        ref<_StringValue> v = dynamic_ref_cast<_StringValue>(iter->second);
        return v->value;
    }

    void setStr(const std::string& key, const std::string& value)
    {
        m_values[key] = gc_new<_StringValue>(value);
    }

    double getDouble(const std::string& key) const
    {
        assert(hasKey(key, DOUBLE));
        map_t::const_iterator iter = m_values.find(key);
        ref<_DoubleValue> v = dynamic_ref_cast<_DoubleValue>(iter->second);
        return v->value;
    }

    void setDouble(const std::string& key, double value)
    {
        m_values[key] = gc_new<_DoubleValue>(value);
    }

    ref<Bundle> getBundle(const std::string& key) const
    {
        assert(hasKey(key, BUNDLE));
        map_t::const_iterator iter = m_values.find(key);
        return dynamic_ref_cast<Bundle>(iter->second);
    }

    void setBundle(const std::string& key, ref<Bundle> value)
    {
        assert(!value.isNull());
        m_values[key] = value;
    }

    void clear()
    {
        m_values.clear();
    }
};

}

#endif

