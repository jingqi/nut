
#include <iostream>

#include <nut/unittest/unittest.h>

#include <nut/container/integer_set.h>
#include <nut/util/string/to_string.h>

using namespace std;
using namespace nut;

static string to_string(const IntegerSet<int> s)
{
    string sb("[");
    for (size_t i = 0, sz = s.size_of_ranges(); i < sz; ++i)
    {
        const IntegerSet<int>::Range& rg = s.range_at(i);
        if (rg.first == rg.last)
        {
            sb += int_to_str(rg.first);
        }
        else
        {
            sb += "(";
            sb += int_to_str(rg.first);
            sb += ",";
            sb += int_to_str(rg.last);
            sb += ")";
        }
        if (i + 1 < sz)
            sb += ",";
    }
    sb += "]";
    return sb;
}

class TestIntegerSet : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
        NUT_REGISTER_CASE(test_size);
        NUT_REGISTER_CASE(test_add_range);
        NUT_REGISTER_CASE(test_remove_range);
        NUT_REGISTER_CASE(test_intersect);
        NUT_REGISTER_CASE(test_merge);
        NUT_REGISTER_CASE(test_remainder);
        NUT_REGISTER_CASE(test_iterator);
    }

    void test_smoking()
    {
        IntegerSet<int> s;
        s.add_value(1);
        s.add_value_range(4,10);
        NUT_TA(s.contains(1));
        NUT_TA(!s.contains(2));
        NUT_TA(s.contains(6));
    }

    void test_size()
    {
        IntegerSet<int> s;
        s.add_value_range(1,2);
        s.add_value_range(12, 26);
        NUT_TA(s.size_of_values() == 17);
        NUT_TA(s.size_of_ranges() == 2);
    }

    void check_add_one_range(int first, int last, const char *expected)
    {
        assert(nullptr != expected);

        IntegerSet<int> l;
        l.add_value_range(4, 6);
        l.add_value_range(12, 15);
        NUT_TA(to_string(l) == "[(4,6),(12,15)]");

        l.add_value_range(first, last);
        NUT_TA(to_string(l) == expected);
    }

    void test_add_range()
    {
        // 在头部添加
        //      原始： [(4,6),(12,15)]
        check_add_one_range(1, 2, "[(1,2),(4,6),(12,15)]");
        check_add_one_range(1, 3, "[(1,6),(12,15)]");
        check_add_one_range(1, 5, "[(1,6),(12,15)]");
        check_add_one_range(1, 7, "[(1,7),(12,15)]");
        check_add_one_range(5, 5, "[(4,6),(12,15)]");
        check_add_one_range(5, 7, "[(4,7),(12,15)]");
        check_add_one_range(5, 11, "[(4,15)]");
        check_add_one_range(5, 12, "[(4,15)]");
        check_add_one_range(5, 15, "[(4,15)]");
        check_add_one_range(5, 16, "[(4,16)]");
        check_add_one_range(4, 15, "[(4,15)]");

        // 在空隙添加
        check_add_one_range(8, 10, "[(4,6),(8,10),(12,15)]");
        check_add_one_range(8, 11, "[(4,6),(8,15)]");

        // 在尾部添加
        check_add_one_range(13, 16, "[(4,6),(12,16)]");
        check_add_one_range(13, 15, "[(4,6),(12,15)]");
        check_add_one_range(16, 17, "[(4,6),(12,17)]");
        check_add_one_range(17, 18, "[(4,6),(12,15),(17,18)]");
        check_add_one_range(1, 20, "[(1,20)]");
    }

    void check_remove_one_range(int first, int last, const char *expected)
    {
        assert(nullptr != expected);

        IntegerSet<int> l;
        l.add_value_range(4, 6);
        l.add_value_range(11, 16);
        l.add_value_range(26, 28);
        l.add_value_range(32, 34);
        NUT_TA(to_string(l) == "[(4,6),(11,16),(26,28),(32,34)]");

        l.remove_value_range(first, last);
        if (to_string(l) != expected)
            cout << to_string(l) << " " << expected << endl;
        NUT_TA(to_string(l) == expected);
    }

    void test_remove_range()
    {
        // 初始 [(4,6),(11,16),(26,28),(32,34)]
        /*check_remove_one_range(1, 2, "[(4,6),(11,16),(26,28),(32,34)]");
        check_remove_one_range(1, 4, "[(5,6),(11,16),(26,28),(32,34)]");
        check_remove_one_range(1, 6, "[(11,16),(26,28),(32,34)]");
        check_remove_one_range(5, 6, "[4,(11,16),(26,28),(32,34)]");*/
        check_remove_one_range(5, 5, "[4,6,(11,16),(26,28),(32,34)]");
        check_remove_one_range(5, 12, "[4,(13,16),(26,28),(32,34)]");
        check_remove_one_range(7, 8, "[(4,6),(11,16),(26,28),(32,34)]");
        check_remove_one_range(7, 16, "[(4,6),(26,28),(32,34)]");
        check_remove_one_range(7, 32, "[(4,6),(33,34)]");
        check_remove_one_range(11, 31, "[(4,6),(32,34)]");
        check_remove_one_range(12, 13, "[(4,6),11,(14,16),(26,28),(32,34)]");
        check_remove_one_range(13, 16, "[(4,6),(11,12),(26,28),(32,34)]");
    }

    void test_intersect()
    {
        IntegerSet<int> x;
        x.add_value_range(1, 3);
        x.add_value_range(5, 10);
        x.add_value_range(13, 24); // [(1,3),(5,10),(13,24)]

        IntegerSet<int> y;
        y.add_value_range(2, 13);
        y.add_value_range(15, 100); // [(2,13),(15,100)]

        IntegerSet<int> rs = x.intersect_with(y);
        NUT_TA(to_string(rs) == "[(2,3),(5,10),13,(15,24)]");
    }

    void test_merge()
    {
        IntegerSet<int> x;
        x.add_value_range(1, 3);
        x.add_value_range(5, 10);
        x.add_value_range(13, 24); // [(1,3),(5,10),(13,24)]

        IntegerSet<int> y;
        y.add_value_range(2, 13);
        y.add_value_range(15, 100); // [(2,13),(15,100)]

        IntegerSet<int> rs = x.merge_with(y);
        NUT_TA(to_string(rs) == "[(1,100)]");
    }

    void test_remainder()
    {
        IntegerSet<int> x;
        x.add_value_range(1, 3);
        x.add_value_range(5, 10);
        x.add_value_range(13, 24); // [(1,3),(5,10),(13,24)]

        IntegerSet<int> y;
        y.add_value_range(2, 13);
        y.add_value_range(15, 100); // [(2,13),(15,100)]

        IntegerSet<int> rs = x.remainder_with(y);
        NUT_TA(to_string(rs) == "[1,14]");
    }

    void test_iterator()
    {
        IntegerSet<int> x;
        x.add_value_range(1,3);
        x.add_value_range(5,10);
        x.add_value_range(13,15);

        int i = 0;
        int arr[] = {2,3,5,6,7,8,9,10};
        for (IntegerSet<int>::const_iterator iter = x.value_iterator(2),
            end = x.value_iterator(11); iter != end; ++iter, ++i)
        {
            NUT_TA(arr[i] == *iter);
        }

        typedef reverse_iterator<IntegerSet<int>::const_iterator> riter_t;
        i = 7;
        for (riter_t iter = riter_t(x.value_iterator(11)),
            end = riter_t(x.value_iterator(2)); iter != end; ++iter, --i)
        {
            // cout << i << " " << arr[i] << " " << *iter << endl;
            NUT_TA(arr[i] == *iter);
        }
    }
};

NUT_REGISTER_FIXTURE(TestIntegerSet, "container, quiet")
