
#include <vector>
#include <iostream>

#include <nut/unittest/unittest.h>

#include <nut/security/encrypt/rsa_pkcs1.h>
#include <nut/container/array.h>

using namespace std;
using namespace nut;

class TestRsaPkcs1 : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking1);
        NUT_REGISTER_CASE(test_smoking2);
    }

    void test_smoking1()
    {
        const uint8_t data[] = {
            1,2,3,4,5,6,7,8,9,0,'d','e','f','a','b','c',
            2,3,4,5,6,7,8,9,0,1,'c','d','e','f','a','b',
            3,4,5,6,7,8,9,0,1,2,'b','c','d','e','f','a',
            4,5,6,7,8,9,0,1,2,3,'a','b','c','d','e','f'
        };
        RSA::PrivateKey key = RSA::gen_key(122);
        NUT_TA(key.n.bit_length() == 121 || key.n.bit_length() == 122);
        NUT_TA((key.n.bit_length() + 7) / 8 == 16); // 16 字节 EB, 5 字节有效数据

        RSA_PKCS1 rp;
        vector<uint8_t> v1, v2;
        rp.set_callback(
            [&] (const void *data, size_t cb) {
                v1.insert(v1.end(), (const uint8_t*) data, (const uint8_t*) data + cb);
            });
        rp.start_public_encrypt(key);
        rp.update_encrypt(data, 7);
        rp.update_encrypt(data + 7, 3);
        rp.update_encrypt(data + 10, 5);
        rp.update_encrypt(data + 15, 49);
        rp.finish_encrypt();
        // cout << v1.size() << endl;
        NUT_TA(v1.size() == 208);

        rp.set_callback(
            [&] (const void *data, size_t cb) {
                v2.insert(v2.end(), (const uint8_t*) data, (const uint8_t*) data + cb);
            });
        rp.start_private_decrypt(key);
        NUT_TA(rp.update_decrypt(v1.data(), 15));
        NUT_TA(rp.update_decrypt(v1.data() + 15, 17));
        NUT_TA(rp.update_decrypt(v1.data() + 32, 19));
        NUT_TA(rp.update_decrypt(v1.data() + 51, 157));
        NUT_TA(rp.finish_decrypt());
        NUT_TA(v2.size() == 64);
        NUT_TA(0 == ::memcmp(v2.data(), data, 64));
    }

    void test_smoking2()
    {
        const uint8_t data[] = {
            1,2,3,4,5,6,7,8,9,0,'d','e','f','a','b','c',
            2,3,4,5,6,7,8,9,0,1,'c','d','e','f','a','b',
            3,4,5,6,7,8,9,0,1,2,'b','c','d','e','f','a',
            4,5,6,7,8,9,0,1,2,3,'a','b','c','d','e','f'
        };
        RSA::PrivateKey key = RSA::gen_key(114);
        NUT_TA(key.n.bit_length() == 113 || key.n.bit_length() == 114);
        NUT_TA((key.n.bit_length() + 7) / 8 == 15); // 15 字节 EB, 4 字节有效数据

        RSA_PKCS1 rp;
        vector<uint8_t> v1, v2;
        rp.set_callback(
            [&] (const void *data, size_t cb) {
                v1.insert(v1.end(), (const uint8_t*) data, (const uint8_t*) data + cb);
            });
        rp.start_private_encrypt(key);
        rp.update_encrypt(data, 7);
        rp.update_encrypt(data + 7, 3);
        rp.update_encrypt(data + 10, 5);
        rp.update_encrypt(data + 15, 49);
        rp.finish_encrypt();
        NUT_TA(v1.size() == 240);

        rp.set_callback(
            [&] (const void *data, size_t cb) {
                v2.insert(v2.end(), (const uint8_t*) data, (const uint8_t*) data + cb);
            });
        rp.start_public_decrypt(key);
        NUT_TA(rp.update_decrypt(v1.data(), 15));
        NUT_TA(rp.update_decrypt(v1.data() + 15, 17));
        NUT_TA(rp.update_decrypt(v1.data() + 32, 19));
        NUT_TA(rp.update_decrypt(v1.data() + 51, 189));
        NUT_TA(rp.finish_decrypt());
        NUT_TA(v2.size() == 64);
        NUT_TA(0 == ::memcmp(v2.data(), data, 64));
    }
};

NUT_REGISTER_FIXTURE(TestRsaPkcs1, "security, encrypt, quiet")
