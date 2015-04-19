
#include <nut/unittest/unit_test.h>

#include <nut/security/digest/md5.h>


NUT_FIXTURE(TestMD5)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void check_md5(const char *msg, const char *rs)
    {
        nut::MD5 digest;
        digest.reset();
        digest.update(msg, strlen(msg));
        digest.digest();
        NUT_TA(digest.get_string_result() == rs);
    }

    void test_smoking()
    {
        check_md5("", "D41D8CD98F00B204E9800998ECF8427E");
        check_md5("a", "0CC175B9C0F1B6A831C399E269772661");
        check_md5("abc", "900150983CD24FB0D6963F7D28E17F72");
        check_md5("message digest", "F96B697D7CB7938D525A2F31AAF161D0");
        check_md5("abcdefghijklmnopqrstuvwxyz", "C3FCD3D76192E4007DFB496CCA67E13B");
        check_md5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "D174AB98D277D9F5A5611C2C9F419D9F");
        check_md5("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57EDF4A22BE3C955AC49DA2E2107B67A");
    }
};

NUT_REGISTER_FIXTURE(TestMD5, "security, digest, quiet")
