#include <bits/stdc++.h>

using namespace std;

// *****

ulong N;
vector<ulong> odd_primes;

// fill array with all odd primes until N
void odd_sieve() {
    odd_primes.clear();
    N |= 1;

    vector<bool> is_composite(N / 2, false);

    for (ulong n = 3, i = 0; n * n <= N; i++, n += 2) {
        if (!is_composite[i]) {
            for (ulong m = n * n, j = (m - 3) >> 1; m <= N; j += n, m += 2 * n) {
                is_composite[j] = true;
            }
        }
    }
    for (ulong i = 0, n = 3; n <= N; i++, n += 2) {
        if (!is_composite[i]) {
            odd_primes.push_back(n);
        }
    }
}

// count number of primes in range [L, R]
ulong count_odd_primes(ulong L, ulong R) {
    assert(R <= N * N);
    L = L | 1;
    R = (R - 1) | 1;
    L = max(3UL, L);
    if (R < L) {
        return 0;
    }
    vector<bool> is_composite((R - L + 2) / 2, false);

    for (ulong p : odd_primes) {
        if (p * p > R) {
            break;
        }
        ulong k = max((p + L - 1) / p | 1, p);
        for (ulong n = k * p, i = (n - L) >> 1; n <= R; i += p, n += 2 * p) {
            is_composite[i] = true;
        }
    }

    // alternatively, gather them up in a vector
    ulong count = 0;
    for (bool b : is_composite) {
        count += !b;
    }
    return count;
}

// *****

void driver() {
    N = 50000;
    odd_sieve();
}
