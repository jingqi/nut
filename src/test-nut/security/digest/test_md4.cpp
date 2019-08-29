
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/security/digest/md4.h>


using namespace std;
using namespace nut;

class TestMD4 : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void check_md4(const char *msg, const char *rs)
    {
        MD4 digest;
        digest.reset();
        digest.update(msg, ::strlen(msg));
        digest.digest();

        if (digest.get_hex_result() != rs)
        {
            cerr << " MD4: \"" << msg << "\" expect " << rs <<
                " got " << digest.get_hex_result() << endl;
        }
        NUT_TA(digest.get_hex_result() == rs);
    }

    void test_smoking()
    {
        check_md4("", "31d6cfe0d16ae931b73c59d7e0c089c0");
        check_md4("a", "bde52cb31de33e46245e05fbdbd6fb24");
        check_md4("abc", "a448017aaf21d8525fc10ae87aa6729d");
        check_md4("message digest", "d9130a8164549fe818874806e1c7014b");
        check_md4("abcdefghijklmnopqrstuvwxyz", "d79e1c308aa5bbcdeea8ed63df412da9");
        check_md4("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "043f8582f241db351ce627e153e7f0e4");
        check_md4("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "e33b4ddc9c38f2199c3e7b164fcc0536");
    }
};

NUT_REGISTER_FIXTURE(TestMD4, "security, digest, quiet")
