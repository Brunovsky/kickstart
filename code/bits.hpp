#include <bits/stdc++.h>

using namespace std;

// *****

inline void next_lexicographical_mask(uint& v) {
    uint t = v | (v - 1);
    v = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
}

inline bool is_power_of_two(uint v) {
    return v && !(v & (v - 1));
}

inline void reverse_bits(uint& v) {
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    v = (v >> 16) | (v << 16);
    // v = (v >> 32) | (v << 32);
}
