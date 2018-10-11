﻿
#ifndef ___HEADFILE_F8CA88A7_CD94_4F1D_AA24_A5E53F2E2E96_
#define ___HEADFILE_F8CA88A7_CD94_4F1D_AA24_A5E53F2E2E96_

#include <nut/platform/platform.h>

#if NUT_PLATFORM_CC_VC
#   include <xutility> // for std::min() and so on
#endif


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
private:
    typedef MDArea<NumT, DIMENSIONS, RealNumT> self_type;

public:
    MDArea()
    {
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            lower[i] = 0;
            higher[i] = 0;
        }
    }

    bool operator==(const self_type& x) const
    {
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            if (lower[i] != x.lower[i] || higher[i] != x.higher[i])
                return false;
        }
        return true;
    }

    bool operator!=(const self_type& x) const
    {
        return !(*this == x);
    }

    /**
     * 清除数据，回到初始状态
     */
    void clear()
    {
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            lower[i] = 0;
            higher[i] = 0;
        }
    }

    /**
     * 扩展区域，以便包含目标区域
     */
    void expand_to_contain(const self_type& x)
    {
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            lower[i] = std::min(lower[i], x.lower[i]);
            higher[i] = std::max(higher[i], x.higher[i]);
        }
    }

    /**
     * 检查区域有效性
     */
    bool is_valid() const
    {
        for (int i = 0; i < DIMENSIONS; ++i)
        {
            if (lower[i] > higher[i])
                return false;
        }
        return true;
    }

    /**
     * 所占的空间
     */
    RealNumT acreage() const
    {
        RealNumT acr = 1;
        for (size_t i = 0; i < DIMENSIONS; ++i)
            acr *= higher[i] - lower[i];
        return acr;
    }

    /**
     * 查看是否完全包含另一个区域
     */
    bool contains(const self_type& x) const
    {
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            if (lower[i] > x.lower[i] || higher[i] < x.higher[i])
                return false;
        }
        return true;
    }

    /**
     * 查看是否与另一个区域有交集
     */
    bool intersects(const self_type& x) const
    {
        // 任意一个维度不想交，则区域不想交
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            if (lower[i] > x.higher[i] || higher[i] < x.lower[i])
                return false;
        }
        return true;
    }

public:
    NumT lower[DIMENSIONS]; // 低边界
    NumT higher[DIMENSIONS]; // 高边界
};

}

#endif
