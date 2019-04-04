
#include "word_array_integer.h"


namespace nut
{

NUT_API unsigned bit1_count(uint8_t a)
{
    a -= (a >> 1) & 0x55;
    a = (a & 0x33) + ((a >> 2) & 0x33);
    a = (a + (a >> 4)) & 0x0f;
    return a & 0x0f;
}

NUT_API unsigned bit1_count(uint16_t a)
{
    a -= (a >> 1) & 0x5555;
    a = (a & 0x3333) + ((a >> 2) & 0x3333);
    a = (a + (a >> 4)) & 0x0f0f;
    a += (a >> 8);
    return a & 0x1f;
}

NUT_API unsigned bit1_count(uint32_t a)
{
    a -= (a >> 1) & 0x55555555;
    a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
    a = (a + (a >> 4)) & 0x0f0f0f0f;
    a += a >> 8;
    a += a >> 16;
    return a & 0x3f;
}

NUT_API unsigned bit1_count(uint64_t a)
{
    a -= (a >> 1) & 0x5555555555555555LL;
    a = (a & 0x3333333333333333LL) + ((a >> 2) & 0x3333333333333333LL);
    a = (a + (a >> 4)) & 0x0f0f0f0f0f0f0f0fLL;
    a += a >> 8;
    a += a >> 16;
    a += a >> 32;
    return a & 0x7f;
}

/**
 * 统计 bit 1 数目
 */
NUT_API size_t bit1_count(const uint8_t *a, size_t N)
{
    assert(nullptr != a && N > 0);

    const size_t word_count = N / sizeof(unsigned);
    size_t ret = 0;
    for (size_t i = 0; i < word_count; ++i)
        ret += bit1_count(reinterpret_cast<const unsigned*>(a)[i]);
    for (size_t i = word_count * sizeof(unsigned); i < N; ++i)
        ret += bit1_count(a[i]);
    return ret;
}

/**
 * 统计 bit 0 数目
 */
NUT_API size_t bit0_count(const uint8_t *a, size_t N)
{
    assert(nullptr != a && N > 0);
    return N * 8 - bit1_count(a, N);
}

NUT_API uint8_t reverse_bits(uint8_t v)
{
    v = (((v & 0xaa) >> 1) | ((v & 0x55) << 1));
    v = (((v & 0xcc) >> 2) | ((v & 0x33) << 2));
    return (v >> 4) | (v << 4);
}

NUT_API uint16_t reverse_bits(uint16_t v)
{
    v = (((v & 0xaaaa) >> 1) | ((v & 0x5555) << 1));
    v = (((v & 0xcccc) >> 2) | ((v & 0x3333) << 2));
    v = (((v & 0xf0f0) >> 4) | ((v & 0x0f0f) << 4));
    return (v >> 8) | (v << 8);
}

NUT_API uint32_t reverse_bits(uint32_t v)
{
    v = (((v & 0xaaaaaaaa) >> 1) | ((v & 0x55555555) << 1));
    v = (((v & 0xcccccccc) >> 2) | ((v & 0x33333333) << 2));
    v = (((v & 0xf0f0f0f0) >> 4) | ((v & 0x0f0f0f0f) << 4));
    v = (((v & 0xff00ff00) >> 8) | ((v & 0x00ff00ff) << 8));
    return (v >> 16) | (v << 16);
}

NUT_API uint64_t reverse_bits(uint64_t v)
{
    v = (((v & 0xaaaaaaaaaaaaaaaaLL) >> 1) | ((v & 0x5555555555555555LL) << 1));
    v = (((v & 0xccccccccccccccccLL) >> 2) | ((v & 0x3333333333333333LL) << 2));
    v = (((v & 0xf0f0f0f0f0f0f0f0LL) >> 4) | ((v & 0x0f0f0f0f0f0f0f0fLL) << 4));
    v = (((v & 0xff00ff00ff00ff00LL) >> 8) | ((v & 0x00ff00ff00ff00ffLL) << 8));
    v = (((v & 0xffff0000ffff0000LL) >> 16) | ((v & 0x0000ffff0000ffffLL) << 16));
    return (v >> 32) | (v << 32);
}

NUT_API int lowest_bit1(uint8_t a)
{
    if (0 == a)
        return -1;

    int ret = 1;
    if (((uint8_t)(a << 4)) == 0) // NOTE 左移操作会先自动转换为 int 类型，故结果需要截断
    {
        ret += 4;
        a >>= 4;
    }
    if (((uint8_t)(a << 6)) == 0)
    {
        ret += 2;
        a >>= 2;
    }
    ret -= a & 1;
    return ret;
}

NUT_API int lowest_bit1(uint16_t a)
{
    if (0 == a)
        return -1;

    int ret = 1;
    if (((uint16_t)(a << 8)) == 0)
    {
        ret += 8;
        a >>= 8;
    }
    if (((uint16_t)(a << 12)) == 0)
    {
        ret += 4;
        a >>= 4;
    }
    if (((uint16_t)(a << 14)) == 0)
    {
        ret += 2;
        a >>= 2;
    }
    ret -= a & 1;
    return ret;
}

NUT_API int lowest_bit1(uint32_t a)
{
    if (0 == a)
        return -1;

    int ret = 1;
    if (((uint32_t)(a << 16)) == 0)
    {
        ret += 16;
        a >>= 16;
    }
    if (((uint32_t)(a << 24)) == 0)
    {
        ret += 8;
        a >>= 8;
    }
    if (((uint32_t)(a << 28)) == 0)
    {
        ret += 4;
        a >>= 4;
    }
    if (((uint32_t)(a << 30)) == 0)
    {
        ret += 2;
        a >>= 2;
    }
    ret -= a & 1;
    return ret;
}

NUT_API int lowest_bit1(uint64_t a)
{
    if (0 == a)
        return -1;

    int ret = 1;
    if (((uint64_t)(a << 32)) == 0)
    {
        ret += 32;
        a >>= 32;
    }
    if (((uint64_t)(a << 48)) == 0)
    {
        ret += 16;
        a >>= 16;
    }
    if (((uint64_t)(a << 56)) == 0)
    {
        ret += 8;
        a >>= 8;
    }
    if (((uint64_t)(a << 60)) == 0)
    {
        ret += 4;
        a >>= 4;
    }
    if (((uint64_t)(a << 62)) == 0)
    {
        ret += 2;
        a >>= 2;
    }
    ret -= a & 1;
    return ret;
}

NUT_API int highest_bit1(uint8_t a)
{
    if (0 == a)
        return -1;

    int ret = 6;
    if (a >> 4 == 0)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 6 == 0)
    {
        ret -= 2;
        a <<= 2;
    }
    ret += a >> 7;
    return ret;
}

NUT_API int highest_bit1(uint16_t a)
{
    if (0 == a)
        return -1;

    int ret = 14;
    if (a >> 8 == 0)
    {
        ret -= 8;
        a <<= 8;
    }
    if (a >> 12 == 0)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 14 == 0)
    {
        ret -= 2;
        a <<= 2;
    }
    ret += a >> 15;
    return ret;
}

NUT_API int highest_bit1(uint32_t a)
{
    if (0 == a)
        return -1;

    int ret = 30;
    if (a >> 16 == 0)
    {
        ret -= 16;
        a <<= 16;
    }
    if (a >> 24 == 0)
    {
        ret -= 8;
        a <<= 8;
    }
    if (a >> 28 == 0)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 30 == 0)
    {
        ret -= 2;
        a <<= 2;
    }
    ret += a >> 31;
    return ret;
}

NUT_API int highest_bit1(uint64_t a)
{
    if (0 == a)
        return -1;

    int ret = 62;
    if (a >> 32 == 0)
    {
        ret -= 32;
        a <<= 32;
    }
    if (a >> 48 == 0)
    {
        ret -= 16;
        a <<= 16;
    }
    if (a >> 56 == 0)
    {
        ret -= 8;
        a <<= 8;
    }
    if (a >> 60 == 0)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 62 == 0)
    {
        ret -= 2;
        a <<= 2;
    }
    ret += a >> 63;
    return ret;
}

}
