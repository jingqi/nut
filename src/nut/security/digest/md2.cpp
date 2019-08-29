
#include <assert.h>
#include <string.h> // for memcpy()

#include "../../util/string/string_utils.h"
#include "md2.h"


namespace nut
{

static const uint8_t MD2_S[256] = {
	41,  46,  67,  201, 162, 216, 124, 1,   61,  54,  84,  161, 236, 240, 6,   19,
    98,  167, 5,   243, 192, 199, 115, 140, 152, 147, 43,  217, 188, 76,  130, 202,
    30,  155, 87,  60,  253, 212, 224, 22,  103, 66,  111, 24,  138, 23,  229, 18,
    190, 78,  196, 214, 218, 158, 222, 73,  160, 251, 245, 142, 187, 47,  238, 122,
    169, 104, 121, 145, 21,  178, 7,   63,  148, 194, 16,  137, 11,  34,  95,  33,
    128, 127, 93,  154, 90,  144, 50,  39,  53,  62,  204, 231, 191, 247, 151, 3,
    255, 25,  48,  179, 72,  165, 181, 209, 215, 94,  146, 42,  172, 86,  170, 198,
    79,  184, 56,  210, 150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4,   241,
    69,  157, 112, 89,  100, 113, 135, 32,  134, 91,  207, 101, 230, 45,  168, 2,
    27,  96,  37,  173, 174, 176, 185, 246, 28,  70,  97,  105, 52,  64,  126, 15,
    85,  71,  163, 35,  221, 81,  175, 58,  195, 92,  249, 206, 186, 197, 234, 38,
    44,  83,  13,  110, 133, 40,  132, 9,   211, 223, 205, 244, 65,  129, 77,  82,
    106, 220, 55,  200, 108, 193, 171, 250, 36,  225, 123, 8,   12,  189, 177, 74,
    120, 136, 149, 139, 227, 99,  232, 109, 233, 203, 213, 254, 59,  0,   29,  57,
    242, 239, 183, 14,  102, 88,  208, 228, 166, 119, 114, 248, 235, 117, 75,  10,
    49,  68,  80,  180, 143, 237, 31,  26,  219, 153, 141, 51,  159, 17,  131, 20
};

MD2::MD2() noexcept
{
    reset();
}

void MD2::reset() noexcept
{
    _buffer_size = 0;
    ::memset(_state, 0, 48);
    ::memset(_checksum, 0, 16);
}

void MD2::update(uint8_t byte) noexcept
{
    update(&byte, 1);
}

void MD2::update(const void *data, size_t cb) noexcept
{
    assert(nullptr != data || 0 == cb);

    const size_t partlen = 16 - _buffer_size;
    size_t i = 0;
    if (cb >= partlen)
    {
        if (0 == _buffer_size)
        {
            transform128bits(data);
        }
        else
        {
            ::memcpy(_buffer + _buffer_size, data, partlen);
            transform128bits(_buffer);
            _buffer_size = 0;
        }

        for (i = partlen; i + 16 <= cb; i += 16)
            transform128bits(((const uint8_t*) data) + i);
    }

    ::memcpy(_buffer + _buffer_size, ((const uint8_t*) data) + i, cb - i);
    _buffer_size += cb - i;
}

void MD2::digest() noexcept
{
    /* Pad */
    const int pad = DIGEST_SIZE - _buffer_size;
    ::memset(_buffer + _buffer_size, pad, pad);

	transform128bits(_buffer);
	transform128bits(_checksum);
}

const uint8_t* MD2::get_result() const noexcept
{
    return _state;
}

std::string MD2::get_hex_result() const noexcept
{
    return hex_encode(_state, DIGEST_SIZE, false);
}

void MD2::transform128bits(const void *block)
{
    assert(nullptr != block);

	for (int i = 0; i < 16; ++i)
    {
		_state[i + 16] = ((const uint8_t*) block)[i];
		_state[i + 32] = _state[i + 16] ^ _state[i];
	}

	int t = 0;
	for (int i = 0; i < 18; ++i)
    {
		for (int j = 0; j < 48; ++j)
        {
			_state[j] ^= MD2_S[t];
			t = _state[j];
		}
		t = (t + i) & 0xff;
	}

	t = _checksum[15];
	for (int i = 0; i < 16; ++i)
    {
		_checksum[i] ^= MD2_S[((const uint8_t*) block)[i] ^ t];
		t = _checksum[i];
	}
}

}
