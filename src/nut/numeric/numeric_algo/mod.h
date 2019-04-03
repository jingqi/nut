
#ifndef ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_
#define ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_

#include "../../nut_config.h"
#include "../big_integer.h"
#include "gcd.h"


namespace nut
{

/**
 * 模乘之前的预计算表计算
 */
template <size_t C>
class ModMultiplyPreBuildTable
{
public:

#if 0 // unoptimized

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n)
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

    ~ModMultiplyPreBuildTable()
    {
        if (nullptr != _table)
        {
            for (size_t i = 0, count = _height * _width; i < count; ++i)
                (_table + i)->~BigInteger();
            ::free(_table);
            _table = nullptr;
        }
    }

    const BigInteger& at(size_t i, size_t j) const
    {
        assert(i < _height && j < _width);
        return _table[i * _width + j];
    }

#else

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n)
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

    ~ModMultiplyPreBuildTable()
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

    const BigInteger& at(size_t i, size_t j) const
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

    size_t get_height() const
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
BigInteger mod_multiply(const BigInteger& b, const BigInteger& n, const ModMultiplyPreBuildTable<C>& table)
{
    assert(b.is_positive() && n.is_positive() && b < n); // 一定要保证 b<n ,以便优化模加运算

    BigInteger s(0);
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
 * 求(a**b)%n，即a的b次方(模n)
 *
 * 参考文献：
 *     [1]潘金贵，顾铁成. 现代计算机常用数据结构和算法[M]. 南京大学出版社. 1994. 576
 */
NUT_API BigInteger mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n);

}

#endif
