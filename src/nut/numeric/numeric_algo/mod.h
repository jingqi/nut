
#ifndef ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_
#define ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_

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
struct ModMultiplyPreBuildTable
{
    const rc_ptr<memory_allocator> alloc;

#if (OPTIMIZE_LEVEL == 0)
    size_t hight, width;
    BigInteger *table;

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n, memory_allocator *ma = NULL)
        : alloc(ma), hight(0), width(0), table(NULL)
    {
        assert(a.is_positive() && n.is_positive() && a < n); // 一定要保证 a<n ，以便进行模加运算

        hight = (32 * n.significant_words_length() + C - 1) / C;
        width = (1 << C) - 1;

        const size_t count = hight * width;
        table = (BigInteger*) ma_realloc(alloc.pointer(), NULL, sizeof(BigInteger) * count);
        assert(NULL != table);

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
        if (NULL != table)
        {
            for (size_t i = 0, count = hight * width; i < count; ++i)
                (table + i)->~BigInteger();
            ma_free(alloc.pointer(), table);
            table = NULL;
        }
    }

    const BigInteger& at(size_t i, size_t j) const
    {
        assert(i < hight && j < width);
        return table[i * width + j];
    }
#else
    size_t hight, width;
    BigInteger **table;
    BigInteger mod;

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n)
        : alloc(a.allocator()), hight(0), width(0), table(NULL)
    {
        assert(a.is_positive() && n.is_positive() && a < n); // 一定要保证 a<n ，以便进行模加运算

        hight = (n.bit_length() + C - 1) / C;
        width = (1 << C) - 1;

        const size_t count = hight * width;
        table = (BigInteger**) ma_realloc(alloc.pointer(), NULL, sizeof(BigInteger*) * count);
        assert(NULL != table);
        ::memset(table, 0, sizeof(BigInteger*) * count);

        table[0] = ma_new<BigInteger>(alloc.pointer(), a);
        mod = n;
    }

    ~ModMultiplyPreBuildTable()
    {
        if (NULL != table)
        {
            for (size_t i = 0; i < hight; ++i)
            {
                for (size_t j = 0; j < width; ++j)
                {
                    if (NULL != table[i * width + j])
                    {
                        ma_delete(alloc.pointer(), table[i * width + j]);
                        table[i * width + j] = NULL;
                    }
                }
            }
            ma_free(alloc.pointer(), table);
            table = NULL;
        }
    }

    const BigInteger& at(size_t i, size_t j) const
    {
        assert(i < hight && j < width);

        const size_t base_off = i * width;
        if (NULL == table[base_off + j])
        {
            if (j == 0)
            {
                table[base_off] = (BigInteger*) ma_realloc(alloc.pointer(), NULL, sizeof(BigInteger));
                new (table[base_off]) BigInteger(at(i - 1, 0) + at(i - 1, width - 1));
            }
            else
            {
                table[base_off + j] = (BigInteger*) ma_realloc(alloc.pointer(), NULL, sizeof(BigInteger));
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
};

/**
* 预算表法求乘模
* 参见 《公开密钥密码算法及其快速实现》.周玉洁.冯国登.国防工业出版社.2002 P57
 */
template <size_t C>
void mod_multiply(const BigInteger& b, const BigInteger& n, const ModMultiplyPreBuildTable<C>& table, BigInteger *rs)
{
    assert(NULL != rs);
    assert(b.is_positive() && n.is_positive() && b < n); // 一定要保证 b<n ,以便优化模加运算

    BigInteger s(0, b.allocator());
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

    *rs = s;
}

/**
 * 求(a**b)%n，即a的b次方(模n)
 * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P576
 */
void mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n, BigInteger *rs);

}

#undef OPTIMIZE_LEVEL

#endif
