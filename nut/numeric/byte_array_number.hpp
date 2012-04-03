/**
 * @file -
 * @author jingqi
 * @date 2011-12-17
 * @last-edit 2012-03-25 21:15:25 jingqi
 */

#ifndef ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_
#define ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_

#include <assert.h>
#include <string.h> // for memset(), memcpy()
#include <stdlib.h> // for malloc()

#include <nut/platform/platform.hpp>
#include <nut/platform/stdint.hpp>

#if defined(NUT_PLATFORM_BITS_64)
#   define machine_hword_type uint32_t
#   define machine_word_type uint64_t
#   define machine_dword_type uint128_t
#elif defined(NUT_PLATFORM_BITS_32)
#   define machine_hword_type uint16_t
#   define machine_word_type uint32_t
#   define machine_dword_type uint64_t
#else
#   define machine_hword_type uint8_t
#   define machine_word_type uint16_t
#   define machine_dword_type uint32_t
#endif

namespace nut
{

/** 是否为0 */
inline bool is_zero(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        if (0 != reinterpret_cast<const machine_word_type*>(a)[i])
            return false;
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        if (0 != a[i])
            return false;
    return true;
}

/** (有符号数)是否为正数 */
inline bool signed_is_positive(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);
    return 0 == (a[N - 1] & 0x80);
}

/** (有符号数)保持值不变的前提下所需的最小字节数组大小 */
inline size_t signed_min_size(const uint8_t *a, size_t N)
{
	assert(NULL != a && N > 0);
	const bool positive = signed_is_positive(a, N);
	const uint8_t skip_value = (positive ? 0 : 0xFF);
	register size_t ret = N;
	while (ret > 1 && a[ret - 1] == skip_value && signed_is_positive(a, ret - 1) == positive)
		--ret;
	return ret;
}

/** (无符号数)保持值不变的前提下所需的最小字节数组大小 */
inline size_t unsigned_min_size(const uint8_t *a, size_t N)
{
	assert(NULL != a && N > 0);
	register size_t ret = N;
	while (ret > 1 && a[ret - 1] == 0)
		--ret;
	return ret;
}

/** (无符号数)小于 */
inline bool unsigned_less_then(const uint8_t *a, const uint8_t *b, size_t N)
{
    assert(NULL != a && NULL != b && N > 0);
    const size_t int_count = N / sizeof(machine_word_type);
    for (register int i = N - 1, bound = int_count * sizeof(machine_word_type); i >= bound; --i)
        if (a[i] != b[i])
            return a[i] < b[i];
    for (register int i = int_count - 1; i >= 0; --i)
        if (reinterpret_cast<const machine_word_type*>(a)[i] != reinterpret_cast<const machine_word_type*>(b)[i])
            return reinterpret_cast<const machine_word_type*>(a)[i] < reinterpret_cast<const machine_word_type*>(b)[i];
    return false; // 相等
}

/** (有符号数)小于 */
inline bool signed_less_then(const uint8_t *a, const uint8_t *b, size_t N)
{
    assert(NULL != a && NULL != b && N > 0);
    const bool positive1 = signed_is_positive(a, N), positive2 = signed_is_positive(b, N);
    if (positive1 != positive2)
        return positive2;

    return unsigned_less_then(a, b, N);
}

/** (有符号数)带符号扩展(截断) */
inline void signed_expand(const uint8_t *a, size_t M, uint8_t *x, size_t N)
{
    assert(NULL != a && M > 0);
    assert(NULL != x && N > 0);
	if (x != a)
		::memcpy(x, a, (M < N ? M : N));
    if (M < N)
        ::memset(x + M, (signed_is_positive(a, M) ? 0 : 0xFF), N - M);
}

/** (无符号数)无符号扩展(截断) */
inline void unsigned_expand(const uint8_t *a, size_t M, uint8_t *x, size_t N)
{
    assert(NULL != a && M > 0);
    assert(NULL != x && N > 0);
	if (x != a)
		::memcpy(x, a, (M < N ? M : N));
    if (M < N)
        ::memset(x + M, 0, N - M);
}

/** 相加 */
inline void add(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    uint8_t carry = 0;
    for (register size_t i = 0; i < word_count; ++i)
    {
        const machine_dword_type pluser1 = reinterpret_cast<const machine_word_type*>(a)[i];
        machine_dword_type pluser2 = reinterpret_cast<const machine_word_type*>(b)[i];
        pluser2 += pluser1 + carry;

        reinterpret_cast<machine_word_type*>(x)[i] = static_cast<machine_word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(machine_word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
    {
        const uint16_t pluser1 = a[i];
        uint16_t pluser2 = b[i];
        pluser2 += pluser1 + carry;

        x[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
}

/** 加1 */
inline void increase(const uint8_t *a, uint8_t *x, size_t N)
{
	assert(NULL != a && NULL != x && N > 0);
	const size_t word_count = N / sizeof(machine_word_type);
	uint8_t carray = 1;
	for (register size_t i = 0; i < word_count; ++i)
	{
		machine_dword_type pluser = reinterpret_cast<const machine_word_type*>(a)[i];
		pluser += carray;

		reinterpret_cast<machine_word_type*>(x)[i] = static_cast<machine_word_type>(pluser);
		carray = static_cast<uint8_t>(pluser >> (sizeof(machine_word_type) * 8));
	}
	for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
	{
		uint16_t pluser = a[i];
		pluser += carray;

		x[i] = static_cast<uint8_t>(pluser);
		carray = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
	}
}

/** 相减 */
inline void sub(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        const machine_dword_type pluser1 = reinterpret_cast<const machine_word_type*>(a)[i];
        machine_dword_type pluser2 = static_cast<machine_word_type>(~(reinterpret_cast<const machine_word_type*>(b)[i]));
        pluser2 += pluser1 + carry;

        reinterpret_cast<machine_word_type*>(x)[i] = static_cast<machine_word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(machine_word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
    {
        uint16_t pluser1 = a[i];
        uint16_t pluser2 = static_cast<uint8_t>(~(b[i]));
        pluser2 += pluser1 + carry;

        x[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
}

/** 减1 */
inline void decrease(const uint8_t *a, uint8_t *x, size_t N)
{
	assert(NULL != a && NULL != x && N > 0);
	const size_t word_count = N / sizeof(machine_word_type);
	uint8_t carray = 0;
	for (register size_t i = 0; i < word_count; ++i)
	{
		machine_dword_type pluser = reinterpret_cast<const machine_word_type*>(a)[i];
		pluser += carray + ((machine_dword_type) ~ (machine_word_type) 0);

		reinterpret_cast<machine_word_type*>(x)[i] = static_cast<machine_word_type>(pluser);
		carray = static_cast<uint8_t>(pluser >> (sizeof(machine_word_type) * 8));
	}
	for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
	{
		uint16_t pluser = a[i];
		pluser += carray + 0x00FF;

		x[i] = static_cast<uint8_t>(pluser);
		carray = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
	}
}

/** (有符号数)取反 */
inline void signed_opposite(const uint8_t *a, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        machine_dword_type pluser1 = static_cast<machine_word_type>(~(reinterpret_cast<const machine_word_type*>(a)[i]));
        pluser1 += carry;

        reinterpret_cast<machine_word_type*>(x)[i] = static_cast<machine_word_type>(pluser1);
        carry = static_cast<uint8_t>(pluser1 >> (sizeof(machine_word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
    {
        uint16_t pluser1 = static_cast<uint8_t>(~(a[i]));
        pluser1 += carry;

        x[i] = static_cast<uint8_t>(pluser1);
        carry = static_cast<uint8_t>(pluser1 >> (sizeof(uint8_t) * 8));
    }
}

/** 相乘 */
inline void multiply(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    uint8_t *ret = (uint8_t*)::malloc(N);
    ::memset(ret, 0, N);

    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
    {
        machine_word_type carry = 0;
        const machine_dword_type mult1 = reinterpret_cast<const machine_word_type*>(a)[i];
        if (mult1 == 0)
            continue;

        register size_t j = 0;
        for (; i + j < word_count; ++j)
        {
            machine_dword_type mult2 = reinterpret_cast<const machine_word_type*>(b)[j];
            mult2 = mult1 * mult2 + reinterpret_cast<machine_word_type*>(ret)[i + j] + carry;

            reinterpret_cast<machine_word_type*>(ret)[i + j] = static_cast<machine_word_type>(mult2);
            carry = static_cast<machine_word_type>(mult2 >> (sizeof(machine_word_type) * 8));
        }

        size_t bound = i * sizeof(machine_word_type);
        for (register size_t k = j * sizeof(machine_word_type); k + bound < N; ++k)
        {
            machine_dword_type mult2 = b[k];
            mult2 = mult1 * mult2 + ret[k + bound] + carry;

            ret[k + bound] = static_cast<uint8_t>(mult2);
            carry = static_cast<machine_word_type>(mult2 >> (sizeof(uint8_t) * 8));
        }
    }

    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
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

/** 左移 */
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

/** (无符号数)右移 */
inline void unsigned_shift_right(const uint8_t *a, uint8_t *x, size_t N, size_t count)
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

/** (有符号数)右移 */
inline void signed_shift_right(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
	assert(NULL != a && NULL != x && N > 0);
	const int bytes_off = count / 8, bits_off = count % 8;
	const uint8_t fill = (signed_is_positive(a, N) ? 0 : 0xFF);
	for (register size_t i = 0; i < N; ++i)
	{
		const uint8_t high = ((i + bytes_off + 1 >= N ? fill : a[i + bytes_off + 1]) << (8 - bits_off));
		const uint8_t low = ((i + bytes_off >= N ? fill : a[i + bytes_off]) >> bits_off);
		x[i] = high | low;
	}
}

/** 循环左移 */
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

/** 循环右移 */
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
 * 相除
 * @param x
 *      商
 * @param y
 *      余数
 */
inline void signed_divide(const uint8_t *a, const uint8_t *b, uint8_t *x, uint8_t *y, size_t N)
{
	assert(NULL != a && NULL != b && (NULL != x || NULL != y) && N > 0);

	// 常量
	const size_t divided_len = signed_min_size(a, N);
    const size_t divider_len = signed_min_size(b, N);
	const bool divided_positive = signed_is_positive(a, N);
	const bool divider_positive = signed_is_positive(b, N);

	// 逐位试商
	uint8_t *remained = (uint8_t*) ::malloc(sizeof(uint8_t) * (divider_len + 1)); // 余数 remained[1 ~ (divider_len + 1)], remained[0]用于左移补位
	::memset(remained + 1, (divided_positive ? 0 : 0xFF), divider_len); // 初始化余数
	bool remained_positive = divided_positive; // 余数的符号
	for (register size_t i = 0; i < divided_len; ++i)
	{
		remained[0] = a[divided_len - i - 1]; // 余数左移时的低位补位部分
		if (NULL != x)
			x[divided_len - i - 1] = 0; // 初始化商，注意，兼容 x==a 的情况

		for (register size_t j = 0; j < 8; ++j)
		{
			shift_left(remained, remained, divider_len + 1, 1); // 余数左移1位
			if (remained_positive == divider_positive)
				sub(remained + 1, b, remained + 1, divider_len);
			else
				add(remained + 1, b, remained + 1, divider_len);

			remained_positive = signed_is_positive(remained + 1, divider_len);
			if (NULL != x && remained_positive == divider_positive)
				x[divided_len - i - 1] |= (1 << (7 - j));
		}
	}

	if (NULL != x)
	{
		// 修正补数形式的商
		signed_expand(x, divided_len, x, N);
		if (is_zero(remained + 1, divider_len))
		{
			if (!divider_positive)
				increase(x, x, N);
		}
		else
		{
			if (!signed_is_positive(x, N))
				increase(x, x, N);
		}
	}
	if (NULL != y)
		signed_expand(remained + 1, divider_len, y, N);

	::free(remained);
}

/** 比特与 */
inline void bit_and(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<machine_word_type*>(x)[i] = reinterpret_cast<const machine_word_type*>(a)[i] & reinterpret_cast<const machine_word_type*>(b)[i];
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        x[i] = a[i] & b[i];
}

/** 比特或 */
inline void bit_or(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<machine_word_type*>(x)[i] = reinterpret_cast<const machine_word_type*>(a)[i] | reinterpret_cast<const machine_word_type*>(b)[i];
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        x[i] = a[i] | b[i];
}

/** 比特异或 */
inline void bit_xor(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<machine_word_type*>(x)[i] = reinterpret_cast<const machine_word_type*>(a)[i] ^ reinterpret_cast<const machine_word_type*>(b)[i];
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        x[i] = a[i] ^ b[i];
}

/** 比特同或 */
inline void bit_nxor(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<machine_word_type*>(x)[i] = ~(reinterpret_cast<const machine_word_type*>(a)[i] ^ reinterpret_cast<const machine_word_type*>(b)[i]);
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        x[i] = ~(a[i] ^ b[i]);
}

/** 比特否 */
inline void bit_not(const uint8_t *a, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<machine_word_type*>(x)[i] = ~(reinterpret_cast<const machine_word_type*>(a)[i]);
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        x[i] = ~(a[i]);
}


}

#undef machine_hword_type
#undef machine_word_type
#undef machine_dword_type

#endif /* head file guarder */

