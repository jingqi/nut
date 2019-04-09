﻿
#ifndef ___HEADFILE___53924D54_39B0_4377_B555_4096B8CC3619_
#define ___HEADFILE___53924D54_39B0_4377_B555_4096B8CC3619_

#include <string>
#include <vector>
#include <functional>

#include "../nut_config.h"
#include "../platform/platform.h"


namespace nut
{

class TestFixture;

class NUT_API TestRegister
{
private:
    typedef std::function<TestFixture*()> new_fixture_func;
    typedef std::function<void(TestFixture*)> delete_fixture_func;

public:
    TestRegister(const char *fixture_name, const char *groups,
                 new_fixture_func&& n, delete_fixture_func&& d);

    static TestRegister** get_link_header();
    TestRegister* get_next_register() const;

    bool match_group(const char *group_name) const;

    const char* get_fixture_name() const;

    TestFixture* new_fixture() const;
    void delete_fixture(TestFixture *p);

private:
    new_fixture_func _new_func = nullptr;
    delete_fixture_func _delete_func = nullptr;

    const char *_fixture_name = nullptr;
    std::vector<std::string> _groups;

    TestRegister *_pnext = nullptr;
};

}

#endif /* head file guarder */
