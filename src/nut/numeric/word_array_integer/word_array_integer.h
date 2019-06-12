/**
 * 由 '字(word)' 的数组来表示的大整数
 *
 * NOTE
 * - word 之间按照 little-endian 顺序存储, 高位 word 在数组末尾.
 * - 对于带符号大整数, 最高位 word 的最高位 bit 表示整个大整数的符号.
 * - word 内部的字节序与本地字节序相同; 即在 big-endian 机器下是 big-endian, 在
 *   little-endian 机器下是 little-endian
 */

#ifndef ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_
#define ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_

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

#include "../../nut_config.h"
#include "../../platform/int_type.h"
#include "../../platform/endian.h"
#include "shift_op.h"


namespace nut
{

/**
 * 将 word 内部字节序当成是 little-endian 来获取指定位置的字节
 */
template <typename T>
constexpr uint8_t get_byte_le(const T *arr, size_t le_byte_index) noexcept
{
#if NUT_ENDIAN_LITTLE_BYTE
    return reinterpret_cast<const uint8_t*>(arr)[le_byte_index];
#else
    return reinterpret_cast<const uint8_t*>(arr + (le_byte_index / sizeof(T)))
        [sizeof(T) - 1 - (le_byte_index % sizeof(T))];
#endif
}

/**
 * 将 word 内部字节序当成是 little-endian 来设置指定位置的字节
 */
template <typename T>
void set_byte_le(T *arr, size_t le_byte_index, uint8_t v) noexcept
{
#if NUT_ENDIAN_LITTLE_BYTE
    reinterpret_cast<uint8_t*>(arr)[le_byte_index] = v;
#else
    const size_t word_index = le_byte_index / sizeof(T);
    const size_t inner_index = sizeof(T) - 1 - (le_byte_index % sizeof(T));
    reinterpret_cast<uint8_t*>(arr + word_index)[inner_index] = v;
#endif
}

/**
 * 是否为0
 *
 * @return a<N> == 0
 */
template <typename T>
bool is_zero(const T *a, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);

    for (size_t i = 0; i < N; ++i)
    {
        if (0 != a[i])
            return false;
    }
    return true;
}

/**
 * (有符号数)是否为正数或者0
 *
 * @return true, 参数 >= 0
 *      false, 参数 < 0
 */
template <typename T>
constexpr bool is_positive(const T *a, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
#if __cplusplus >= 201402L // C++14 constexpr function feature
    assert(nullptr != a && N > 0);
#endif
    return 0 == (a[N - 1] & (((T) 1) << (8 * sizeof(T) - 1)));
}

/**
 * (有符号数)是否为负数
 *
 * @return true, 参数 < 0
 *      false, 参数 >= 0
 */
template <typename T>
constexpr bool is_negative(const T *a, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    return !is_positive(a, N);
}

/**
 * (有符号数)有效'字'数
 *
 * @return 返回值>=1
 */
template <typename T>
size_t signed_significant_size(const T *a, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);

    const bool positive = is_positive(a, N);
    const T skip_value = (positive ? 0 : ~(T)0);
    size_t ret = N;
    while (ret > 1 && a[ret - 1] == skip_value &&
           is_positive(a, ret - 1) == positive)
        --ret;
    return ret;
}

/**
 * (无符号数)有效字数
 *
 * @return 返回值>=1
 */
template <typename T>
size_t unsigned_significant_size(const T *a, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);

    size_t ret = N;
    while (ret > 1 && a[ret - 1] == 0)
        --ret;
    return ret;
}

/**
 * (有符号数)比较
 *
 * @return <0 if a<M> < b<N>
 *         0 if a<M> == b<N>
 *         >0 if a<M> > b<N>
 */
template <typename T>
int signed_compare(const T *a, size_t M, const T *b, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0);

    const bool positive1 = is_positive(a, M), positive2 = is_positive(b, N);
    if (positive1 != positive2)
        return positive2 ? -1 : 1;

    // 同号比较
    const T fill = (positive1 ? 0 : ~(T)0);
    for (ssize_t i = std::max(M, N) - 1; i >= 0; --i)
    {
        const T op1 = (i < (ssize_t) M ? a[i] : fill);
        const T op2 = (i < (ssize_t) N ? b[i] : fill);
        if (op1 != op2)
            return op1 < op2 ? -1 : 1;
    }
    return 0; // 相等
}

/**
 * (无符号数)比较
 *
 * @return <0 if a<M> < b<N>
 *         0 if a<M> == b<N>
 *         >0 if a<M> > b<N>
 */
template <typename T>
int unsigned_compare(const T *a, size_t M, const T *b, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0);

    for (ssize_t i = std::max(M, N) - 1; i >= 0; --i)
    {
        const T op1 = (i < (ssize_t) M ? a[i] : 0);
        const T op2 = (i < (ssize_t) N ? b[i] : 0);
        if (op1 != op2)
            return op1 < op2 ? -1 : 1;
    }
    return 0; // 相等
}

/**
 * (有符号数)带符号扩展(或者截断)
 * a<M> -> x<N>
 */
template <typename T>
void signed_expand(const T *a, size_t M, T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    const int fill = (is_positive(a, M) ? 0 : 0xff); // NOTE 需要在 memmove() 之前算出来，避免操作数被破坏
    if (x != a)
        ::memmove(x, a, sizeof(T) * std::min(M, N));
    if (M < N)
        ::memset(x + M, fill, sizeof(T) * (N - M));
}

/**
 * (无符号数)无符号扩展(或者截断)
 * a<M> -> x<N>
 */
template <typename T>
void unsigned_expand(const T *a, size_t M, T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    if (x != a)
        ::memmove(x, a, sizeof(T) * std::min(M, N));
    if (M < N)
        ::memset(x + M, 0, sizeof(T) * (N - M));
}

/**
 * (有符号数)相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t signed_add(const T *a, size_t M, const T *b, size_t N, T *x, size_t P) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (T*) ::alloca(sizeof(T) * P);

    uint8_t carry = 0;
    const T filla = (is_positive(a, M) ? 0 : ~(T)0),
        fillb = (is_positive(b, N) ? 0 : ~(T)0);
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? a[i] : filla);
        dword_type pluser2 = (i < N ? b[i] : fillb);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<T>(pluser2);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[0]);
#else
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(T))); // 稍慢
#endif
    }

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * P);
    return carry;
}

/**
 * (无符号数)相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t unsigned_add(const T *a, size_t M, const T *b, size_t N, T *x, size_t P) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (T*) ::alloca(sizeof(T) * P);

    uint8_t carry = 0;
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? a[i] : 0);
        dword_type pluser2 = (i < N ? b[i] : 0);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<T>(pluser2);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[0]);
#else
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(T)));
#endif
    }

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * P);
    return carry;
}

/**
 * 加1
 * x<N> += 1
 *
 * @return 进位
 */
template <typename T>
uint8_t increase(T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != x && N > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    uint8_t carry = 1;
    for (size_t i = 0; i < N && 0 != carry; ++i)
    {
        dword_type pluser = x[i];
        pluser += carry;

        x[i] = static_cast<T>(pluser);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[0]);
#else
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(T)));
#endif
    }
    return carry;
}

/**
 * (有符号数)相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t signed_sub(const T *a, size_t M, const T *b, size_t N, T *x, size_t P) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (T*) ::alloca(sizeof(T) * P);

    const T filla = (is_positive(a, M) ? 0 : ~(T)0),
        fillb = (is_positive(b, N) ? 0 : ~(T)0);
    uint8_t carry = 1;
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? a[i] : filla);
        dword_type pluser2 = static_cast<T>(~(i < N ? b[i] : fillb));
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<T>(pluser2);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[0]);
#else
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(T)));
#endif
    }

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * P);
    return carry;
}

/**
 * (无符号数)相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t unsigned_sub(const T *a, size_t M, const T *b, size_t N, T *x, size_t P) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (T*) ::alloca(sizeof(T) * P);

    uint8_t carry = 1;
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? a[i] : 0);
        dword_type pluser2 = static_cast<T>(~(i < N ? b[i] : 0));
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<T>(pluser2);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[0]);
#else
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(T)));
#endif
    }

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * P);
    return carry;
}

/**
 * 减1
 * x<N> -= 1
 *
 * @return 进位
 */
template <typename T>
uint8_t decrease(T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != x && N > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    uint8_t carry = 0;
    for (size_t i = 0; i < N && 1 != carry; ++i)
    {
        dword_type pluser = x[i];
        pluser += carry + (dword_type)(T) ~(T)0;

        x[i] = static_cast<T>(pluser);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[0]);
#else
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(T)));
#endif
    }
    return carry;
}

/**
 * (有符号数)取相反数
 * x<N> = -a<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t signed_negate(const T *a, size_t M, T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if (a < x && x < a + M)
        retx = (T*) ::alloca(sizeof(T) * N);

    uint8_t carry = 1;
    const T fill = (is_positive(a, M) ? 0 : ~(T)0);
    for (size_t i = 0; i < N; ++i)
    {
        dword_type pluser = static_cast<T>(~(i < M ? a[i] : fill));
        pluser += carry;

        retx[i] = static_cast<T>(pluser);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[0]);
#else
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(T)));
#endif
    }

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, N);
    return carry;
}

/**
 * (无符号数)取相反数
 * x<N> = -a<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t unsigned_negate(const T *a, size_t M, T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if (a < x && x < a + M)
        retx = (T*) ::alloca(sizeof(T) * N);

    uint8_t carry = 1;
    for (size_t i = 0; i < N; ++i)
    {
        dword_type pluser = static_cast<T>(~(i < M ? a[i] : 0));
        pluser += carry;

        retx[i] = static_cast<T>(pluser);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[0]);
#else
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(T)));
#endif
    }

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, N);
    return carry;
}

}

#endif /* head file guarder */
