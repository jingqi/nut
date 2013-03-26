/**
 * @file -
 * @author jingqi
 * @date 2013-03-26
 * @last-edit 2013-03-26 20:29:45 jingqi
 * @brief
 */

#ifndef ___HEADFILE_F8CA88A7_CD94_4F1D_AA24_A5E53F2E2E96_
#define ___HEADFILE_F8CA88A7_CD94_4F1D_AA24_A5E53F2E2E96_

#include <xutility>

namespace nut
{

/**
 * 多维区域
 * 
 * @param NumT 数字类型; 可以是 int、float 等
 * @param DIMENSIONS 维数，大于等于1
 * @param RealNumT 实数，避免乘法运算溢出，常用的是float，double等
 */
template <typename NumT, size_t DIMENSIONS = 2, typename RealNumT = double>
class MDArea
{
    typedef MDArea<NumT, DIMENSIONS, RealNumT> self;

public:
    NumT left[DIMENSIONS]; // 左边界
    NumT right[DIMENSIONS]; // 右边界

    MDArea()
    {
        for (register size_t i = 0; i < DIMENSIONS; ++i)
        {
            left[i] = 0;
            right[i] = 0;
        }
    }

    bool operator==(const self& x) const
    {
        for (register int i = 0; i < DIMENSIONS; ++i)
            if (left[i] != x.left[i] || right[i] != x.right[i])
                return false;
        return true;
    }

    bool operator!=(const self& x) const
    {
        return !(*this == x);
    }

    /**
     * 扩展区域，以便包含目标区域
     */
    void expandToContain(const self& x)
    {
        for (register int i = 0; i < DIMENSIONS; ++i)
        {
            left[i] = std::min(left[i], x.left[i]);
            right[i] = std::max(right[i], x.right[i]);
        }
    }

    /**
     * 检查区域有效性
     */
    bool isValid() const
    {
        for (register int i = 0; i < DIMENSIONS; ++i)
            if (left[i] > right[i])
                return false;
        return true;
    }

    /**
     * 所占的空间
     */
    RealNumT acreage() const
    {
        RealNumT acr = 1;
        for (register int i = 0; i < DIMENSIONS; ++i)
            acr *= right[i] - left[i];
        return acr;
    }

    /**
     * 查看是否完全包含另一个区域
     */
    bool contains(const self& x) const
    {
        for (register int i = 0; i < DIMENSIONS; ++i)
            if (!(left[i] <= x.left[i] && x.right[i] <= right[i]))
                return false;
        return true;
    }

    /**
     * 查看是否与另一个区域有交集
     */
    bool intersects(const self& x) const
    {
        // 任意一个维度不想交，则区域不想交
        for (register size_t i = 0; i < DIMENSIONS; ++i)
            if (left[i] > x.right[i] || right[i] < x.left[i])
                return false;
        return true;
    }
};

}


#endif
