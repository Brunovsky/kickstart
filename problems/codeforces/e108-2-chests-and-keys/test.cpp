#include <bits/stdc++.h>

using namespace std;

inline void next_lexicographical_mask(uint& v) {
    uint c = v & -v, r = v + c;
    v = c ? (((r ^ v) >> 2) / c) | r : UINT_MAX;
}

#define FOR_EACH_MASK(mask, size, max_size)                                        \
    for (uint mask = (1 << (size)) - 1, m##mask = 1 << (max_size); mask < m##mask; \
         next_lexicographical_mask(mask))

int main() {
    FOR_EACH_MASK (mask, 2, 4) { cout << mask << endl; }
    return 0;
}
