
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/numeric/big_integer.h>
#include <nut/numeric/word_array_integer.h>
#include <nut/numeric/numeric_algo/mod.h>
#include <nut/numeric/numeric_algo/ntt.h>
#include <nut/numeric/numeric_algo/prime.h>
#include <nut/time/performance_counter.h>


using namespace std;
using namespace nut;

class TestNTT : public TestFixture
{
    virtual void register_cases() final override
    {
        NUT_REGISTER_CASE(test_smoke);
        NUT_REGISTER_CASE(test_profile);
    }

    void test_smoke()
    {
        NUT_TA(psedoprime(BigInteger(NUT_NTT_M)));
        // NUT_TA(is_ordm<ntt_word_type>(NUT_NTT_K, NUT_NTT_M));

        const size_t bits = 2010;
		BigInteger a = BigInteger::rand_positive(bits), b = BigInteger::rand_positive(bits);
        size_t a_len = a.significant_words_length(), b_len = b.significant_words_length();
        BigInteger::word_type *rs = (BigInteger::word_type*) ::malloc(
            sizeof(BigInteger::word_type) * (a_len + b_len));
        unsigned_ntt_multiply(a.data(), a_len, b.data(), b_len, rs, a_len + b_len);
        BigInteger c(rs, sizeof(BigInteger::word_type) * (a_len + b_len), true);
        ::free(rs);

        // cout << endl << a.to_string() << endl << "*" << endl << b.to_string() << endl << "=" << endl << (a * b).to_string() << endl;
        // cout << c.to_string() << endl;

		NUT_TA(c == a * b);
    }

    void test_profile()
    {
        const size_t bits = 50000;
		BigInteger a = BigInteger::rand_positive(bits), b = BigInteger::rand_positive(bits);
        size_t a_len = a.significant_words_length(), b_len = b.significant_words_length();
        BigInteger::word_type *rs = (BigInteger::word_type*) ::malloc(
            sizeof(BigInteger::word_type) * (a_len + b_len));

        PerformanceCounter s = PerformanceCounter::now();
        unsigned_multiply(a.data(), a_len, b.data(), b_len, rs, a_len + b_len);
        PerformanceCounter f1 = PerformanceCounter::now();
        unsigned_ntt_multiply(a.data(), a_len, b.data(), b_len, rs, a_len + b_len);
        PerformanceCounter f2 = PerformanceCounter::now();
        ::free(rs);
        printf(" %.6fs(orgin %.6fs)", f2 - f1, f1 - s);
    }
};

NUT_REGISTER_FIXTURE(TestNTT, "numeric,quiet")
