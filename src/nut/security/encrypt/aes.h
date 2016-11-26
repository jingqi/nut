
#ifndef ___HEADFILE_A752E201_5C0D_4C54_A685_A9FF3296ED3C_
#define ___HEADFILE_A752E201_5C0D_4C54_A685_A9FF3296ED3C_

#include <stdint.h>

#include "../../nut_config.h"


namespace nut
{

/**
 * Advanced Encryption Standard
 */
class NUT_API AES
{
    /** Encryption round keys */
    uint32_t _erk[64];
    /** Decryption round keys */
    uint32_t _drk[64];
    /** Number of rounds */
    int _nr = 10;

public:
    AES();

    /**
     * AES key scheduling routine
     *
     * @param nbits Bits of key, should be 128 / 192 / 256
     * @return True if success
     */
    bool set_key(const uint8_t *key, int nbits);

    /**
     * AES 128-bit block encryption routine
     */
    void encrypt(const uint8_t input[16], uint8_t output[16]) const;

    /**
     * AES 128-bit block decryption routine
     */
    void decrypt(const uint8_t input[16], uint8_t output[16]) const;
};

}

#endif
