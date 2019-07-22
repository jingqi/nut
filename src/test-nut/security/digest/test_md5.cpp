
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/security/digest/md5.h>


using namespace std;
using namespace nut;

class TestMD5 : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void check_md5(const char *msg, const char *rs)
    {
        MD5 digest;
        digest.reset();
        digest.update(msg, ::strlen(msg));
        digest.digest();

        if (digest.get_hex_result() != rs)
        {
            cerr << " MD5: \"" << msg << "\" expect " << rs <<
                " got " << digest.get_hex_result() << endl;
        }
        NUT_TA(digest.get_hex_result() == rs);
    }

    void test_smoking()
    {
        check_md5("", "d41d8cd98f00b204e9800998ecf8427e");
        check_md5("a", "0cc175b9c0f1b6a831c399e269772661");
        check_md5("abc", "900150983cd24fb0d6963f7d28e17f72");
        check_md5("message digest", "f96b697d7cb7938d525a2f31aaf161d0");
        check_md5("abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b");
        check_md5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "d174ab98d277d9f5a5611c2c9f419d9f");
        check_md5("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57edf4a22be3c955ac49da2e2107b67a");
    }
};

NUT_REGISTER_FIXTURE(TestMD5, "security, digest, quiet")
