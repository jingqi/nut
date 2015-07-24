
#ifndef ___HEADFILE___53924D54_39B0_4377_B555_4096B8CC3619_
#define ___HEADFILE___53924D54_39B0_4377_B555_4096B8CC3619_

#include <string>
#include <vector>

#include <nut/platform/platform.h>

DLL_API void** nut_get_register_header();

#define NUT_UNITTEST_IMPL                           \
    DLL_API void** nut_get_register_header()        \
    {                                               \
        static nut::TestRegister* header = NULL;    \
        return (void**) &header;                    \
    }

namespace nut
{

class TestFixture;

class TestRegister
{
    typedef TestFixture* (*new_fixture_func)();
    typedef void (*delete_fixture_func)(TestFixture*);

    new_fixture_func _new_func = NULL;
    delete_fixture_func _delete_func = NULL;
    const char *_fixture_name = NULL;
    std::vector<std::string> _groups;
    TestRegister *_pnext = NULL;

public:
    TestRegister(const char *fixture_name, const char *groups,
                 new_fixture_func n, delete_fixture_func d);

    bool match_group(const char *group_name) const;

    const char* get_fixture_name() const
    {
        return _fixture_name;
    }

    TestRegister* get_next_register() const
    {
        return _pnext;
    }

    TestFixture* new_fixture() const;

    void delete_fixture(TestFixture *p);
};

}

#endif /* head file guarder */
