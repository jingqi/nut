
#ifndef ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_
#define ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_

#include "../../nut_config.h"
#include "../big_integer.h"
#include "gcd.h"


// 优化程度，>= 0
#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 模乘之前的预计算表计算
 */
template <size_t C>
class ModMultiplyPreBuildTable
{
public:

#if (OPTIMIZE_LEVEL == 0)

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n)
    {
        assert(a.is_positive() && n.is_positive() && a < n); // 一定要保证 a<n ，以便进行模加运算

        hight = (32 * n.significant_words_length() + C - 1) / C;
        width = (1 << C) - 1;

        const size_t count = hight * width;
        table = (BigInteger*) ::malloc(sizeof(BigInteger) * count);
        assert(nullptr != table);

        // 填充第一行
        new (table + 0) BigInteger(a);
        for (size_t i = 1; i < width; ++i)
        {
            new (table + i) BigInteger(table[0] + table[i - 1]);
            if (table[i] >= n)
                table[i] -= n;
        }
        // 填充其他行
        for (size_t i = 1; i < hight; ++i)
        {
            const size_t base_off = i * width;
            new (table + base_off) BigInteger(table[base_off - width] + table[base_off - 1]);
            if (table[base_off] >= n)
                table[base_off] -= n;

            for (size_t j = 1; j < width; ++j)
            {
                new (table + base_off + j) BigInteger(table[base_off] + table[base_off + j - 1]);
                if (table[base_off + j] >= n)
                    table[base_off + j] -= n;
            }
        }
    }

    ~ModMultiplyPreBuildTable()
    {
        if (nullptr != table)
        {
            for (size_t i = 0, count = hight * width; i < count; ++i)
                (table + i)->~BigInteger();
            ::free(table);
            table = nullptr;
        }
    }

    const BigInteger& at(size_t i, size_t j) const
    {
        assert(i < hight && j < width);
        return table[i * width + j];
    }

#else

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n)
    {
        assert(a.is_positive() && n.is_positive() && a < n); // 一定要保证 a<n ，以便进行模加运算

        hight = (n.bit_length() + C - 1) / C;
        width = (1 << C) - 1;

        const size_t count = hight * width;
        table = (BigInteger**) ::malloc(sizeof(BigInteger*) * count);
        assert(nullptr != table);
        ::memset(table, 0, sizeof(BigInteger*) * count);

        table[0] = (BigInteger*) ::malloc(sizeof(BigInteger));
        new (table[0]) BigInteger(a);
        mod = n;
    }

    ~ModMultiplyPreBuildTable()
    {
        if (nullptr != table)
        {
            for (size_t i = 0; i < hight; ++i)
            {
                for (size_t j = 0; j < width; ++j)
                {
                    if (nullptr != table[i * width + j])
                    {
                        table[i * width + j]->~BigInteger();
                        ::free(table[i * width + j]);
                        table[i * width + j] = nullptr;
                    }
                }
            }
            ::free(table);
            table = nullptr;
        }
    }

    const BigInteger& at(size_t i, size_t j) const
    {
        assert(i < hight && j < width);

        const size_t base_off = i * width;
        if (nullptr == table[base_off + j])
        {
            if (j == 0)
            {
                table[base_off] = (BigInteger*) ::malloc(sizeof(BigInteger));
                new (table[base_off]) BigInteger(at(i - 1, 0) + at(i - 1, width - 1));
            }
            else
            {
                table[base_off + j] = (BigInteger*) ::malloc(sizeof(BigInteger));
                new (table[base_off + j]) BigInteger(at(i, 0) + at(i, j - 1));
            }

            if (*table[base_off + j] >= mod) // 模加
                *table[base_off + j] -= mod;
        }
        return *table[base_off + j];
    }

#endif

private:
    ModMultiplyPreBuildTable(const ModMultiplyPreBuildTable<C>&);

public:
#if (OPTIMIZE_LEVEL == 0)
    size_t hight = 0, width = 0;
    BigInteger *table = nullptr;
#else
    size_t hight = 0, width = 0;
    BigInteger **table = nullptr;
    BigInteger mod;
#endif
};

/**
* 预算表法求乘模
*
* 参考文献：
*     [1]周玉洁，冯国登. 公开密钥密码算法及其快速实现[M]. 国防工业出版社. 2002. 57
 */
template <size_t C>
void mod_multiply(const BigInteger& b, const BigInteger& n, const ModMultiplyPreBuildTable<C>& table, BigInteger *rs)
{
    assert(nullptr != rs);
    assert(b.is_positive() && n.is_positive() && b < n); // 一定要保证 b<n ,以便优化模加运算

    BigInteger s(0);
    size_t limit = (b.bit_length() + C - 1) / C;
    if (table.hight < limit)
        limit = table.hight;
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

    *rs = std::move(s);
}

/**
 * 求(a**b)%n，即a的b次方(模n)
 *
 * 参考文献：
 *     [1]潘金贵，顾铁成. 现代计算机常用数据结构和算法[M]. 南京大学出版社. 1994. 576
 */
NUT_API void mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n, BigInteger *rs);

}

#undef OPTIMIZE_LEVEL

#endif
