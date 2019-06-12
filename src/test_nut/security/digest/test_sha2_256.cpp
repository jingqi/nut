
#include <iostream>

#include <nut/unittest/unittest.h>

#include <nut/security/digest/sha2_256.h>


using namespace std;
using namespace nut;

class TestSHA2_256 : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void check_sha2_256(const char *msg, const char *rs)
    {
        SHA2_256 digest;
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
        check_sha2_256("", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
        check_sha2_256("a", "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb");
        check_sha2_256("abc", "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
        check_sha2_256("abcd", "88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589");
        check_sha2_256("message digest", "f7846f55cf23e14eebeab5b4e1550cad5b509e3348fbc4efa3a1413d393cb650");
        check_sha2_256("abcdefghijklmnopqrstuvwxyz", "71c480df93d6ae2f1efad1447c66c9525e316218cf51fc8d9ed832f2daf18b73");
        check_sha2_256("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "db4bfcbd4da0cd85a60c3c37d3fbd8805c77f15fc6b1fdfe614ee0a7c8fdb4c0");
        check_sha2_256("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "f371bc4a311f2b009eef952dd83ca80e2b60026c8e935592d0f9c308453c813e");
    }
};

NUT_REGISTER_FIXTURE(TestSHA2_256, "security, digest, quiet")
