
#include <iostream>

#include <nut/unittest/unittest.h>

#include <nut/security/digest/sha1.h>


using namespace std;
using namespace nut;

class TestSHA1 : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void check_sha1(const char *msg, const char *rs)
    {
        SHA1 digest;
        digest.reset();
        digest.update(msg, ::strlen(msg));
        digest.digest();

        if (digest.get_hex_result() != rs)
        {
            cerr << " SHA1: \"" << msg << "\" expect " << rs <<
                " got " << digest.get_hex_result() << endl;
        }
        NUT_TA(digest.get_hex_result() == rs);
    }

    void test_smoking()
    {
        check_sha1("", "da39a3ee5e6b4b0d3255bfef95601890afd80709");
        check_sha1("a", "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8");
        check_sha1("abc", "a9993e364706816aba3e25717850c26c9cd0d89d");
        check_sha1("abcd", "81fe8bfe87576c3ecb22426f8e57847382917acf");
        check_sha1("message digest", "c12252ceda8be8994d5fa0290a47231c1d16aae3");
        check_sha1("abcdefghijklmnopqrstuvwxyz", "32d10c7b8cf96570ca04ce37f2a19d84240d3a89");
        check_sha1("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "761c457bf73b14d27e9e9265c46f4b4dda11f940");
        check_sha1("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "50abf5706a150990a08b2c5ea40fa0e585554732");
    }
};

NUT_REGISTER_FIXTURE(TestSHA1, "security, digest, quiet")
