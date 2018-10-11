
#ifndef ___HEADFILE_C24BA2B9_EF7B_4DD5_A131_C61E1FC45B87_
#define ___HEADFILE_C24BA2B9_EF7B_4DD5_A131_C61E1FC45B87_

#include <assert.h>
#include <stddef.h> // for ptrdiff_t
#include <vector>
#include <iterator>
#include <algorithm> // for std::min() and so on

#include <nut/platform/int_type.h> // for ssize_t in windows VC

namespace nut
{

/**
 * An ascending set of integers, like
 * [(1,3) (6,6) (8,9)] contains 1 2 3 6 8 9
 */
template <typename IntType = int>
class IntegerSet
{
private:
    typedef IntegerSet<IntType> self_type;

public:
    typedef IntType int_type;

    class Range
    {
    public:
        Range() = default;

        Range(int_type f, int_type l)
            : first(f), last(l)
        {
            assert(f <= l);
        }

        bool operator==(const Range& x) const
        {
            return first == x.first && last == x.last;
        }

        bool operator!=(const Range& x) const
        {
            return !(*this == x);
        }

        void set(int_type f, int_type l)
        {
            first = f;
            last = l;
        }

    public:
        int_type first = 0, last = 0;
    };

    class const_iterator
    {
        const self_type *_container = nullptr;
        int_type _value = 0;
        ssize_t _index = 0;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef int_type                        value_type;
        typedef ptrdiff_t                       difference_type;
        typedef int_type                        reference;       // FIXME 这里实际上无法返回引用
        typedef int_type*                       pointer;

    public:
        const_iterator(const self_type *container, int_type value, ssize_t index)
            : _container(container), _value(value), _index(index)
        {
            assert(nullptr != _container);
        }

        int_type operator*() const
        {
            assert(nullptr != _container);
            assert(0 <= _index && _index < (ssize_t) _container->_ranges.size());
            return _value;
        }

        const_iterator& operator++()
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

        const_iterator& operator--()
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

        const_iterator operator++(int)
        {
            const_iterator ret = *this;
            ++*this;
            return ret;
        }

        const_iterator operator--(int)
        {
            const_iterator ret = *this;
            --*this;
            return ret;
        }

        bool operator==(const const_iterator& x) const
        {
            assert(nullptr != _container && _container == x._container);
            if (_index != x._index)
                return false;
            if (0 <= _index && _index < (ssize_t) _container->_ranges.size())
                return _value == x._value;
            return true;
        }

        bool operator!=(const const_iterator& x) const
        {
            return !(*this == x);
        }
    };

private:
    /**
     * 将两个集合 x、y 放到数据轴上，每一个轴坐标的状态
     *
     *       None  SingleX    SingleY   XAndY
     *    x        ********            *******
     *    y                  *****************
     *  ------------------------------------------> axis
     */
    enum class AxisState
    {
        None,        // none
        SingleX,     // single x
        SingleY,     // single y
        XAndY        // x and y
    };

public:
    IntegerSet() = default;

    bool operator==(const self_type& x) const
    {
        if (this == &x)
            return true;
        if (_ranges.size() != x._ranges.size())
            return false;
        for (size_t i = 0, sz = _ranges.size(); i < sz; ++i)
        {
            if (_ranges.at(i) != x._ranges.at(i))
                return false;
        }
        return true;
    }

    bool operator!=(const self_type& x) const
    {
        return !(*this == x);
    }

    void add_value(int_type value)
    {
        add_value_range(value, value);
    }

    void add_value_range(int_type first_value, int_type last_value)
    {
        assert(first_value <= last_value);

        // 对空容器优化
        if (_ranges.empty())
        {
            _ranges.push_back(Range(first_value, last_value));
            return;
        }

        // 对于头部进行优化
        Range& head = _ranges[0];
        if (last_value + 1 < head.first)
        {
            _ranges.insert(_ranges.begin(), Range(first_value, last_value));
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
            _ranges.push_back(Range(first_value, last_value));
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
            _ranges.insert(_ranges.begin() + i1, Range(min_left, max_right));
        } else {
            _ranges.insert(_ranges.begin() + i1, Range(first_value, last_value));
        }
    }

    // 查询指定整数是否在该容器中
    bool contains(int_type value)
    {
        return binary_search(value) >= 0;
    }

    void remove_value(int_type value)
    {
        remove_value_range(value, value);
    }

    void remove_value_range(int_type first_value, int_type last_value)
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
                _ranges.insert(_ranges.begin() + i1 + 1, Range(last_value + 1, old_last));
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

    void clear()
    {
        _ranges.clear();
    }

    size_t size_of_values() const
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

    bool empty() const
    {
        return _ranges.empty();
    }

    int_type get_first_value() const
    {
        assert(!_ranges.empty());
        return _ranges.at(0).first;
    }

    int_type get_last_value() const
    {
        assert(!_ranges.empty());
        return _ranges.at(_ranges.size() - 1).last;
    }

    ssize_t index_of(int_type value) const
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

    int_type value_at(size_t index) const
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

    const_iterator begin() const
    {
        if (_ranges.empty())
            return const_iterator(this, 0, 0);
        return const_iterator(this, _ranges.at(0).first, 0);
    }

    const_iterator end() const
    {
        return const_iterator(this, 0, _ranges.size());
    }

    const_iterator value_iterator(int_type value) const
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

    size_t size_of_ranges() const
    {
        return _ranges.size();
    }

    const Range& range_at(size_t index) const
    {
        return _ranges.at(index);
    }

    self_type intersect_with(const self_type& x) const
    {
        self_type result;
        intersect(*this, x, &result);
        return result;
    }

    self_type merge_with(const self_type& x) const
    {
        self_type result;
        merge(*this, x, &result);
        return result;
    }

    self_type remainder_with(const self_type& x) const
    {
        self_type result;
        remainder(*this, x, &result);
        return result;
    }

    /**
     * 两个容器做交集
     * 例如
     * 容器 [(1,3),(5,10),(13,24)]
     * 容器 [(2,13),(15,100)]
     * 交集 [(2,3),(5,10),13,(15,24)]
     */
    static void intersect(const self_type& x, const self_type& y, self_type *result)
    {
        assert(nullptr != result && result != &x && result != &y);
        result->clear();

        size_t index1 = 0, index2 = 0;
        AxisState state = AxisState::None;
        int_type first_of_interact = 0;
        while (index1 / 2 < x._ranges.size() && index2 / 2 < y._ranges.size())
        {
            const Range& rg1 = x._ranges.at(index1 / 2);
            const int_type value1 = (0 == (index1 % 2) ? rg1.first : rg1.last);

            const Range& rg2 = y._ranges.at(index2 / 2);
            const int_type value2 = (0 == (index2 % 2) ? rg2.first : rg2.last);

            // 对于边界重叠的情况，优先进入状态 XAndY
            switch (state)
            {
            case AxisState::None:
                assert(0 == (index1 % 2));
                assert(0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleX;
                    ++index1;
                }
                else
                {
                    state = AxisState::SingleY;
                    ++index2;
                }
                break;

            case AxisState::SingleX:
                assert(1 == (index1 % 2));
                assert(0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::None;
                    ++index1;
                }
                else
                {
                    state = AxisState::XAndY;
                    ++index2;
                    first_of_interact = value2;
                }
                break;

            case AxisState::SingleY:
                assert(0 == (index1 % 2));
                assert(1 == (index2 % 2));
                if (value1 <= value2)
                {
                    state = AxisState::XAndY;
                    ++index1;
                    first_of_interact = value1;
                }
                else
                {
                    state = AxisState::None;
                    ++index2;
                }
                break;

            case AxisState::XAndY:
                assert(1 == (index1 % 2));
                assert(1 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleY;
                    ++index1;
                    result->_ranges.push_back(Range(first_of_interact, value1));
                }
                else
                {
                    state = AxisState::SingleX;
                    ++index2;
                    result->_ranges.push_back(Range(first_of_interact, value2));
                }
                break;

            default:
                assert(false); // Illegal state
            }
        }
    }

    /**
     * 两个容器做并集
     */
    static void merge(const self_type& x, const self_type& y, self_type *result)
    {
        assert(nullptr != result && result != &x && result != &y);
        result->clear();

        // 状态机基本上和intersectWith()方法中一样
        size_t index1 = 0, index2 = 0;
        AxisState state = AxisState::None;
        int_type first_of_merge = 0;
        while (index1 / 2 < x._ranges.size() || index2 / 2 < y._ranges.size())
        {
            int_type value1;
            if (index1 / 2 < x._ranges.size())
            {
                const Range& rg1 = x._ranges.at(index1 / 2);
                value1 = (0 == (index1 % 2) ? rg1.first : rg1.last);
            }
            else
            {
                value1 = y._ranges.at(y._ranges.size() - 1).last + 1; // Same effect as max integer
            }

            int_type value2;
            if (index2 / 2 < y._ranges.size())
            {
                const Range& rg2 = y._ranges.at(index2 / 2);
                value2 = (0 == (index2 % 2) ? rg2.first : rg2.last);
            }
            else
            {
                value2 = x._ranges.at(x._ranges.size() - 1).last + 1; // Same effect as max integer
            }

            // 对于边界重叠的情况，优先进入状态 XAndY
            switch (state)
            {
            case AxisState::None:
                assert(0 == (index1 % 2));
                assert(0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleX;
                    ++index1;
                    first_of_merge = value1;
                }
                else
                {
                    state = AxisState::SingleY;
                    ++index2;
                    first_of_merge = value2;
                }
                break;

            case AxisState::SingleX:
                assert(1 == (index1 % 2));
                assert(0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::None;
                    ++index1;
                    if (result->_ranges.size() > 0 && result->_ranges.at(result->_ranges.size() - 1).last + 1 == first_of_merge)
                        result->_ranges[result->_ranges.size() - 1].last = value1;
                    else
                        result->_ranges.push_back(Range(first_of_merge, value1));
                }
                else
                {
                    state = AxisState::XAndY;
                    ++index2;
                }
                break;

            case AxisState::SingleY:
                assert(0 == (index1 % 2));
                assert(1 == (index2 % 2));
                if (value1 <= value2)
                {
                    state = AxisState::XAndY;
                    ++index1;
                }
                else
                {
                    state = AxisState::None;
                    ++index2;
                    if (result->_ranges.size() > 0 && result->_ranges.at(result->_ranges.size() - 1).last + 1 == first_of_merge)
                        result->_ranges[result->_ranges.size() - 1].last = value2;
                    else
                        result->_ranges.push_back(Range(first_of_merge, value2));
                }
                break;

            case AxisState::XAndY:
                assert(1 == (index1 % 2));
                assert(1 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleY;
                    ++index1;
                }
                else
                {
                    state = AxisState::SingleX;
                    ++index2;
                }
                break;

            default:
                assert(false); // Illegal state
            }
        }
    }

    /**
     * 两个容器做补集
     * {x} - {y}
     */
    static void remainder(const self_type& x, const self_type& y, self_type *result)
    {
        assert(nullptr != result && result != &x && result != &y);
        result->clear();

        size_t index1 = 0, index2 = 0;
        AxisState state = AxisState::None;
        int_type first_of_remainder = 0;
        while (index1 / 2 < x._ranges.size() || index2 / 2 < y._ranges.size())
        {
            int_type value1;
            if (index1 / 2 < x._ranges.size())
            {
                const Range& rg1 = x._ranges.at(index1 / 2);
                value1 = (0 == (index1 % 2) ? rg1.first : rg1.last);
            }
            else
            {
                value1 = y._ranges.at(y._ranges.size() - 1).last + 1; // Same effect as max integer
            }

            int_type value2;
            if (index2 / 2 < y._ranges.size())
            {
                const Range& rg2 = y._ranges.at(index2 / 2);
                value2 = (0 == (index2 % 2) ? rg2.first : rg2.last);
            }
            else
            {
                value2 = x._ranges.at(x._ranges.size() - 1).last + 1; // Same effect as max integer
            }

            // 对于边界重叠的情况，优先进入状态 XAndY 和 SingleY
            switch (state)
            {
            case AxisState::None:
                assert(0 == (index1 % 2));
                assert(0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::SingleX;
                    ++index1;
                    first_of_remainder = value1;
                }
                else
                {
                    state = AxisState::SingleY;
                    ++index2;
                }
                break;

            case AxisState::SingleX:
                assert(1 == (index1 % 2));
                assert(0 == (index2 % 2));
                if (value1 < value2)
                {
                    state = AxisState::None;
                    ++index1;
                    result->_ranges.push_back(Range(first_of_remainder, value1));
                }
                else
                {
                    state = AxisState::XAndY;
                    ++index2;
                    result->_ranges.push_back(Range(first_of_remainder, value2 - 1));
                }
                break;

            case AxisState::SingleY:
                assert(0 == (index1 % 2));
                assert(1 == (index2 % 2));
                if (value1 <= value2)
                {
                    state = AxisState::XAndY;
                    ++index1;
                }
                else
                {
                    state = AxisState::None;
                    ++index2;
                }
                break;

            case AxisState::XAndY:
                assert(1 == (index1 % 2));
                assert(1 == (index2 % 2));
                if (value1 <= value2)
                {
                    state = AxisState::SingleY;
                    ++index1;
                }
                else
                {
                    state = AxisState::SingleX;
                    ++index2;
                    first_of_remainder = value2 + 1;
                }
                break;

            default:
                assert(false); // Illegal state
            }
        }
    }

private:
    /**
     * 使用值做二分查找，如果找到则返回找到的位置(>=0)，否则返回 (-insertPoint-1)，
     * insertPoint 是用来做插入的位置
     *
     * @return 找到则返回 >=0，否则 <0
     */
    ssize_t binary_search(int_type value)
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
