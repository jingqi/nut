
#include <nut/unittest/unittest.h>
#include <nut/container/comparable.h>

using namespace std;
using namespace nut;

class TestA
{
public:
    TestA(int m)
        : member(m)
    {}

    bool operator<(const TestA& x) const
    {
        return member < x.member;
    }

    int member;
};

class TestB
{
public:
    TestB(int m)
        : member(m)
    {}

    int compare(const TestB& x) const
    {
        return member - x.member;
    }

    int member;
};

class TestComparable : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        int a1 = 1, a2 = 2;
        NUT_TA(!has_compare_method<int>::value);
        NUT_TA(compare(a1, a2) < 0);
        NUT_TA(compare(a2, a2) == 0);
        NUT_TA(compare(a2, a1) > 0);

        float b1 = 2.1, b2 = 2.2;
        NUT_TA(!has_compare_method<float>::value);
        NUT_TA(compare(b1, b2) < 0);
        NUT_TA(compare(b2, b2) == 0);
        NUT_TA(compare(b2, b1) > 0);

        bool c1 = false, c2 = true;
        NUT_TA(!has_compare_method<bool>::value);
        NUT_TA(compare(c1, c2) < 0);
        NUT_TA(compare(c2, c2) == 0);
        NUT_TA(compare(c2, c1) > 0);

        TestA d1(1), d2(2);
        NUT_TA(!has_compare_method<TestA>::value);
        NUT_TA(compare(d1, d2) < 0);
        NUT_TA(compare(d2, d2) == 0);
        NUT_TA(compare(d2, d1) > 0);

        TestB e1(1), e2(2);
        NUT_TA(has_compare_method<TestB>::value);
        NUT_TA(compare(e1, e2) < 0);
        NUT_TA(compare(e2, e2) == 0);
        NUT_TA(compare(e2, e1) > 0);

        string f1("a"), f2("b");
        NUT_TA(has_compare_method<string>::value);
        NUT_TA(compare(f1, f2) < 0);
        NUT_TA(compare(f2, f2) == 0);
        NUT_TA(compare(f2, f1) > 0);
    }
};

NUT_REGISTER_FIXTURE(TestComparable, "container, quiet")
