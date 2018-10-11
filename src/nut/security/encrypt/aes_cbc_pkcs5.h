
#ifndef ___HEADFILE_770F82A8_D4E3_45A1_A6B9_1790204ACF29_
#define ___HEADFILE_770F82A8_D4E3_45A1_A6B9_1790204ACF29_

#include <stdint.h>

#include <nut/container/array.h>

#include "../../nut_config.h"
#include "aes.h"


namespace nut
{

/**
 * AES/CBC/PKCS5
 *
 * 加解密方式：AES
 * 分组方式：CBC
 * 填充方式：PKCS5
 */
class NUT_API AES_CBC_PKCS5
{
public:
    /**
     * 开始加密
     *
     * @param key       密钥
     * @param key_bits  密钥的比特长度，只能取128, 192, 256
     * @param iv        16字节初始化向量，加密时可随机给出
     */
    void start_encrypt(const void* key, int key_bits, const void *iv);

    /**
     * 更新要加密的数据
     */
    void update_encrypt(const void *data, size_t data_len);

    /**
     * 结束加密过程
     */
    void finish_encrypt();

    /**
     * 开始解密
     *
     * @param key       密钥
     * @param key_bits  密钥的比特长度，只能取128, 192, 256
     * @param iv        16字节初始化向量，必须与加密时使用的是相同的
     */
    void start_decrypt(const void *key, int key_bits, const void *iv);

    /**
     * 更新要解密的数据
     */
    void update_decrypt(const void *data, size_t data_len);

    /**
     * 结束解密过程
     *
     * @return 解密失败则返回 false
     */
    bool finish_decrypt();

    /**
     * 获取加密或者解密结果
     */
    COWArray<uint8_t> get_result() const;

private:
    enum
    {
        READY,
        IN_ENCRYPT,
        IN_DECRYPT
    } _state = READY;
    uint8_t _data_buf[16], _iv[16];
    size_t _data_buf_size = 0;
    COWArray<uint8_t> _result;
    AES _aes;
};

}

#endif
