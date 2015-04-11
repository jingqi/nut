
#include <nut/unittest/unittest.h>

#include <iostream>
#include <nut/container/lru_data_cache.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestLRUDataCache)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_smoking()
    {
        LRUDataCache<int> c(7); // capacity in bytes
        const void *s = NULL;
        size_t cb = 0;
        NUT_TA(!c.get(1,&s,&cb) && NULL == s && 0 == cb);
        c.put(1,"ab",3);
        NUT_TA(c.get(1,&s,&cb) && std::string("ab") == (const char*)s && 3 == cb);
        c.put(2,"cde",4);
        NUT_TA(c.get(2,&s,&cb) && std::string("cde") == (const char*)s && 4 == cb);
        c.put(3,"fghi",5);
        NUT_TA(c.get(3,&s,&cb) && std::string("fghi") == (const char*)s && 5 == cb);
        s = NULL;
        cb = 0;
        NUT_TA(!c.get(1,&s,&cb) && NULL == s && 0 == cb);

        c.remove(3);
        NUT_TA(!c.get(3,&s,&cb) && NULL == s && 0 == cb);

        c.clear();
        NUT_TA(!c.get(2,&s, &cb) && NULL == s && 0 == cb);
    }

};

NUT_REGISTER_FIXTURE(TestLRUDataCache, "container, quiet")
