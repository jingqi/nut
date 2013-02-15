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
            if (*table[base_off + j] >= mod) // 模加
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
        uint32_t j = 0; // bits window
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
 * {t + [(t mod r) * n' mod r] * n} / r
 */
inline BigInteger _montgomery(const BigInteger& t, size_t rlen, const BigInteger& n, const BigInteger& nn)
{
    assert(t.is_positive() && rlen > 0 && n.is_positive() && nn.is_positive());

    // 计算 t % r
    size_t min_sig = (rlen + 8 * sizeof(BigInteger::word_type) - 1) / (8 * sizeof(BigInteger::word_type));
    if (t.significant_words_length() < min_sig)
        min_sig = t.significant_words_length();
    BigInteger rs(t.buffer(), min_sig, true);
    rs.limit_positive_bits_to(rlen);

    rs.multiply_to_len(nn, rlen); // rs = (rs * nn) % r
    rs *= n;
    rs += t;
    rs >>= rlen;

    if (rs >= n)
        rs -= n;
    return rs;
}

/**
 * 变形的蒙哥马利算法
 *
 * 算法来源:
 *      王金荣，周赟，王红霞. Montgomery模平方算法及其应用[J]. 计算机工程，2007，33(24)：155 - 156
 */
inline BigInteger _montgomery(const BigInteger& t, const BigInteger& n, BigInteger::word_type nn)
{
    assert(t.is_positive() && n.is_positive() && nn > 0);

    const size_t r_word_count = n.significant_words_length();
    BigInteger rs(t);
    rs.resize(r_word_count * 2 + 1);
    for (register size_t i = 0; i < r_word_count; ++i)
    {
        const BigInteger::word_type op1 = static_cast<BigInteger::word_type>(rs.word_at(i) * nn);
        if (0 == op1)
            continue;

        BigInteger::word_type carry = 0;
        for (register size_t j = 0; j < r_word_count; ++j)
        {
            BigInteger::dword_type op2 = n.word_at(j);
            op2 *= op1;
            op2 += rs.word_at(i + j);
            op2 += carry;

            rs.buffer()[i + j] = static_cast<BigInteger::word_type>(op2);
            carry = static_cast<BigInteger::word_type>(op2 >> (8 * sizeof(BigInteger::word_type)));
        }

        for (register size_t j = i; j < r_word_count; ++j)
        {
            if (0 == carry)
                break;

            BigInteger::dword_type op = rs.word_at(j + r_word_count);
            op += carry;

            rs.buffer()[j + r_word_count] = static_cast<BigInteger::word_type>(op);
            carry = static_cast<BigInteger::word_type>(op >> (8 * sizeof(BigInteger::word_type)));
        }
    }

    rs >>= 8 * sizeof(BigInteger::word_type) * r_word_count;

    while (rs >= n)
        rs -= n;
    return rs;
}

/**
 * 已知:
 *      n为奇数, r=2**p，(n,r互质)
 * 求 rr, nn 使得:
 *      r * rr - n * nn = 1 (rr为r模n的逆元，nn为n模r的负逆元)
 *
 * 算法来源：
 *      雷明，叶新，张焕国. Montgomery算法及其快速实现[J]. 计算机工程，2003，29(14)：46
 *
 * @return rr, nn 都为正数
 */
inline void _mont_extended_euclid(size_t rlen, const BigInteger& n, BigInteger *_rr, BigInteger *_nn)
{
    BigInteger r(1);
    r <<= rlen;

    /**
     * 构建恒等式并计算：
     * (1) r * rr - n * nn = s
     * (2) r * n - n * r = 0
     * ->等式(1)用于变形，等式(2)用于加到等式(1)上以帮助其变形
     * ->初始时 rr=n+1, nn=r, s=r
     */
    BigInteger rr(n), nn(r);
    ++rr;
    size_t slen = rlen;
    while (slen > 0)
    {
        const int lb = rr.lowest_bit();
        assert(lb >= 0);
        if (lb > 0)
        {
            const int min_shift = (lb < (int) slen ? lb : (int) slen);
            rr >>= min_shift;
            nn >>= min_shift;
            slen -= min_shift;
        }
        else
        {
            rr += n;
            nn += r;
        }
    }

    if (NULL != _rr)
        *_rr = rr;
    if (NULL != _nn)
        *_nn = nn;
}

/**
 * 使用 Montgomery 算法优化
 */
inline BigInteger _odd_mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n)
{
    assert(a.is_positive() && b.is_positive() && n.is_positive());
    assert(a < n && n.bit_at(0) == 1);

#if (OPTIMIZE_LEVEL == 0)
    // 预运算
    const size_t rlen = n.bit_length();
    BigInteger r(1), nn;
    r <<= rlen;
    extended_euclid(r, n, NULL, NULL, &nn);
    if (nn.is_positive())
        nn = r - (nn % r);
    else
        nn = (-nn) % r;

    // 循环计算
    const BigInteger m = (a << rlen) % n;
    BigInteger ret(m);
    for (register size_t i = b.bit_length() - 1; i > 0; --i)
    {
        ret = _montgomery(ret * ret, rlen, n, nn);
        if (0 != b.bit_at(i - 1))
            ret = _montgomery(ret * m, rlen, n, nn);
    }

    // 处理返回值
    return _montgomery(ret, rlen, n, nn);
#elif (OPTIMIZE_LEVEL == 1)
    /**
     * 变形的蒙哥马利算法
     *      在这里其效率低于原始的蒙哥马利算法, 可能是非等长的大整数运算效率高于定长大整数运算的缘故
     */

    // 预运算
    const size_t r_word_count = n.significant_words_length();
    BigInteger nn;
    _mont_extended_euclid(8 * sizeof(BigInteger::word_type), BigInteger(n.word_at(0)), NULL, &nn);
    BigInteger::word_type nnn = nn.word_at(0);

    // 循环计算
    const BigInteger m = (a << (8 * sizeof(BigInteger::word_type) * r_word_count)) % n;
    BigInteger ret(m);
    for (register int i = ((int) b.bit_length()) - 2; i >= 0; --i)
    {
        ret *= ret;
        ret = _montgomery(ret, n, nnn);
        if (0 != b.bit_at(i))
        {
            ret *= m;
            ret = _montgomery(ret, n, nnn);
        }
    }

    // 处理返回值
    return _montgomery(ret, n, nnn);
#else
    // 预运算
    const size_t rlen = n.bit_length();
    BigInteger nn;
    _mont_extended_euclid(rlen, n, NULL, &nn);
    nn.limit_positive_bits_to(rlen);

    // 循环计算
    const BigInteger m = (a << rlen) % n;
    BigInteger ret(m);
    for (register int i = ((int) b.bit_length()) - 2; i >= 0; --i)
    {
        ret *= ret;
        ret = _montgomery(ret, rlen, n, nn);
        if (0 != b.bit_at(i))
        {
            ret *= m;
            ret = _montgomery(ret, rlen, n, nn);
        }
    }

    // 处理返回值
    return _montgomery(ret, rlen, n, nn);
#endif
}

/**
 * 计算 (a ** b) mod (2 ** p)
 */
inline BigInteger _mod_pow_2(const BigInteger& a, const BigInteger& b, size_t p)
{
    assert(a.is_positive() && b.is_positive() && p > 0);

    BigInteger ret(1);
    for (register size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
    {
        ret.multiply_to_len(ret, p);

        if (0 != b.bit_at(i - 1))
            ret.multiply_to_len(a, p);
    }
    return ret;
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
    // 模是奇数，应用蒙哥马利算法
    if (n.bit_at(0) == 1)
        return _odd_mod_pow(a < n ? a : a % n, b, n);

    // 模是偶数，应用中国余数定理
    const size_t p = n.lowest_bit();
    BigInteger n1(n), n2(1);
    n1 >>= p;
    n2 <<= p;

    BigInteger a1 = (n1 == 1 ? BigInteger(0) : _odd_mod_pow(a % n1, b, n1));
    BigInteger a2 = _mod_pow_2(a < n ? a : a % n, b, p);

    BigInteger y1, y2;
    extended_euclid(n2, n1, NULL, &y1, NULL);
    if (y1 < 0)
        y1 = n1 + (y1 % n1);
    extended_euclid(n1, n2, NULL, &y2, NULL);
    if (y2 < 0)
        y2 = n2 + (y2 % n2);

    return (a1 * n2 * y1 + a2 * n1 * y2) % n;
#endif
}


}

#undef OPTIMIZE_LEVEL

#endif


