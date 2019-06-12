
#ifndef ___HEADFILE_C24BA2B9_EF7B_4DD5_A131_C61E1FC45B87_
#define ___HEADFILE_C24BA2B9_EF7B_4DD5_A131_C61E1FC45B87_

#include <assert.h>
#include <stddef.h> // for ptrdiff_t
#include <vector>
#include <string>
#include <iterator>
#include <algorithm> // for std::min() and so on
#include <utility> // for std::forward()

#include "../platform/int_type.h" // for ssize_t in windows VC
#include "../util/string/to_string.h"
#include "comparable.h"


namespace nut
{

/**
 * An ascending set of integers
 *
 * eg. {[1,3],6,[8,9]} contains 1 2 3 6 8 9
 */
template <typename Integral = int>
class IntegerSet
{
private:
    typedef IntegerSet<Integral> self_type;

public:
    typedef typename std::enable_if<std::is_integral<Integral>::value,Integral>::type int_type;

    class Range
    {
    public:
        Range() = default;

        Range(int_type f, int_type l) noexcept
            : first(f), last(l)
        {
            assert(f <= l);
        }

        bool operator==(const Range& x) const noexcept
        {
            return first == x.first && last == x.last;
        }

        bool operator!=(const Range& x) const noexcept
        {
            return !(*this == x);
        }

        void set(int_type f, int_type l) noexcept
        {
            first = f;
            last = l;
        }

    public:
        int_type first = 0, last = 0;
    };

    class const_iterator
    {
    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef int_type                        value_type;
        typedef ptrdiff_t                       difference_type;
        typedef int_type                        reference;       // FIXME 这里实际上无法返回引用
        typedef int_type*                       pointer;

    public:
        const_iterator(const self_type *container, int_type value, ssize_t index) noexcept
            : _container(container), _value(value), _index(index)
        {
            assert(nullptr != _container);
        }

        int_type operator*() const noexcept
        {
            assert(nullptr != _container);
            assert(0 <= _index && _index < (ssize_t) _container->_ranges.size());
            return _value;
        }

        const_iterator& operator++() noexcept
        {
            assert(nullptr != _container);
            assert(0 <= _index && _index < (ssize_t) _container->_ranges.size());

            const Range& rg = _container->_ranges.at(_index);
            if (_value < rg.last)
            {
                ++_value;
            }
            else
            {
                ++_index;
                if (_index < (ssize_t) _container->_ranges.size())
                    _value = _container->_ranges.at(_index).first;
            }
            return *this;
        }

        const_iterator& operator--() noexcept
        {
            assert(nullptr != _container);
            assert(0 <= _index && _index < (ssize_t) _container->_ranges.size());

            const Range& rg = _container->_ranges.at(_index);
            if (_value > rg.first)
            {
                --_value;
            }
            else
            {
                --_index;
                if (_index >= 0)
                    _value = _container->_ranges.at(_index).last;
            }
            return *this;
        }

        const_iterator operator++(int) noexcept
        {
            const_iterator ret = *this;
            ++*this;
            return ret;
        }

        const_iterator operator--(int) noexcept
        {
            const_iterator ret = *this;
            --*this;
            return ret;
        }

        bool operator==(const const_iterator& x) const noexcept
        {
            assert(nullptr != _container && _container == x._container);
            if (_index != x._index)
                return false;
            if (0 <= _index && _index < (ssize_t) _container->_ranges.size())
                return _value == x._value;
            return true;
        }

        bool operator!=(const const_iterator& x) const noexcept
        {
            return !(*this == x);
        }

    private:
        const self_type *_container = nullptr;
        int_type _value = 0;
        ssize_t _index = 0;
    };

private:
    /**
     * 将两个集合 x、y 放到数据轴上，每一个轴坐标的状态
     *
     *       None  SingleA    SingleB   AAndB
     *    A        ********            *******
     *    B                  *****************
     *  ------------------------------------------> integer axis
     */
    enum class AxisState
    {
        None,        // none
        SingleA,     // single A
        SingleB,     // single B
        AAndB        // A and B
    };

public:
    IntegerSet() = default;

    IntegerSet(IntegerSet&& x) noexcept
        : _ranges(std::forward<std::vector<Range>>(x._ranges))
    {}

    IntegerSet(const IntegerSet& x) = default;

    IntegerSet& operator=(IntegerSet&& x) noexcept
    {
        _ranges = std::forward<std::vector<Range>>(x._ranges);
    }

    IntegerSet& operator=(const IntegerSet& x) = default;

    bool operator==(const self_type& x) const noexcept
    {
        if (this == &x)
            return true;
        else if (_ranges.size() != x._ranges.size())
            return false;
        for (size_t i = 0, sz = _ranges.size(); i < sz; ++i)
        {
            if (_ranges.at(i) != x._ranges.at(i))
                return false;
        }
        return true;
    }

    bool operator!=(const self_type& x) const noexcept
    {
        return !(*this == x);
    }

    bool operator<(const self_type& x) const noexcept
    {
        return compare(x) < 0;
    }

    bool operator>(const self_type& x) const noexcept
    {
        return x < *this;
    }

    bool operator<=(const self_type& x) const noexcept
    {
        return !(x < *this);
    }

    bool operator>=(const self_type& x) const noexcept
    {
        return !(*this < x);
    }

    /**
     * 求并集
     */
    self_type operator+(const self_type& x) const noexcept
    {
        return *this | x;
    }

    /**
     * 求补集
     *
     * {...} - {...}
     */
    self_type operator-(const self_type& x) const noexcept
    {
        self_type ret;
        size_t index1 = 0, index2 = 0;
        AxisState state = AxisState::None;
        int_type first_of_remainder = 0;
        while (index1 / 2 < _ranges.size() || index2 / 2 < x._ranges.size())
        {
            int_type value1;
            if (index1 / 2 < _ranges.size())
            {
                const Range& rg1 = _ranges.at(index1 / 2);
                value1 = (0 == (index1 % 2) ? rg1.first : rg1.last);
            }
            else
            {
                value1 = x._ranges.at(x._ranges.size() - 1).last + 1; // Same effect as max integer
            }

            int_type value2;
            if (index2 / 2 < x._ranges.size())
            {
                const Range& rg2 = x._ranges.at(index2 / 2);
                value2 = (0 == (index2 % 2) ? rg2.first : rg2.last);
            }
            else
            {
                value2 = _ranges.at(_ranges.size() - 1).last + 1; // Same effect as max integer
            }

            // 对于边界重叠的情况，优先进入状态 AAndB 和 SingleB
            switch (state)
            {
            case AxisState::None:
                assert(0 == (index1 % 2) && 0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleA;
                    ++index1;
                    first_of_remainder = value1;
                }
                else
                {
                    state = AxisState::SingleB;
                    ++index2;
                }
                break;

            case AxisState::SingleA:
                assert(1 == (index1 % 2) && 0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::None;
                    ++index1;
                    ret._ranges.emplace_back(first_of_remainder, value1);
                }
                else
                {
                    state = AxisState::AAndB;
                    ++index2;
                    ret._ranges.emplace_back(first_of_remainder, value2 - 1);
                }
                break;

            case AxisState::SingleB:
                assert(0 == (index1 % 2) && 1 == (index2 % 2));
                if (value1 <= value2)
                {
                    state = AxisState::AAndB;
                    ++index1;
                }
                else
                {
                    state = AxisState::None;
                    ++index2;
                }
                break;

            case AxisState::AAndB:
                assert(1 == (index1 % 2) && 1 == (index2 % 2));
                if (value1 <= value2)
                {
                    state = AxisState::SingleB;
                    ++index1;
                }
                else
                {
                    state = AxisState::SingleA;
                    ++index2;
                    first_of_remainder = value2 + 1;
                }
                break;

            default:
                assert(false); // Illegal state
            }
        }
        return ret;
    }

    /**
     * 求并集
     */
    self_type operator|(const self_type& x) const noexcept
    {
        // 状态机基本上和求交集方法中一样
        self_type ret;
        size_t index1 = 0, index2 = 0;
        AxisState state = AxisState::None;
        int_type first_of_merge = 0;
        while (index1 / 2 < _ranges.size() || index2 / 2 < x._ranges.size())
        {
            int_type value1;
            if (index1 / 2 < _ranges.size())
            {
                const Range& rg1 = _ranges.at(index1 / 2);
                value1 = (0 == (index1 % 2) ? rg1.first : rg1.last);
            }
            else
            {
                value1 = x._ranges.at(x._ranges.size() - 1).last + 1; // Same effect as max integer
            }

            int_type value2;
            if (index2 / 2 < x._ranges.size())
            {
                const Range& rg2 = x._ranges.at(index2 / 2);
                value2 = (0 == (index2 % 2) ? rg2.first : rg2.last);
            }
            else
            {
                value2 = _ranges.at(_ranges.size() - 1).last + 1; // Same effect as max integer
            }

            // 对于边界重叠的情况，优先进入状态 AAndB
            switch (state)
            {
            case AxisState::None:
                assert(0 == (index1 % 2) && 0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleA;
                    ++index1;
                    first_of_merge = value1;
                }
                else
                {
                    state = AxisState::SingleB;
                    ++index2;
                    first_of_merge = value2;
                }
                break;

            case AxisState::SingleA:
                assert(1 == (index1 % 2) && 0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::None;
                    ++index1;
                    if (ret._ranges.size() > 0 &&
                        ret._ranges.at(ret._ranges.size() - 1).last + 1 == first_of_merge)
                        ret._ranges[ret._ranges.size() - 1].last = value1;
                    else
                        ret._ranges.emplace_back(first_of_merge, value1);
                }
                else
                {
                    state = AxisState::AAndB;
                    ++index2;
                }
                break;

            case AxisState::SingleB:
                assert(0 == (index1 % 2) && 1 == (index2 % 2));
                if (value1 <= value2)
                {
                    state = AxisState::AAndB;
                    ++index1;
                }
                else
                {
                    state = AxisState::None;
                    ++index2;
                    if (ret._ranges.size() > 0 &&
                        ret._ranges.at(ret._ranges.size() - 1).last + 1 == first_of_merge)
                        ret._ranges[ret._ranges.size() - 1].last = value2;
                    else
                        ret._ranges.emplace_back(first_of_merge, value2);
                }
                break;

            case AxisState::AAndB:
                assert(1 == (index1 % 2) && 1 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleB;
                    ++index1;
                }
                else
                {
                    state = AxisState::SingleA;
                    ++index2;
                }
                break;

            default:
                assert(false); // Illegal state
            }
        }
        return ret;
    }

    /**
     * 求交集
     *
     * 例如：
     *   容器 [(1,3),(5,10),(13,24)]
     *   容器 [(2,13),(15,100)]
     *   交集 [(2,3),(5,10),13,(15,24)]
     */
    self_type operator&(const self_type& x) const noexcept
    {
        self_type ret;
        size_t index1 = 0, index2 = 0;
        AxisState state = AxisState::None;
        int_type first_of_interact = 0;
        while (index1 / 2 < _ranges.size() && index2 / 2 < x._ranges.size())
        {
            const Range& rg1 = _ranges.at(index1 / 2);
            const int_type value1 = (0 == (index1 % 2) ? rg1.first : rg1.last);

            const Range& rg2 = x._ranges.at(index2 / 2);
            const int_type value2 = (0 == (index2 % 2) ? rg2.first : rg2.last);

            // 对于边界重叠的情况，优先进入状态 AAndB
            switch (state)
            {
            case AxisState::None:
                assert(0 == (index1 % 2) && 0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleA;
                    ++index1;
                }
                else
                {
                    state = AxisState::SingleB;
                    ++index2;
                }
                break;

            case AxisState::SingleA:
                assert(1 == (index1 % 2) && 0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::None;
                    ++index1;
                }
                else
                {
                    state = AxisState::AAndB;
                    ++index2;
                    first_of_interact = value2;
                }
                break;

            case AxisState::SingleB:
                assert(0 == (index1 % 2) && 1 == (index2 % 2));
                if (value1 <= value2)
                {
                    state = AxisState::AAndB;
                    ++index1;
                    first_of_interact = value1;
                }
                else
                {
                    state = AxisState::None;
                    ++index2;
                }
                break;

            case AxisState::AAndB:
                assert(1 == (index1 % 2) && 1 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleB;
                    ++index1;
                    ret._ranges.emplace_back(first_of_interact, value1);
                }
                else
                {
                    state = AxisState::SingleA;
                    ++index2;
                    ret._ranges.emplace_back(first_of_interact, value2);
                }
                break;

            default:
                assert(false); // Illegal state
            }
        }
        return ret;
    }

    /**
     * 求两集合相互补集的并集，eg. A ^ B = (A - B) | (B - A)
     */
    self_type operator^(const self_type& x) const noexcept
    {
        self_type ret;
        size_t index1 = 0, index2 = 0;
        AxisState state = AxisState::None;
        int_type first_of_remainder = 0;
        while (index1 / 2 < _ranges.size() || index2 / 2 < x._ranges.size())
        {
            int_type value1;
            if (index1 / 2 < _ranges.size())
            {
                const Range& rg1 = _ranges.at(index1 / 2);
                value1 = (0 == (index1 % 2) ? rg1.first : rg1.last);
            }
            else
            {
                value1 = x._ranges.at(x._ranges.size() - 1).last + 1; // Same effect as max integer
            }

            int_type value2;
            if (index2 / 2 < x._ranges.size())
            {
                const Range& rg2 = x._ranges.at(index2 / 2);
                value2 = (0 == (index2 % 2) ? rg2.first : rg2.last);
            }
            else
            {
                value2 = _ranges.at(_ranges.size() - 1).last + 1; // Same effect as max integer
            }

            switch (state)
            {
            case AxisState::None:
                assert(0 == (index1 % 2) && 0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleA;
                    ++index1;
                    first_of_remainder = value1;
                }
                else if (value1 > value2)
                {
                    state = AxisState::SingleB;
                    ++index2;
                    first_of_remainder = value2;
                }
                else
                {
                    state = AxisState::AAndB;
                    ++index1;
                    ++index2;
                }
                break;

            case AxisState::SingleA:
                assert(1 == (index1 % 2) && 0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::None;
                    ++index1;
                    ret._ranges.emplace_back(first_of_remainder, value1);
                }
                else
                {
                    state = AxisState::AAndB;
                    ++index2;
                    ret._ranges.emplace_back(first_of_remainder, value2 - 1);
                }
                break;

            case AxisState::SingleB:
                assert(0 == (index1 % 2) && 1 == (index2 % 2));
                if (value1 > value2)
                {
                    state = AxisState::None;
                    ++index2;
                    ret._ranges.emplace_back(first_of_remainder, value2);
                }
                else
                {
                    state = AxisState::AAndB;
                    ++index1;
                    ret._ranges.emplace_back(first_of_remainder, value1 - 1);
                }
                break;

            case AxisState::AAndB:
                assert(1 == (index1 % 2) && 1 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleB;
                    ++index1;
                    first_of_remainder = value1 + 1;
                }
                else if (value1 > value2)
                {
                    state = AxisState::SingleA;
                    ++index2;
                    first_of_remainder = value2 + 1;
                }
                else
                {
                    state = AxisState::None;
                    ++index1;
                    ++index2;
                }
                break;

            default:
                assert(false); // Illegal state
            }
        }
        return ret;
    }

    /**
     * 求并集
     */
    self_type& operator+=(const self_type& x) noexcept
    {
        *this = *this + x;
        return *this;
    }
    
    /**
     * 求补集
     */
    self_type& operator-=(const self_type& x) noexcept
    {
        *this = *this - x;
        return *this;
    }

    /**
     * 求并集
     */
    self_type& operator|=(const self_type& x) noexcept
    {
        *this = *this | x;
        return *this;
    }
    
    /**
     * 求交集
     */
    self_type& operator&=(const self_type& x) noexcept
    {
        *this = *this & x;
        return *this;
    }

    /**
     * 求两集合相互补集的并集
     */
    self_type& operator^=(const self_type& x) noexcept
    {
        *this = *this ^ x;
        return *this;
    }

    int compare(const self_type& x) const noexcept
    {
        if (this == &x)
            return 0;

        const size_t lsz = _ranges.size(), rsz = x._ranges.size();
        size_t i = 0;
        for (; i < lsz && i < rsz; ++i)
        {
            const Range& rg1 = _ranges.at(i);
            const Range& rg2 = x._ranges.at(i);
            if (rg1.first != rg2.first)
                return rg1.first < rg2.first ? -1 : 1;
            else if (rg1.last < rg2.last)
                return i + 1 >= lsz ? -1 : 1;
            else if (rg1.last > rg2.last)
                return i + 1 < rsz ? -1 : 1;
        }
        return i < lsz ? 1 : (i < rsz ? -1 : 0);
    }

    void add_value(int_type value) noexcept
    {
        add_value_range(value, value);
    }

    void add_value_range(int_type first_value, int_type last_value) noexcept
    {
        assert(first_value <= last_value);

        // 对空容器优化
        if (_ranges.empty())
        {
            _ranges.emplace_back(first_value, last_value);
            return;
        }

        // 对于头部进行优化
        Range& head = _ranges[0];
        if (last_value + 1 < head.first)
        {
            _ranges.emplace(_ranges.begin(), first_value, last_value);
            return;
        }
        else if (last_value <= head.last)
        {
            if (first_value >= head.first)
                return;
            head.first = first_value;
            return;
        }

        // 对于末尾进行优化
        Range& tail = _ranges[_ranges.size() - 1];
        if (first_value > tail.last + 1)
        {
            _ranges.emplace_back(first_value, last_value);
            return;
        }
        else if (first_value >= tail.last)
        {
            if (last_value <= tail.last)
                return;
            tail.last = last_value;
            return;
        }

        // 二分查找法确定可以合并的 range 范围
        ssize_t i1 = binary_search(first_value - 1), i2 = binary_search(last_value + 1);
        if (i1 < 0)
            i1 = -i1 - 1;
        if (i2 < 0)
            i2 = -i2 - 2;

        if (i1 <= i2)
        {
            const int_type min_left = std::min(first_value, _ranges.at(i1).first);
            const int_type max_right = std::max(last_value, _ranges.at(i2).last);
            _ranges.erase(_ranges.begin() + i1, _ranges.begin() + i2 + 1);
            _ranges.emplace(_ranges.begin() + i1, min_left, max_right);
        } else {
            _ranges.emplace(_ranges.begin() + i1, first_value, last_value);
        }
    }

    // 查询指定整数是否在该容器中
    bool contains(int_type value) noexcept
    {
        return binary_search(value) >= 0;
    }

    void remove_value(int_type value) noexcept
    {
        remove_value_range(value, value);
    }

    void remove_value_range(int_type first_value, int_type last_value) noexcept
    {
        assert(first_value <= last_value);

        // 二分查找法确定范围
        ssize_t i1 = binary_search(first_value), i2 = binary_search(last_value);
        if (i1 < 0)
            i1 = -i1 - 1;
        if (i2 < 0)
            i2 = -i2 - 2;
        if (i1 < i2)
        {
            Range *range = &_ranges[i1];
            if (range->first < first_value)
            {
                range->last = first_value - 1;
                ++i1;
            }
            range = &_ranges[i2];
            if (range->last > last_value)
            {
                range->first = last_value + 1;
                --i2;
            }
            if (i1 <= i2)
                _ranges.erase(_ranges.begin() + i1, _ranges.begin() + i2 + 1);
            return;
        }
        else if (i1 == i2)
        {
            Range& range = _ranges[i1];
            if (first_value <= range.first && last_value >= range.last)
            {
                _ranges.erase(_ranges.begin() + i1);
                return;
            }
            else if (first_value > range.first && last_value < range.last)
            {
                const int_type old_last = range.last;
                range.last = first_value - 1;
                // NOTE Reference to variable 'range' is invalid after insertion!!
                _ranges.emplace(_ranges.begin() + i1 + 1, last_value + 1, old_last);
                return;
            }
            else if (first_value <= range.first)
            {
                range.first = last_value + 1;
                return;
            }
            else
            {
                range.last = first_value - 1;
                return;
            }
        }
    }

    void clear() noexcept
    {
        _ranges.clear();
    }

    size_t size_of_values() const noexcept
    {
        size_t ret = 0;
        for (size_t i = 0, sz = _ranges.size(); i < sz; ++i)
        {
            const Range& rg = _ranges.at(i);
            assert(rg.first <= rg.last);
            ret += rg.last - rg.first + 1;
        }
        return ret;
    }

    bool empty() const noexcept
    {
        return _ranges.empty();
    }

    int_type get_first_value() const noexcept
    {
        assert(!_ranges.empty());
        return _ranges.at(0).first;
    }

    int_type get_last_value() const noexcept
    {
        assert(!_ranges.empty());
        return _ranges.at(_ranges.size() - 1).last;
    }

    ssize_t index_of(int_type value) const noexcept
    {
        size_t index = 0;
        for (size_t i = 0, sz = _ranges.size(); i < sz; ++i)
        {
            const Range& rg = _ranges.at(i);
            assert(rg.first <= rg.last);

            if (value < rg.first)
                return -1;

            if (value > rg.last)
            {
                index += rg.last - rg.first + 1;
                continue;
            }

            index += value - rg.first;
            return index;
        }
        return -1;
    }

    int_type value_at(size_t index) const noexcept
    {
        for (size_t i = 0, sz = _ranges.size(); i < sz; ++i)
        {
            const Range& rg = _ranges.at(i);
            assert(rg.first <= rg.last);

            if (index <= rg.last - rg.first)
                return rg.first + index;
            index -= rg.last - rg.first + 1;
        }
        // Index out of range
        assert(false);
    }

    const_iterator begin() const noexcept
    {
        if (_ranges.empty())
            return const_iterator(this, 0, 0);
        return const_iterator(this, _ranges.at(0).first, 0);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(this, 0, _ranges.size());
    }

    const_iterator value_iterator(int_type value) const noexcept
    {
        // 用二分法找到迭代的起止点
        ssize_t left = -1, right = _ranges.size();
        while (left + 1 < right)
        {
            ssize_t middle = (left + right) / 2;
            const Range& rg = _ranges.at(middle);
            if (value < rg.first)
            {
                right = middle;
            }
            else if (value > rg.last)
            {
                left = middle;
            }
            else
            {
                right = middle;
                break;
            }
        }
        const ssize_t first_range_index = right;
        if (first_range_index < (ssize_t) _ranges.size())
        {
            const Range& rg = _ranges.at(first_range_index);
            if (value < rg.first)
                value = rg.first;
        }
        return const_iterator(this, value, first_range_index);
    }

    size_t size_of_ranges() const noexcept
    {
        return _ranges.size();
    }

    const Range& range_at(size_t index) const noexcept
    {
        return _ranges.at(index);
    }

    std::string to_string() const noexcept
    {
        std::string s("{");
        for (size_t i = 0, sz = _ranges.size(); i < sz; ++i)
        {
            const Range& rg = _ranges.at(i);
            if (rg.first == rg.last)
            {
                s += int_to_str(rg.first);
            }
            else
            {
                s.push_back('[');
                s += int_to_str(rg.first);
                s.push_back(',');
                s += int_to_str(rg.last);
                s.push_back(']');
            }
            if (i + 1 < sz)
                s.push_back(',');
        }
        s.push_back('}');
        return s;
    }

    std::wstring to_wstring() const noexcept
    {
        std::wstring s(L"{");
        for (size_t i = 0, sz = _ranges.size(); i < sz; ++i)
        {
            const Range& rg = _ranges.at(i);
            if (rg.first == rg.last)
            {
                s += int_to_wstr(rg.first);
            }
            else
            {
                s.push_back(L'[');
                s += int_to_wstr(rg.first);
                s.push_back(L',');
                s += int_to_wstr(rg.last);
                s.push_back(L']');
            }
            if (i + 1 < sz)
                s.push_back(L',');
        }
        s.push_back(L'}');
        return s;
    }

private:
    /**
     * 使用值做二分查找，如果找到则返回找到的位置(>=0)，否则返回 (-insertPoint-1)，
     * insertPoint 是用来做插入的位置
     *
     * @return 找到则返回 >=0，否则 <0
     */
    ssize_t binary_search(int_type value) noexcept
    {
        ssize_t left = -1, right = _ranges.size();
        while (left + 1 < right)
        {
            ssize_t middle = (left + right) / 2;
            const Range& range = _ranges.at(middle);
            if (value > range.last)
                left = middle;
            else if (value < range.first)
                right = middle;
            else
                return middle;
        }
        return -(right + 1);
    }

private:
    std::vector<Range> _ranges;
};

}

#endif
