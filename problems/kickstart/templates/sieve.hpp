#include <bits/stdc++.h>

using namespace std;

// *****

namespace sieve {

using u64 = uint64_t;

u64 N;
vector<u64> odd_primes;

// fill array with all odd primes until N
void odd_sieve() {
    odd_primes.clear();
    N |= 1;

    vector<bool> is_composite(N / 2, false);

    for (u64 n = 3, i = 0; n * n <= N; i++, n += 2) {
        if (!is_composite[i]) {
            for (u64 m = n * n, j = (m - 3) >> 1; m <= N; j += n, m += 2 * n) {
                is_composite[j] = true;
            }
        }
    }
    for (u64 i = 0, n = 3; n <= N; i++, n += 2) {
        if (!is_composite[i]) {
            odd_primes.push_back(n);
        }
    }
}

// count number of primes in range [L, R]
u64 count_odd_primes(u64 L, u64 R) {
    assert(L <= R && R <= N * N);

    L |= 1, R |= 1;
    L = max(3UL, L);
    vector<bool> is_composite((R - L + 2) / 2, false);

    for (u64 p : odd_primes) {
        if (p * p > R) {
            break;
        }
        u64 k = max((p + L - 1) / p | 1, p);
        for (u64 n = k * p, i = (n - L) >> 1; n <= R; i += p, n += 2 * p) {
            is_composite[i] = 1;
        }
    }

    u64 count = 0;
    for (u64 c : is_composite) {
        count += !c;
    }
    return count;
}

// *****

} // namespace sieve
