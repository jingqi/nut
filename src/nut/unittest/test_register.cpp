
#include <assert.h>
#include <string.h> // for strcmp()

#include "test_register.h"

namespace nut
{

/**
 * 判断字符是否是空白
 */
static bool is_blank(char c)
{
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

/**
 * 去除字符串末尾的空白
 */
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

/**
 * 拆分如 "test, quiet" 的字符串为 ["test", "quiet"]
 */
static void split_groups(const char *groups, std::vector<std::string> *rs)
{
    assert(nullptr != groups && nullptr != rs);

    const char GROUP_SPLITER = ','; // 分组分隔符

    std::string s;
    for (size_t i = 0; 0 != groups[i]; ++i)
    {
        const char c = groups[i];
        if (GROUP_SPLITER == c)
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
        else if (s.length() > 0 || !is_blank(c))
        {
            s.push_back(c);
        }
    }
    s = trim_end(s);
    if (s.length() > 0)
        rs->push_back(s);
}

TestRegister::TestRegister(const char *fixture_name, const char *groups,
                           new_fixture_func n, delete_fixture_func d)
    : _new_func(n), _delete_func(d), _fixture_name(fixture_name)
{
    assert(nullptr != fixture_name);
    assert(nullptr != groups);
    assert(nullptr != n);
    assert(nullptr != d);

    // 分离出组别
    split_groups(groups, &_groups);

    // 将实例添加到链表中
    _pnext = *get_link_header();
    *get_link_header() = this;
}

TestRegister** TestRegister::get_link_header()
{
    static TestRegister *header = nullptr;
    return &header;
}

bool TestRegister::match_group(const char *group_name) const
{
    assert(nullptr != group_name);
    for (std::vector<std::string>::const_iterator iter = _groups.begin();
         iter != _groups.end(); ++iter)
    {
        if (0 == ::strcmp(iter->c_str(), group_name))
            return true;
    }
    return false;
}

const char* TestRegister::get_fixture_name() const
{
    return _fixture_name;
}

TestRegister* TestRegister::get_next_register() const
{
    return _pnext;
}

TestFixture* TestRegister::new_fixture() const
{
    assert(nullptr != _new_func);
    return _new_func();
}

void TestRegister::delete_fixture(TestFixture *p)
{
    assert(nullptr != p);
    _delete_func(p);
}

}
