
#include <iostream>

#include <nut/unittest/unit_test.h>

#include <nut/security/digest/sha1.h>


using namespace std;

NUT_FIXTURE(TestSHA1)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void check_sha1(const char *msg, const char *rs)
    {
        nut::SHA1 digest;
        digest.reset();
        digest.update(msg, ::strlen(msg));
        digest.digest();

        if (digest.get_string_result() != rs)
        {
            cerr << " SHA1: \"" << msg << "\" expect " << rs <<
                " got " << digest.get_string_result() << endl;
        }
        NUT_TA(digest.get_string_result() == rs);
    }

    void test_smoking()
    {
        check_sha1("", "DA39A3EE5E6B4B0D3255BFEF95601890AFD80709");
        check_sha1("a", "86F7E437FAA5A7FCE15D1DDCB9EAEAEA377667B8");
        check_sha1("abc", "A9993E364706816ABA3E25717850C26C9CD0D89D");
        check_sha1("abcd", "81FE8BFE87576C3ECB22426F8E57847382917ACF");
        check_sha1("message digest", "C12252CEDA8BE8994D5FA0290A47231C1D16AAE3");
        check_sha1("abcdefghijklmnopqrstuvwxyz", "32D10C7B8CF96570CA04CE37F2A19D84240D3A89");
        check_sha1("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "761C457BF73B14D27E9E9265C46F4B4DDA11F940");
        check_sha1("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "50ABF5706A150990A08B2C5EA40FA0E585554732");
    }
};

NUT_REGISTER_FIXTURE(TestSHA1, "security, digest, quiet")
