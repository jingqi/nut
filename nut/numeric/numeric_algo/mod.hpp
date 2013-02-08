/**
 * @file -
 * @author jingqi
 * @date 2012-11-25
 * @last-edit 2013-02-08 16:28:16 jingqi
 */

#ifndef ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_
#define ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_

#include "../biginteger.hpp"
#include "gcd.hpp"

// 优化程度，>= 0
#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 模乘之前的预计算表计算
 *
 * @return 返回的指针需要delete[] 掉
 */
#if (OPTIMIZE_LEVEL == 0)
template <size_t C>
struct ModMultiplyPreBuildTable
{
    size_t hight, width;
    BigInteger *table;

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n)
        : hight(0), width(0), table(NULL)
    {
        assert(a.is_positive() && n.is_positive() && a < n); // 一定要保证 a<n ，以便进行模加运算

        hight = (32 * n.significant_words_length() + C - 1) / C;
        width = (1 << C) - 1;
        table = new BigInteger[hight * width];
        assert(NULL != table);

        // 填充第一行
        table[0] = a;
        for (register size_t i = 1; i < width; ++i)
        {
            table[i] = table[0] + table[i - 1];
            if (table[i] >= n)
                table[i] -= n;
        }
        // 填充其他行
        for (register size_t i = 1; i < hight; ++i)
        {
            const size_t base_off = i * width;
            table[base_off] = table[base_off - width] + table[base_off - 1];
            if (table[base_off] >= n)
                table[base_off] -= n;

            for (register size_t j = 1; j < width; ++j)
            {
                table[base_off + j] = table[base_off] + table[base_off + j - 1];
                if (table[base_off + j] >= n)
                    table[base_off + j] -= n;
            }
        }
    }

    ~ModMultiplyPreBuildTable()
    {
        if (NULL != table)
            delete[] table;
        table = NULL;
    }

    const BigInteger& at(size_t i, size_t j) const
    {
        assert(i < hight && j < width);
        return table[i * width + j];
    }

private:
    ModMultiplyPreBuildTable(const ModMultiplyPreBuildTable<C>& x);
};
#else
template <size_t C>
struct ModMultiplyPreBuildTable
{
    size_t hight, width;
    BigInteger **table;
    BigInteger mod;

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n)
        : hight(0), width(0), table(NULL)
    {
        assert(a.is_positive() && n.is_positive() && a < n); // 一定要保证 a<n ，以便进行模加运算

        hight = (n.bit_length() + C - 1) / C;
        width = (1 << C) - 1;
        table = new BigInteger*[hight * width];
        assert(NULL != table);
        ::memset(table, 0, sizeof(BigInteger*) * hight * width);
        table[0] = new BigInteger(a);
        mod = n;
    }

    ~ModMultiplyPreBuildTable()
    {
        if (NULL != table)
        {
            for (register size_t i = 0; i < hight; ++i)
                for (register size_t j = 0; j < width; ++j)
                    if (NULL != table[i * width + j])
                        delete table[i * width + j];
            delete[] table;
        }
        table = NULL;
    }

    const BigInteger& at(size_t i, size_t j) const
    {
        assert(i < hight && j < width);

        const size_t base_off = i * width;
        if (NULL == table[base_off + j])
        {
            if (j == 0)
                table[base_off] = new BigInteger(at(i - 1, 0) + at(i - 1, width - 1));
            else
                table[base_off + j] = new BigInteger(at(i, 0) + at(i, j - 1));
            if (*table[base_off + j] >= mod)
                *table[base_off + j] -= mod;
        }
        return *table[base_off + j];
    }

private:
    ModMultiplyPreBuildTable(const ModMultiplyPreBuildTable<C>& x);
};
#endif

/**
* 预算表法求乘模
* 参见 《公开密钥密码算法及其快速实现》.周玉洁.冯国登.国防工业出版社.2002 P57
 */
template <size_t C>
BigInteger mod_multiply(const BigInteger& b, const BigInteger& n, const ModMultiplyPreBuildTable<C>& table)
{
    assert(b.is_positive() && n.is_positive() && b < n); // 一定要保证 b<n ,以便优化模加运算

    BigInteger s(0);
    size_t limit = (b.bit_length() + C - 1) / C;
    if (table.hight < limit)
        limit = table.hight;
    for (register size_t i = 0; i < limit; ++i)
    {
        uint32_t j = 0; // window
        for (register size_t k = 1; k <= C; ++k)
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
 * 蒙哥马利算法
 */
inline BigInteger _montgomery(const BigInteger& t, size_t rlen, const BigInteger& n, const BigInteger& nn)
{
    NUT_STATIC_ASSERT(sizeof(BigInteger::word_type) == 4);

    // return (t + (((t % r) * nn) % r) * n) / r;
    BigInteger rs(t);
    rs.resize_bits_positive(rlen);
    assert(rs == t % (BigInteger(1) << rlen));
    rs *= nn;
    rs.resize_bits_positive(rlen);
    rs *= n;
    rs += t;
    rs >>= rlen;

    if (rs >= n)
        rs -= n;
    return rs;
}

/**
 * 使用 Montgomery 算法优化
 */
inline BigInteger _odd_mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n)
{
    assert(a.is_positive() && b.is_positive() && n.is_positive());
    assert(n.bit_at(0) == 1);

    const BigInteger aa(a % n);

    // 预运算
    const size_t rlen = n.bit_length();
    BigInteger r(1), nn;
    r <<= rlen;
    extended_euclid(r, n, NULL, NULL, &nn);
    if (nn.is_positive())
        nn = r - (nn % r);
    else
        nn = (-nn) % r;
    assert((nn * n) % r == r - 1);

    const BigInteger m = (aa << rlen) % n;
    BigInteger ret(m);
    for (register size_t i = b.bit_length() - 1; i > 0; --i)
    {
        ret = _montgomery(ret * ret, rlen, n, nn);
        if (0 != b.bit_at(i - 1))
            ret = _montgomery(ret * m, rlen, n, nn);
    }
    return _montgomery(ret, rlen, n, nn);
}

/**
 * 求(a**b)%n，即a的b次方(模n)
 * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P576
 */
inline BigInteger mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n)
{
    assert(a.is_positive() && b.is_positive() && n.is_positive());

    if (b.is_zero())
        return BigInteger(n == 1 ? 0 : 1);
    else if (a == 1)
        return BigInteger(n == 1 ? 0 : 1);
    else if (a.is_zero())
        return BigInteger(0);

#if (OPTIMIZE_LEVEL == 0)
    BigInteger ret(1);
    for (register size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
    {
        ret = (ret * ret) % n;
        if (0 != b.bit_at(i - 1))
            ret = (ret * a) % n;
    }
    return ret;
#elif (OPTIMIZE_LEVEL == 1)
    const size_t bbc = b.bit_count();
     if (bbc > 400) // 400 是一个经验数据
    {
        ModMultiplyPreBuildTable<4> table(a % n, n); /// 经测试，预算表模板参数取4比较合适
        BigInteger ret(1);
        for (register size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
        {
            ret = (ret * ret) % n;
            if (0 != b.bit_at(i - 1))
                ret = mod_multiply(ret, n, table);
        }
        return ret;
    }
    else
    {
        BigInteger ret(1);
        for (register size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
        {
            ret = (ret * ret) % n;
            if (0 != b.bit_at(i - 1))
                ret = (ret * a) % n;
        }
        return ret;
    }
#else
    if (n.bit_at(0) == 1)
        return _odd_mod_pow(a, b, n);

    BigInteger ret(1);
    for (register size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
    {
        ret = (ret * ret) % n;
        if (0 != b.bit_at(i - 1))
            ret = (ret * a) % n;
    }
    return ret;
#endif
}


}

#undef OPTIMIZE_LEVEL

#endif


