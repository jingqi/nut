
#include "mod.h"
#include "gcd.h"


namespace nut
{

/**
 * 蒙哥马利算法
 * {t + [(t mod r) * n' mod r] * n} / r
 */
static BigInteger _montgomery(const BigInteger& t, size_t rlen, const BigInteger& n, const BigInteger& nn)
{
    assert(t.is_positive() && rlen > 0 && n.is_positive() && nn.is_positive());
    typedef BigInteger::word_type word_type;

    // 计算 t % r
    size_t min_sig = (rlen + 8 * sizeof(word_type) - 1) / (8 * sizeof(word_type));
    if (t.significant_words_length() < min_sig)
        min_sig = t.significant_words_length();
    BigInteger s(t.data(), sizeof(word_type) * min_sig, true);
    s.limit_positive_bits_to(rlen);

    s.multiply_to_len(nn, rlen); // rs = (rs * nn) % r
    s *= n;
    s += t;
    s >>= rlen;

    if (s >= n)
        s -= n;

    return s;
}

#if 0 // unoptimized
/**
 * 变形的蒙哥马利算法
 *
 * 参考文献：
 *      [1]王金荣，周赟，王红霞. Montgomery模平方算法及其应用[J]. 计算机工程，2007，33(24). 155-156
 */
static BigInteger _montgomery2(const BigInteger& t, const BigInteger& n, BigInteger::word_type nn)
{
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

    return ret;
}
#endif

/**
 * 已知:
 *      n为奇数, r=2**p，(n,r互质)
 * 求 rr, nn 使得:
 *      r * rr - n * nn = 1 (rr为r模n的逆元，nn为n模r的负逆元)
 *
 * 参考文献：
 *      [1]雷明，叶新，张焕国. Montgomery算法及其快速实现[J]. 计算机工程，2003，29(14). 46
 *
 * @note 返回的 rr, nn 都为正数
 */
static void _mont_extended_euclid(size_t rlen, const BigInteger& n, BigInteger *rr, BigInteger *nn)
{
    assert(nullptr != rr || nullptr != nn);

    BigInteger r(1);
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

    if (nullptr != rr)
        *rr = std::move(ret_rr);
    if (nullptr != nn)
        *nn = std::move(ret_nn);
}

/**
 * 滑动窗口算法、蒙哥马利算法的预算表
 */
class MontgomeryPreBuildTable
{
public:
    MontgomeryPreBuildTable(size_t wnd_sz, const BigInteger& m, size_t rlen,
            const BigInteger& n, const BigInteger& nn)
        : _table(nullptr), _size(0)
    {
        assert(0 < wnd_sz && wnd_sz < 16);

        _size = 1LL << (wnd_sz - 1);
        _table = (BigInteger**) ::malloc(sizeof(BigInteger*) * _size);
        assert(nullptr != _table);
        ::memset(_table, 0, sizeof(BigInteger*) * _size);

        _table[0] = (BigInteger*) ::malloc(sizeof(BigInteger));
        new (_table[0]) BigInteger(m);
        const BigInteger mm = _montgomery(m * m, rlen, n, nn);
        for (size_t i = 1; i < _size; ++i)
        {
            _table[i] = (BigInteger*) ::malloc(sizeof(BigInteger));
            new (_table[i]) BigInteger(_montgomery(*_table[i - 1] * mm, rlen, n, nn));
        }
    }

    ~MontgomeryPreBuildTable()
    {
        if (nullptr != _table)
        {
            for (size_t i = 0; i < _size; ++i)
            {
                if (nullptr != _table[i])
                {
                    _table[i]->~BigInteger();
                    ::free(_table[i]);
                    _table[i] = nullptr;
                }
            }
            ::free(_table);
            _table = nullptr;
            _size = 0;
        }
    }

    const BigInteger& at(size_t i) const
    {
        assert(i < _size);
        return *_table[i];
    }

private:
    MontgomeryPreBuildTable(const MontgomeryPreBuildTable&) = delete;
    MontgomeryPreBuildTable& operator=(const MontgomeryPreBuildTable&) = delete;

private:
    BigInteger **_table;
    size_t _size;
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
static BigInteger _odd_mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n)
{
    assert(a.is_positive() && b.is_positive() && n.is_positive());
    assert(a < n && n.bit_at(0) == 1);

#if 0 // unoptimized
    // 预运算
    const size_t rlen = n.bit_length();
    BigInteger r(1), nn;
    r <<= rlen;
    extended_euclid(r, n, nullptr, nullptr, &nn);
    if (nn.is_positive())
        nn = r - (nn % r);
    else
        nn = (-nn) % r;

    // 循环计算
    const BigInteger m = (a << rlen) % n;
    BigInteger ret(m);
    for (size_t i = b.bit_length() - 1; i > 0; --i)
    {
        ret *= ret;
        ret = _montgomery(ret, rlen, n, nn);
        if (0 != b.bit_at(i - 1))
        {
            ret *= m;
            ret = _montgomery(ret, rlen, n, nn);
        }
    }

    // 处理返回值
    return _montgomery(ret, rlen, n, nn);
#elif 0 // unoptimized
    /**
     * 变形的蒙哥马利算法
     *      在这里其效率低于原始的蒙哥马利算法, 可能是非等长的大整数运算效率高于定长大整数运算的缘故
     */

    // 预运算
    const size_t r_word_count = n.significant_words_length();
    BigInteger nn;
    _mont_extended_euclid(8 * sizeof(BigInteger::word_type), BigInteger(n.word_at(0)), nullptr, &nn);
    BigInteger::word_type nnn = nn.word_at(0);

    // 循环计算
    const BigInteger m = (a << (8 * sizeof(BigInteger::word_type) * r_word_count)) % n;
    BigInteger ret(m);
    for (int i = ((int) b.bit_length()) - 2; i >= 0; --i)
    {
        ret *= ret;
        ret = _montgomery2(ret, n, nnn);
        if (0 != b.bit_at(i))
        {
            ret *= m;
            ret = _montgomery2(ret, n, nnn);
        }
    }

    // 处理返回值
    return _montgomery2(ret, n, nnn);
#elif 0 // unoptimized
    /**
     * 特殊的扩展欧几里得算法
     */

    // 预运算
    const size_t rlen = n.bit_length();
    BigInteger nn;
    _mont_extended_euclid(rlen, n, nullptr, &nn); // 特殊的欧几里得算法
    nn.limit_positive_bits_to(rlen);

    // 循环计算
    const BigInteger m = (a << rlen) % n;
    BigInteger ret(m);
    for (int i = ((int) b.bit_length()) - 2; i >= 0; --i)
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
#else
    /**
     * 使用滑动窗口算法优化
     */

    // 准备蒙哥马利相关变量
    const size_t rlen = n.bit_length();
    BigInteger nn(0);
    _mont_extended_euclid(rlen, n, nullptr, &nn);
    nn.limit_positive_bits_to(rlen);

    // 准备预运算表
    int bits_left = (int) b.bit_length() - 1; // 剩余还未处理的比特数
    assert(bits_left >= 0);
    const size_t wnd_size = _best_wnd(bits_left); // 滑动窗口大小
    const uint32_t WND_MASK = ~(((uint32_t) 1) << wnd_size);
    const BigInteger m = (a << rlen) % n;
    const MontgomeryPreBuildTable tbl(wnd_size, m, rlen, n, nn);

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
                    ret = _montgomery(ret, rlen, n, nn);
                }
                squre_count = 0;

                ret *= tbl.at(wnd >> 1);
                ret = _montgomery(ret, rlen, n, nn);
                wnd = 0;
            }
        }
        else if (0 == squre_count) // 窗口之间的0
        {
            ret *= ret;
            ret = _montgomery(ret, rlen, n, nn);
        }
        else // 窗口内部的0
        {
            ++squre_count;
        }
    }

    // 处理返回值
    return _montgomery(ret, rlen, n, nn);
#endif
}

/**
 * 计算 (a ** b) mod (2 ** p)
 */
static BigInteger _mod_pow_2(const BigInteger& a, const BigInteger& b, size_t p)
{
    assert(a.is_positive() && b.is_positive() && p > 0);

    BigInteger ret(1);
    for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
    {
        ret.multiply_to_len(ret, p);

        if (0 != b.bit_at(i - 1))
            ret.multiply_to_len(a, p);
    }

    return ret;
}

/**
 * 求(a**b)%n，即a的b次方(模n)
 *
 * 参考文献：
 *      [1]潘金贵，顾铁成. 现代计算机常用数据结构和算法[M]. 南京大学出版社. 1994. 576
 */
NUT_API BigInteger mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n)
{
    assert(a.is_positive() && b.is_positive() && n.is_positive());

    if (b.is_zero())
        return BigInteger(1 == n ? 0 : 1);
    else if (a == 1)
        return BigInteger(1 == n ? 0 : 1);
    else if (a.is_zero())
        return BigInteger(0);

#if 0 // unoptimized
    BigInteger ret(1);
    for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
    {
        ret = (ret * ret) % n;
        if (0 != b.bit_at(i - 1))
            ret = (ret * a) % n;
    }
    return ret;
#elif 0 // unoptimized
    const size_t bbc = b.bit_count();
     if (bbc > 400) // 400 是一个经验数据
    {
        ModMultiplyPreBuildTable<4> table(a % n, n); /// 经测试，预算表模板参数取4比较合适
        BigInteger ret(1);
        for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
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
        for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
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
        return _odd_mod_pow(a % n, b, n);

    // 模是偶数，应用中国余数定理
    const size_t p = n.lowest_bit();
    BigInteger n1(n), n2(1);
    n1 >>= p;
    n2 <<= p;

    BigInteger a1(0);
    if (n1 != 1)
        a1 = _odd_mod_pow(a % n1, b, n1);

    const BigInteger a2 = _mod_pow_2(a % n, b, p);

    BigInteger y1(0);
    extended_euclid(n2, n1, nullptr, &y1, nullptr);
    if (!y1.is_positive())
    {
        // y1 = n1 + (y1 % n1);
        y1 %= n1;
        y1 += n1;
    }
    BigInteger y2(0);
    extended_euclid(n1, n2, nullptr, &y2, nullptr);
    if (!y2.is_positive())
    {
        // y2 = n2 + (y2 % n2);
        y2 %= n2;
        y2 += n2;
    }

    return (a1 * n2 * y1 + a2 * n1 * y2) % n;
#endif
}

}
