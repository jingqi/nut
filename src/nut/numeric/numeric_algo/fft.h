
#ifndef ___HEADFILE_583C901E_7939_41C8_A24C_AF094D969CD9_
#define ___HEADFILE_583C901E_7939_41C8_A24C_AF094D969CD9_

#include <assert.h>
#include <stdlib.h> // for ::malloc()
#include <complex>

#include "../word_array_integer.h"


// - 大整数拆成多项式, = a0 + a1*base + a2*pow(base,2) + a3*pow(base, 3) + ...
// - NUT_FFT_BASE_BYTES 越大, 性能越快，但是误差会增大
#define NUT_FFT_BASE_BYTES 2

namespace nut
{

typedef uint64_t fft_word_type;
typedef std::complex<double> fft_complex_type;

static_assert(std::is_unsigned<fft_word_type>::value, "Unexpected integer type");
static_assert(sizeof(fft_word_type) > 2 * NUT_FFT_BASE_BYTES,
              "Unexpected integer type");

/**
 * 快速傅立叶变换
 *
 * @param dft DFT / IDFT
 */
NUT_API void FFT(fft_complex_type *a, unsigned loglen, size_t len, bool dft = true);

/**
 * 利用快速傅立叶变换求大数的成绩，时间复杂度为 O(nlogn)
 */
template <typename T>
void unsigned_fft_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);

    // 确定 FFT 规模
    size_t wc_len = (sizeof(T) * (M + N) + NUT_FFT_BASE_BYTES - 1) / NUT_FFT_BASE_BYTES;
    unsigned loglen = highest_bit1((uint64_t) wc_len);
    if ((1ULL << loglen) < wc_len)
        ++loglen;
    assert((1ULL << loglen) >= wc_len);
    wc_len = (1ULL << loglen);

    // 准备复数向量
    fft_complex_type *aa = (fft_complex_type*) ::malloc(sizeof(fft_complex_type) * wc_len * 2);
    fft_complex_type *bb = aa + wc_len;
    for (size_t i = 0; i < wc_len; ++i)
    {
        fft_word_type va = 0, vb = 0;
        for (unsigned j = 0; j < NUT_FFT_BASE_BYTES; ++j)
        {
            const size_t in_byte_index = i * NUT_FFT_BASE_BYTES + j;
            if (in_byte_index < sizeof(T) * M)
                set_byte_le(&va, j, get_byte_le(a, in_byte_index));
            if (in_byte_index < sizeof(T) * N)
                set_byte_le(&vb, j, get_byte_le(b, in_byte_index));
        }
        new (aa + i) fft_complex_type(va, 0);
        new (bb + i) fft_complex_type(vb, 0);
    }

    // FFT 变换、逆变换
    FFT(aa, loglen, wc_len, true);
    FFT(bb, loglen, wc_len, true);
    for (size_t i = 0; i < wc_len; ++i)
        aa[i] = aa[i] * bb[i];
    FFT(aa, loglen, wc_len, false);

    // 将结果转换为整数，并计算进位
    const size_t ans_len = std::min(wc_len, (sizeof(T) * P + NUT_FFT_BASE_BYTES - 1) / NUT_FFT_BASE_BYTES);
    fft_word_type *ans = (fft_word_type*) ::malloc(sizeof(fft_word_type) * ans_len);
    for (size_t i = 0; i < ans_len; ++i)
        ans[i] = aa[i].real() + 0.5;
    ::free(aa);
    for (size_t i = 0; i < ans_len - 1; ++i)
    {
        ans[i + 1] += ans[i] >> (8 * NUT_FFT_BASE_BYTES);
        ans[i] &= ~((~(fft_word_type)0) << (8 * NUT_FFT_BASE_BYTES));
    }

    // 写入返回值
    for (size_t i = 0; i < ans_len; ++i)
    {
        for (unsigned j = 0; j < NUT_FFT_BASE_BYTES; ++j)
        {
            const size_t out_byte_index = i * NUT_FFT_BASE_BYTES + j;
            if (out_byte_index >= sizeof(T) * P)
                break;
            set_byte_le(x, out_byte_index, get_byte_le(ans + i, j));
        }
    }
    for (size_t i = ans_len * NUT_FFT_BASE_BYTES; i < sizeof(T) * P; ++i)
        set_byte_le(x, i, 0);
    ::free(ans);
}

template <typename T>
void signed_fft_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);

    // FFT 算法不能处理负数的补数形式
    T *aa = nullptr, *bb = nullptr;
    size_t MM = M, NN = N;
    bool neg = false;
    if (!is_positive(a, M))
    {
        ++MM;
        aa = (T*) ::malloc(sizeof(T) * MM);
        signed_negate(a, M, aa, MM);
        neg = !neg;
    }
    if (!is_positive(b, N))
    {
        ++NN;
        bb = (T*) ::malloc(sizeof(T) * NN);
        signed_negate(b, N, bb, NN);
        neg = !neg;
    }

    // 调用 FFT 算法
    unsigned_fft_multiply((nullptr != aa ? aa : a), MM,
                          (nullptr != bb ? bb : b), NN, x, P);
    if (nullptr != aa)
        ::free(aa);
    if (nullptr != bb)
        ::free(bb);

    // 还原符号
    if (neg)
        signed_negate(x, P, x, P);
}

}

#endif
