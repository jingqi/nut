
#include <stdio.h>


#include <nut/unittest/unittest.h>

#include <nut/security/encrypt/aes.h>

using namespace nut;

NUT_FIXTURE(TestAES)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void test_smoking()
    {
        const bool dbg_output = false;

        uint8_t AES_enc_test[3][16] =
        {
            { 0xA0, 0x43, 0x77, 0xAB, 0xE2, 0x59, 0xB0, 0xD0,
            0xB5, 0xBA, 0x2D, 0x40, 0xA5, 0x01, 0x97, 0x1B },
            { 0x4E, 0x46, 0xF8, 0xC5, 0x09, 0x2B, 0x29, 0xE2,
            0x9A, 0x97, 0x1A, 0x0C, 0xD1, 0xF6, 0x10, 0xFB },
            { 0x1F, 0x67, 0x63, 0xDF, 0x80, 0x7A, 0x7E, 0x70,
            0x96, 0x0D, 0x4C, 0xD3, 0x11, 0x8E, 0x60, 0x1A }
        };

        uint8_t AES_dec_test[3][16] =
        {
            { 0xF5, 0xBF, 0x8B, 0x37, 0x13, 0x6F, 0x2E, 0x1F,
            0x6B, 0xEC, 0x6F, 0x57, 0x20, 0x21, 0xE3, 0xBA },
            { 0xF1, 0xA8, 0x1B, 0x68, 0xF6, 0xE5, 0xA6, 0x27,
            0x1A, 0x8C, 0xB2, 0x4E, 0x7D, 0x94, 0x91, 0xEF },
            { 0x4D, 0xE0, 0xC6, 0xDF, 0x7C, 0xB1, 0x69, 0x72,
            0x84, 0x60, 0x4D, 0x60, 0x27, 0x1B, 0xC5, 0x9A }
        };
        int m, n, i, j;
        AES aes;
        unsigned char buf[16];
        unsigned char key[32];

        for( m = 0; m < 2; m++ )
        {
            if (dbg_output)
            {
                printf( "\n Rijndael Monte Carlo Test (ECB mode) - " );
                if( m == 0 )
                    printf( "encryption\n\n" );
                if( m == 1 )
                    printf( "decryption\n\n" );
            }

            for( n = 0; n < 3; n++ )
            {
                if (dbg_output)
                {
                    printf( " Test %d, key size = %3d bits: ", n + 1, 128 + n * 64 );
                    fflush( stdout );
                }

                memset( buf, 0, 16 );
                memset( key, 0, 16 + n * 8 );

                for( i = 0; i < 400; i++ )
                {
                    aes.set_key(key, 128 + n * 64 );

                    for( j = 0; j < 9999; j++ )
                    {
                        if( m == 0 ) aes.encrypt(buf, buf );
                        if( m == 1 ) aes.decrypt(buf, buf );
                    }

                    if( n > 0 )
                    {
                        for( j = 0; j < (n << 3); j++ )
                        {
                            key[j] ^= buf[j + 16 - (n << 3)];
                        }
                    }

                    if( m == 0 ) aes.encrypt(buf, buf );
                    if( m == 1 ) aes.decrypt(buf, buf );

                    for( j = 0; j < 16; j++ )
                    {
                        key[j + (n << 3)] ^= buf[j];
                    }
                }

                if( ( m == 0 && memcmp( buf, AES_enc_test[n], 16 ) != 0 ) ||
                    ( m == 1 && memcmp( buf, AES_dec_test[n], 16 ) != 0 ) )
                {
                    if (dbg_output)
                        printf( "failed!\n" );
                    NUT_TA(false);
                    return;
                }

                if (dbg_output)
                    printf( "passed.\n" );
                NUT_TA(true);
            }
        }

        if (dbg_output)
            printf( "\n" );

    }
};

NUT_REGISTER_FIXTURE(TestAES, "security, encrypt, quiet")
