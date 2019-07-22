
#include <nut/unittest/unittest.h>

#include <nut/security/digest/adler32.h>


using namespace nut;

class TestAdler32 : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_adler32);
        NUT_REGISTER_CASE(test_rolling_adler32);
    }

    void check_adler32(const char *msg, uint32_t expected)
    {
        Adler32 ad;
        ad.reset();
        ad.update(msg, strlen(msg));
        NUT_TA(ad.digest() == expected);
    }

    void test_adler32()
    {
        check_adler32("Wikipedia", 0x11E60398);
    }

    void test_rolling_adler32()
    {
        RollingAdler32 ad(3);
        ad.initialize();
        ad.update("abcd", 4);
        uint32_t v = ad.get_result();
        ad.update("bcd", 3);
        uint32_t vv = ad.get_result();
        NUT_TA(v == vv);
    }
};

NUT_REGISTER_FIXTURE(TestAdler32, "security, digest, quiet")
