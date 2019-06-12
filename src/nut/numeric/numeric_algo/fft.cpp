
#include <math.h>

#include "fft.h"


namespace nut
{

static const double PI = ::acos(-1.0); // 定义Pi的值

static void bit_rev(fft_complex_type *a, unsigned loglen, size_t len) noexcept
{
    assert(nullptr != a);
    for (size_t i = 0; i < len; ++i)
    {
        size_t t = i, p = 0;
        for (unsigned j = 0; j < loglen; ++j)
        {
            p <<= 1;
            p |= (t & 1);
            t >>= 1;
        }
        if (p < i)
        {
            const fft_complex_type temp = a[p];
            a[p] = a[i];
            a[i] = temp;
        }
    }
}

NUT_API void FFT(fft_complex_type *a, unsigned loglen, size_t len, bool dft) noexcept
{
    bit_rev(a, loglen, len);

    size_t m = 2;
    for (unsigned s = 1; s <= loglen; ++s, m <<= 1)
    {
        const fft_complex_type wn(::cos(2 * PI * (dft ? 1 : -1) / m),
                                  ::sin(2 * PI * (dft ? 1 : -1) / m));
        for (size_t i = 0; i < len; i += m)
        {
            fft_complex_type w(1.0, 0);
            for (size_t j = 0; j < m / 2; ++j)
            {
                const fft_complex_type u = a[i + j];
                const fft_complex_type v = w * a[i + j + m / 2];
                a[i + j] = u + v;
                a[i + j + m / 2]= u - v;
                w = w * wn;
            }
        }
    }
    if (!dft)
    {
        for (size_t i = 0; i < len; ++i)
        {
            a[i].real(a[i].real() / len);
            a[i].imag(a[i].imag() / len);
        }
    }
}

}
