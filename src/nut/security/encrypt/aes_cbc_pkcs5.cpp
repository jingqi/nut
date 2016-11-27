
#include <assert.h>

#include <nut/platform/platform.h>

#include "aes_cbc_pkcs5.h"

namespace nut
{

static void xor_buf(void *buf1, const void *buf2, size_t len = 16)
{
    assert(NULL != buf1 && NULL != buf2);
    for (size_t i = 0; i < len; ++i)
        ((uint8_t*) buf1)[i] ^= ((const uint8_t*) buf2)[i];
}

void AES_CBC_PKCS5::start_encrypt(const void* key, int key_bits, const void *iv)
{
    assert(NULL != key && NULL != iv);
    _state = IN_ENCRYPT;
    _data_buf_size = 0;
    _result.resize(0);
    bool rs = _aes.set_key(key, key_bits);
    assert(rs);
    UNUSED(rs);
    ::memcpy(_iv, iv, 16);
}

void AES_CBC_PKCS5::update_encrypt(const void *data, size_t data_len)
{
    assert(NULL != data);
    assert(IN_ENCRYPT == _state && _data_buf_size < 16);

    if (_data_buf_size != 0 && _data_buf_size + data_len >= 16)
    {
        ::memcpy(_data_buf + _data_buf_size, data, 16 - _data_buf_size);
        xor_buf(_iv, _data_buf, 16);
        _aes.encrypt(_iv, _iv);
        _result.append((const uint8_t*) _iv, (const uint8_t*) _iv + 16);

        ((const uint8_t*&) data) += 16 - _data_buf_size;
        data_len -= 16 - _data_buf_size;
        _data_buf_size = 0;
    }

    size_t i = 0;
    for (; i < data_len / 16; ++i)
    {
        xor_buf(_iv, data, 16);
        _aes.encrypt(_iv, _iv);
        _result.append((const uint8_t*) _iv, (const uint8_t*) _iv + 16);

        ((const uint8_t*&) data) += 16;
    }

    ::memcpy(_data_buf + _data_buf_size, data, data_len & 0x0f);
    _data_buf_size += data_len & 0x0f;
    assert(_data_buf_size < 16);
}

void AES_CBC_PKCS5::finish_encrypt()
{
    assert(IN_ENCRYPT == _state && _data_buf_size < 16);

    /* PKCS5 填充 */
    ::memset(_data_buf + _data_buf_size, 16 - _data_buf_size, 16 - _data_buf_size);
    xor_buf(_iv, _data_buf, 16);
    _aes.encrypt(_iv, _iv);

    _result.append((const uint8_t*) _iv, (const uint8_t*) _iv + 16);
    _state = READY;
    _data_buf_size = 0;
}

void AES_CBC_PKCS5::start_decrypt(const void *key, int key_bits, const void *iv)
{
    assert(NULL != key && NULL != iv);
    _state = IN_DECRYPT;
    _data_buf_size = 0;
    _result.resize(0);
    bool rs = _aes.set_key(key, key_bits);
    assert(rs);
    UNUSED(rs);
    ::memcpy(_iv, iv, 16);
}


void AES_CBC_PKCS5::update_decrypt(const void *data, size_t data_len)
{
    assert(NULL != data);
    assert(IN_DECRYPT == _state && _data_buf_size < 16);

    uint8_t buf[16];
    if (_data_buf_size != 0 && _data_buf_size + data_len >= 16)
    {
        ::memcpy(_data_buf + _data_buf_size, data, 16 - _data_buf_size);
        _aes.decrypt(_data_buf, buf);
        xor_buf(buf, _iv, 16);
        _result.append((const uint8_t*) buf, (const uint8_t*) buf + 16);
        ::memcpy(_iv, _data_buf, 16);

        ((const uint8_t*&) data) += 16 - _data_buf_size;
        data_len -= 16 - _data_buf_size;
        _data_buf_size = 0;
    }

    size_t i = 0;
    for (; i < data_len / 16; ++i)
    {
        _aes.decrypt(data, buf);
        xor_buf(buf, _iv, 16);
        _result.append((const uint8_t*) buf, (const uint8_t*) buf + 16);
        ::memcpy(_iv, data, 16);

        ((const uint8_t*&) data) += 16;
    }

    ::memcpy(_data_buf + _data_buf_size, data, data_len & 0x0f);
    _data_buf_size += data_len & 0x0f;
    assert(_data_buf_size < 16);
}

bool AES_CBC_PKCS5::finish_decrypt()
{
    assert(IN_DECRYPT == _state);
    if (0 != _data_buf_size || _result.size() <= 0)
        return false;

    const uint8_t last_byte = _result[_result.size() - 1];
    if (_result.size() < last_byte)
        return false;
    for (size_t i = 0; i < last_byte; ++i)
    {
        if (_result[_result.size() - i - 1] != last_byte)
            return false;
    }
    _result.resize(_result.size() - last_byte);
    _state = READY;
    _data_buf_size = 0;
    return true;
}

COWArray<uint8_t> AES_CBC_PKCS5::get_result() const
{
    return _result;
}

}
