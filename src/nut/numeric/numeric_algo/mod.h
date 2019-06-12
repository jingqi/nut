
#ifndef ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_
#define ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_

#include <type_traits>

#include "../../nut_config.h"
#include "../../platform/int_type.h"
#include "../big_integer.h"
#include "../word_array_integer/mul_op.h"
#include "../word_array_integer/bit_op.h"
#include "gcd.h"


namespace nut
{

/**
 * 计算模乘 a * b % n
 *
 * NOTE 注意避免中间过程溢出
 *
 * @return a * b % n
 */
template <typename T>
constexpr T mul_mod(T a, T b, T n) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    typedef typename StdInt<T>::double_unsigned_type dword_type;
    return ((dword_type) a) * b % n;
}

#if NUT_HAS_INT128
template <>
NUT_API uint128_t mul_mod(uint128_t a, uint128_t b, uint128_t n) noexcept;
#else
template <>
NUT_API uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t n) noexcept;
#endif


/**
 * 模乘之前的预计算表计算
 */
template <size_t C>
class ModMultiplyPreBuildTable
{
public:

#if 0 // unoptimized

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n) noexcept
    {
        assert(a.is_positive() && n.is_positive() && a < n); // 一定要保证 a<n ，以便进行模加运算

        _height = (32 * n.significant_words_length() + C - 1) / C;
        _width = (1 << C) - 1;

        const size_t count = _height * _width;
        _table = (BigInteger*) ::malloc(sizeof(BigInteger) * count);
        assert(nullptr != _table);

        // 填充第一行
        new (_table + 0) BigInteger(a);
        for (size_t i = 1; i < _width; ++i)
        {
            new (_table + i) BigInteger(_table[0] + _table[i - 1]);
            if (_table[i] >= n)
                _table[i] -= n;
        }
        // 填充其他行
        for (size_t i = 1; i < _height; ++i)
        {
            const size_t base_off = i * _width;
            new (_table + base_off) BigInteger(_table[base_off - _width] + _table[base_off - 1]);
            if (_table[base_off] >= n)
                _table[base_off] -= n;

            for (size_t j = 1; j < _width; ++j)
            {
                new (_table + base_off + j) BigInteger(_table[base_off] + _table[base_off + j - 1]);
                if (_table[base_off + j] >= n)
                    _table[base_off + j] -= n;
            }
        }
    }

    ~ModMultiplyPreBuildTable() noexcept
    {
        if (nullptr != _table)
        {
            for (size_t i = 0, count = _height * _width; i < count; ++i)
                (_table + i)->~BigInteger();
            ::free(_table);
            _table = nullptr;
        }
    }

    const BigInteger& at(size_t i, size_t j) const noexcept
    {
        assert(i < _height && j < _width);
        return _table[i * _width + j];
    }

#else

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n) noexcept
    {
        assert(a.is_positive() && n.is_positive() && a < n); // 一定要保证 a<n ，以便进行模加运算

        _height = (n.bit_length() + C - 1) / C;
        _width = (1 << C) - 1;

        const size_t count = _height * _width;
        _table = (BigInteger**) ::malloc(sizeof(BigInteger*) * count);
        assert(nullptr != _table);
        ::memset(_table, 0, sizeof(BigInteger*) * count);

        _table[0] = (BigInteger*) ::malloc(sizeof(BigInteger));
        new (_table[0]) BigInteger(a);
        _mod = n;
    }

    ~ModMultiplyPreBuildTable() noexcept
    {
        if (nullptr != _table)
        {
            for (size_t i = 0; i < _height; ++i)
            {
                for (size_t j = 0; j < _width; ++j)
                {
                    if (nullptr != _table[i * _width + j])
                    {
                        _table[i * _width + j]->~BigInteger();
                        ::free(_table[i * _width + j]);
                        _table[i * _width + j] = nullptr;
                    }
                }
            }
            ::free(_table);
            _table = nullptr;
        }
    }

    const BigInteger& at(size_t i, size_t j) const noexcept
    {
        assert(i < _height && j < _width);

        const size_t base_off = i * _width;
        if (nullptr == _table[base_off + j])
        {
            if (j == 0)
            {
                _table[base_off] = (BigInteger*) ::malloc(sizeof(BigInteger));
                new (_table[base_off]) BigInteger(at(i - 1, 0) + at(i - 1, _width - 1));
            }
            else
            {
                _table[base_off + j] = (BigInteger*) ::malloc(sizeof(BigInteger));
                new (_table[base_off + j]) BigInteger(at(i, 0) + at(i, j - 1));
            }

            if (*_table[base_off + j] >= _mod) // 模加
                *_table[base_off + j] -= _mod;
        }
        return *_table[base_off + j];
    }

#endif

    size_t get_height() const noexcept
    {
        return _height;
    }

private:
    ModMultiplyPreBuildTable(const ModMultiplyPreBuildTable<C>&) = delete;
    ModMultiplyPreBuildTable& operator=(const ModMultiplyPreBuildTable&) = delete;

private:
#if 0 // unoptimized
    size_t _height = 0, _width = 0;
    BigInteger *_table = nullptr;
#else
    size_t _height = 0, _width = 0;
    BigInteger **_table = nullptr;
    BigInteger _mod;
#endif
};

/**
* 预算表法求乘模
*
* 参考文献：
*     [1]周玉洁，冯国登. 公开密钥密码算法及其快速实现[M]. 国防工业出版社. 2002. 57
 */
template <size_t C>
BigInteger mul_mod(const BigInteger& b, const BigInteger& n, const ModMultiplyPreBuildTable<C>& table) noexcept
{
    assert(b.is_positive() && n.is_positive() && b < n); // 一定要保证 b<n ,以便优化模加运算

    BigInteger s; // s = 0
    size_t limit = (b.bit_length() + C - 1) / C;
    if (table.get_height() < limit)
        limit = table.get_height();
    for (size_t i = 0; i < limit; ++i)
    {
        uint32_t j = 0; // bits window
        for (size_t k = 1; k <= C; ++k)
        {
            j <<= 1;
            j |= b.bit_at(i * C + (C - k));
        }
        if (j > 0)
        {
            s += table.at(i, j - 1);
            if (s >= n)
                s -= n;
        }
    }

    return s;
}

/**
 * 计算模幂 (a ** b) % n，即a的b次方(mod n)
 *
 * NOTE 注意避免中间过程溢出
 *
 * 参考文献：
 *     [1]潘金贵，顾铁成. 现代计算机常用数据结构和算法[M]. 南京大学出版社. 1994. 576
 */
template <typename T>
T pow_mod(T a, T b, T n) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");

    if (0 == b || 1 == a)
        return 1 == n ? 0 : 1;
    else if (0 == a)
        return 0;

    T ret = 1;
    for (int i = highest_bit1(b); i >= 0; --i) // 从高位向低有效位取bit
    {
        ret = mul_mod(ret, ret, n);
        if (0 != ((b >> i) & 1))
            ret = mul_mod(ret, a, n);
    }
    return ret;
}

NUT_API BigInteger pow_mod(const BigInteger& a, const BigInteger& b, const BigInteger& n) noexcept;

/**
 * 判定 r 是否是素数 n 的原根
 *
 * NOTE 这个操作非常耗时, 只应该在线下使用
 */
template <typename T>
bool is_ordm(T r, T n) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(0 < r && r < n && n >= 2);

    // NOTE 具体算法参考 http://blog.leanote.com/post/rockdu/0330
    if (1 == r && 2 != n)
        return false;
    const T nm1 = n - 1;
    for (T i = 2; i * i < nm1; ++i)
    {
        if (0 != nm1 % i)
            continue;
        if (1 == pow_mod(r, i, n) || 1 == pow_mod(r, nm1 / i, n))
            return false;
    }
    return true;
}

/**
 * 找原根
 *
 * NOTE 这个操作非常耗时, 只应该在线下使用
 */
template <typename T>
T find_ordm(T n) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");

    for (T i = 2; i < n; ++i)
    {
        if (is_ordm(i, n))
            return i;
    }
    return 0;
}

}

#endif
