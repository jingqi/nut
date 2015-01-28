
#include <stdio.h>


#include <nut/unittest/unittest.hpp>

#include <nut/security/encrypt/aes_cbc_pkcs5.hpp>

using namespace nut;


NUT_FIXTURE(TestAesCbcPkcs5)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_smoking()
    {
        const uint8_t data[] = {
            1,2,3,4,5,6,7,8,9,0,'a','b','c','d','e','f',
            1,2,3,4,5,6,7,8,9,0,'a','b','c','d','e','f',
            1,2,3,4,5,6,7,8,9,0,'a','b','c','d','e','f',
            1,2,3,4,5,6,7,8,9,0,'a','b','c','d','e','f'
        };
        
        const uint8_t key[16] = {
            1, 3, 5, 6, 8, 9, 0xe, 0x1c, 7, 8, 11, 23, 36, 41, 15, 90
        };
        const uint8_t iv[16] = {
            45, 4, 9, 24, 2, 0, 0x1e, 12, 2, 58, 65, 14, 4, 6, 3, 14
        };

        AES_CBC_PKCS5 acp;
        {
            acp.start_encrypt(key, sizeof(key) * 8, iv);
            acp.update_encrypt(data, 7);
            acp.finish_encrypt();
            ByteArray ba = acp.get_result();

            acp.start_decrypt(key, sizeof(key) * 8, iv);
            acp.update_decrypt(ba.data(), ba.size());
            acp.finish_decrypt();
            ba = acp.get_result();
            NUT_TA(7 == ba.size());
            NUT_TA(0 == ::memcmp(ba.data(), data, 7));
        }

        {
            acp.start_encrypt(key, sizeof(key) * 8, iv);
            acp.update_encrypt(data, 16);
            acp.finish_encrypt();
            ByteArray ba = acp.get_result();

            acp.start_decrypt(key, sizeof(key) * 8, iv);
            acp.update_decrypt(ba.data(), ba.size());
            acp.finish_decrypt();
            ba = acp.get_result();
            NUT_TA(16 == ba.size());
            NUT_TA(0 == ::memcmp(ba.data(), data, 16));
        }
        
        {
            acp.start_encrypt(key, sizeof(key) * 8, iv);
            acp.update_encrypt(data, 7);
            acp.update_encrypt(data + 7, 19);
            acp.finish_encrypt();
            ByteArray ba = acp.get_result();

            acp.start_decrypt(key, sizeof(key) * 8, iv);
            acp.update_decrypt(ba.data(), ba.size());
            acp.finish_decrypt();
            ba = acp.get_result();
            NUT_TA(26 == ba.size());
            NUT_TA(0 == ::memcmp(ba.data(), data, 26));
        }
    }
};

NUT_REGISTER_FIXTURE(TestAesCbcPkcs5, "security, encrypt, quiet")
