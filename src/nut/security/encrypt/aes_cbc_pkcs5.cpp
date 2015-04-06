
#include "aes_cbc_pkcs5.hpp"

#include <assert.h>

namespace nut
{

static void xor_buf(uint8_t *buf1, const uint8_t *buf2, size_t len = 16)
{
    assert(NULL != buf1 && NULL != buf2);
    for (size_t i = 0; i < len; ++i)
        buf1[i] ^= buf2[i];
}

void AES_CBC_PKCS5::start_encrypt(const uint8_t* key, int key_bits, const uint8_t iv[16])
{
    assert(NULL != key && NULL != iv);
    m_state = IN_ENCRYPT;
    m_data_buf_size = 0;
    m_result.resize(0);
    bool rs = m_aes.set_key(key, key_bits);
    assert(rs);
    ::memcpy(m_iv, iv, 16);
}

void AES_CBC_PKCS5::update_encrypt(const uint8_t *data, size_t data_len)
{
    assert(NULL != data);
    assert(IN_ENCRYPT == m_state && m_data_buf_size < 16);

    if (m_data_buf_size != 0 && m_data_buf_size + data_len >= 16)
    {
        ::memcpy(m_data_buf + m_data_buf_size, data, 16 - m_data_buf_size);
        xor_buf(m_iv, m_data_buf, 16);
        m_aes.encrypt(m_iv, m_iv);
        m_result.append(m_iv, 16);

        data += 16 - m_data_buf_size;
        data_len -= 16 - m_data_buf_size;
        m_data_buf_size = 0;
    }

    size_t i = 0;
    for (; i < data_len / 16; ++i)
    {
        xor_buf(m_iv, data + i * 16, 16);
        m_aes.encrypt(m_iv, m_iv);
        m_result.append(m_iv, 16);
    }

    ::memcpy(m_data_buf + m_data_buf_size, data + i * 16, data_len - i * 16);
    m_data_buf_size += data_len - i * 16;
    assert(m_data_buf_size < 16);
}

void AES_CBC_PKCS5::finish_encrypt()
{
    assert(IN_ENCRYPT == m_state && m_data_buf_size < 16);
    /* PKCS5 填充 */
    for (size_t i = m_data_buf_size; i < 16; ++i)
        m_data_buf[i] = 16 - m_data_buf_size;
    xor_buf(m_iv, m_data_buf, 16);
    m_aes.encrypt(m_iv, m_iv);

    m_result.append(m_iv, 16);
    m_state = READY;
    m_data_buf_size = 0;
}

void AES_CBC_PKCS5::start_decrypt(const uint8_t *key, int key_bits, const uint8_t iv[16])
{
    assert(NULL != key && NULL != iv);
    m_state = IN_DECRYPT;
    m_data_buf_size = 0;
    m_result.resize(0);
    bool rs = m_aes.set_key(key, key_bits);
    assert(rs);
    ::memcpy(m_iv, iv, 16);
}


void AES_CBC_PKCS5::update_decrypt(const uint8_t *data, size_t data_len)
{
    assert(NULL != data);
    assert(IN_DECRYPT == m_state && m_data_buf_size < 16);

    uint8_t buf[16];
    if (m_data_buf_size != 0 && m_data_buf_size + data_len >= 16)
    {
        ::memcpy(m_data_buf + m_data_buf_size, data, 16 - m_data_buf_size);
        m_aes.decrypt(m_data_buf, buf);
        xor_buf(buf, m_iv, 16);
        m_result.append(buf,16);
        ::memcpy(m_iv, m_data_buf, 16);

        data += 16 - m_data_buf_size;
        data_len -= 16 - m_data_buf_size;
        m_data_buf_size = 0;
    }

    size_t i = 0;
    for (; i < data_len / 16; ++i)
    {
        m_aes.decrypt(data + i * 16, buf);
        xor_buf(buf, m_iv, 16);
        m_result.append(buf, 16);
        ::memcpy(m_iv, data + i * 16, 16);
    }

    ::memcpy(m_data_buf + m_data_buf_size, data + i * 16, data_len - i * 16);
    m_data_buf_size += data_len - i * 16;
    assert(m_data_buf_size < 16);
}

bool AES_CBC_PKCS5::finish_decrypt()
{
    assert(IN_DECRYPT == m_state);
    if (0 != m_data_buf_size || m_result.size() <= 0)
        return false;

    const uint8_t last_byte = m_result[m_result.size() - 1];
    for (size_t i = 0; i < last_byte; ++i)
        if (m_result[m_result.size() - i - 1] != last_byte)
            return false;
    m_result.resize(m_result.size() - last_byte);
    m_state = READY;
    m_data_buf_size = 0;
    return true;
}

}
