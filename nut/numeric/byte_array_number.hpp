/**
 * @file -
 * @author jingqi
 * @date 2011-12-17
 * @last-edit 2012-03-25 21:15:25 jingqi
 * @brief
 *
 * 关于函数命名后缀：
 *     "signed"后缀标记了处理有符号数的函数；"unsigned"后缀标记了处理无符号数的函数；其他函数则通用于有符号数和无符号数。
 *     "assign"后缀标记了+=、-=、&=、>>=等操作，计算结果将回馈给第一个参数。
 */

#ifndef ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_
#define ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_

#include <assert.h>
#include <string.h> // for memset(), memcpy()
#include <stdlib.h> // for malloc()

#include <nut/platform/platform.hpp>
#include <nut/platform/stdint.hpp>

// 定义半字、字、双字类型数
#if defined(NUT_PLATFORM_BITS_64)
#   define hword_type uint32_t
#   define word_type uint64_t
#   define dword_type uint128_t
#elif defined(NUT_PLATFORM_BITS_32)
#   define hword_type uint16_t
#   define word_type uint32_t
#   define dword_type uint64_t
#else
#   define hword_type uint8_t
#   define word_type uint16_t
#   define dword_type uint32_t
#endif

namespace nut
{

/**
 * (有符号数、无符号数)是否为0
 *
 * @return a<N> == 0
 */
inline bool is_zero(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);
    const size_t word_count = N / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
        if (0 != reinterpret_cast<const word_type*>(a)[i])
            return false;
    for (register size_t i = sizeof(word_type) * word_count; i < N; ++i)
        if (0 != a[i])
            return false;
    return true;
}

/**
 * (有符号数)是否为正数
 *
 * @return a<N> >= 0
 */
inline bool is_positive_signed(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);
    return 0 == (a[N - 1] & 0x80);
}

/**
 * (有符号数)有效字节数
 *
 * @return 返回值>=1
 */
inline size_t significant_size_signed(const uint8_t *a, size_t N)
{
	assert(NULL != a && N > 0);
	const bool positive = is_positive_signed(a, N);
	const uint8_t skip_value = (positive ? 0 : 0xFF);
	register size_t ret = N;
	while (ret > 1 && a[ret - 1] == skip_value && is_positive_signed(a, ret - 1) == positive)
		--ret;
	return ret;
}

/**
 * (无符号数)有效字节数
 *
 * @return 返回值>=1
 */
inline size_t significant_size_unsigned(const uint8_t *a, size_t N)
{
	assert(NULL != a && N > 0);
	register size_t ret = N;
	while (ret > 1 && a[ret - 1] == 0)
		--ret;
	return ret;
}

/**
 * (无符号数)小于
 *
 * @return a<N> < b<N>
 */
inline bool less_then_unsigned(const uint8_t *a, const uint8_t *b, size_t N)
{
	assert(NULL != a && NULL != b && N > 0);
	const size_t word_count = N / sizeof(word_type);
	for (register int i = N - 1, bound = sizeof(word_type) * word_count; i >= bound; --i)
		if (a[i] != b[i])
			return a[i] < b[i];
	for (register int i = word_count - 1; i >= 0; --i)
		if (reinterpret_cast<const word_type*>(a)[i] != reinterpret_cast<const word_type*>(b)[i])
			return reinterpret_cast<const word_type*>(a)[i] < reinterpret_cast<const word_type*>(b)[i];
	return false; // 相等
}

/**
 * (无符号数)小于
 *
 * @return a<M> < b<N>
 */
inline bool less_then_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N)
{
	assert(NULL != a && M > 0 && NULL != b && N > 0);
	const size_t word_count = (M < N ? M : N) / sizeof(word_type);
	for (register int i = (M > N ? M : N) - 1, bound = sizeof(word_type) * word_count;
		i >= bound; --i)
	{
		const uint8_t op1 = (i < (int) M ? a[i] : 0);
		const uint8_t op2 = (i < (int) N ? b[i] : 0);
		if (op1 != op2)
			return op1 < op2;
	}
	for (register int i = word_count - 1; i >= 0; --i)
		if (reinterpret_cast<const word_type*>(a)[i] != reinterpret_cast<const word_type*>(b)[i])
			return reinterpret_cast<const word_type*>(a)[i] < reinterpret_cast<const word_type*>(b)[i];
	return false; // 相等
}

/**
 * (有符号数)小于
 *
 * @return a<N> < b<N>
 */
inline bool less_then_signed(const uint8_t *a, const uint8_t *b, size_t N)
{
    assert(NULL != a && NULL != b && N > 0);
    const bool positive1 = is_positive_signed(a, N), positive2 = is_positive_signed(b, N);
    if (positive1 != positive2)
        return positive2;

    return less_then_unsigned(a, b, N);
}

/**
 * (有符号数)小于
 *
 * @return a<M> < b<N>
 */
inline bool less_then_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N)
{
	assert(NULL != a && M > 0 && NULL != b && N > 0);
	const bool positive1 = is_positive_signed(a, M), positive2 = is_positive_signed(b, N);
	if (positive1 != positive2)
		return positive2;

	const uint8_t fill = (positive1 ? 0 : 0xFF);
	const size_t word_count = (M < N ? M : N) / sizeof(word_type);
	for (register int i = (M > N ? M : N) - 1, bound = sizeof(word_type) * word_count;
		i >= bound; --i)
	{
		const uint8_t op1 = (i < (int) M ? a[i] : fill);
		const uint8_t op2 = (i < (int) N ? b[i] : fill);
		if (op1 != op2)
			return op1 < op2;
	}
	for (register int i = word_count - 1; i >= 0; --i)
		if (reinterpret_cast<const word_type*>(a)[i] != reinterpret_cast<const word_type*>(b)[i])
			return reinterpret_cast<const word_type*>(a)[i] < reinterpret_cast<const word_type*>(b)[i];
	return false; // 相等
}

/**
 * (有符号数)带符号扩展(截断)
 * a<M> -> x<N>
 */
inline void expand_signed(const uint8_t *a, size_t M, uint8_t *x, size_t N)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
	if (x != a)
		::memcpy(x, a, (M < N ? M : N));
    if (M < N)
        ::memset(x + M, (is_positive_signed(a, M) ? 0 : 0xFF), N - M);
}

/**
 * (有符号数)带符号扩展(截断)
 * x<M> -> x<N>
 */
inline void expand_assign_signed(uint8_t *x, size_t M, size_t N)
{
	assert(NULL != x && M > 0 && N > 0);
	expand_signed(x, M, x, N);
}

/**
 * (无符号数)无符号扩展(截断)
 * a<M> -> x<N>
 */
inline void expand_unsigned(const uint8_t *a, size_t M, uint8_t *x, size_t N)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
	if (x != a)
		::memcpy(x, a, (M < N ? M : N));
    if (M < N)
        ::memset(x + M, 0, N - M);
}

/**
 * (无符号数)无符号扩展(截断)
 * x<M> -> x<N>
 */
inline void expand_assign_unsigned(uint8_t *x, size_t M, size_t N)
{
	assert(NULL != x && M > 0 && N > 0);
	expand_unsigned(x, M, x, N);
}

/**
 * (有符号数、无符号数)相加
 * x<N> = a<N> + b<N>
 *
 * @return 进位
 */
inline uint8_t add(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(word_type);
    register uint8_t carry = 0;
    for (register size_t i = 0; i < word_count; ++i)
    {
        const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
        dword_type pluser2 = reinterpret_cast<const word_type*>(b)[i];
        pluser2 += pluser1 + carry;

        reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
    {
        const uint16_t pluser1 = a[i];
        uint16_t pluser2 = b[i];
        pluser2 += pluser1 + carry;

        x[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
	return carry;
}

/**
 * (有符号数、无符号数)相加
 * a<N> += b<N>
 *
 * @return 进位
 */
inline uint8_t add_assign(uint8_t *a, const uint8_t *b, size_t N)
{
	assert(NULL != a && NULL != b && N > 0);
	return add(a, b, a, N);
}

/**
 * (有符号数)相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
inline uint8_t add_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
	assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
	const size_t word_count = (M < N ? (M < P ? M : P) : (N < P ? N : P)) / sizeof(word_type);
	register uint8_t carry = 0;
	for (register size_t i = 0; i < word_count; ++i)
	{
		const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
		dword_type pluser2 = reinterpret_cast<const word_type*>(b)[i];
		pluser2 += pluser1 + carry;

		reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser2);
		carry = static_cast<word_type>(pluser2 >> (sizeof(word_type) * 8));
	}
	const uint16_t filla = (is_positive_signed(a, M) ? 0 : 0x00FF), fillb = (is_positive_signed(b, N) ? 0 : 0x00FF);
	for (register size_t i = sizeof(word_type) * word_count; i < P; ++i)
	{
		const uint16_t pluser1 = (i < M ? a[i] : filla);
		uint16_t pluser2 = (i < N ? b[i] : fillb);
		pluser2 += pluser1 + carry;

		x[i] = static_cast<uint8_t>(pluser2);
		carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
	}
	return carry;
}

/**
 * (无符号数)相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
inline uint8_t add_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
	assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
	const size_t word_count = (M < N ? (M < P ? M : P) : (N < P ? N : P)) / sizeof(word_type);
	register uint8_t carry = 0;
	for (register size_t i = 0; i < word_count; ++i)
	{
		const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
		dword_type pluser2 = reinterpret_cast<const word_type*>(b)[i];
		pluser2 += pluser1 + carry;

		reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser2);
		carry = static_cast<word_type>(pluser2 >> (sizeof(word_type) * 8));
	}
	for (register size_t i = word_count * sizeof(word_type); i < P; ++i)
	{
		const uint16_t pluser1 = (i < M ? a[i] : 0);
		uint16_t pluser2 = (i < N ? b[i] : 0);
		pluser2 += pluser1 + carry;

		x[i] = static_cast<uint8_t>(pluser2);
		carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
	}
	return carry;
}

/**
 * (有符号数、无符号数)加1
 * x<N> = a<N> + 1
 *
 * @return 进位
 */
inline uint8_t increase(const uint8_t *a, uint8_t *x, size_t N)
{
	assert(NULL != a && NULL != x && N > 0);
	const size_t word_count = N / sizeof(word_type);
	register uint8_t carry = 1;
	for (register size_t i = 0; i < word_count; ++i)
	{
		dword_type pluser = reinterpret_cast<const word_type*>(a)[i];
		pluser += carry;

		reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser);
		carry = static_cast<uint8_t>(pluser >> (sizeof(word_type) * 8));
	}
	for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
	{
		uint16_t pluser = a[i];
		pluser += carry;

		x[i] = static_cast<uint8_t>(pluser);
		carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
	}
	return carry;
}

/**
 * (有符号数、无符号数)加1
 * x<N> += 1
 *
 * @return 进位
 */
inline uint8_t increase_assign(uint8_t *x, size_t N)
{
	assert(NULL != x && N > 0);
	const size_t word_count = N / sizeof(word_type);
	register uint8_t carry = 1;
	for (register size_t i = 0; i < word_count && 0 != carry; ++i)
	{
		dword_type pluser = reinterpret_cast<const word_type*>(x)[i];
		pluser += carry;

		reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser);
		carry = static_cast<uint8_t>(pluser >> (sizeof(word_type) * 8));
	}
	for (register size_t i = word_count * sizeof(word_type); i < N && 0 != carry; ++i)
	{
		uint16_t pluser = x[i];
		pluser += carry;

		x[i] = static_cast<uint8_t>(pluser);
		carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
	}
	return carry;
}

/**
 * (有符号数、无符号数)相减
 * x<N> = a<N> - b<N>
 *
 * @return 进位
 */
inline uint8_t sub(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(word_type);
    uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
        dword_type pluser2 = static_cast<word_type>(~(reinterpret_cast<const word_type*>(b)[i]));
        pluser2 += pluser1 + carry;

        reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
    {
        uint16_t pluser1 = a[i];
        uint16_t pluser2 = static_cast<uint8_t>(~(b[i]));
        pluser2 += pluser1 + carry;

        x[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
	return carry;
}

/**
 * (有符号数、无符号数)相减
 * a<N> -= b<N>
 *
 * @return 进位
 */
inline uint8_t sub_assign(uint8_t *a, const uint8_t *b, size_t N)
{
	assert(NULL != a && NULL != b && N > 0);
	return sub(a, b, a, N);
}

/**
 * (有符号数)相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
inline uint8_t sub_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
	assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
	const size_t word_count = (M < N ? (M < P ? M : P) : (N < P ? N : P)) / sizeof(word_type);
	register uint8_t carry = 1;
	for (register size_t i = 0; i < word_count; ++i)
	{
		const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
		dword_type pluser2 = static_cast<word_type>(~(reinterpret_cast<const word_type*>(b)[i]));
		pluser2 += pluser1 + carry;

		reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser2);
		carry = static_cast<word_type>(pluser2 >> (sizeof(word_type) * 8));
	}
	const uint16_t filla = (is_positive_signed(a, M) ? 0 : 0x00FF), fillb = (is_positive_signed(b, N) ? 0 : 0x00FF);
	for (register size_t i = word_count * sizeof(word_type); i < P; ++i)
	{
		const uint16_t pluser1 = (i < M ? a[i] : filla);
		uint16_t pluser2 = static_cast<uint8_t>(~(i < N ? b[i] : fillb));
		pluser2 += pluser1 + carry;

		x[i] = static_cast<uint8_t>(pluser2);
		carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
	}
	return carry;
}

/**
 * (无符号数)相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
inline uint8_t sub_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
	assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
	const size_t word_count = (M < N ? (M < P ? M : P) : (N < P ? N : P)) / sizeof(word_type);
	register uint8_t carry = 1;
	for (register size_t i = 0; i < word_count; ++i)
	{
		const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
		dword_type pluser2 = static_cast<word_type>(~(reinterpret_cast<const word_type*>(b)[i]));
		pluser2 += pluser1 + carry;

		reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser2);
		carry = static_cast<word_type>(pluser2 >> (sizeof(word_type) * 8));
	}
	for (register size_t i = word_count * sizeof(word_type); i < P; ++i)
	{
		const uint16_t pluser1 = (i < M ? a[i] : 0);
		uint16_t pluser2 = static_cast<uint8_t>(~(i < N ? b[i] : 0));
		pluser2 += pluser1 + carry;

		x[i] = static_cast<uint8_t>(pluser2);
		carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
	}
	return carry;
}

/**
 * (有符号数、无符号数)减1
 * x<N> = a<N> - 1
 *
 * @return 进位
 */
inline uint8_t decrease(const uint8_t *a, uint8_t *x, size_t N)
{
	assert(NULL != a && NULL != x && N > 0);
	const size_t word_count = N / sizeof(word_type);
	uint8_t carry = 0;
	for (register size_t i = 0; i < word_count; ++i)
	{
		dword_type pluser = reinterpret_cast<const word_type*>(a)[i];
		pluser += carry + ((dword_type) ~ (word_type) 0);

		reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser);
		carry = static_cast<uint8_t>(pluser >> (sizeof(word_type) * 8));
	}
	for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
	{
		uint16_t pluser = a[i];
		pluser += carry + 0x00FF;

		x[i] = static_cast<uint8_t>(pluser);
		carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
	}
	return carry;
}

/**
 * (有符号数、无符号数)减1
 * x<N> -= 1
 *
 * @return 进位
 */
inline uint8_t decrease_assign(uint8_t *x, size_t N)
{
	assert(NULL != x && N > 0);
	const size_t word_count = N / sizeof(word_type);
	uint8_t carry = 0;
	for (register size_t i = 0; i < word_count && 1 != carry; ++i)
	{
		dword_type pluser = reinterpret_cast<const word_type*>(x)[i];
		pluser += carry + ((dword_type) ~ (word_type) 0);

		reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser);
		carry = static_cast<uint8_t>(pluser >> (sizeof(word_type) * 8));
	}
	for (register size_t i = word_count * sizeof(word_type) && 1 != carry; i < N; ++i)
	{
		uint16_t pluser = x[i];
		pluser += carry + 0x00FF;

		x[i] = static_cast<uint8_t>(pluser);
		carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
	}
	return carry;
}

/**
 * (有符号数)取相反数
 * x<N> = -a<N>
 *
 * @return 进位
 */
inline uint8_t opposite_signed(const uint8_t *a, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != x && N > 0);
    const size_t word_count = N / sizeof(word_type);
    uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        dword_type pluser1 = static_cast<word_type>(~(reinterpret_cast<const word_type*>(a)[i]));
        pluser1 += carry;

        reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser1);
        carry = static_cast<uint8_t>(pluser1 >> (sizeof(word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
    {
        uint16_t pluser1 = static_cast<uint8_t>(~(a[i]));
        pluser1 += carry;

        x[i] = static_cast<uint8_t>(pluser1);
        carry = static_cast<uint8_t>(pluser1 >> (sizeof(uint8_t) * 8));
    }
	return carry;
}

/**
 * (有符号数)取相反数
 * x<N> = -x<N>
 *
 * @return 进位
 */
inline uint8_t opposite_assign_signed(uint8_t *x, size_t N)
{
	assert(NULL != x && N > 0);
	return opposite_signed(x, x, N);
}

/**
 * 相乘
 * x<N> = a<N> * b<N>
 */
inline void multiply(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    uint8_t *ret = (uint8_t*)::malloc(N);
    ::memset(ret, 0, N);

    const size_t word_count = N / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
    {
        word_type carry = 0;
        const dword_type mult1 = reinterpret_cast<const word_type*>(a)[i];
        if (mult1 == 0)
            continue;

        register size_t j = 0;
        for (; i + j < word_count; ++j)
        {
            dword_type mult2 = reinterpret_cast<const word_type*>(b)[j];
            mult2 = mult1 * mult2 + reinterpret_cast<word_type*>(ret)[i + j] + carry;

            reinterpret_cast<word_type*>(ret)[i + j] = static_cast<word_type>(mult2);
            carry = static_cast<word_type>(mult2 >> (sizeof(word_type) * 8));
        }

        size_t bound = i * sizeof(word_type);
        for (register size_t k = j * sizeof(word_type); k + bound < N; ++k)
        {
            dword_type mult2 = b[k];
            mult2 = mult1 * mult2 + ret[k + bound] + carry;

            ret[k + bound] = static_cast<uint8_t>(mult2);
            carry = static_cast<word_type>(mult2 >> (sizeof(uint8_t) * 8));
        }
    }

    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
    {
        uint8_t carry = 0;
        const uint16_t mult1 = a[i];
        if (mult1 == 0)
            continue;

        for (register size_t j = 0; i + j < N; ++j)
        {
            uint16_t mult2 = b[j];
            mult2 = mult1 * mult2 + ret[i + j] + carry;

            ret[i + j] = static_cast<uint8_t>(mult2);
            carry = static_cast<uint8_t>(mult2 >> (sizeof(uint8_t) * 8));
        }
    }

    ::memcpy(x, ret, N);
    ::free(ret);
}

/**
 * (有符号数)相乘
 * x<P> = a<M> * b<N>
 */
inline void multiply_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
	assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
	const uint8_t filla = (is_positive_signed(a,M) ? 0 : 0xFF), fillb = (is_positive_signed(b,N) ? 0 : 0xFF);
	uint8_t *ret = (uint8_t*) ::malloc(P);
	::memset(ret, 0, P);
	for (register size_t i = 0; i < P; ++i)
	{
		uint8_t carry = 0;
		const uint16_t mult1 = (i < M ? a[i] : filla);
		if (i >= M && 0 == filla)
			break;
		if (mult1 == 0)
			continue;

		for (register size_t j = 0; i + j < P; ++j)
		{
			uint16_t mult2 = (j < N ? b[j] : fillb);
			mult2 = mult1 * mult2 + ret[i + j] + carry;

			ret[i + j] = static_cast<uint8_t>(mult2);
			carry = static_cast<uint8_t>(mult2 >> (sizeof(uint8_t) * 8));
		}
	}

	::memcpy(x, ret, P);
	::free(ret);
}

/**
 * (无符号数)相乘
 * x<P> = a<M> * b<N>
 */
inline void multiply_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
	assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
	uint8_t *ret = (uint8_t*) ::malloc(P);
	::memset(ret, 0, P);
	for (register size_t i = 0; i < P && i < M; ++i)
	{
		uint8_t carry = 0;
		const uint16_t mult1 = a[i];
		if (mult1 == 0)
			continue;

		for (register size_t j = 0; i + j < P; ++j)
		{
			uint16_t mult2 = (j < N ? b[j] : 0);
			mult2 = mult1 * mult2 + ret[i + j] + carry;

			ret[i + j] = static_cast<uint8_t>(mult2);
			carry = static_cast<uint8_t>(mult2 >> (sizeof(uint8_t) * 8));
		}
	}

	::memcpy(x, ret, P);
	::free(ret);
}

/**
 * 左移
 * x<N> = a<N> << count
 */
inline void shift_left(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
	assert(NULL != a && NULL != x && N > 0);
	const int bytes_off = count / 8, bits_off = count % 8;
	for (register int i = N - 1; i >= 0; --i)
	{
		const uint8_t high = ((i - bytes_off >= 0 ? a[i - bytes_off] : 0) << bits_off);
		const uint8_t low = ((i - bytes_off - 1 >= 0 ? a[i - bytes_off - 1] : 0) >> (8 - bits_off));
		x[i] = high | low;
	}
}

/**
 * 左移
 * x<N> <<= count
 */
inline void shift_left_assign(uint8_t *x, size_t N, size_t count)
{
	assert(NULL != x && N > 0);
	shift_left(x, x, N, count);
}

/**
 * (无符号数)右移
 * x<N> = a<N> >> count
 */
inline void shift_right_unsigned(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
	assert(NULL != a && NULL != x && N > 0);
	const int bytes_off = count / 8, bits_off = count % 8;
	for (register size_t i = 0; i < N; ++i)
	{
		const uint8_t high = ((i + bytes_off + 1 >= N ? 0 : a[i + bytes_off + 1]) << (8 - bits_off));
		const uint8_t low = ((i + bytes_off >= N ? 0 : a[i + bytes_off]) >> bits_off);
		x[i] = high | low;
	}
}

/**
 * (无符号数)右移
 * x<N> >>= count
 */
inline void shift_right_assign_unsigned(uint8_t *x, size_t N, size_t count)
{
	assert(NULL != x && N > 0);
	shift_right_unsigned(x, x, N, count);
}

/**
 * (有符号数)右移
 * x<N> = a<N> >> count
 */
inline void shift_right_signed(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
	assert(NULL != a && NULL != x && N > 0);
	const int bytes_off = count / 8, bits_off = count % 8;
	const uint8_t fill = (is_positive_signed(a, N) ? 0 : 0xFF);
	for (register size_t i = 0; i < N; ++i)
	{
		const uint8_t high = ((i + bytes_off + 1 >= N ? fill : a[i + bytes_off + 1]) << (8 - bits_off));
		const uint8_t low = ((i + bytes_off >= N ? fill : a[i + bytes_off]) >> bits_off);
		x[i] = high | low;
	}
}

/**
 * (有符号数)右移
 * x<N> >>= count
 */
inline void shift_right_assign_signed(uint8_t *x, size_t N, size_t count)
{
	assert(NULL != x && N > 0);
	shift_right_signed(x, x, N, count);
}

/**
 * 循环左移
 * x<N> = a<N> <<< count
 */
inline void circle_shift_left(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
	assert(NULL != a && NULL != x && N > 0);
	uint8_t *ret = (uint8_t*) ::malloc(sizeof(uint8_t) * N);
	const int bytes_off = count / 8, bits_off = count % 8;
	for (register size_t i = 0; i < N; ++i)
	{
		const uint8_t high = (a[(i + N - (bytes_off % N)) % N] << bits_off);
		const uint8_t low = (a[(i + N - (bytes_off % N) - 1) % N] >> (8 - bits_off));
		ret[i] = high | low;
	}
	::memcpy(x, ret, sizeof(uint8_t) * N);
	::free(ret);
}

/**
 * 循环右移
 * x<N> = a<N> >>> count
 */
inline void circle_shift_right(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
	assert(NULL != a && NULL != x && N > 0);
	uint8_t *ret = (uint8_t*) ::malloc(sizeof(uint8_t) * N);
	const int bytes_off = count / 8, bits_off = count % 8;
	for (register size_t i = 0; i < N; ++i)
	{
		const uint8_t high = (a[(i + bytes_off + 1) % N] << (8 - bits_off));
		const uint8_t low = (a[(i + bytes_off) % N] >> bits_off);
		ret[i] = high | low;
	}
	::memcpy(x, ret, sizeof(uint8_t) * N);
	::free(ret);
}

/**
 * (有符号数)相除
 * x<N> = a<N> / b<N>
 * y<N> = a<N> % b<N>
 *
 * @param x
 *      商
 * @param y
 *      余数
 */
inline void divide_signed(const uint8_t *a, const uint8_t *b, uint8_t *x, uint8_t *y, size_t N)
{
	assert(NULL != a && NULL != b && N > 0);
	assert(NULL != x || NULL != y);
	assert(!is_zero(b, N)); // 被除数不能为0

	// 常量
	const size_t divided_len = significant_size_signed(a, N);
    const size_t divider_len = significant_size_signed(b, N);
	const bool divided_positive = is_positive_signed(a, N);
	const bool divider_positive = is_positive_signed(b, N);

	// 逐位试商
	uint8_t *remained = (uint8_t*) ::malloc(sizeof(uint8_t) * (divider_len + 1)); // 余数 remained[1 ~ (divider_len + 1)], remained[0]用于左移补位
	::memset(remained + 1, (divided_positive ? 0 : 0xFF), divider_len); // 初始化余数
	bool remained_positive = divided_positive; // 余数的符号
	for (register size_t i = 0; i < divided_len; ++i)
	{
		const size_t divided_byte_pos = divided_len - i - 1;
		remained[0] = a[divided_byte_pos]; // 余数左移时的低位补位部分
		if (NULL != x)
			x[divided_byte_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

		for (register size_t j = 0; j < 8; ++j)
		{
			shift_left(remained, remained, divider_len + 1, 1); // 余数左移1位
			if (remained_positive == divider_positive)
				sub(remained + 1, b, remained + 1, divider_len);
			else
				add(remained + 1, b, remained + 1, divider_len);

			remained_positive = is_positive_signed(remained + 1, divider_len);
			if (NULL != x && remained_positive == divider_positive)
				x[divided_byte_pos] |= (1 << (7 - j));
		}
	}

	const bool remained_is_zero = is_zero(remained + 1, divider_len);
	if (NULL != x)
	{
		/**
		 修正补数形式的商:
		   如果除尽且除数为负数，则商加1
		   如果未除尽且商为负数，则商加1
		 */
		if (remained_is_zero)
		{
			if (!divider_positive)
				increase_assign(x, divided_len);
		}
		else
		{
			if (!is_positive_signed(x, divided_len))
				increase_assign(x, divided_len);
		}
		expand_assign_signed(x, divided_len, N);
	}
	if (NULL != y)
	{
		/**
		 恢复余数:
		 如果未除尽且余数符号与被除数不一致，余数需加修正
		 */
		if (!remained_is_zero && remained_positive != divided_positive)
		{
			if (divider_positive == divided_positive)
				add(remained + 1, b, remained + 1, divider_len);
			else
				sub(remained + 1, b, remained + 1, divider_len);
		}
		expand_signed(remained + 1, divider_len, y, N);
	}

	::free(remained);
}

/**
 * (有符号数)相除
 * x<P> = a<M> / b<N>
 * y<Q> = a<M> % b<N>
 *
 * @param x
 *      商
 * @param y
 *      余数
 */
inline void divide_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P, uint8_t *y, size_t Q)
{
	assert(NULL != a && M > 0 && NULL != b && N > 0);
	assert((NULL != x && P > 0) || (NULL != y && Q > 0));
	assert(x != b); // 允许 x==a，但是不允许 x==b，否则会破坏数据
	assert(!is_zero(b, N)); // 被除数不能为0

	// 常量
	const size_t divided_len = significant_size_signed(a, M);
    const size_t divider_len = significant_size_signed(b, N);
	const bool divided_positive = is_positive_signed(a, M);
	const bool divider_positive = is_positive_signed(b, N);

	// 逐位试商
	uint8_t *remained = (uint8_t*) ::malloc(sizeof(uint8_t) * (divider_len + 1)); // 余数 remained[1 ~ (divider_len + 1)], remained[0]用于左移补位
	::memset(remained + 1, (divided_positive ? 0 : 0xFF), divider_len); // 初始化余数
	bool remained_positive = divided_positive; // 余数的符号
	bool x_positive = true; // x 的符号
	for (register size_t i = 0; i < divided_len; ++i)
	{
		const size_t divided_byte_pos = divided_len - i - 1;
		remained[0] = a[divided_byte_pos]; // 余数左移时的低位补位部分
		if (NULL != x && divided_byte_pos < P)
			x[divided_byte_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

		for (register size_t j = 0; j < 8; ++j)
		{
			shift_left(remained, remained, divider_len + 1, 1); // 余数左移1位
			if (remained_positive == divider_positive)
				sub(remained + 1, b, remained + 1, divider_len);
			else
				add(remained + 1, b, remained + 1, divider_len);

			remained_positive = is_positive_signed(remained + 1, divider_len);
			if (remained_positive == divider_positive && NULL != x && divided_byte_pos < P)
				x[divided_byte_pos] |= (1 << (7 - j));
			if (i == 0 && j == 0 && remained_positive == divider_positive)
				x_positive = false;
		}
	}

	const bool remained_is_zero = is_zero(remained + 1, divider_len);
	if (NULL != x)
	{
		/**
		 修正补数形式的商:
		   如果除尽且除数为负数，则商加1
		   如果未除尽且商为负数，则商加1
		 */
		if (remained_is_zero)
		{
			if (!divider_positive)
				increase_assign(x, divided_len);
		}
		else
		{
			if (!x_positive)
				increase_assign(x, divided_len);
		}
		if (divided_len < N)
			expand_assign_signed(x, divided_len, N);
	}
	if (NULL != y)
	{
		/**
		 恢复余数:
		 如果未除尽且余数符号与被除数不一致，余数需加修正
		 */
		if (!remained_is_zero && remained_positive != divided_positive)
		{
			if (divider_positive == divided_positive)
				add(remained + 1, b, remained + 1, divider_len);
			else
				sub(remained + 1, b, remained + 1, divider_len);
		}
		expand_signed(remained + 1, divider_len, y, Q);
	}

	::free(remained);
}

/**
 * (无符号数)相除
 * x<N> = a<N> / b<N>
 * y<N> = a<N> % b<N>
 *
 * @param x
 *    商
 * @param y
 *    余数
 */
inline void divide_unsigned(const uint8_t *a, const uint8_t *b, uint8_t *x, uint8_t *y, size_t N)
{
	assert(NULL != a && NULL != b && N > 0);
	assert(NULL != x || NULL != y);
	assert(!is_zero(b, N)); // 被除数不能为0

	// 常量
	const size_t divided_len = significant_size_unsigned(a, N);
	const size_t divider_len = significant_size_unsigned(b, N);

	// 逐位试商
	uint8_t *remained = (uint8_t*) ::malloc(sizeof(uint8_t) * (divider_len + 1)); // 余数 remained[1 ~ (divider_len + 1)], remained[0]用于左移补位
	::memset(remained + 1, 0, divider_len); // 初始化余数
	bool remained_positive = true;
	for (register size_t i = 0; i < divided_len; ++i)
	{
		const size_t divided_byte_pos = divided_len - i - 1;
		remained[0] = a[divided_byte_pos]; // 余数左移时的低位补位部分
		if (NULL != x)
			x[divided_byte_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

		for (register size_t j = 0; j < 8; ++j)
		{
			shift_left_assign(remained, divider_len + 1, 1); // 余数左移1位
			if (remained_positive)
				sub_assign(remained + 1, b, divider_len);
			else
				add_assign(remained + 1, b, divider_len);

			remained_positive = is_positive_signed(remained + 1, divider_len);
			if (NULL != x && remained_positive)
				x[divided_byte_pos] |= (1 << (7 - j));
		}

		// 商
		if (NULL != x && divided_len < N)
			expand_assign_unsigned(x, divided_len, N);
		// 余数
		if (NULL != y)
		{
			/**
			 恢复余数:
			 如果未除尽且余数符号与被除数不一致，余数需加上除数
			 */
			if (!is_zero(remained + 1, divider_len) && !remained_positive)
				add_assign(remained + 1, b, divider_len);
			expand_unsigned(remained + 1, divider_len, y, N);
		}

		::free(remained);
	}
}

/**
 * (无符号数)相除
 * x<P> = a<M> / b<N>
 * y<Q> = a<M> % b<N>
 *
 * @param x
 *    商
 * @param y
 *    余数
 */
inline void divide_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P, uint8_t *y, size_t Q)
{
	assert(NULL != a && M > 0 && NULL != b && N > 0);
	assert((NULL != x && P > 0) || (NULL != y && Q > 0));
	assert(x != b); // 允许 x==a，但是不允许 x==b，否则会破坏数据
	assert(!is_zero(b, N)); // 被除数不能为0

	// 常量
	const size_t divided_len = significant_size_unsigned(a, M);
	const size_t divider_len = significant_size_unsigned(b, N);

	// 逐位试商
	uint8_t *remained = (uint8_t*) ::malloc(sizeof(uint8_t) * (divider_len + 1)); // 余数 remained[1 ~ (divider_len + 1)], remained[0]用于左移补位
	::memset(remained + 1, 0, divider_len); // 初始化余数
	bool remained_positive = true;
	for (register size_t i = 0; i < divided_len; ++i)
	{
		const size_t divided_byte_pos = divided_len - i - 1;
		remained[0] = a[divided_byte_pos]; // 余数左移时的低位补位部分
		if (NULL != x && divided_byte_pos < P)
			x[divided_byte_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

		for (register size_t j = 0; j < 8; ++j)
		{
			shift_left_assign(remained, divider_len + 1, 1); // 余数左移1位
			if (remained_positive)
				sub_assign(remained + 1, b, divider_len);
			else
				add_assign(remained + 1, b, divider_len);

			remained_positive = is_positive_signed(remained + 1, divider_len);
			if (remained_positive && NULL != x && divided_byte_pos < P)
				x[divided_byte_pos] |= (1 << (7 - j));
		}

		// 商
		if (NULL != x && divided_len < P)
			expand_assign_unsigned(x, divided_len, P);
		// 余数
		if (NULL != y)
		{
			/**
			 恢复余数:
			 如果未除尽且余数符号与被除数不一致，余数需加上除数
			 */
			if (!is_zero(remained + 1, divider_len) && !remained_positive)
				add_assign(remained + 1, b, divider_len);
			expand_unsigned(remained + 1, divider_len, y, Q);
		}

		::free(remained);
	}
}

/**
 * 按位与
 * x<N> = a<N> & b<N>
 */
inline void bit_and(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<word_type*>(x)[i] = reinterpret_cast<const word_type*>(a)[i] & reinterpret_cast<const word_type*>(b)[i];
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
        x[i] = a[i] & b[i];
}

/**
 * 按位与
 * a<N> &= b<N>
 */
inline void bit_and_assign(uint8_t *a, const uint8_t *b, size_t N)
{
	assert(NULL != a && NULL != b && N > 0);
	bit_and(a, b, a, N);
}

/**
 * 按位或
 * x<N> = a<N> | b<N>
 */
inline void bit_or(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<word_type*>(x)[i] = reinterpret_cast<const word_type*>(a)[i] | reinterpret_cast<const word_type*>(b)[i];
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
        x[i] = a[i] | b[i];
}

/**
 * 按位或
 * a<N> |= b<N>
 */
inline void bit_or_assign(uint8_t *a, const uint8_t *b, size_t N)
{
	assert(NULL != a && NULL != b && N > 0);
	bit_or(a, b, a, N);
}

/**
 * 按位异或
 * x<N> = a<N> ^ b<N>
 */
inline void bit_xor(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<word_type*>(x)[i] = reinterpret_cast<const word_type*>(a)[i] ^ reinterpret_cast<const word_type*>(b)[i];
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
        x[i] = a[i] ^ b[i];
}

/**
 * 按位异或
 * a<N> ^= b<N>
 */
inline void bit_xor_assign(uint8_t *a, const uint8_t *b, size_t N)
{
	assert(NULL != a && NULL != b && N > 0);
	bit_xor(a, b, a, N);
}

/**
 * 按位同或
 * x<N> = ~(a<N> ^ b<N>)
 */
inline void bit_nxor(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<word_type*>(x)[i] = ~(reinterpret_cast<const word_type*>(a)[i] ^ reinterpret_cast<const word_type*>(b)[i]);
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
        x[i] = ~(a[i] ^ b[i]);
}

/**
 * 按位同或
 * a<N> = ~(a<N> ^ b<N>)
 */
inline void bit_nxor_assign(uint8_t *a, const uint8_t *b, size_t N)
{
	assert(NULL != a && NULL != b && N > 0);
	bit_nxor(a, b, a, N);
}

/**
 * 按位取反
 * x<N> = ~a<N>
 */
inline void bit_not(const uint8_t *a, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != x && N > 0);
    const size_t word_count = N / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<word_type*>(x)[i] = ~(reinterpret_cast<const word_type*>(a)[i]);
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
        x[i] = ~(a[i]);
}

/**
 * 按位取反
 * x<N> = ~x<N>
 */
inline void bit_not_assign(uint8_t *x, size_t N)
{
	assert(NULL != x && N > 0);
	bit_not(x, x, N);
}

}

#undef hword_type
#undef word_type
#undef dword_type

#endif /* head file guarder */

