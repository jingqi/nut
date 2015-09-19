
#include <nut/unittest/unit_test.h>

#include <nut/container/array.h>

using namespace std;
using namespace nut;

typedef enrc<Array<int> > rcarray;

NUT_FIXTURE(TestArray)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_array_smoking)
    NUT_CASE(test_array_insert_erase)
    NUT_CASE(test_array_ma)
    NUT_CASE(test_cowarray)
    NUT_CASES_END()

    void test_array_smoking()
    {
        Array<int> a;
        NUT_TA(a.size() == 0);

        a.push_back(12);
        NUT_TA(a.size() == 1);
        NUT_TA(a.capacity() >= 1);
        NUT_TA(a.at(0) == 12);

        a.push_back(11);
        NUT_TA(a.size() == 2);
        NUT_TA(a.capacity() >= 2);
        NUT_TA(a.at(0) == 12);
        NUT_TA(a.at(1) == 11);

        NUT_TA(*a.begin() == 12);
        NUT_TA(*(a.end() - 1) == 11);
        NUT_TA(a.data()[0] == 12);
        NUT_TA(a.data()[1] == 11);

        a.pop_back();
        NUT_TA(a.size() == 1);
        NUT_TA(a.at(0) == 12);

        Array<int> b(a);
        NUT_TA(b.size() == 1);
        NUT_TA(a == b);
        NUT_TA(!(a != b));

        Array<int> c;
        NUT_TA(c.size() == 0);
        NUT_TA(!(c == a));
        NUT_TA(c != a);
        c = a;
        NUT_TA(c.size() == 1);
        NUT_TA(c == a);
    }

    void test_array_insert_erase()
    {
        Array<int> a;
        a.insert(0, 2);
        NUT_TA(a.size() == 1);
        NUT_TA(a.at(0) == 2);

        a.insert(0, 54);
        NUT_TA(a.size() == 2);
        NUT_TA(a.at(0) == 54 && a.at(1) == 2);

        a.insert(1, 85);
        NUT_TA(a.size() == 3);
        NUT_TA(a.at(0) == 54 && a.at(1) == 85 && a.at(2) == 2);

        a.erase(1);
        NUT_TA(a.size() == 2 && a.at(0) == 54 && a.at(1) == 2);

        a.erase(0, 2);
        NUT_TA(a.size() == 0);

        a.push_back(12);
        a.push_back(11);
        a.clear();
        NUT_TA(a.size() == 0 && a.begin() == a.end());

        Array<int> b;
        a.push_back(11);
        a.push_back(12);
        b.push_back(21);
        b.push_back(22);
        a.insert(1, b.begin(), b.end());
        NUT_TA(a.size() == 4);
        NUT_TA(a.at(0) == 11);
        NUT_TA(a.at(1) == 21);
        NUT_TA(a.at(2) == 22);
        NUT_TA(a.at(3) == 12);
    }

    void test_array_ma()
    {
        rc_ptr<sys_ma> ma = rc_new<sys_ma>();
        {
            Array<int> a(0, ma);
            NUT_TA(ma->get_ref() == 2);
            a.push_back(45);
            a.push_back(46);
        }
        NUT_TA(ma->get_ref() == 1);
    }

    void test_cowarray() // copy on write
    {
        COWArray<int> a;
        a.push_back(12);

        COWArray<int> b(a), c;
        c = a;
        NUT_TA(static_cast<const COWArray<int>& >(a).data() == static_cast<const COWArray<int>& >(b).data());
        NUT_TA(static_cast<const COWArray<int>& >(a).data() == static_cast<const COWArray<int>& >(c).data());
        a.push_back(12);
        NUT_TA(static_cast<const COWArray<int>& >(a).data() != static_cast<const COWArray<int>& >(b).data());
        NUT_TA(static_cast<const COWArray<int>& >(b).data() == static_cast<const COWArray<int>& >(c).data());
    }
};

NUT_REGISTER_FIXTURE(TestArray, "container, quiet")
