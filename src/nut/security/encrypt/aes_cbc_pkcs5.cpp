
#include <assert.h>
#include <memory.h> // for ::memset()

#include "../../platform/platform.h"
#include "aes_cbc_pkcs5.h"


namespace nut
{

static void xor_buf(void *dst, const void *src, size_t len = 16) noexcept
{
    assert(nullptr != dst && nullptr != src);
    for (size_t i = 0; i < len; ++i)
        ((uint8_t*) dst)[i] ^= ((const uint8_t*) src)[i];
}

void AES_CBC_PKCS5::set_callback(callback_type&& cb) noexcept
{
    _callback = std::forward<callback_type>(cb);
}

void AES_CBC_PKCS5::set_callback(const callback_type& cb) noexcept
{
    _callback = cb;
}

void AES_CBC_PKCS5::start_encrypt(const void* key, int key_bits, const void *iv) noexcept
{
    assert(nullptr != key && nullptr != iv);

    _state = State::InEncrypt;
    ::memcpy(_iv, iv, 16);
    _input_buffer_size = 0;
    bool rs = _aes.set_key(key, key_bits);
    assert(rs); UNUSED(rs);
}

void AES_CBC_PKCS5::update_encrypt(const void *data_, size_t data_len) noexcept
{
    assert(nullptr != data_ || 0 == data_len);
    assert(State::InEncrypt == _state && _input_buffer_size < 16);

    if (0 == data_len)
        return;

    const uint8_t *data = (const uint8_t*) data_;
    if (0 != _input_buffer_size && _input_buffer_size + data_len >= 16)
    {
        ::memcpy(_input_buffer + _input_buffer_size, data, 16 - _input_buffer_size);
        xor_buf(_iv, _input_buffer, 16);
        _aes.encrypt(_iv, _iv);
        _callback(_iv, 16);

        data += 16 - _input_buffer_size;
        data_len -= 16 - _input_buffer_size;
        _input_buffer_size = 0;
    }

    while (data_len >= 16)
    {
        xor_buf(_iv, data, 16);
        _aes.encrypt(_iv, _iv);
        _callback(_iv, 16);

        data += 16;
        data_len -= 16;
    }

    ::memcpy(_input_buffer + _input_buffer_size, data, data_len);
    _input_buffer_size += data_len;
    assert(_input_buffer_size < 16);
}

void AES_CBC_PKCS5::finish_encrypt() noexcept
{
    assert(State::InEncrypt == _state && _input_buffer_size < 16);

    /* PKCS5 填充 */
    ::memset(_input_buffer + _input_buffer_size, 16 - _input_buffer_size, 16 - _input_buffer_size);
    xor_buf(_iv, _input_buffer, 16);
    _aes.encrypt(_iv, _iv);

    _callback(_iv, 16);
    _input_buffer_size = 0;
}

void AES_CBC_PKCS5::start_decrypt(const void *key, int key_bits, const void *iv) noexcept
{
    assert(nullptr != key && nullptr != iv);

    _state = State::InDecrypt;
    ::memcpy(_iv, iv, 16);
    _input_buffer_size = 0;
    _decrypt_buffer_has_data = false;
    bool rs = _aes.set_key(key, key_bits);
    assert(rs); UNUSED(rs);
}

bool AES_CBC_PKCS5::update_decrypt(const void *data_, size_t data_len) noexcept
{
    assert(nullptr != data_ || 0 == data_len);
    assert(State::InDecrypt == _state && _input_buffer_size < 16);

    if (0 == data_len || State::Error == _state)
        return State::Error != _state;

    const uint8_t *data = (const uint8_t*) data_;
    if (0 != _input_buffer_size && _input_buffer_size + data_len >= 16)
    {
        ::memcpy(_input_buffer + _input_buffer_size, data, 16 - _input_buffer_size);
        if (_decrypt_buffer_has_data)
            _callback(_decrypt_buffer, 16);
        _aes.decrypt(_input_buffer, _decrypt_buffer);
        xor_buf(_decrypt_buffer, _iv, 16);
        _decrypt_buffer_has_data = true;
        ::memcpy(_iv, _input_buffer, 16);

        data += 16 - _input_buffer_size;
        data_len -= 16 - _input_buffer_size;
        _input_buffer_size = 0;
    }

    while (data_len >= 16)
    {
        if (_decrypt_buffer_has_data)
            _callback(_decrypt_buffer, 16);
        _aes.decrypt(data, _decrypt_buffer);
        xor_buf(_decrypt_buffer, _iv, 16);
        _decrypt_buffer_has_data = true;
        ::memcpy(_iv, data, 16);

        data += 16;
        data_len -= 16;
    }

    ::memcpy(_input_buffer + _input_buffer_size, data, data_len);
    _input_buffer_size += data_len;
    assert(_input_buffer_size < 16);
    return true;
}

bool AES_CBC_PKCS5::finish_decrypt() noexcept
{
    assert(State::InDecrypt == _state);

    if (State::Error == _state)
        return false;

    if (0 != _input_buffer_size || !_decrypt_buffer_has_data)
    {
        _state = State::Error;
        return false;
    }

    const uint8_t last_byte = _decrypt_buffer[15];
    if (last_byte < 1 || last_byte > 16)
    {
        _state = State::Error;
        return false;
    }
    for (size_t i = 16 - last_byte; i < 16; ++i)
    {
        if (_decrypt_buffer[i] != last_byte)
        {
            _state = State::Error;
            return false;
        }
    }
    if (last_byte < 16)
        _callback(_decrypt_buffer, 16 - last_byte);
    _decrypt_buffer_has_data = false;
    return true;
}

}
