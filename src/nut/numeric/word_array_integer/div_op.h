
#ifndef ___HEADFILE_1D880997_973C_431B_AC94_E18F107D26F1_
#define ___HEADFILE_1D880997_973C_431B_AC94_E18F107D26F1_

#include <assert.h>
#include <string.h> // for memset(), memcpy(), memmove()
#include <algorithm>
#include <type_traits>

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <malloc.h> // for ::alloca()
#else
#   include <alloca.h>
#endif

#include "shift_op.h"
#include "word_array_integer.h"


namespace nut
{

/**
 * (有符号数)相除
 * x<P> = a<M> / b<N>
 * y<Q> = a<M> % b<N>
 *
 * @param x 商
 * @param y 余数
 */
template <typename T>
void signed_divide(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, T *y, size_t Q) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0);
    assert((nullptr != x && P > 0) || (nullptr != y && Q > 0));
    assert(nullptr == x || P == 0 || nullptr == y || Q == 0 || y >= x + P || x >= y + Q); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    // 常量
    const size_t dividend_len = signed_significant_size(a, M);
    const size_t divisor_len = signed_significant_size(b, N);
    const bool dividend_positive = is_positive(a, M); /// 先把变量算出来，避免操作数被破坏
    const bool divisor_positive = is_positive(b, N);
    const size_t quotient_len = std::min(P, dividend_len);

    // 避免输入数据在计算中途被破坏
    const bool alloc_quotient = (nullptr != x &&
                                 ((a - P < x && x < a) ||
                                  (b - P < x && x < b + N))); // 兼容 x==a 的情况
    const bool alloc_remainder = (nullptr == y || Q < divisor_len ||
                                  (a - Q < y && y < a + M) ||
                                  (b - Q < y && y < b + N));
    T *quotient = x; // 商, 可以为 nullptr
    T *remainder = y; // 余数, 不能为 nullptr
    if (alloc_quotient && alloc_remainder)
    {
        quotient = (T*) ::alloca(sizeof(T) * (quotient_len + divisor_len));
        remainder = quotient + quotient_len;
    }
    else if (alloc_quotient)
    {
        quotient = (T*) ::alloca(sizeof(T) * (quotient_len));
    }
    else if (alloc_remainder)
    {
        remainder = (T*) ::alloca(sizeof(T) * divisor_len);
    }

    // 逐位试商
    ::memset(remainder, (dividend_positive ? 0 : 0xFF), sizeof(T) * divisor_len); // 初始化余数
    bool remainder_positive = dividend_positive; // 余数的符号
    bool quotient_positive = true; // 商的符号
    for (size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_word_pos = dividend_len - i - 1;
        const T next_dividend_word = a[dividend_word_pos]; // 余数左移时的低位补位部分
        if (nullptr != quotient && dividend_word_pos < P)
            quotient[dividend_word_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (size_t j = 0; j < 8 * sizeof(T); ++j)
        {
            // 余数左移1位
            signed_shift_left(remainder, divisor_len, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_word >> (8 * sizeof(T) - 1 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive == divisor_positive)
                signed_sub(remainder, divisor_len, b, divisor_len, remainder, divisor_len);
            else
                signed_add(remainder, divisor_len, b, divisor_len, remainder, divisor_len);

            // 试商结果
            remainder_positive = is_positive(remainder, divisor_len);
            if (remainder_positive == divisor_positive)
            {
                if (nullptr != quotient && dividend_word_pos < P)
                    quotient[dividend_word_pos] |= (1 << (8 * sizeof(T) - 1 - j));
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
    if (nullptr != x)
    {
        assert(nullptr != quotient);
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
        signed_expand(quotient, quotient_len, x, P);
    }

    /**
        恢复余数:
        如果未除尽且余数符号与被除数不一致，余数需加修正
    */
    if (nullptr != y)
    {
        if (!remainder_is_zero && remainder_positive != dividend_positive)
        {
            if (divisor_positive == dividend_positive)
                signed_add(remainder, divisor_len, b, divisor_len, remainder, divisor_len);
            else
                signed_sub(remainder, divisor_len, b, divisor_len, remainder, divisor_len);
        }
        signed_expand(remainder, divisor_len, y, Q);
    }
}

/**
 * (无符号数)相除
 * x<P> = a<M> / b<N>
 * y<Q> = a<M> % b<N>
 *
 * @param x 商
 * @param y 余数
 */
template <typename T>
void unsigned_divide(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, T *y, size_t Q) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0);
    assert((nullptr != x && P > 0) || (nullptr != y && Q > 0));
    assert(nullptr == x || P == 0 || nullptr == y || Q == 0 || y >= x + P || x >= y + Q); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    // 常量
    const size_t dividend_len = unsigned_significant_size(a, M);
    const size_t divisor_len = unsigned_significant_size(b, N);
    const size_t quotient_len = std::min(P, dividend_len);

    // 避免数据在计算中途被破坏
    const bool alloc_quotient = (nullptr != x &&
                                 ((a - P < x && x < a) ||
                                  (b - P < x && x < b + N))); // 兼容 x==a 的情况
    const bool alloc_remainder = (nullptr == y || Q < divisor_len ||
                                  (a - Q < y && y < a + M) ||
                                  (b - Q < y && y < b + N));
    T *quotient = x; // 商，可以为 nullptr
    T *remainder = y; // 余数，不能为 nullptr
    if (alloc_quotient && alloc_remainder)
    {
        quotient = (T*) ::alloca(sizeof(T) * (quotient_len + divisor_len));
        remainder = quotient + quotient_len;
    }
    else if (alloc_quotient)
    {
        quotient = (T*) ::alloca(sizeof(T) * quotient_len);
    }
    else if (alloc_remainder)
    {
        remainder = (T*) ::alloca(sizeof(T) * divisor_len);
    }

    // 逐位试商
    ::memset(remainder, 0, sizeof(T) * divisor_len); // 初始化余数
    bool remainder_positive = true;
    for (size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_word_pos = dividend_len - i - 1;
        const T next_dividend_word = a[dividend_word_pos]; // 余数左移时的低位补位部分
        if (nullptr != quotient && dividend_word_pos < P)
            quotient[dividend_word_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (size_t j = 0; j < 8 * sizeof(T); ++j)
        {
            // 余数左移1位
            unsigned_shift_left(remainder, divisor_len, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_word >> (8 * sizeof(T) - 1 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive)
                unsigned_sub(remainder, divisor_len, b, divisor_len, remainder, divisor_len);
            else
                unsigned_add(remainder, divisor_len, b, divisor_len, remainder, divisor_len);

            // 试商结果
            remainder_positive = is_positive(remainder, divisor_len);
            if (remainder_positive && nullptr != quotient && dividend_word_pos < P)
                quotient[dividend_word_pos] |= (1 << (8 * sizeof(T) - 1 - j));
        }
    }

    // 商
    if (nullptr != x)
        unsigned_expand(quotient, quotient_len, x, P);

    /**
        恢复余数:
        如果未除尽且余数符号与被除数不一致，余数需加上除数
    */
    if (nullptr != y)
    {
        if (!is_zero(remainder, divisor_len) && !remainder_positive)
            unsigned_add(remainder, divisor_len, b, divisor_len, remainder, divisor_len);
        unsigned_expand(remainder, divisor_len, y, Q);
    }
}

}

#endif
