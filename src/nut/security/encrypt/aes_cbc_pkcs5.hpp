
#ifndef ___HEADFILE_770F82A8_D4E3_45A1_A6B9_1790204ACF29_
#define ___HEADFILE_770F82A8_D4E3_45A1_A6B9_1790204ACF29_

#include <stdint.h>

#include <nut/container/byte_array.hpp>

#include "aes.hpp"

namespace nut
{

/**
 * AES/CBC/PKCS5
 *
 * 加解密方式：AES
 * 分组方式：CBC
 * 填充方式：PKCS5
 */
class AES_CBC_PKCS5
{
    enum
    {
        READY,
        IN_ENCRYPT,
        IN_DECRYPT
    } m_state;
    uint8_t m_data_buf[16], m_iv[16];
    size_t m_data_buf_size;
    ByteArray m_result;
    AES m_aes;

public:
    /**
     * 开始加密
     *
     * @param key       密钥
     * @param key_bits  密钥的比特长度，只能取128, 192, 256
     * @param iv        初始化向量，加密时可随机给出
     */
    void start_encrypt(const uint8_t* key, int key_bits, const uint8_t iv[16]);

    /**
     * 更新要加密的数据
     */
    void update_encrypt(const uint8_t *data, size_t data_len);

    /**
     * 结束加密过程
     */
    void finish_encrypt();

public:
    /**
    * @param key       密钥
    * @param key_bits  密钥的比特长度，只能取128, 192, 256
    * @param iv        初始化向量，必须与加密时使用的是相同的
     */
    void start_decrypt(const uint8_t *key, int key_bits, const uint8_t iv[16]);

    /**
     * 更新要解密的数据
     */
    void update_decrypt(const uint8_t *data, size_t data_len);

    /**
     * 结束解密过程
     *
     * @param 解密失败则返回 false
     */
    bool finish_decrypt();

public:
    /**
     * 获取加密或者解密结果
     */
    const ByteArray& get_result() const
    {
        return m_result;
    }
};

}

#endif
