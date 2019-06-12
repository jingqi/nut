
#ifndef ___HEADFILE_E3329C69_6A3B_4872_AA20_CB9005EC3A1C_
#define ___HEADFILE_E3329C69_6A3B_4872_AA20_CB9005EC3A1C_

#include <assert.h>
#include <string.h> // for ::memset(), ::memcpy(), ::memmove()
#include <algorithm>
#include <type_traits>

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <malloc.h> // for ::alloca()
#else
#   include <alloca.h>
#endif


#include "../../platform/int_type.h"
#include "../../platform/endian.h"
#include "shift_op.h"
#include "word_array_integer.h"


namespace nut
{

/**
 * (无符号数/正数)平方优化
 *
 *               a  b  c  d  e
 *          *    a  b  c  d  e
 *          -------------------
 *              ae be ce de ee
 *           ad bd cd dd de
 *        ac bc cc cd ce
 *     ab bb bc bd be
 *  aa ab ac ad ae
 *
 * 可拆成三部分，上下两部分是重复的：
 *
 *              ae be ce de
 *           ad bd cd
 *        ac bc             ee
 *     ab             dd
 *              cc
 *        bb             de
 *  aa             cd ce
 *           bc bd be
 *     ab ac ad ae
 */
template <typename T>
void _unsigned_square(const T *a, size_t M, T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);
    assert(is_positive(a, M));
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if (a - N < x && x < a + M)
        retx = (T*) ::alloca(sizeof(T) * N);

    // 先计算一半
    ::memset(retx, 0, sizeof(T) * N);
    for (size_t i = 0; i < M - 1; ++i)
    {
        if (i * 2 + 1 >= N)
            break;

        const dword_type op1 = a[i];
        if (0 == op1)
            continue;

        T carry = 0;
        for (size_t j = i + 1; j < M && i + j < N; ++j)
        {
            dword_type op2 = a[j];
            op2 = op1 * op2 + retx[i + j] + carry;

            retx[i + j] = static_cast<T>(op2);
#if NUT_ENDIAN_LITTLE_BYTE
            carry = reinterpret_cast<const T*>(&op2)[1];
#elif NUT_ENDIAN_BIG_BYTE
            carry = reinterpret_cast<const T*>(&op2)[0];
#else
            carry = static_cast<T>(op2 >> (8 * sizeof(T)));
#endif
        }
        if (i + M < N)
            retx[i + M] = carry;
    }

    // 再加上另一半
    const size_t limit = std::min(N, M * 2);
    unsigned_shift_left(retx, limit, retx, limit, 1);

    // 加上中间对称线
    T carry = 0;
    for (size_t i = 0; i < M; ++i)
    {
        if (i * 2 >= N)
            break;

        dword_type op = a[i];
        op = op * op + retx[i * 2] + carry;

        retx[i * 2] = static_cast<T>(op);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = reinterpret_cast<const T*>(&op)[1];
#elif NUT_ENDIAN_BIG_BYTE
        carry = reinterpret_cast<const T*>(&op)[0];
#else
        carry = static_cast<T>(op >> (8 * sizeof(T)));
#endif

        if (0 != carry && i * 2 + 1 < N)
        {
            op = retx[i * 2 + 1];
            op += carry;

            retx[i * 2 + 1] = static_cast<T>(op);
#if NUT_ENDIAN_LITTLE_BYTE
            carry = reinterpret_cast<const T*>(&op)[1];
#elif NUT_ENDIAN_BIG_BYTE
            carry = reinterpret_cast<const T*>(&op)[0];
#else
            carry = static_cast<T>(op >> (8 * sizeof(T)));
#endif
        }
    }

    // 回写结果
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * N);
}

/**
 * (有符号数)相乘
 * x<P> = a<M> * b<N>
 */
template <typename T>
void signed_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    if (a == b && M == N && is_positive(a, M))
    {
        _unsigned_square(a, M, x, P);
        return;
    }

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a - P < x && x < a + M) || (b - P < x && x < b + N))
        retx = (T*) ::alloca(sizeof(T) * P);

    // 乘法
    const T filla = (is_positive(a,M) ? 0 : ~(T)0),
        fillb = (is_positive(b,N) ? 0 : ~(T)0); // NOTE 先把变量算出来，避免操作数被破坏
    ::memset(retx, 0, sizeof(T) * P);
    for (size_t i = 0; i < P; ++i)
    {
        if (i >= M && 0 == filla)
            break;

        const dword_type mult1 = (i < M ? a[i] : filla);
        if (0 == mult1)
            continue;

        T carry = 0; // 这个进位包括乘法的，故此会大于1
        for (size_t j = 0; i + j < P; ++j)
        {
            if (j >= N && 0 == fillb && 0 == carry)
                break;

            dword_type mult2 = (j < N ? b[j] : fillb);
            mult2 = mult1 * mult2 + retx[i + j] + carry;

            retx[i + j] = static_cast<T>(mult2);
#if NUT_ENDIAN_LITTLE_BYTE
            carry = reinterpret_cast<const T*>(&mult2)[1];
#elif NUT_ENDIAN_BIG_BYTE
            carry = reinterpret_cast<const T*>(&mult2)[0];
#else
            carry = static_cast<T>(mult2 >> (8 * sizeof(T)));
#endif
        }
    }

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * P);
}

/**
 * (无符号数)相乘
 * x<P> = a<M> * b<N>
 */
template <typename T>
void unsigned_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a - P < x && x < a + M) || (b - P < x && x < b + N))
        retx = (T*) ::alloca(sizeof(T) * P);

    // 乘法
    ::memset(retx, 0, sizeof(T) * P);
    for (size_t i = 0; i < P; ++i)
    {
        if (i >= M)
            break;

        const dword_type mult1 = a[i];
        if (0 == mult1)
            continue;

        T carry = 0; // 这个进位包括乘法的，故此会大于1
        for (size_t j = 0; i + j < P; ++j)
        {
            if (j >= N && 0 == carry)
                break;

            dword_type mult2 = (j < N ? b[j] : 0);
            mult2 = mult1 * mult2 + retx[i + j] + carry;

            retx[i + j] = static_cast<T>(mult2);
#if NUT_ENDIAN_LITTLE_BYTE
            carry = reinterpret_cast<const T*>(&mult2)[1];
#elif NUT_ENDIAN_BIG_BYTE
            carry = reinterpret_cast<const T*>(&mult2)[0];
#else
            carry = static_cast<T>(mult2 >> (8 * sizeof(T)));
#endif
        }
    }

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * P);
}

}

#endif
