
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/security/digest/md2.h>


using namespace std;
using namespace nut;

class TestMD2 : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void check_md2(const char *msg, const char *rs)
    {
        MD2 digest;
        digest.reset();
        digest.update(msg, ::strlen(msg));
        digest.digest();

        if (digest.get_hex_result() != rs)
        {
            cerr << " MD2: \"" << msg << "\" expect " << rs <<
                " got " << digest.get_hex_result() << endl;
        }
        NUT_TA(digest.get_hex_result() == rs);
    }

    void test_smoking()
    {
        check_md2("", "8350e5a3e24c153df2275c9f80692773");
        check_md2("a", "32ec01ec4a6dac72c0ab96fb34c0b5d1");
        check_md2("abc", "da853b0d3f88d99b30283a69e6ded6bb");
        check_md2("message digest", "ab4f496bfb2a530b219ff33031fe06b0");
        check_md2("abcdefghijklmnopqrstuvwxyz", "4e8ddff3650292ab5a4108c3aa47940b");
        check_md2("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "da33def2a42df13975352846c30338cd");
        check_md2("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "d5976f79d83d3a0dc9806c3c66f3efd8");
    }
};

NUT_REGISTER_FIXTURE(TestMD2, "security, digest, quiet")
