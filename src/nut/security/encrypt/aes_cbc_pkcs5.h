
#ifndef ___HEADFILE_770F82A8_D4E3_45A1_A6B9_1790204ACF29_
#define ___HEADFILE_770F82A8_D4E3_45A1_A6B9_1790204ACF29_

#include <stdint.h>
#include <functional>

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
    typedef std::function<void(const void*,size_t)> callback_type;

public:
    void set_callback(callback_type&& cb) noexcept;
    void set_callback(const callback_type& cb) noexcept;

    /**
     * 开始加密
     *
     * @param key       密钥
     * @param key_bits  密钥的比特长度，只能取128, 192, 256
     * @param iv        16字节初始化向量，加密时可随机给出
     */
    void start_encrypt(const void* key, int key_bits, const void *iv) noexcept;

    /**
     * 加密
     */
    void update_encrypt(const void *data, size_t data_len) noexcept;

    /**
     * 填充小节，结束一段加密过程
     *
     * NOTE 如果要复用 key 和当前 iv, 后面可以接着直接 update_encrypt()
     */
    void finish_encrypt() noexcept;

    /**
     * 开始解密
     *
     * @param key       密钥
     * @param key_bits  密钥的比特长度，只能取128, 192, 256
     * @param iv        16字节初始化向量，必须与加密时使用的是相同的
     */
    void start_decrypt(const void *key, int key_bits, const void *iv) noexcept;

    /**
     * 解密
     */
    bool update_decrypt(const void *data, size_t data_len) noexcept;

    /**
     * 结束一段解密过程
     *
     * NOTE 如果要复用 key 和当前 iv, 后面可以接着直接 update_decrypt()
     *
     * @return 解密失败则返回 false
     */
    bool finish_decrypt() noexcept;

private:
    enum class State
    {
        Ready,
        InEncrypt,
        InDecrypt,
        Error,
    } _state = State::Ready;

    uint8_t _iv[16];

    uint8_t _input_buffer[16];
    size_t _input_buffer_size = 0;

    /* 仅仅给解密过程使用 */
    uint8_t _decrypt_buffer[16];
    bool _decrypt_buffer_has_data = false;

    AES _aes;

    callback_type _callback;
};

}

#endif
