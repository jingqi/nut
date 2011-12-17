
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/numeric/signedinteger.hpp>

using namespace nut;


NUT_FIXTURE(TestSignedInteger)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASE(testComparator)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testSmoking()
    {
        // +
        NUT_TA((SignedInteger<3>(12) + SignedInteger<3>(13)).longValue() == 25);
        NUT_TA((SignedInteger<3>(12) + SignedInteger<3>(-11)).longValue() == 1);
        NUT_TA((SignedInteger<3>(12) + SignedInteger<3>(-13)).longValue() == -1);

        // -
        NUT_TA((SignedInteger<3>(12) - SignedInteger<3>(1)).longValue() == 11);
        NUT_TA((SignedInteger<3>(12) - SignedInteger<3>(-1)).longValue() == 13);
        NUT_TA((SignedInteger<3>(12) - SignedInteger<3>(13)).longValue() == -1);

        // *
        NUT_TA((SignedInteger<3>(12) * SignedInteger<3>(1)).longValue() == 12);
        NUT_TA((SignedInteger<13>(12) * SignedInteger<13>(24)).longValue() == 12 * 24);
        NUT_TA(((SignedInteger<13>(0x12345L) * SignedInteger<13>(0x12345L)).longValue() & 0xFFFFFFL) == 
            ((0x12345L * 0x12345L) & 0xFFFFFFL));
    }

    void testComparator()
    {
        NUT_TA(SignedInteger<3>(2) < SignedInteger<3>(5));
        NUT_TA(SignedInteger<3>(9) > SignedInteger<3>(-5));
        NUT_TA(SignedInteger<3>(9) > SignedInteger<3>(5));
        NUT_TA(!(SignedInteger<3>(2) < SignedInteger<3>(2)));
    }
};

NUT_REGISTER_FIXTURE(TestSignedInteger, "numeric,quiet")
