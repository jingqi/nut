
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/security/digest/crc16.h>
#include <nut/util/string/string_utils.h>


using namespace std;
using namespace nut;

class TestCRC16 : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void check_crc16(const char *msg, uint16_t rs)
    {
        CRC16 crc;
        crc.update(msg, ::strlen(msg));

        if (crc.get_result() != rs)
        {
            cerr << " CRC16: \"" << msg << "\" expect " << format("%x", rs) <<
                " got " << format("%x", crc.get_result()) << endl;
        }
        NUT_TA(crc.get_result() == rs);
    }

    void test_smoking()
    {
        check_crc16("", 0);
        check_crc16("a", 0x7c87);
        check_crc16("abc", 0x9dd6);
        check_crc16("message digest", 0x9ba6);
        check_crc16("abcdefghijklmnopqrstuvwxyz", 0x63ac);
        check_crc16("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 0x7db0);
        check_crc16("12345678901234567890123456789012345678901234567890123456789012345678901234567890", 0xe73a);
    }
};

NUT_REGISTER_FIXTURE(TestCRC16, "security, digest, quiet")
