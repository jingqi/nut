
#include <nut/unittest/unittest.hpp>

#include <iostream>
#include <nut/util/string/kmp.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestKMP)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testKmpSearch)
    NUT_CASE(testKmpSearchW)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testKmpSearch()
    {
        const char *example = "abcabdabcabc";
        int kmp_expect[12] = {0, 0, 0, 1, 2, 0, 1, 2, 3, 4, 5, 3};
        int kmp[12];
        kmp_build_next(example, kmp, 12);
        for (int i = 0; i < 12; ++i)
            NUT_TA(kmp[i] == kmp_expect[i]);

        const char *src = "abcabcabdabcabc";
        int i = kmp_search(src, ::strlen(src), 1, example, kmp, 12);
        NUT_TA(i == 3);
    }

    void testKmpSearchW()
    {
        const wchar_t *example = L"abcabdabcabc";
        int kmp_expect[12] = {0, 0, 0, 1, 2, 0, 1, 2, 3, 4, 5, 3};
        int kmp[12];
        kmp_build_next(example, kmp, 12);
        for (int i = 0; i < 12; ++i)
            NUT_TA(kmp[i] == kmp_expect[i]);

        const wchar_t *src = L"abcabcabdabcabc";
        int i = kmp_search(src, ::wcslen(src), 1, example, kmp, 12);
        NUT_TA(i == 3);
    }
};

NUT_REGISTER_FIXTURE(TestKMP, "util, quiet")
