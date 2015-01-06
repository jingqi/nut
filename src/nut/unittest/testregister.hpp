/**
 * @file -
 * @author jingqi
 * @date 2011-11-12 13:16
 */

#ifndef ___HEADFILE___53924D54_39B0_4377_B555_4096B8CC3619_
#define ___HEADFILE___53924D54_39B0_4377_B555_4096B8CC3619_

#include <assert.h>
#include <string.h> // for strcmp()
#include <string>
#include <vector>
#include <nut/platform/platform.hpp>

namespace nut { class TestRegister; }

DLL_API void** nut_get_register_header();

#define NUT_UNITTEST_IMPL \
DLL_API void** nut_get_register_header() \
{ \
    static nut::TestRegister* header = NULL; \
    return (void**) &header; \
}


namespace nut
{

class TestRegister
{
    typedef TestFixture* (*new_fixture_func)();
    typedef void (*delete_fixture_func)(TestFixture*);

    new_fixture_func m_new_func;
    delete_fixture_func m_delete_func;
    const char *m_fixture_name;
    std::vector<std::string> m_groups;
    TestRegister *m_pnext;

public:
    TestRegister(const char *fixture_name, const char *groups, new_fixture_func n, delete_fixture_func d)
        : m_new_func(n), m_delete_func(d), m_fixture_name(fixture_name)
    {
        assert(NULL != fixture_name);
        assert(NULL != groups);
        assert(NULL != n);
        assert(NULL != d);

        // 分离出组别
        m_groups = split_groups(groups);

        // 将实例添加到链表中
        m_pnext = *(TestRegister**)nut_get_register_header();
        *(TestRegister**)nut_get_register_header() = this;
    }

    bool match_group(const char *group_name) const
    {
        assert(NULL != group_name);
        for (std::vector<std::string>::const_iterator iter = m_groups.begin(); iter != m_groups.end(); ++iter)
        {
            if (0 == ::strcmp(iter->c_str(), group_name))
                return true;
        }
        return false;
    }

    const char* get_fixture_name() const { return m_fixture_name; }

    TestRegister* get_next_register() const { return m_pnext; }

    TestFixture* new_fixture() const { return m_new_func(); }

    void delete_fixture(TestFixture *p)
    {
        assert(NULL != p);
        m_delete_func(p);
    }

private:
    /** 判断字符是否是空白 */
    static inline bool is_blank(char c)
    {
        return c == ' ' || c == '\r' || c == '\n' || c == '\t';
    }

    /** 去除字符串末尾的空白 */
    static std::string trim_end(const std::string& s)
    {
        std::string ret = s;
        while (ret.length() > 0)
        {
            char c = ret.at(ret.length() - 1);
            if (!is_blank(c))
                return ret;
            ret.resize(ret.length() - 1);
        }
        return ret;
    }

    /** 拆分如 "test, quiet" 的字符串为 ["test", "quiet"] */
    static std::vector<std::string> split_groups(const char *groups)
    {
        assert(NULL != groups);

        const char GROUP_SPLITER = ','; // 分组分隔符

        std::vector<std::string> ret;
        std::string s;
        for (int i = 0; '\0' != groups[i]; ++i)
        {
            if (GROUP_SPLITER == groups[i])
            {
                // 去除尾部空格
                s = trim_end(s);

                // 添加到结果
                if (s.length() > 0)
                {
                    ret.push_back(s);
                    s.clear();
                }
            }
            else if (s.length() > 0 || !is_blank(groups[i]))
            {
                s.push_back(groups[i]);
            }
        }
        s = trim_end(s);
        if (s.length() > 0)
            ret.push_back(s);

        return ret;
    }
};

}

#endif /* head file guarder */
