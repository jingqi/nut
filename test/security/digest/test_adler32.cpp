
#include <nut/unittest/unittest.hpp>

#include <nut/security/digest/adler32.hpp>


NUT_FIXTURE(TestAdler32)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testAdler32)
    NUT_CASE(testRollingAdler32)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void checkAdler32(const char *msg, uint32_t expected)
    {
        nut::Adler32 ad;
        ad.reset();
        ad.update(msg, strlen(msg));
        NUT_TA(ad.digest() == expected);
    }

    void testAdler32()
    {
        checkAdler32("Wikipedia", 0x11E60398);
    }

    void testRollingAdler32()
    {
        nut::RollingAdler32 ad(3);
        ad.initialize();
        ad.update("abcd", 4);
        uint32_t v = ad.getResult();
        ad.update("bcd", 3);
        uint32_t vv = ad.getResult();
        NUT_TA(v == vv);
    }
};

NUT_REGISTER_FIXTURE(TestAdler32, "security, quiet")
