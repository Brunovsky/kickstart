#ifndef BITS_HPP
#define BITS_HPP

#include <bits/stdc++.h>

using namespace std;

inline void next_lexicographical_mask(unsigned& v) {
    unsigned c = v & -v, r = v + c;
    v = c ? (((r ^ v) >> 2) / c) | r : 0;
}

inline bool is_power_of_two(unsigned v) { return v && !(v & (v - 1)); }

inline void reverse_bits(unsigned& v) {
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    v = (v >> 16) | (v << 16);
}

/**
 * Iterate through the set bits of an unsigned integer mask
 * - bit: name for variable holding the shifted bit (1 << n)
 * - mask: an unsigned integer mask (expression)
 * mask should not be 0.
 */
#define FOR_EACH_BIT(bit, mask)                                                      \
    for (remove_const<decltype(mask)>::type z##bit = (mask), bit = z##bit & -z##bit; \
         z##bit; z##bit ^= bit, z##bit && (bit = z##bit & -z##bit))

#define FOR_EACH_BIT_NUMBER(bit, n, mask)                                            \
    for (remove_const<decltype(mask)>::type z##bit = (mask), bit = z##bit & -z##bit, \
                                            n = __builtin_ctz(bit);                  \
         z##bit;                                                                     \
         z##bit ^= bit, z##bit && (bit = z##bit & -z##bit, n = __builtin_ctz(bit)))

/**
 * Iterate through the integer masks with `size` set bits using the lowest `max_size`
 * bits only. In other words, iterate through all subsets with cardinality size of a
 * set with cardinality max_size.
 * - mask: name for variable holding the generated mask
 * - size: number of set bits in mask (expression).
 * - max_size: how many bit positions to iterate through (expression)
 * size and max_size should not be 0.
 */
#define FOR_EACH_MASK(mask, size, max_size)                                            \
    for (unsigned mask = (1 << (size)) - 1, m##mask = 1 << (max_size); mask < m##mask; \
         next_lexicographical_mask(mask))

/**
 * Iterate through the subsets of an integer mask
 * - subset: name for variable holding subset mask.
 * - mask: an integer mask (expression)
 */
// includes 0 but not mask
#define FOR_EACH_PROPER_SUBSET(subset, mask)                                      \
    for (remove_const<decltype(mask)>::type s##subset = (mask),                   \
                                            subset = s##subset & (s##subset - 1), \
                                            break##subset = !s##subset;           \
         !break##subset;                                                          \
         subset ? (subset = s##subset & (subset - 1)) : (break##subset = 1))

// includes mask but not 0
#define FOR_EACH_NONZERO_SUBSET(subset, mask)                                       \
    for (remove_const<decltype(mask)>::type s##subset = (mask), subset = s##subset; \
         subset; subset = s##subset & (subset - 1))

// includes 0 and mask
#define FOR_EACH_SUBSET(subset, mask)                                               \
    for (remove_const<decltype(mask)>::type s##subset = (mask), subset = s##subset, \
                                            break##subset = !s##subset;             \
         !break##subset;                                                            \
         subset ? (subset = s##subset & (subset - 1)) : (break##subset = 1))

string dlsbits(unsigned v) {
    string s;
    while (v > 0) {
        s.push_back('0' + (v & 1));
        v >>= 1;
    }
    return s;
}

string dmsbits(unsigned v) {
    string s;
    while (v > 0) {
        s.push_back('0' + (v & 1));
        v >>= 1;
    }
    reverse(begin(s), end(s));
    return s;
}

string lsbits(unsigned v, unsigned bits = 32) {
    string s(bits, '0');
    FOR_EACH_BIT_NUMBER (bit, n, v)
        if (n < bits)
            s[n] = '1';
    return s;
}

string msbits(unsigned v, unsigned bits = 32) {
    string s(bits, '0');
    FOR_EACH_BIT_NUMBER (bit, n, v)
        if (n < bits)
            s[bits - n - 1] = '1';
    return s;
}

#endif // BITS_HPP
