
#ifndef ___HEADFILE_A752E201_5C0D_4C54_A685_A9FF3296ED3C_
#define ___HEADFILE_A752E201_5C0D_4C54_A685_A9FF3296ED3C_

#include <stdint.h>

namespace nut
{

class AES
{
    /** encryption round keys */
    uint32_t _erk[64];
    /** decryption round keys */
    uint32_t _drk[64];
    /** number of rounds */
    int _nr;

public:
    AES();

    /**
     * AES key scheduling routine
     *
     * @return true, if success
     */
    bool set_key(const uint8_t *key, int nbits);

    /**
     * AES 128-bit block encryption routine
     */
    void encrypt(const uint8_t input[16], uint8_t output[16]);

    /**
     * AES 128-bit block decryption routine
     */
    void decrypt(const uint8_t input[16], uint8_t output[16]);
};

}

#endif
