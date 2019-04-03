﻿
#ifndef ___HEADFILE_E8AE3A78_775C_49E6_A3A9_14202F72E750_
#define ___HEADFILE_E8AE3A78_775C_49E6_A3A9_14202F72E750_

#include <stdint.h>
#include <functional>

#include "../../nut_config.h"
#include "rsa.h"


namespace nut
{

/**
 * RSA 分组加密
 *
 * 加解密方式：RSA
 * 分组方式：PKCS1
 * 填充方式：PKCS1
 *
 * 参见: https://www.cnblogs.com/NathanYang/p/9182929.html
 */
class NUT_API RSA_PKCS1
{
public:
    typedef std::function<void(const void*,size_t)> callback_type;

public:
    ~RSA_PKCS1();

    void set_callback(callback_type&& cb);
    void set_callback(const callback_type& cb);

    /**
     * 开始加密
     */
    void start_encrypt(const RSA::PublicKey& key);
    void start_encrypt(const RSA::PrivateKey& key);

    /**
     * 加密
     */
    void update_encrypt(const void *data, size_t data_len);

    /**
     * 填充小节，结束一段加密过程
     */
    void finish_encrypt();

    /**
     * 开始解密
     */
    void start_decrypt(const RSA::PublicKey& key);
    void start_decrypt(const RSA::PrivateKey& key);

    /**
     * 解密
     */
    bool update_decrypt(const void *data, size_t data_len);

    /**
     * 结束一段解密过程
     *
     * @return 解密失败则返回 false
     */
    bool finish_decrypt();

private:
    void pack_eb(size_t data_len);
    bool unpack_eb(const BigInteger& output);

private:
    RSA::PublicKey _public_key;
    RSA::PrivateKey _private_key;
    bool _use_public_key = true;
    size_t _encryption_block_size = 0; // EB 大小只跟 key 的长度相关

    /*
     * 加密块, big-endian
     *
     * EB = 00 + BT + PS + 00 + D
     */
    uint8_t *_encryption_block = nullptr;
    size_t _input_size = 0;

    /* 用于运算的整数, little-endian */
    BigInteger _input_integer;

    bool _has_error = false;
    callback_type _callback;
};

}

#endif
