
#include <nut/unittest/unittest.h>

#include <iostream>
#include <nut/util/string/kmp.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestKMP)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_kmp_search)
    NUT_CASE(test_kmp_searchw)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_kmp_search()
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

    void test_kmp_searchw()
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

NUT_REGISTER_FIXTURE(TestKMP, "util, string, quiet")
