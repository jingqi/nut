
#ifndef ___HEADFILE_F8CA88A7_CD94_4F1D_AA24_A5E53F2E2E96_
#define ___HEADFILE_F8CA88A7_CD94_4F1D_AA24_A5E53F2E2E96_

#include <algorithm> // for std::min()
#include <type_traits> // for std::enable_if

#include "../../../platform/platform.h"


namespace nut
{

/**
 * 多维区域
 *
 * @param NUM_TYPE 数字类型; 可以是 int、float 等
 * @param DIMENSIONS 维数，大于等于1
 * @param FLOAT_TYPE 实数，避免乘法运算溢出，常用的是 float，double 等
 */
template <typename NUM_TYPE, size_t DIMENSIONS = 2, typename FLOAT_TYPE = double>
class MDArea
{
private:
    typedef typename std::enable_if<std::is_integral<NUM_TYPE>::value ||
                                    std::is_floating_point<NUM_TYPE>::value,
                                    NUM_TYPE>::type num_type;

    typedef typename std::enable_if<std::is_floating_point<FLOAT_TYPE>::value,
                                    FLOAT_TYPE>::type float_type;

    typedef MDArea<num_type, DIMENSIONS, float_type> self_type;

public:
    MDArea() noexcept
    {
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            lower[i] = 0;
            higher[i] = 0;
        }
    }

    bool operator==(const self_type& x) const noexcept
    {
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            if (lower[i] != x.lower[i] || higher[i] != x.higher[i])
                return false;
        }
        return true;
    }

    bool operator!=(const self_type& x) const noexcept
    {
        return !(*this == x);
    }

    /**
     * 清除数据，回到初始状态
     */
    void clear() noexcept
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
    void expand_to_contain(const self_type& x) noexcept
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
    bool is_valid() const noexcept
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
    float_type acreage() const noexcept
    {
        float_type acr = 1;
        for (size_t i = 0; i < DIMENSIONS; ++i)
            acr *= higher[i] - lower[i];
        return acr;
    }

    /**
     * 查看是否完全包含另一个区域
     */
    bool contains(const self_type& x) const noexcept
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
    bool intersects(const self_type& x) const noexcept
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
    num_type lower[DIMENSIONS]; // 低边界
    num_type higher[DIMENSIONS]; // 高边界
};

}

#endif
