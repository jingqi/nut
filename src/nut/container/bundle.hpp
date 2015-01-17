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

#include <nut/mem/sys_ma.hpp>

namespace nut
{

struct _BundleElementBase
{
    virtual ~_BundleElementBase() {}
};

template <typename T>
struct _BundleElement : public _BundleElementBase
{
    T value;

    _BundleElement(const T& v)
        : value(v)
    {}
};

template <typename MemAlloc = sys_ma>
class Bundle
{
    typedef Bundle<MemAlloc> self_type;
    int volatile m_ref_count;
    MemAlloc *m_alloc;

    typedef std::map<std::string, _BundleElementBase*> map_t;
    map_t m_values;

private:
    explicit Bundle(const Bundle<MemAlloc>&);
    Bundle<MemAlloc> operator=(const Bundle<MemAlloc>&);

    Bundle(MemAlloc *ma)
        : m_ref_count(0), m_alloc(ma)
    {
        assert(NULL != ma);
        ma->add_ref();
    }

    virtual ~Bundle()
    {
        clear();
        m_alloc->rls_ref();
        m_alloc = NULL;
    }

public:
    static self_type* create(MemAlloc *ma = NULL)
    {
        if (NULL == ma)
            ma = MemAlloc::create();
        else
            ma->add_ref();
        self_type *ret = (self_type*) ma->alloc(sizeof(self_type));
        assert(NULL != ret);
        new (ret) self_type(ma);
        ret->add_ref();
        ma->rls_ref();
        return ret;
    }

    int add_ref()
    {
        return atomic_add(&m_ref_count, 1) + 1;
    }

    int rls_ref()
    {
        const int ret = atomic_add(&m_ref_count, -1) - 1;
        if (0 == ret)
        {
            MemAlloc *ma = m_alloc;
            assert(NULL != ma);
            ma->add_ref();
            this->~Bundle();
            ma->free(this);
            ma->rls_ref();
        }
        return ret;
    }

public:
    bool has_key(const std::string& key) const
    {
        map_t::const_iterator iter = m_values.find(key);
        if (iter == m_values.end() || NULL == iter->second)
            return false;
        return true;
    }

    template <typename T>
    const T& get_value(const std::string& key) const
    {
        map_t::const_iterator iter = m_values.find(key);
        assert(iter != m_values.end());
        _BundleElementBase *e = iter->second;
        assert(NULL != e);
        _BundleElement<T> *be = dynamic_cast<_BundleElement<T>*>(e);
        assert(NULL != be);
        return be->value;
    }

    template <typename T>
    void set_value(const std::string& key, const T& value)
    {
        map_t::const_iterator iter = m_values.find(key);
        if (iter != m_values.end())
        {
            _BundleElementBase *e = iter->second;
            e->~_BundleElementBase();
            m_alloc->free(e);
        }
        _BundleElement<T> *be = (_BundleElement<T>*) m_alloc->alloc(sizeof(_BundleElement<T>));
        new (be) _BundleElement<T>(value);
        m_values[key] = be;
    }

    void clear()
    {
        for (map_t::const_iterator iter = m_values.begin(), end = m_values.end();
             iter != end; ++iter)
        {
            _BundleElementBase *e = iter->second;
            e->~_BundleElementBase();
            m_alloc->free(e);
        }
        m_values.clear();
    }
};

}

#endif
