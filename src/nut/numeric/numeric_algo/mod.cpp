
#include "mod.h"
#include "gcd.h"

// 优化程度，>= 0
#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 蒙哥马利算法
 * {t + [(t mod r) * n' mod r] * n} / r
 */
static void _montgomery(const BigInteger& t, size_t rlen, const BigInteger& n, const BigInteger& nn, BigInteger *rs)
{
    assert(NULL != rs);
    assert(t.is_positive() && rlen > 0 && n.is_positive() && nn.is_positive());
    typedef BigInteger::word_type word_type;

    // 计算 t % r
    size_t min_sig = (rlen + 8 * sizeof(word_type) - 1) / (8 * sizeof(word_type));
    if (t.significant_words_length() < min_sig)
        min_sig = t.significant_words_length();
    BigInteger s(t.data(), min_sig, true, rs->allocator());
    s.limit_positive_bits_to(rlen);

    s.multiply_to_len(nn, rlen); // rs = (rs * nn) % r
    s *= n;
    s += t;
    s >>= rlen;

    if (s >= n)
        s -= n;

    *rs = std::move(s);
}

#if OPTIMIZE_LEVEL == 1
/**
 * 变形的蒙哥马利算法
 *
 * 算法来源:
 *      王金荣，周赟，王红霞. Montgomery模平方算法及其应用[J]. 计算机工程，2007，33(24)：155 - 156
 */
static void _montgomery2(const BigInteger& t, const BigInteger& n, BigInteger::word_type nn, BigInteger *rs)
{
    assert(NULL != rs);
    assert(t.is_positive() && n.is_positive() && nn > 0);
    typedef BigInteger::word_type word_type;
    typedef BigInteger::dword_type dword_type;

    const size_t r_word_count = n.significant_words_length();
    BigInteger ret(t);
    ret.resize(r_word_count * 2 + 1);
    for (size_t i = 0; i < r_word_count; ++i)
    {
        const word_type op1 = static_cast<word_type>(ret.word_at(i) * nn);
        if (0 == op1)
            continue;

        word_type carry = 0;
        for (size_t j = 0; j < r_word_count; ++j)
        {
            dword_type op2 = n.word_at(j);
            op2 *= op1;
            op2 += ret.word_at(i + j);
            op2 += carry;

            ret.data()[i + j] = static_cast<word_type>(op2);
            carry = static_cast<word_type>(op2 >> (8 * sizeof(word_type)));
        }

        for (size_t j = i; j < r_word_count; ++j)
        {
            if (0 == carry)
                break;

            dword_type op = ret.word_at(j + r_word_count);
            op += carry;

            ret.data()[j + r_word_count] = static_cast<word_type>(op);
            carry = static_cast<word_type>(op >> (8 * sizeof(word_type)));
        }
    }

    ret >>= 8 * sizeof(word_type) * r_word_count;

    while (ret >= n)
        ret -= n;

    *rs = std::move(ret);
}
#endif

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
static void _mont_extended_euclid(size_t rlen, const BigInteger& n, BigInteger *rr, BigInteger *nn)
{
    assert(NULL != rr || NULL != nn);

    BigInteger r(1, n.allocator());
    r <<= rlen;

    /**
     * 构建恒等式并计算：
     * (1) r * rr - n * nn = s
     * (2) r * n - n * r = 0
     * ->等式(1)用于变形，等式(2)用于加到等式(1)上以帮助其变形
     * ->初始时 rr = n + 1, nn = r, s = r
     */
    BigInteger ret_rr(n), ret_nn(r);
    ++ret_rr;
    size_t slen = rlen;
    while (slen > 0)
    {
        const int lb = ret_rr.lowest_bit();
        assert(lb >= 0);
        if (lb > 0)
        {
            const int min_shift = (lb < (int) slen ? lb : (int) slen);
            ret_rr >>= min_shift;
            ret_nn >>= min_shift;
            slen -= min_shift;
        }
        else
        {
            ret_rr += n;
            ret_nn += r;
        }
    }

    if (NULL != rr)
        *rr = std::move(ret_rr);
    if (NULL != nn)
        *nn = std::move(ret_nn);
}

/**
 * 滑动窗口算法、蒙哥马利算法的预算表
 */
struct MontgomeryPreBuildTable
{
    const rc_ptr<memory_allocator> alloc;
    BigInteger **table;
    size_t size;

    MontgomeryPreBuildTable(size_t wnd_sz, const BigInteger& m, size_t rlen,
            const BigInteger& n, const BigInteger& nn, memory_allocator *ma = NULL)
        : alloc(ma), table(NULL), size(0)
    {
        assert(0 < wnd_sz && wnd_sz < 16);

        size = 1 << (wnd_sz - 1);
        table = (BigInteger**) ma_realloc(alloc.pointer(), NULL, sizeof(BigInteger*) * size);
        assert(NULL != table);
        ::memset(table, 0, sizeof(BigInteger*) * size);

        table[0] = (BigInteger*) ma_realloc(alloc.pointer(), NULL, sizeof(BigInteger));
        new (table[0]) BigInteger(m);
        BigInteger mm(0, m.allocator());
        _montgomery(m * m, rlen, n, nn, &mm);
        for (size_t i = 1; i < size; ++i)
        {
            table[i] = (BigInteger*) ma_realloc(alloc.pointer(), NULL, sizeof(BigInteger));
            new (table[i]) BigInteger(0, m.allocator());
            _montgomery(*table[i - 1] * mm, rlen, n, nn, table[i]);
        }
    }

    ~MontgomeryPreBuildTable()
    {
        if (NULL != table)
        {
            for (size_t i = 0; i < size; ++i)
            {
                if (NULL != table[i])
                {
                    table[i]->~BigInteger();
                    ma_free(alloc.pointer(), table[i]);
                    table[i] = NULL;
                }
            }
            ma_free(alloc.pointer(), table);
            table = NULL;
            size = 0;
        }
    }

    const BigInteger& at(size_t i) const
    {
        assert(i < size);
        return *table[i];
    }

private:
    MontgomeryPreBuildTable(const MontgomeryPreBuildTable&);
};

/**
 * 计算滑动窗口算法的最佳窗口大小
 */
static size_t _best_wnd(size_t bit_len)
{
    // 参考 java 的 BigInteger.bnExpModThreshTable
    static const size_t TBL[] = {
        7, 25, 81, 241, 673, 1793
    };

    int left = -1, right = sizeof(TBL) / sizeof(TBL[0]);
    while (right - left > 1)
    {
        const int mid = (left + right) / 2;
        if (TBL[mid] == bit_len)
            return mid + 1;
        else if (TBL[mid] < bit_len)
            left = mid;
        else
            right = mid;
    }
    return left + 2;
}

/**
 * 使用 Montgomery 算法优化
 */
static void _odd_mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n, BigInteger *rs)
{
    assert(NULL != rs);
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
    for (size_t i = b.bit_length() - 1; i > 0; --i)
    {
        _montgomery(ret * ret, rlen, n, nn, &ret);
        if (0 != b.bit_at(i - 1))
            _montgomery(ret * m, rlen, n, nn, &ret);
    }

    // 处理返回值
    _montgomery(ret, rlen, n, nn, rs);
    return;
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
    for (int i = ((int) b.bit_length()) - 2; i >= 0; --i)
    {
        ret *= ret;
        _montgomery2(ret, n, nnn, &ret);
        if (0 != b.bit_at(i))
        {
            ret *= m;
            _montgomery2(ret, n, nnn, &ret);
        }
    }

    // 处理返回值
    _montgomery2(ret, n, nnn, rs);
    return;
#elif (OPTIMIZE_LEVEL == 2)
    /**
     * 特殊的扩展欧几里得算法
     */

    // 预运算
    const size_t rlen = n.bit_length();
    BigInteger nn;
    _mont_extended_euclid(rlen, n, NULL, &nn); // 特殊的欧几里得算法
    nn.limit_positive_bits_to(rlen);

    // 循环计算
    const BigInteger m = (a << rlen) % n;
    BigInteger ret(m);
    for (int i = ((int) b.bit_length()) - 2; i >= 0; --i)
    {
        ret *= ret;
        _montgomery(ret, rlen, n, nn, &ret);
        if (0 != b.bit_at(i))
        {
            ret *= m;
            _montgomery(ret, rlen, n, nn, &ret);
        }
    }

    // 处理返回值
    _montgomery(ret, rlen, n, nn, rs);
    return;
#else
    /**
     * 使用滑动窗口算法优化
     */

    // 准备蒙哥马利相关变量
    const size_t rlen = n.bit_length();
    BigInteger nn(0, a.allocator());
    _mont_extended_euclid(rlen, n, NULL, &nn);
    nn.limit_positive_bits_to(rlen);

    // 准备预运算表
    int bits_left = (int) b.bit_length() - 1; // 剩余还未处理的比特数
    assert(bits_left >= 0);
    const size_t wnd_size = _best_wnd(bits_left); // 滑动窗口大小
    const uint32_t WND_MASK = ~(((uint32_t) 1) << wnd_size);
    const BigInteger m = (a << rlen) % n;
    const MontgomeryPreBuildTable tbl(wnd_size, m, rlen, n, nn, n.allocator());

    // 计算过程
    BigInteger ret(m);
    uint32_t wnd = 0; // 比特窗口
    size_t squre_count = 0; // 需要平方的次数
    while (bits_left > 0)
    {
        wnd <<= 1;
        wnd &= WND_MASK;
        if (0 != b.bit_at(--bits_left)) // b 的最后一个比特必定为1，因为 b 为奇数
        {
            wnd |= (uint32_t) 1;

            ++squre_count;
            bool term = true;
            for (size_t i = 1;
                 squre_count + i <= wnd_size && bits_left >= (int) i;
                ++i)
            {
                if (0 != b.bit_at(bits_left - i))
                {
                    term = false;
                    break;
                }
            }

            if (term)
            {
                for (size_t i = 0; i < squre_count; ++i)
                {
                    ret *= ret;
                    _montgomery(ret, rlen, n, nn, &ret);
                }
                squre_count = 0;

                ret *= tbl.at(wnd >> 1);
                _montgomery(ret, rlen, n, nn, &ret);
                wnd = 0;
            }
        }
        else if (0 == squre_count) // 窗口之间的0
        {
            ret *= ret;
            _montgomery(ret, rlen, n, nn, &ret);
        }
        else // 窗口内部的0
        {
            ++squre_count;
        }
    }

    // 处理返回值
    _montgomery(ret, rlen, n, nn, rs);
    return;
#endif
}

/**
 * 计算 (a ** b) mod (2 ** p)
 */
static void _mod_pow_2(const BigInteger& a, const BigInteger& b, size_t p, BigInteger *rs)
{
    assert(NULL != rs);
    assert(a.is_positive() && b.is_positive() && p > 0);

    BigInteger ret(1, rs->allocator());
    for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
    {
        ret.multiply_to_len(ret, p);

        if (0 != b.bit_at(i - 1))
            ret.multiply_to_len(a, p);
    }

    *rs = std::move(ret);
}

/**
 * 求(a**b)%n，即a的b次方(模n)
 * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P576
 */
void mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n, BigInteger *rs)
{
    assert(NULL != rs);
    assert(a.is_positive() && b.is_positive() && n.is_positive());

    if (b.is_zero())
    {
        *rs = (n == 1 ? 0 : 1);
        return;
    }
    else if (a == 1)
    {
        *rs = (n == 1 ? 0 : 1);
        return;
    }
    else if (a.is_zero())
    {
        rs->set_zero();
        return;
    }

#if (OPTIMIZE_LEVEL == 0)
    BigInteger ret(1, rs->allocator());
    for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
    {
        ret = (ret * ret) % n;
        if (0 != b.bit_at(i - 1))
            ret = (ret * a) % n;
    }
    *rs = std::move(ret);
    return;
#elif (OPTIMIZE_LEVEL == 1)
    const size_t bbc = b.bit_count();
     if (bbc > 400) // 400 是一个经验数据
    {
        ModMultiplyPreBuildTable<4> table(a % n, n); /// 经测试，预算表模板参数取4比较合适
        BigInteger ret(1, rs->allocator());
        for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
        {
            ret = (ret * ret) % n;
            if (0 != b.bit_at(i - 1))
                mod_multiply(ret, n, table, &ret);
        }
        *rs = std::move(ret);
        return;
    }
    else
    {
        BigInteger ret(1, rs->allocator());
        for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
        {
            ret = (ret * ret) % n;
            if (0 != b.bit_at(i - 1))
                ret = (ret * a) % n;
        }
        *rs = std::move(ret);
        return;
    }
#else
    // 模是奇数，应用蒙哥马利算法
    if (n.bit_at(0) == 1)
    {
        _odd_mod_pow(a < n ? a : a % n, b, n, rs);
        return;
    }

    // 模是偶数，应用中国余数定理
    const size_t p = n.lowest_bit();
    BigInteger n1(n), n2(1, a.allocator());
    n1 >>= p;
    n2 <<= p;

    BigInteger a1(0, a.allocator());
    if (n1 != 1)
        _odd_mod_pow(a % n1, b, n1, &a1);

    BigInteger a2(0, a.allocator());
    _mod_pow_2((a < n ? a : a % n), b, p, &a2);

    BigInteger y1(0, a.allocator());
    extended_euclid(n2, n1, NULL, &y1, NULL);
    if (y1 < 0)
    {
        // y1 = n1 + (y1 % n1);
        y1 %= n1;
        y1 += n1;
    }
    BigInteger y2(0,a.allocator());
    extended_euclid(n1, n2, NULL, &y2, NULL);
    if (y2 < 0)
    {
        // y2 = n2 + (y2 % n2);
        y2 %= n2;
        y2 += n2;
    }

    // *rs = (a1 * n2 * y1 + a2 * n1 * y2) % n
    *rs = a1 * n2 * y1 + a2 * n1 * y2;
    *rs %= n;
    return;
#endif
}

}
