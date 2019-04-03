
#include <stdlib.h> // for ::malloc() and ::free()

#include <nut/platform/endian.h>

#include "rsa_pkcs1.h"


// 加密块中非数据部分最小大小
#define MIN_NON_DATA_SIZE 11

namespace nut
{

RSA_PKCS1::~RSA_PKCS1()
{
    if (nullptr != _encryption_block)
        ::free(_encryption_block);
    _encryption_block = nullptr;
}

void RSA_PKCS1::set_callback(callback_type&& cb)
{
    _callback = std::forward<callback_type>(cb);
}

void RSA_PKCS1::set_callback(const callback_type& cb)
{
    _callback = cb;
}

void RSA_PKCS1::start_encrypt(const RSA::PublicKey& key)
{
    _public_key = key;
    _use_public_key = true;
    _encryption_block_size = (key.n.bit_length() + 7) / 8;
    assert(_encryption_block_size > MIN_NON_DATA_SIZE);
    _encryption_block = (uint8_t*) ::realloc(_encryption_block, _encryption_block_size);
    _input_size = 0;
    _has_error = false;
}

void RSA_PKCS1::start_encrypt(const RSA::PrivateKey& key)
{
    _private_key = key;
    _use_public_key = false;
    _encryption_block_size = (key.n.bit_length() + 7) / 8;
    assert(_encryption_block_size > MIN_NON_DATA_SIZE);
    _encryption_block = (uint8_t*) ::realloc(_encryption_block, _encryption_block_size);
    _input_size = 0;
    _has_error = false;
}

void RSA_PKCS1::pack_eb(size_t data_len)
{
    // Fill eb
    if (data_len < _encryption_block_size - MIN_NON_DATA_SIZE)
    {
        ::memmove(_encryption_block + _encryption_block_size - data_len,
                  _encryption_block + MIN_NON_DATA_SIZE, data_len);
    }

    _encryption_block[0] = 0;
    _encryption_block[_encryption_block_size - data_len - 1] = 0;

    if (_use_public_key)
    {
        _encryption_block[1] = 2;

        for (size_t i = _encryption_block_size - data_len - 2; i >= 2; --i)
        {
            const uint8_t v = (uint8_t) ::rand(); // 这里的随机数质量并不太重要
            _encryption_block[i] = (0 == v ? 1 : v); // 填充不能为 0
        }
    }
    else
    {
        _encryption_block[1] = 1;

        ::memset(_encryption_block + 2, 0xff, _encryption_block_size - data_len - 3);
    }

    // Swap big-endian to little-endian
    bswap(_encryption_block, _encryption_block_size);

    // Set to BigInteger
    _input_integer.set(_encryption_block, _encryption_block_size, false);
}

void RSA_PKCS1::update_encrypt(const void *data_, size_t data_len)
{
    assert(nullptr != data_ || 0 == data_len);
    assert(_input_size < _encryption_block_size - MIN_NON_DATA_SIZE);

    if (0 == data_len)
        return;

    const uint8_t *data = (const uint8_t*) data_;
    const size_t max_data_size = _encryption_block_size - MIN_NON_DATA_SIZE;
    const size_t output_word_size =
        (_encryption_block_size + sizeof(BigInteger::word_type) - 1) / sizeof(BigInteger::word_type);
    if (0 != _input_size && _input_size + data_len >= max_data_size)
    {
        // Pack
        const size_t new_input_size = max_data_size - _input_size;
        ::memcpy(_encryption_block + MIN_NON_DATA_SIZE + _input_size, data, new_input_size);
        pack_eb(max_data_size);

        // Encode
        BigInteger output;
        if (_use_public_key)
            output = RSA::transfer(_input_integer, _public_key);
        else
            output = RSA::transfer(_input_integer, _private_key);

        // Callback
        output.resize(output_word_size);
        _callback(output.data(), _encryption_block_size);

        // Calculate remaining size
        data += new_input_size;
        data_len -= new_input_size;
        _input_size = 0;
    }

    while (data_len >= max_data_size)
    {
        // Pack
        ::memcpy(_encryption_block + MIN_NON_DATA_SIZE, data, max_data_size);
        pack_eb(max_data_size);

        // Encode
        BigInteger output;
        if (_use_public_key)
            output = RSA::transfer(_input_integer, _public_key);
        else
            output = RSA::transfer(_input_integer, _private_key);

        // Callback
        output.resize(output_word_size);
        _callback(output.data(), _encryption_block_size);

        data += max_data_size;
        data_len -= max_data_size;
    }

    ::memcpy(_encryption_block + MIN_NON_DATA_SIZE + _input_size, data, data_len);
    _input_size += data_len;
    assert(_input_size < max_data_size);
}

void RSA_PKCS1::finish_encrypt()
{
    assert(_input_size < _encryption_block_size - MIN_NON_DATA_SIZE);

    if (0 == _input_size)
        return;

    // Pack
    pack_eb(_input_size);

    // Encode
    BigInteger output;
    if (_use_public_key)
        output = RSA::transfer(_input_integer, _public_key);
    else
        output = RSA::transfer(_input_integer, _private_key);

    // Callback
    const size_t output_word_size =
        (_encryption_block_size + sizeof(BigInteger::word_type) - 1) / sizeof(BigInteger::word_type);
    output.resize(output_word_size);
    _callback(output.data(), _encryption_block_size);
    _input_size = 0;
}

void RSA_PKCS1::start_decrypt(const RSA::PublicKey& key)
{
    _public_key = key;
    _use_public_key = true;
    _encryption_block_size = (key.n.bit_length() + 7) / 8;
    assert(_encryption_block_size > MIN_NON_DATA_SIZE);
    _encryption_block = (uint8_t*) ::realloc(_encryption_block, _encryption_block_size);
    _input_size = 0;
    _has_error = false;
}

void RSA_PKCS1::start_decrypt(const RSA::PrivateKey& key)
{
    _private_key = key;
    _use_public_key = false;
    _encryption_block_size = (key.n.bit_length() + 7) / 8;
    assert(_encryption_block_size > MIN_NON_DATA_SIZE);
    _encryption_block = (uint8_t*) ::realloc(_encryption_block, _encryption_block_size);
    _input_size = 0;
    _has_error = false;
}

bool RSA_PKCS1::unpack_eb(const BigInteger& output)
{
    // Load from BigInteger
    const size_t sig_bytes_len = std::min(
        _encryption_block_size, output.significant_words_length() * sizeof(BigInteger::word_type));
    ::memcpy(_encryption_block, output.data(), sig_bytes_len);
    if (sig_bytes_len < _encryption_block_size)
        ::memset(_encryption_block + sig_bytes_len, 0, _encryption_block_size - sig_bytes_len);

    // Swap little-endian to big-endian
    bswap(_encryption_block, _encryption_block_size);

    // Unpack
    if (0 != _encryption_block[0])
        return false;

    size_t data_index = 2;
    if (_use_public_key)
    {
        if (1 != _encryption_block[1])
            return false;
        while (0xff == _encryption_block[data_index] && data_index < _encryption_block_size)
            ++data_index;
        if (data_index < 10 || data_index > _encryption_block_size - 1 || 0 != _encryption_block[data_index])
            return false;
        ++data_index;
    }
    else
    {
        if (2 != _encryption_block[1])
            return false;
        while (0 != _encryption_block[data_index] && data_index < _encryption_block_size)
            ++data_index;
        if (data_index < 10 || data_index > _encryption_block_size - 1 || 0 != _encryption_block[data_index])
            return false;
        ++data_index;
    }
    if (data_index < _encryption_block_size)
        _callback(_encryption_block + data_index, _encryption_block_size - data_index);
    return true;
}

bool RSA_PKCS1::update_decrypt(const void *data_, size_t data_len)
{
    assert(nullptr != data_ || 0 == data_len);
    assert(_input_size < _encryption_block_size);

    if (0 == data_len || _has_error)
        return !_has_error;

    const uint8_t *data = (const uint8_t*) data_;
    if (0 != _input_size && _input_size + data_len >= _encryption_block_size)
    {
        // Decode
        ::memcpy(_encryption_block + _input_size, data, _encryption_block_size - _input_size);
        _input_integer.set(_encryption_block, _encryption_block_size, false);
        BigInteger output;
        if (_use_public_key)
            output = RSA::transfer(_input_integer, _public_key);
        else
            output = RSA::transfer(_input_integer, _private_key);

        // Unpack
        if (!unpack_eb(output))
        {
            _has_error = true;
            return false;
        }

        // Calculate remaining size
        data += _encryption_block_size - _input_size;
        data_len -= _encryption_block_size - _input_size;
        _input_size = 0;
    }

    while (data_len >= _encryption_block_size)
    {
        // Decode
        _input_integer.set(data, _encryption_block_size, false);
        BigInteger output;
        if (_use_public_key)
            output = RSA::transfer(_input_integer, _public_key);
        else
            output = RSA::transfer(_input_integer, _private_key);

        // Unpack
        if (!unpack_eb(output))
        {
            _has_error = true;
            return false;
        }

        // Calculate remaining size
        data += _encryption_block_size;
        data_len -= _encryption_block_size;
    }

    ::memcpy(_encryption_block + _input_size, data, data_len);
    _input_size += data_len;
    assert(_input_size < _encryption_block_size);
    return true;
}

bool RSA_PKCS1::finish_decrypt()
{
    if (0 != _input_size)
    {
        _has_error = true;
        return false;
    }
    return true;
}

}
