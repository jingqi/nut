/**
 * @file -
 * @author jingqi
 * @date 2011-12-17
 * @last-edit 2013-01-31 09:12:48 jingqi
 * @brief
 *
 * 用于用word_type数组表示的有符号大数
 */

#ifndef ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_
#define ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_

#include <assert.h>
#include <string.h> // for memset(), memcpy(), memmove()
#include <stdlib.h> // for malloc()

#include <nut/platform/platform.hpp>
#include <nut/platform/stdint.hpp>

// 优化程度，>= 0
#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 是否为0
 *
 * @return a<N> == 0
 */
template <typename T>
bool is_zero(const T *a, size_t N)
{
    assert(NULL != a && N > 0);

    for (register size_t i = 0; i < N; ++i)
        if (0 != a[i])
            return false;
    return true;
}

/**
 * 是否为正数或者0
 *
 * @return true, 参数 >= 0
 *      false, 参数 < 0
 */
template <typename T>
bool is_positive(const T *a, size_t N)
{
    assert(NULL != a && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    return 0 == (a[N - 1] & (((word_type) 1) << (8 * sizeof(word_type) - 1)));
}

/**
 * 有效字数
 *
 * @return 返回值>=1
 */
template <typename T>
size_t significant_size(const T *a, size_t N)
{
    assert(NULL != a && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const bool positive = is_positive(a, N);
    const word_type skip_value = (positive ? 0 : ~(word_type)0);
    register size_t ret = N;
    while (ret > 1 && reinterpret_cast<const word_type*>(a)[ret - 1] == skip_value && is_positive(a, ret - 1) == positive)
        --ret;
    return ret;
}

/**
 * 是否相等
 */
template <typename T>
bool equals(const T *a, size_t M, const T *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const bool positive1 = is_positive(a, M), positive2 = is_positive(b, N);
    if (positive1 != positive2)
        return false;

    const word_type fill = (positive1 ? 0 : ~(word_type)0);
    const word_type limit = (M > N ? M : N);
    for (register size_t i = 0; i < limit; ++i)
        if ((i < M ? reinterpret_cast<const word_type*>(a)[i] : fill) !=
            (i < N ? reinterpret_cast<const word_type*>(b)[i] : fill))
            return false;
    return true;
}

/**
 * 小于
 *
 * @return a<M> < b<N>
 */
template <typename T>
bool less_than(const T *a, size_t M, const T *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const bool positive1 = is_positive(a, M), positive2 = is_positive(b, N);
    if (positive1 != positive2)
        return positive2;

    // 同号比较
    const word_type fill = (positive1 ? 0 : ~(word_type)0);
    for (register int i = (M > N ? M : N) - 1; i >= 0; --i)
    {
        const word_type op1 = (i < (int)M ? reinterpret_cast<const word_type*>(a)[i] : fill);
        const word_type op2 = (i < (int)N ? reinterpret_cast<const word_type*>(b)[i] : fill);
        if (op1 != op2)
            return op1 < op2;
    }
    return false; // 相等
}

/**
 * 带符号扩展(或者截断)
 * a<M> -> x<N>
 */
template <typename T>
void expand(const T *a, size_t M, T *x, size_t N)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const uint8_t fill = (is_positive(a, M) ? 0 : 0xFF); /// 先把变量算出来，避免操作数被破坏
    if (x != a)
        ::memmove(x, a, (M < N ? M : N) * sizeof(word_type));
    if (M < N)
        ::memset(x + M, fill, (N - M) * sizeof(word_type));
}

/**
 * 相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t add(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (word_type*) ::malloc(sizeof(word_type) * P);

    register uint8_t carry = 0;
    const word_type filla = (is_positive(a, M) ? 0 : ~(word_type)0), fillb = (is_positive(b, N) ? 0 : ~(word_type)0);
    for (register size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? reinterpret_cast<const word_type*>(a)[i] : filla);
        dword_type pluser2 = (i < N ? reinterpret_cast<const word_type*>(b)[i] : fillb);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(word_type)));
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * P);
        ::free(retx);
    }
    return carry;
}

/**
 * 加1
 * x<N> += 1
 *
 * @return 进位
 */
template <typename T>
uint8_t increase(T *x, size_t N)
{
    assert(NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    register uint8_t carry = 1;
    for (register size_t i = 0; i < N && 0 != carry; ++i)
    {
        dword_type pluser = reinterpret_cast<const word_type*>(x)[i];
        pluser += carry;
        
        x[i] = static_cast<T>(pluser);
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(word_type)));
    }
    return carry;
}

/**
 * 相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t sub(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<word_type>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (word_type*) ::malloc(sizeof(word_type) * P);

    const word_type filla = (is_positive(a, M) ? 0 : ~(word_type)0), fillb = (is_positive(b, N) ? 0 : ~(word_type)0);
    register uint8_t carry = 1;
    for (register size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? reinterpret_cast<const word_type*>(a)[i] : filla);
        dword_type pluser2 = static_cast<word_type>(~(i < N ? reinterpret_cast<const word_type*>(b)[i] : fillb));
        pluser2 += pluser1 + carry;
        
        retx[i] = static_cast<word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(word_type)));
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * P);
        ::free(retx);
    }
    return carry;
}

/**
 * 减1
 * x<N> -= 1
 *
 * @return 进位
 */
template <typename T>
uint8_t decrease(T *x, size_t N)
{
    assert(NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    register uint8_t carry = 0;
    for (register size_t i = 0; i < N && 1 != carry; ++i)
    {
        dword_type pluser = reinterpret_cast<const word_type*>(x)[i];
        pluser += carry + (dword_type)(word_type)~(word_type)0;

        x[i] = static_cast<T>(pluser);
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(word_type)));
    }
    return carry;
}

/**
 * 取相反数
 * x<N> = -a<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t negate(const T *a, size_t M, T *x, size_t N)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if (a < x && x < a + M)
        retx = (word_type*) ::malloc(sizeof(word_type) * N);

    register uint8_t carry = 1;
    const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
    for (register size_t i = 0; i < N; ++i)
    {
        dword_type pluser = static_cast<word_type>(~(i < M ? reinterpret_cast<const word_type*>(a)[i] : fill));
        pluser += carry;

        retx[i] = static_cast<word_type>(pluser);
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(word_type)));
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
    return carry;
}

/**
 * 相乘
 * x<P> = a<M> * b<N>
 */
template <typename T>
void multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a - P < x && x < a + M) || (b - P < x && x < b + N))
        retx = (word_type*) ::malloc(sizeof(word_type) * P);

    // 乘法
    const word_type filla = (is_positive(a,M) ? 0 : ~(word_type)0), fillb = (is_positive(b,N) ? 0 : ~(word_type)0); /// 先把变量算出来，避免操作数被破坏
    ::memset(retx, 0, sizeof(word_type) * P);
    for (register size_t i = 0; i < P; ++i)
    {
        word_type carry = 0; // 这个进位包括乘法的，故此会大于1
        const dword_type mult1 = (i < M ? reinterpret_cast<const word_type*>(a)[i] : filla);
        if (i >= M && 0 == filla)
            break;
        if (mult1 == 0)
            continue;

        for (register size_t j = 0; i + j < P; ++j)
        {
            dword_type mult2 = (j < N ? reinterpret_cast<const word_type*>(b)[j] : fillb);
            mult2 = mult1 * mult2 + retx[i + j] + carry;

            retx[i + j] = static_cast<word_type>(mult2);
            carry = static_cast<word_type>(mult2 >> (8 * sizeof(word_type)));
        }
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * P);
        ::free(retx);
    }
}

/**
 * 相除
 * x<P> = a<M> / b<N>
 * y<Q> = a<M> % b<N>
 *
 * @param x
 *      商
 * @param y
 *      余数
 */
template <typename T>
void divide(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, T *y, size_t Q)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    assert((NULL != x && P > 0) || (NULL != y && Q > 0));
    assert(NULL == x || P <= 0 || NULL == y || Q <= 0 || y >= x + P || x >= y + Q); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    typedef typename StdInt<T>::unsigned_type word_type;

    // 常量
    const size_t dividend_len = significant_size(a, M);
    const size_t divisor_len = significant_size(b, N);
    const bool dividend_positive = is_positive(a, M); /// 先把变量算出来，避免操作数被破坏
    const bool divisor_positive = is_positive(b, N);
    const size_t quotient_len = (P < dividend_len ? P : dividend_len);

    // 避免数据在计算中途被破坏
    word_type *quotient = reinterpret_cast<word_type*>(x); // 商，可以为 NULL
    if ((a - P < x && x < a) || (b - P < x && x < b + N)) // 兼容 x==a 的情况
        quotient = (word_type*) ::malloc(sizeof(word_type) * quotient_len);
    word_type *remainder = reinterpret_cast<word_type*>(y); // 余数，不能为 NULL
    if (NULL == y || Q < divisor_len || (a - Q < y && y < a + M) || (b - Q < y && y < b + N))
        remainder = (word_type*) ::malloc(sizeof(word_type) * divisor_len);

    // 逐位试商
    ::memset(remainder, (dividend_positive ? 0 : 0xFF), sizeof(word_type) * divisor_len); // 初始化余数
    bool remainder_positive = dividend_positive; // 余数的符号
    bool quotient_positive = true; // 商的符号
    for (register size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_word_pos = dividend_len - i - 1;
        const word_type next_dividend_word = reinterpret_cast<const word_type*>(a)[dividend_word_pos]; // 余数左移时的低位补位部分
        if (NULL != quotient && dividend_word_pos < P)
            quotient[dividend_word_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (register size_t j = 0; j < 8 * sizeof(word_type); ++j)
        {
            // 余数左移1位
            shift_left(remainder, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_word >> (8 * sizeof(word_type) - 1 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive == divisor_positive)
                sub(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len);
            else
                add(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len);

            // 试商结果
            remainder_positive = is_positive(remainder, divisor_len);
            if (remainder_positive == divisor_positive)
            {
                if (NULL != quotient && dividend_word_pos < P)
                    quotient[dividend_word_pos] |= (1 << (8 * sizeof(word_type) - 1 - j));
                if (0 == i && 0 == j)
                    quotient_positive = false;
            }
        }
    }

    /**
        修正补数形式的商:
        如果除尽且除数为负数，则商加1
        如果未除尽且商为负数，则商加1
    */
    const bool remainder_is_zero = is_zero(remainder, divisor_len);
    if (NULL != x)
    {
        assert(NULL != quotient);
        if (remainder_is_zero)
        {
            if (!divisor_positive)
                increase(quotient, quotient_len);
        }
        else
        {
            if (!quotient_positive)
                increase(quotient, quotient_len);
        }
        expand(quotient, quotient_len, reinterpret_cast<word_type*>(x), P);
    }

    /**
        恢复余数:
        如果未除尽且余数符号与被除数不一致，余数需加修正
    */
    if (NULL != y)
    {
        if (!remainder_is_zero && remainder_positive != dividend_positive)
        {
            if (divisor_positive == dividend_positive)
                add(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len);
            else
                sub(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len);
        }
        expand(remainder, divisor_len, reinterpret_cast<word_type*>(y), Q);
    }

    // 释放空间
    if (quotient != reinterpret_cast<word_type*>(x))
        ::free(quotient);
    if (remainder != reinterpret_cast<word_type*>(y))
        ::free(remainder);
}

/**
 * 左移
 * x<N> = a<N> << count
 */
template <typename T>
void shift_left(const T *a, T *x, size_t N, size_t count)
{
    assert(NULL != a && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if (a - N < x && x < a - count / (8 * sizeof(word_type)))
        retx = (word_type*) ::malloc(sizeof(word_type) * N);

    const int words_off = count / (8 * sizeof(word_type)), bits_off = count % (8 * sizeof(word_type));
    for (register int i = N - 1; i >= 0; --i)
    {
        const word_type high = (i - words_off >= 0 ? reinterpret_cast<const word_type*>(a)[i - words_off] : 0)
            << bits_off;
        const word_type low = (i - words_off - 1 >= 0 ? reinterpret_cast<const word_type*>(a)[i - words_off - 1] : 0)
            >> (8 * sizeof(word_type) - bits_off);
        retx[i] = high | low;
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
}

/**
 * 右移
 * x<N> = a<N> >> count
 */
template <typename T>
void shift_right(const T *a, T *x, size_t N, size_t count)
{
    assert(NULL != a && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if (a + count / (8 * sizeof(word_type)) < x && x < a + N)
        retx = (word_type*) ::malloc(sizeof(word_type) * N);

    const size_t bytes_off = count / (8 * sizeof(word_type)), bits_off = count % (8 * sizeof(word_type));
    const word_type fill = (is_positive(a, N) ? 0 : ~(word_type)0);
    for (register size_t i = 0; i < N; ++i)
    {
        const word_type high = (i + bytes_off + 1 >= N ? fill : reinterpret_cast<const word_type*>(a)[i + bytes_off + 1])
            << (8 * sizeof(word_type) - bits_off);
        const word_type low = (i + bytes_off >= N ? fill : reinterpret_cast<const word_type*>(a)[i + bytes_off])
            >> bits_off;
        retx[i] = high | low;
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
}

/**
 * 按位与
 * x<N> = a<N> & b<N>
 */
template <typename T>
void bit_and(const T *a, const T *b, T *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (word_type*) ::malloc(sizeof(word_type) * N);

    for (register size_t i = 0; i < N; ++i)
        retx[i] = a[i] & b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
#else
    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
        for (register size_t i = 0; i < N; ++i)
            x[i] = a[i] & b[i];
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
        for (register size_t i = N - 1; i >= 0; --i)
            x[i] = a[i] & b[i];
    }
    else
    {
        // 避免区域交叉覆盖
        word_type *retx = (word_type*) ::malloc(sizeof(word_type) * N);

        for (register size_t i = 0; i < N; ++i)
            retx[i] = a[i] & b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
#endif
}

/**
 * 按位或
 * x<N> = a<N> | b<N>
 */
template <typename T>
void bit_or(const T *a, const T *b, T *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (word_type*) ::malloc(sizeof(word_type) * N);

    for (register size_t i = 0; i < N; ++i)
        retx[i] = a[i] | b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
#else
    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
        for (register size_t i = 0; i < N; ++i)
            x[i] = a[i] | b[i];
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
        for (register int i = N - 1; i >= 0; --i)
            x[i] = a[i] | b[i];
    }
    else
    {
        // 避免区域交叉覆盖
        word_type *retx = (word_type*) ::malloc(sizeof(word_type) * N);

        for (register size_t i = 0; i < N; ++i)
            retx[i] = a[i] | b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
#endif
}

/**
 * 按位异或
 * x<N> = a<N> ^ b<N>
 */
template <typename T>
void bit_xor(const T *a, const T *b, T *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (word_type*) ::malloc(sizeof(word_type) * N);

    for (register size_t i = 0; i < N; ++i)
        retx[i] = a[i] ^ b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
#else
    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
        for (register size_t i = 0; i < N; ++i)
            x[i] = a[i] ^ b[i];
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
        for (register int i = N - 1; i >= 0; --i)
            x[i] = a[i] ^ b[i];
    }
    else
    {
        // 避免区域交叉覆盖
        word_type *retx = (word_type*) ::malloc(sizeof(word_type) * N);

        for (register size_t i = 0; i < N; ++i)
            retx[i] = a[i] ^ b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
#endif
}

/**
 * 按位同或
 * x<N> = ~(a<N> ^ b<N>)
 */
template <typename T>
void bit_nxor(const T *a, const T *b, T *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (word_type*) ::malloc(sizeof(word_type) * N);

    for (register size_t i = 0; i < N; ++i)
        retx[i] = ~(a[i] ^ b[i]);

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
#else
    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
        for (register size_t i = 0; i < N; ++i)
            x[i] = ~(a[i] ^ b[i]);
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
        for (register int i = N - 1; i >= 0; --i)
            x[i] = ~(a[i] ^ b[i]);
    }
    else
    {
        // 避免区域交叉覆盖
        word_type *retx = (word_type*) ::malloc(sizeof(word_type) * N);

        for (register size_t i = 0; i < N; ++i)
            retx[i] = ~(a[i] ^ b[i]);

        // 回写数据
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
#endif
}

/**
 * 按位取反
 * x<N> = ~a<N>
 */
template <typename T>
void bit_not(const T *a, T *x, size_t N)
{
    assert(NULL != a && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if (a < x && x < a + N)
        retx = (word_type*) ::malloc(sizeof(word_type) * N);

    for(register size_t i = 0; i < N; ++i)
        retx[i] = ~a[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        ::free(retx);
    }
#else
    if (x < a)
    {
        for (register size_t i = 0; i < N; ++i)
            x[i] = ~a[i];
    }
    else
    {
        for (register int i = N - 1; i >= 0; --i)
            x[i] = ~a[i];
    }
#endif
}

inline size_t _bit_length(uint32_t a)
{
    if (a == 0)
        return 0;

    size_t ret = 31;
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

inline size_t _bit_length(uint8_t a)
{
    if (a == 0)
        return 0;

    size_t ret = 7;
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

/**
 * 正数 bit length
 */
inline size_t bit_length(const uint8_t *a,  size_t N)
{
    assert(NULL != a && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    for (register int i = N - 1; i >= 0; --i)
        if (0 != a[i])
            return i * 8 + _bit_length(a[i]);
    return 0;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (register int i = N - 1, limit = bits32_count * sizeof(uint32_t); i >= limit; --i)
        if (0 != a[i])
            return i * 8 + _bit_length(a[i]);
    for (register int i = bits32_count - 1; i >= 0; --i)
        if (0 != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _bit_length(reinterpret_cast<const uint32_t*>(a)[i]);
    return 0;
#endif
}

inline size_t _bit0_length(uint32_t a)
{
    if (a == 0xFFFFFFFF)
        return 0;

    size_t ret = 32;
    if (a >> 16 == 0xFFFF)
    {
        ret -= 16;
        a <<= 16;
    }
    if (a >> 24 == 0xFF)
    {
        ret -= 8;
        a <<= 8;
    }
    if (a >> 28 == 0x0F)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 30 == 0x03)
    {
        ret -= 2;
        a <<= 2;
    }
    ret -= a >> 31;
    return ret;
}

inline size_t _bit0_length(uint8_t a)
{
    if (a == 0xFF)
        return 0;

    size_t ret = 8;
    if (a >> 4 == 0x0F)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 6 == 0x03)
    {
        ret -= 2;
        a <<= 2;
    }
    ret -= a >> 7;
    return ret;
}

/**
 * 负数 bit length
 */
inline size_t bit0_length(const uint8_t *a,  size_t N)
{
    assert(NULL != a && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    for (register int i = N - 1; i >= 0; --i)
        if (0xFF != a[i])
            return i * 8 + _bit0_length(a[i]);
    return 0;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (register int i = N - 1, limit = bits32_count * sizeof(uint32_t); i >= limit; --i)
        if (0xFF != a[i])
            return i * 8 + _bit0_length(a[i]);
    for (register int i = bits32_count - 1; i >= 0; --i)
        if (0xFFFFFFFF != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _bit0_length(reinterpret_cast<const uint32_t*>(a)[i]);
    return 0;
#endif
}

inline size_t _bit_count(uint32_t a)
{
	a = a - ((a >> 1) & 0x55555555);
	a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
	a = (a + (a >> 4)) & 0x0f0f0f0f;
	a = a + (a >> 8);
	a = a + (a >> 16);
	return a & 0x3f;
}

inline size_t _bit_count(uint8_t a)
{
    a = a - ((a >> 1) & 0x55);
	a = (a & 0x33) + ((a >> 2) & 0x33);
	a = (a + (a >> 4)) & 0x0f;
    return a & 0x0f;
}

/**
 * 统计 bit 1 数目
 */
inline size_t bit_count(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    size_t ret = 0;
    for (register size_t i = 0; i < N; ++i)
        ret += _bit_count(a[i]);
    return ret;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    size_t ret = 0;
    for (register size_t i = 0; i < bits32_count; ++i)
        ret += _bit_count(reinterpret_cast<const uint32_t*>(a)[i]);
    for (register size_t i = bits32_count * sizeof(uint32_t); i < N; ++i)
        ret += _bit_count(a[i]);
    return ret;
#endif
}

inline int _lowest_bit(uint32_t a)
{
    if (a == 0)
        return -1;

    size_t ret = 1;
    if ((uint32_t)(a << 16) == 0)
    {
        ret += 16;
        a >>= 16;
    }
    if ((uint32_t)(a << 24) == 0)
    {
        ret += 8;
        a >>= 8;
    }
    if ((uint32_t)(a << 28) == 0)
    {
        ret += 4;
        a >>= 4;
    }
    if ((uint32_t)(a << 30) == 0)
    {
        ret += 2;
        a >>= 2;
    }
    ret -= a & 1;
    return ret;
}

inline int _lowest_bit(uint8_t a)
{
    if (a == 0)
        return -1;

    size_t ret = 1;
    if ((uint32_t)(a << 4) == 0)
    {
        ret += 4;
        a >>= 4;
    }
    if ((uint32_t)(a << 6) == 0)
    {
        ret += 2;
        a >>= 2;
    }
    ret -= a & 1;
    return ret;
}

/**
 * 返回从低位到高位第一个 bit 1 的位置
 *
 * @return -1 if not found
 *      >0 if found
 */
inline int lowest_bit(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    for (register size_t i = 0; i < N; ++i)
        if (0 != a[i])
            return i * 8 + _lowest_bit(a[i]);
    return -1;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (register size_t i = 0; i < bits32_count; ++i)
        if (0 != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _lowest_bit(reinterpret_cast<const uint32_t*>(a)[i]);
    for (register size_t i = bits32_count * sizeof(uint32_t); i < N; ++i)
        if (0 != a[i])
            return i * 8 + _lowest_bit(a[i]);
    return -1;
#endif
}

}

#undef OPTIMIZE_LEVEL

#endif /* head file guarder */

