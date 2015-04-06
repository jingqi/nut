
#include <assert.h>
#include <string.h> // for strcmp()

#include "testregister.h"

namespace nut
{

/** 判断字符是否是空白 */
static bool is_blank(char c)
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
static void split_groups(const char *groups, std::vector<std::string> *rs)
{
    assert(NULL != groups && NULL != rs);

    const char GROUP_SPLITER = ','; // 分组分隔符

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
                rs->push_back(s);
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
        rs->push_back(s);
}

TestRegister::TestRegister(const char *fixture_name, const char *groups, new_fixture_func n, delete_fixture_func d)
    : m_new_func(n), m_delete_func(d), m_fixture_name(fixture_name)
{
    assert(NULL != fixture_name);
    assert(NULL != groups);
    assert(NULL != n);
    assert(NULL != d);

    // 分离出组别
    split_groups(groups, &m_groups);

    // 将实例添加到链表中
    m_pnext = *(TestRegister**)nut_get_register_header();
    *(TestRegister**)nut_get_register_header() = this;
}

bool TestRegister::match_group(const char *group_name) const
{
    assert(NULL != group_name);
    for (std::vector<std::string>::const_iterator iter = m_groups.begin(); iter != m_groups.end(); ++iter)
    {
        if (0 == ::strcmp(iter->c_str(), group_name))
            return true;
    }
    return false;
}

TestFixture* TestRegister::new_fixture() const
{
    assert(NULL != m_new_func);
    return m_new_func();
}

void TestRegister::delete_fixture(TestFixture *p)
{
    assert(NULL != p);
    m_delete_func(p);
}

}
