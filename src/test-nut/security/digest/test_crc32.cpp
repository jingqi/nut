
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/security/digest/crc32.h>
#include <nut/util/string/string_utils.h>


using namespace std;
using namespace nut;

class TestCRC32 : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void check_crc32(const char *msg, uint32_t rs)
    {
        CRC32 crc;
        const size_t len = ::strlen(msg);
        if (len < 16)
        {
            crc.update(msg, len);
        }
        else
        {
            // 故意分两次
            crc.update(msg, 15);
            crc.update(msg + 15, len - 15);
        }

        if (crc.get_result() != rs)
        {
            cerr << " CRC32: \"" << msg << "\" expect " << format("%x", rs) <<
                " got " << format("%x", crc.get_result()) << endl;
        }
        NUT_TA(crc.get_result() == rs);
    }

    void test_smoking()
    {
        check_crc32("", 0);
        check_crc32("a", 0xe8b7be43);
        check_crc32("abc", 0x352441c2);
        check_crc32("message digest", 0x20159d7f);
        check_crc32("abcdefghijklmnopqrstuvwxyz", 0x4c2750bd);
        check_crc32("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 0x1fc2e6d2);
        check_crc32("12345678901234567890123456789012345678901234567890123456789012345678901234567890", 0x7ca94a72);
    }
};

NUT_REGISTER_FIXTURE(TestCRC32, "security, digest, quiet")
