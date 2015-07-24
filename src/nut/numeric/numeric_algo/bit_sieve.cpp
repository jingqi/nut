﻿
#include "bit_sieve.h"
#include "prime.h"

namespace nut
{

BitSieve BitSieve::_small_sieve;

/**
 * Given a bit index return unit index containing it.
 */
static int unit_index(int bit_index)
{
    return (int) (((unsigned) bit_index) >> 6);
}

/**
 * Return a unit that masks the specified bit in its unit.
 */
static int64_t bit(int bit_index)
{
    return ((int64_t) 1) << (bit_index & ((1 << 6) - 1));
}

/**
 * Construct a "small sieve" with a base of 0.  This constructor is
 * used internally to generate the set of "small primes" whose multiples
 * are excluded from sieves generated by the main (package private)
 * constructor, BitSieve(BigInteger base, int search_len).  The length
 * of the sieve generated by this constructor was chosen for performance;
 * it controls a tradeoff between how much time is spent constructing
 * other sieves, and how much time is wasted testing composite candidates
 * for primality.  The length was chosen experimentally to yield good
 * performance.
 */
BitSieve::BitSieve()
{
    _length = 150 * 64;
    _bits_cap = unit_index(_length - 1) + 1;
    _bits = (int64_t*) ::malloc(sizeof(int64_t) * _bits_cap);
    ::memset(_bits, 0, sizeof(int64_t) * _bits_cap);

    // Mark 1 as composite
    set(0);
    int next_index = 1;
    int next_prime = 3;

    // Find primes and remove their multiples from sieve
    do
    {
        sieve_single(_length, next_index + next_prime, next_prime);
        next_index = sieve_search(_length, next_index + 1);
        next_prime = 2 * next_index + 1;
    } while((next_index > 0) && (next_prime < _length));
}

/**
 * Construct a bit sieve of search_len bits used for finding prime number
 * candidates. The new sieve begins at the specified base, which must
 * be even.
 */
BitSieve::BitSieve(const BigInteger& base, int search_len)
{
    /*
     * Candidates are indicated by clear bits in the sieve. As a candidates
     * nonprimality is calculated, a bit is set in the sieve to eliminate
     * it. To reduce storage space and increase efficiency, no even numbers
     * are represented in the sieve (each bit in the sieve represents an
     * odd number).
     */
    _bits_cap = unit_index(search_len - 1) + 1;
    _bits = (int64_t*) ::malloc(sizeof(int64_t) * _bits_cap);
    ::memset(_bits, 0, _bits_cap * sizeof(int64_t));
    _length = search_len;
    int start = 0;

    int step = _small_sieve.sieve_search(_small_sieve._length, start);
    int converted_step = (step * 2) + 1;

    // Construct the large sieve at an even offset specified by base
    do
    {
        // Calculate base mod converted_step
        start = (int) (base % converted_step).llong_value();

        // Take each multiple of step out of sieve
        start = converted_step - start;
        if (0 == start % 2)
            start += converted_step;
        sieve_single(search_len, (start - 1) / 2, converted_step);

        // Find next prime from small sieve
        step = _small_sieve.sieve_search(_small_sieve._length, step + 1);
        converted_step = (step * 2) + 1;
    } while (step > 0);
}

BitSieve::~BitSieve()
{
    if (NULL != _bits)
        ::free(_bits);
    _bits = NULL;
    _bits_cap = 0;
}

/**
 * Get the value of the bit at the specified index.
 */
bool BitSieve::get(int bit_index)
{
    int ui = unit_index(bit_index);
    return ((_bits[ui] & bit(bit_index)) != 0);
}

/**
 * Set the bit at the specified index.
 */
void BitSieve::set(int bit_index)
{
    int ui = unit_index(bit_index);
    _bits[ui] |= bit(bit_index);
}

/**
 * This method returns the index of the first clear bit in the search
 * array that occurs at or after start. It will not search past the
 * specified limit. It returns -1 if there is no such clear bit.
 */
int BitSieve::sieve_search(int limit, int start)
{
    if (start >= limit)
        return -1;

    int index = start;
    do
    {
        if (!get(index))
            return index;
        index++;
    } while (index < limit - 1);
    return -1;
}

/**
 * Sieve a single set of multiples out of the sieve. Begin to remove
 * multiples of the specified step starting at the specified start index,
 * up to the specified limit.
 */
void BitSieve::sieve_single(int limit, int start, int step)
{
    while (start < limit)
    {
        set(start);
        start += step;
    }
}

/**
 * Test probable primes in the sieve and return successful candidates.
 */
BigInteger BitSieve::retrieve(const BigInteger& init_value, int certainty)
{
    // Examine the sieve one long at a time to find possible primes
    BigInteger candidate = init_value + 1;
    for (int i = 0; i < _bits_cap; ++i)
    {
        uint64_t next_long = ~_bits[i];
        for (int j = 0; j < 64; ++j)
        {
            if ((next_long & 1) == 1)
            {
                if (miller_rabin(candidate, certainty))
                    return candidate;
            }
            next_long >>= 1;
            candidate += 2;
        }
    }

    candidate.set_zero();
    return candidate;
}

}
