
#include <stdio.h>


#include <nut/unittest/unittest.h>

#include <nut/security/encrypt/aes_cbc_pkcs5.h>
#include <nut/container/array.h>

using namespace nut;

class TestAesCbcPkcs5 : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
        NUT_REGISTER_CASE(test_steam);
    }

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

        AES_CBC_PKCS5 acp_enc, acp_dec;
        Array<uint8_t> ba1, ba2;
        acp_enc.set_callback(
            [&] (const void *data, size_t cb) {
                ba1.append((const uint8_t*) data, (const uint8_t*) data + cb);
            });
        acp_dec.set_callback(
            [&] (const void *data, size_t cb) {
                ba2.append((const uint8_t*) data, (const uint8_t*) data + cb);
            });

        {
            acp_enc.start_encrypt(key, sizeof(key) * 8, iv);
            acp_enc.update_encrypt(data, 7);
            acp_enc.finish_encrypt();

            acp_dec.start_decrypt(key, sizeof(key) * 8, iv);
            acp_dec.update_decrypt(ba1.data(), ba1.size());
            acp_dec.finish_decrypt();
            NUT_TA(7 == ba2.size());
            NUT_TA(0 == ::memcmp(ba2.data(), data, 7));
        }

        {
            ba1.clear();
            acp_enc.start_encrypt(key, sizeof(key) * 8, iv);
            acp_enc.update_encrypt(data, 16);
            acp_enc.finish_encrypt();

            ba2.clear();
            acp_dec.start_decrypt(key, sizeof(key) * 8, iv);
            acp_dec.update_decrypt(ba1.data(), ba1.size());
            acp_dec.finish_decrypt();
            NUT_TA(16 == ba2.size());
            NUT_TA(0 == ::memcmp(ba2.data(), data, 16));
        }

        {
            ba1.clear();
            acp_enc.start_encrypt(key, sizeof(key) * 8, iv);
            acp_enc.update_encrypt(data, 7);
            acp_enc.update_encrypt(data + 7, 19);
            acp_enc.finish_encrypt();

            ba2.clear();
            acp_dec.start_decrypt(key, sizeof(key) * 8, iv);
            acp_dec.update_decrypt(ba1.data(), ba1.size());
            acp_dec.finish_decrypt();
            NUT_TA(26 == ba2.size());
            NUT_TA(0 == ::memcmp(ba2.data(), data, 26));
        }
    }

    void test_steam()
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

        AES_CBC_PKCS5 acp_enc, acp_dec;
        Array<uint8_t> ba1, ba2, ba3, ba4;

        // Encrypt package 1
        acp_enc.set_callback(
            [&] (const void *data, size_t cb) {
                ba1.append((const uint8_t*) data, (const uint8_t*) data + cb);
            });
        acp_enc.start_encrypt(key, sizeof(key) * 8, iv);
        acp_enc.update_encrypt(data, 27);
        acp_enc.finish_encrypt();

        // Encrypt package 2
        acp_enc.set_callback(
            [&] (const void *data, size_t cb) {
                ba2.append((const uint8_t*) data, (const uint8_t*) data + cb);
            });
        acp_enc.update_encrypt(data, 37);
        acp_enc.finish_encrypt();

        // Decrypt package 1
        acp_dec.set_callback(
            [&] (const void *data, size_t cb) {
                ba3.append((const uint8_t*) data, (const uint8_t*) data + cb);
            });
        acp_dec.start_decrypt(key, sizeof(key) * 8, iv);
        acp_dec.update_decrypt(ba1.data(), ba1.size());
        acp_dec.finish_decrypt();
        NUT_TA(27 == ba3.size());
        NUT_TA(0 == ::memcmp(ba3.data(), data, 27));

        // Decrypt package 2
        acp_dec.set_callback(
            [&] (const void *data, size_t cb) {
                ba4.append((const uint8_t*) data, (const uint8_t*) data + cb);
            });
        acp_dec.update_decrypt(ba2.data(), ba2.size());
        acp_dec.finish_decrypt();
        NUT_TA(37 == ba4.size());
        NUT_TA(0 == ::memcmp(ba4.data(), data, 37));
    }
};

NUT_REGISTER_FIXTURE(TestAesCbcPkcs5, "security, encrypt, quiet")
