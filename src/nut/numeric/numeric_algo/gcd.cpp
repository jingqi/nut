
#include <stack>

#include "gcd.h"

// 优化程度，>= 0
#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 求最大公约数
 */
BigInteger gcd(const BigInteger& a, const BigInteger& b)
{
    /// 下面几个算法，随着规模增大，优化后的优势越明显。
    /// 规模较小时优化的算法反而不如没优化的算法有效率。
#if (OPTIMIZE_LEVEL == 0)
    /**
     * 欧几里德(EUCLID)算法，求最大公约数
     * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P563
     */
    if (b.is_zero())
        return a;
    return gcd(b, a % b);
#elif (OPTIMIZE_LEVEL == 1)
    /**
     * 利用二进制特性的gcd算法
     * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P590
     */
    if (b.is_zero())
        return a;
    if (a.bit_at(0) == 0 && b.bit_at(0) == 0)
        return gcd(a >> 1, b >> 1) << 1;
    else if (a.bit_at(0) == 0)
        return gcd(b, a >> 1);
    else if (b.bit_at(0) == 0)
        return gcd(a, b >> 1);
    else if (a > b)
        return gcd(b, (a - b) >> 1);
    else
        return gcd(a, (b - a) >> 1);
#elif (OPTIMIZE_LEVEL == 2)
    /**
     * 进一步优化上一个实现
     */
    size_t left_shift = 0;
    BigInteger aa(a), bb(b);
    while (!bb.is_zero())
    {
        if (aa.bit_at(0) == 0 && bb.bit_at(0) == 0)
        {
            aa >>= 1;
            bb >>= 1;
            ++left_shift;
        }
        else if (aa.bit_at(0) == 0)
        {
            aa >>= 1;
            BigInteger::swap(&aa, &bb); // 交换a b, 尽量使 b < a
        }
        else if (bb.bit_at(0) == 0)
        {
            bb >>= 1;
        }
        else if (aa > bb)
        {
            aa = (aa - bb) >> 1;
            BigInteger::swap(&aa, &bb); // 交换a b, 尽量使 b < a
        }
        else
        {
            bb = (bb - aa) >> 1;
        }
    }
    aa <<= left_shift;
    return aa;
#elif (OPTIMIZE_LEVEL == 3)
    /**
     * 进一步优化上一个实现
     */
    size_t left_shift = 0;
    BigInteger aa(a), bb(b);
    while (true)
    {
        const int lb1 = aa.lowest_bit(), lb2 = bb.lowest_bit();
        if (lb1 < 0)
        {
            bb <<= left_shift;
            return bb;
        }
        else if (lb2 < 0)
        {
            aa <<= left_shift;
            return aa;
        }
        else if (lb1 > 0 && lb2 > 0)
        {
            const int min_lb = (lb1 < lb2 ? lb1 : lb2);
            aa >>= min_lb;
            bb >>= min_lb;
            left_shift += min_lb;
        }
        else if (lb1 > 0)
        {
            aa >>= lb1;
        }
        else if (lb2 > 0)
        {
            bb >>= lb2;
        }
        else if (aa > bb)
        {
            aa = (aa - bb) >> 1;
        }
        else
        {
            bb = (bb - aa) >> 1;
        }
    }
#else
    /**
     * 综合两种算法，小规模时用一种，较大规模时用另一种
     */
    const size_t EMPIRICAL_BOUND = 10; /// 经验数据，根据性能测试结果得来
    if (sizeof(BigInteger::word_type) * a.significant_words_length() < EMPIRICAL_BOUND ||
        sizeof(BigInteger::word_type) * b.significant_words_length() < EMPIRICAL_BOUND)
    {
        // 规模较小，直接运算比较高效
        BigInteger aa(a), bb(b);
        while (!bb.is_zero())
        {
            aa %= bb;
            BigInteger::swap(&aa, &bb); // 交换 a, b
        }
        return aa;
    }

    // 规模较大，使用较复杂的策略
    BigInteger aa(a), bb(b);
    size_t left_shift = 0;
    while (true)
    {
        const int lb1 = aa.lowest_bit(), lb2 = bb.lowest_bit();
        if (lb1 < 0)
        {
            bb <<= left_shift;
            return bb;
        }
        else if (lb2 < 0)
        {
            aa <<= left_shift;
            return aa;
        }
        else if (lb1 > 0 && lb2 > 0)
        {
            const int min_lb = (lb1 < lb2 ? lb1 : lb2);
            aa >>= min_lb;
            bb >>= min_lb;
            left_shift += min_lb;
        }
        else if (lb1 > 0)
        {
            aa >>= lb1;
        }
        else if (lb2 > 0)
        {
            bb >>= lb2;
        }
        else if (aa > bb)
        {
            aa -= bb;
            aa >>= 1;
        }
        else
        {
            bb -= aa;
            bb >>= 1;
        }
    }
#endif
}

/**
 * 扩展欧几里得算法
 */
void extended_euclid(const BigInteger& a, const BigInteger& b, BigInteger *d, BigInteger *x, BigInteger *y)
{
#if (OPTIMIZE_LEVEL == 0)
    /**
     * 欧几里得(EUCLID)算法的推广形式
     * d = gcd(a, b) = ax + by
     * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P564
     */
    if (b.is_zero())
    {
        if (NULL != d)
            *d = a;
        if (NULL != x)
            *x = 1;
        if (NULL != y)
            *y = 0;
        return;
    }

    BigInteger xx, yy;
    extended_euclid(b, a % b, d, &yy, &xx);
    if (NULL != x)
        *x = xx;
    if (NULL != y)
        *y = yy - a / b * xx;
#elif (OPTIMIZE_LEVEL == 1)
    /**
     * 推广的 Euclidean 算法
     * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P590
     * 参见 《公开密钥密码算法及其快速实现》.周玉洁.冯国登.国防工业出版社.2002 P63
     */
    if (b.is_zero())
    {
        if (NULL != d)
            *d = a;
        if (NULL != x)
            *x = 1;
        if (NULL != y)
            *y = 0;
        return;
    }

    if (a.bit_at(0) == 0 && b.bit_at(0) == 0)
    {
        extended_euclid(a >> 1, b >> 1, d, x, y);
        // d = dd * 2;
        // x = xx; y = yy;
        if (NULL != d)
            *d <<= 1;
        return;
    }
    else if (a.bit_at(0) == 0)
    {
        BigInteger xx;
        extended_euclid(b, a >> 1, d, y, &xx);
        if (xx.bit_at(0) == 0)
        {
            // d = dd;
            // x = xx / 2; y = yy;
            if (NULL != x)
                *x = xx >> 1;
        }
        else
        {
            // d = dd;
            // x = (xx + b) / 2; y = yy - a / 2;
            if (NULL != x)
                *x = (xx + b) >> 1;
            if (NULL != y)
                *y -= a >> 1;
        }
        return;
    }
    else if (b.bit_at(0) == 0)
    {
        BigInteger yy;
        extended_euclid(a, b >> 1, d, x, &yy);
        if (yy.bit_at(0) == 0)
        {
            // d = dd;
            // x = xx; y = yy / 2;
            if (NULL != y)
                *y = yy >> 1;
        }
        else
        {
            // d = dd;
            // x = xx - b / 2; y = (yy + a) / 2;
            if (NULL != x)
                *x -= b >> 1;
            if (NULL != y)
                *y = (yy + a) >> 1;
        }
        return;
    }
    else if (a > b)
    {
        BigInteger xx;
        extended_euclid(b, (a - b) >> 1, d, y, &xx);
        if (xx.bit_at(0) == 0)
        {
            // d = dd;
            // x = xx / 2; y = yy - xx / 2;
            if (NULL != x)
                *x = xx >> 1;
            if (NULL != y)
                *y -= xx >> 1;
        }
        else
        {
            // d = dd;
            // x = (xx + b) / 2; y = yy - (xx + a) / 2;
            // 或者 x = (xx - b) / 2; y = yy - (xx - a) / 2;
            if (NULL != x)
                *x = (xx + b) >> 1;
            if (NULL != y)
                *y -= (xx + a) >> 1;
        }
        return;
    }
    else
    {
        BigInteger yy;
        extended_euclid(a, (b - a) >> 1, d, x, &yy);
        if (yy.bit_at(0) == 0)
        {
            // d = dd;
            // x = xx - yy / 2; y = yy / 2;
            if (NULL != x)
                *x -= yy >> 1;
            if (NULL != y)
                *y = yy >> 1;
        }
        else
        {
            // d = dd;
            // x = xx - (yy + b) / 2; y = (yy + a) / 2;
            // 或者 x == xx - (yy - b) / 2; y = (yy - a) / 2;
            if (NULL != x)
                *x -= (yy + b) >> 1;
            if (NULL != y)
                *y = (yy + a) >> 1;
        }
        return;
    }
#elif (OPTIMIZE_LEVEL == 2)
    /**
     * 上一个算法的进一步优化
     */
    const int lb1 = a.lowest_bit(), lb2 = b.lowest_bit();
    if (lb1 < 0)
    {
        if (NULL != d)
            *d = b;
        if (NULL != x)
            *x = 0;
        if (NULL != y)
            *y = 1;
        return;
    }
    else if (lb2 < 0)
    {
        if (NULL != d)
            *d = a;
        if (NULL != x)
            *x = 1;
        if (NULL != y)
            *y = 0;
        return;
    }
    else if (lb1 > 0 && lb2 > 0)
    {
        const int min_lb = (lb1 < lb2 ? lb1 : lb2);
        extended_euclid(a >> min_lb, b >> min_lb, d, x, y);
        if (NULL != d)
            *d <<= min_lb;
        return;
    }
    else if (lb1 > 0)
    {
        BigInteger xx;
        extended_euclid(a >> lb1, b, d, &xx, y);
        int done = 0;
        while (done < lb1)
        {
            const int lbx = xx.lowest_bit();
            if (lbx < 0)
            {
                break;
            }
            else if (lbx == 0)
            {
                xx = (xx + b) >> 1;
                if (NULL != y)
                    *y -= a >> (lb1 - done);
                ++done;
            }
            else
            {
                const int min_lb = (lbx < lb1 - done ? lbx : lb1 - done);
                xx >>= min_lb;
                done += min_lb;
            }
        }
        if (NULL != x)
            *x = xx;
        return;
    }
    else if (lb2 > 0)
    {
        BigInteger yy;
        extended_euclid(a, b >> lb2, d, x, &yy);
        int done = 0;
        while (done < lb2)
        {
            const int lby = yy.lowest_bit();
            if (lby < 0)
            {
                break;
            }
            else if (lby == 0)
            {
                yy = (yy + a) >> 1;
                if (NULL != x)
                    *x -= b >> (lb2 - done);
                ++done;
            }
            else
            {
                const int min_lb = (lby < lb2 - done ? lby : lb2 - done);
                yy >>= min_lb;
                done += min_lb;
            }
        }
        if (NULL != y)
            *y = yy;
        return;
    }
    else if (a > b)
    {
        BigInteger xx;
        extended_euclid((a - b) >> 1, b, d, &xx, y);
        if (xx.bit_at(0) == 0)
        {
            // d = dd;
            // x = xx / 2; y = yy - xx / 2;
            if (NULL != x)
                *x = xx >> 1;
            if (NULL != y)
                *y -= xx >> 1;
        }
        else
        {
            // d = dd;
            // x = (xx + b) / 2; y = yy - (xx + a) / 2;
            // 或者 x = (xx - b) / 2; y = yy - (xx - a) / 2;
            if (NULL != x)
                *x = (xx + b) >> 1;
            if (NULL != y)
                *y -= (xx + a) >> 1;
        }
        return;
    }
    else
    {
        BigInteger yy;
        extended_euclid(a, (b - a) >> 1, d, x, &yy);
        if (yy.bit_at(0) == 0)
        {
            // d = dd;
            // x = xx - yy / 2; y = yy / 2;
            if (NULL != x)
                *x -= yy >> 1;
            if (NULL != y)
                *y = yy >> 1;
        }
        else
        {
            // d = dd;
            // x = xx - (yy + b) / 2; y = (yy + a) / 2;
            // 或者 x == xx - (yy - b) / 2; y = (yy - a) / 2;
            if (NULL != x)
                *x -= (yy + b) >> 1;
            if (NULL != y)
                *y = (yy + a) >> 1;
        }
        return;
    }
#else
    /**
     * 综合优化，并去除递归调用(处理超大规模数时导致栈溢出)
     */
    const size_t EMPIRICAL_BOUND = 10; /// 经验数据，根据性能测试结果得来
    if (sizeof(BigInteger::word_type) * a.significant_words_length() < EMPIRICAL_BOUND ||
        sizeof(BigInteger::word_type) * b.significant_words_length() < EMPIRICAL_BOUND)
    {
        BigInteger aa(a), bb(b);
        std::stack<BigInteger> as;
        while (!bb.is_zero())
        {
            as.push(aa);
            aa %= bb;
            BigInteger::swap(&aa, &bb);
        }
        if (NULL != d)
            *d = aa;

        BigInteger xx(1, a.allocator()), yy(0, a.allocator());
        while (!as.empty())
        {
            BigInteger::swap(&xx, &yy);
            BigInteger::swap(&aa, &bb);
            aa = as.top();
            as.pop();
            yy -= aa / bb * xx;
        }
        if (NULL != x)
            *x = xx;
        if (NULL != y)
            *y = yy;
        return;
    }

    BigInteger aa(a), bb(b), xx(1, a.allocator()), yy(0, a.allocator());
    size_t left_shift = 0;
    std::stack<int> lb1s, lb2s;
    while (true)
    {
        const int lb1 = aa.lowest_bit(), lb2 = bb.lowest_bit();
        if (lb1 < 0)
        {
            if (NULL != d)
                *d = (bb << left_shift);
            xx = 0;
            yy = 1;
            break;
        }
        else if (lb2 < 0)
        {
            if (NULL != d)
                *d = (aa << left_shift);
            xx = 1;
            yy = 0;
            break;
        }
        else if (lb1 > 0 && lb2 > 0)
        {
            lb1s.push(lb1);
            lb2s.push(lb2);
            const int min_lb = (lb1 < lb2 ? lb1 : lb2);
            aa >>= min_lb;
            bb >>= min_lb;
            left_shift += min_lb;
        }
        else if (lb1 > 0)
        {
            lb1s.push(lb1);
            lb2s.push(lb2);
            aa >>= lb1;
        }
        else if (lb2 > 0)
        {
            lb1s.push(lb1);
            lb2s.push(lb2);
            bb >>= lb2;
        }
        else if (aa > bb)
        {
            lb1s.push(-1);
            lb2s.push(-1);
            aa -= bb;
            aa >>= 1;
        }
        else
        {
            lb1s.push(-2);
            lb2s.push(-2);
            bb -= aa;
            bb >>= 1;
        }
    }
    if (NULL == x && NULL == y)
        return;

    while (!lb1s.empty())
    {
        const int lb1 = lb1s.top(), lb2 = lb2s.top();
        lb1s.pop();
        lb2s.pop();

        if (lb1 > 0 && lb2 > 0)
        {
            const int min_lb = (lb1 < lb2 ? lb1 : lb2);
            aa <<= min_lb;
            bb <<= min_lb;
        }
        else if (lb1 > 0)
        {
            aa <<= lb1;
            int done = 0;
            while (done < lb1)
            {
                const int lbx = xx.lowest_bit();
                if (lbx < 0)
                {
                    break;
                }
                else if (lbx == 0)
                {
                    xx += bb;
                    xx >>= 1;
                    yy -= aa >> (lb1 - done);
                    ++done;
                }
                else
                {
                    const int min_lb = (lbx < lb1 - done ? lbx : lb1 - done);
                    xx >>= min_lb;
                    done += min_lb;
                }
            }
        }
        else if (lb2 > 0)
        {
            bb <<= lb2;
            int done = 0;
            while (done < lb2)
            {
                const int lby = yy.lowest_bit();
                if (lby < 0)
                {
                    break;
                }
                else if (lby == 0)
                {
                    yy += aa;
                    yy >>= 1;
                    xx -= bb >> (lb2 - done);
                    ++done;
                }
                else
                {
                    const int min_lb = (lby < lb2 - done ? lby : lb2 - done);
                    yy >>= min_lb;
                    done += min_lb;
                }
            }
        }
        else if (lb1 == -1)
        {
            aa <<= 1;
            aa += bb;
            if (xx.bit_at(0) == 0)
            {
                // d = dd;
                // x = xx / 2; y = yy - xx / 2;
                yy -= xx >> 1; // 必须放在下一句前面
                xx >>= 1;
            }
            else
            {
                // d = dd;
                // x = (xx + b) / 2; y = yy - (xx + a) / 2;
                // 或者 x = (xx - b) / 2; y = yy - (xx - a) / 2;
                yy -= (xx + aa) >> 1; // 必须放在下一句前面
                xx += bb;
                xx >>= 1;
            }
        }
        else
        {
            bb <<= 1;
            bb += aa;
            if (yy.bit_at(0) == 0)
            {
                // d = dd;
                // x = xx - yy / 2; y = yy / 2;
                xx -= yy >> 1; // 必须放在下一句前面
                yy >>= 1;
            }
            else
            {
                // d = dd;
                // x = xx - (yy + b) / 2; y = (yy + a) / 2;
                // 或者 x == xx - (yy - b) / 2; y = (yy - a) / 2;
                xx -= (yy + bb) >> 1; // 必须放在下一句前面
                yy += aa;
                yy >>= 1;
            }
        }
    }
    if (NULL != x)
        *x = xx;
    if (NULL != y)
        *y = yy;
#endif
}

}
