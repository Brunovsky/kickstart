#ifndef BITS_HPP
#define BITS_HPP

#include <bits/stdc++.h>

using namespace std;

inline void next_lexicographical_mask(uint& v) {
    uint c = v & -v, r = v + c;
    v = c ? (((r ^ v) >> 2) / c) | r : 0;
}

inline bool is_power_of_two(uint v) { return v && !(v & (v - 1)); }

inline void reverse_bits(uint& v) {
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    v = (v >> 16) | (v << 16);
}

/**
 * Iterate through the set bits of an unsigned integer mask
 * - mask: an unsigned integer mask (expression)
 * - n: name for variable holding the set bit index
 * - bit: name for variable holding the shifted bit (1 << n)
 * mask should not be 0.
 */
#define FOR_EACH_BIT(mask, n, bit)                                                      \
    for (uint z##n##bit = (mask), bit = z##n##bit & -z##n##bit, n = __builtin_ctz(bit); \
         z##n##bit; z##n##bit ^= bit,                                                   \
              z##n##bit && (bit = z##n##bit & -z##n##bit, (n = __builtin_ctz(bit))))

/**
 * Iterate through the integer masks with `size` set bits using the lowest `max_size`
 * bits only. In other words, iterate through all subsets with cardinality size of a
 * set with cardinality max_size.
 * - mask: name for variable holding the generated mask
 * - size: number of set bits in mask (expression).
 * - max_size: how many bit positions to iterate through (expression)
 * size and max_size should not be 0.
 */
#define FOR_EACH_MASK(mask, size, max_size)                                        \
    for (uint mask = (1 << (size)) - 1, m##mask = 1 << (max_size); mask < m##mask; \
         next_lexicographical_mask(mask))

/**
 * Iterate through the proper, non-empty subsets of an unsigned integer mask
 * - subset: name for variable holding subset mask.
 * - mask: an unsigned integer mask (expression)
 * subset != 0 and subset != mask.
 */
#define FOR_EACH_SUBSET(subset, mask)                                           \
    for (uint s##subset = (mask), subset = s##subset & (s##subset - 1); subset; \
         subset = s##subset & (subset - 1))

string dlsbits(uint v) {
    string s;
    while (v > 0) {
        s.push_back('0' + (v & 1));
        v >>= 1;
    }
    return s;
}

string dmsbits(uint v) {
    string s;
    while (v > 0) {
        s.push_back('0' + (v & 1));
        v >>= 1;
    }
    reverse(begin(s), end(s));
    return s;
}

string lsbits(uint v, uint bits = 32) {
    string s(bits, '0');
    FOR_EACH_BIT (v, n, bit)
        if (n < bits)
            s[n] = '1';
    return s;
}

string msbits(uint v, uint bits = 32) {
    string s(bits, '0');
    FOR_EACH_BIT (v, n, bit)
        if (n < bits)
            s[bits - n - 1] = '1';
    return s;
}

#endif // BITS_HPP
