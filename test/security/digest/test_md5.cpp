
#include <nut/unittest/unittest.hpp>

#include <nut/security/digest/md5.hpp>


NUT_FIXTURE(TestMD5)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void checkMd5(const char *msg, const char *rs)
    {
        nut::MD5 digest;
        digest.initialize();
        digest.update(msg, strlen(msg));
        digest.finalize();
        NUT_TA(digest.getStringResult() == rs);
    }

    void testSmoking()
    {
        checkMd5("", "D41D8CD98F00B204E9800998ECF8427E");
        checkMd5("a", "0CC175B9C0F1B6A831C399E269772661");
        checkMd5("abc", "900150983CD24FB0D6963F7D28E17F72");
        checkMd5("message digest", "F96B697D7CB7938D525A2F31AAF161D0");
        checkMd5("abcdefghijklmnopqrstuvwxyz", "C3FCD3D76192E4007DFB496CCA67E13B");
        checkMd5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "D174AB98D277D9F5A5611C2C9F419D9F");
        checkMd5("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57EDF4A22BE3C955AC49DA2E2107B67A");
    }
};

NUT_REGISTER_FIXTURE(TestMD5, "security, quiet")
