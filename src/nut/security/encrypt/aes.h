
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
public:
    AES() noexcept;

    /**
     * AES key scheduling routine
     *
     * @param nbits Bits of key, should be 128 / 192 / 256
     * @return True if success
     */
    bool set_key(const void *key, int nbits) noexcept;

    /**
     * AES 128-bit block encryption routine
     *
     * @param input 16 bytes input data
     * @param output 16 bytes output buffer
     */
    void encrypt(const void *input, void *output) const noexcept;

    /**
     * AES 128-bit block decryption routine
     *
     * @param input 16 bytes input data
     * @param output 16 bytes output buffer
     */
    void decrypt(const void *input, void *output) const noexcept;

private:
    /** Encryption round keys */
    uint32_t _erk[64];
    /** Decryption round keys */
    uint32_t _drk[64];
    /** Number of rounds */
    int _nr = 10;
};

}

#endif
