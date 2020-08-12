#include <bits/stdc++.h>

using namespace std;

// *****

inline void next_lexicographical_mask(uint& v) {
    uint t = v | (v - 1);
    v = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
}

inline bool is_power_of_two(uint v) { return v && !(v & (v - 1)); }

inline void reverse_bits(uint& v) {
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    v = (v >> 16) | (v << 16);
    // v = (v >> 32) | (v << 32);
}

/**
 * Iterate through the set bits of an unsigned integer mask
 * mask: an unsigned integer expression
 * n: name for variable holding the set bit index
 * bit: name for variable holding the shifted bit (1 << n)
 * mask should not be 0.
 */
#define FOR_EACH_BIT(mask, n, bit)                                     \
    for (uint _##n_##bit = (mask), bit = (_##n_##bit) & -(_##n_##bit), \
              n = __builtin_ctz(bit);                                  \
         _##n_##bit; _##n_##bit ^= bit,                                \
              _##n_##bit && (bit = _##n_##bit & -_##n_##bit, (n = __builtin_ctz(bit))))

/**
 * Iterate through the unsigned integer masks with 'size' set bits using the lowest
 * 'max_size' bits only.
 * mask: name for variable holding the generated mask
 * size: number of set bits in mask.
 * max_size: how many bit positions to iterate through
 * size and max_size should not be 0.
 */
#define FOR_EACH_MASK(mask, size, max_size)                                              \
    for (uint mask = (1 << (size)) - 1, max_##mask = 1 << (max_size); mask < max_##mask; \
         next_lexicographical_mask(mask))

inline string lsbits(uint v, uint bits = 32) {
    string s(bits, '0');
    FOR_EACH_BIT(v, n, bit) if (n < bits) s[n] = '1';
    return s;
}
