#include <bits/stdc++.h>

using namespace std;

// *****

ulong N;
vector<ulong> small_primes;

inline bool ordered(ulong a, ulong b, ulong c) {
    return a <= b && b <= c;
}

// Find all odd primes at most N, inclusive. Start at 3.
void small_odd_sieve() {
    assert(small_primes.empty());
    small_primes.reserve(30'000);
    N |= 1; // make N odd.

    vector<int8_t> is_composite(N / 2, 0);

    for (ulong n = 3, i = 0; n <= N; ++i, n += 2) {
        if (is_composite[i]) {
            continue;
        }
        ulong m = n * n;
        for (ulong j = (m - 3) >> 1; m <= N; j += n, m += 2 * n) {
            is_composite[j] = 1;
        }
    }

    for (ulong i = 0, n = 3; n <= N; ++i, n += 2) {
        if (!is_composite[i]) {
            small_primes.push_back(n);
        }
    }
}

// Count all odd primes in [L, R], inclusive. Require R at most N * N.
ulong sieve(ulong L, ulong R) {
    assert(R <= N * N);

    if (R < L || R == 0) {
        return 0;
    }
    L = L | 1;
    R = (R - 1) | 1;
    if (R < L) {
        return 0;
    }

    vector<int8_t> is_composite((R - L + 2) / 2, 0);

    for (const ulong p : small_primes) {
        if (p * p > R) {
            break;
        }
        const ulong k = max((p + L - 1) / p | 1, p);
        ulong n = k * p;
        for (ulong i = (n - L) >> 1; n <= R; i += p, n += 2 * p) {
            is_composite[i] = 1;
        }
    }

    ulong count = 0;
    for (bool b : is_composite) {
        count += !b;
    }
    return count;
}

// Count integers in the range [L, R] that are 2 modulo 4.
// There are cleaner ways of doing this
ulong count_k1(ulong L, ulong R) {
    ulong count = (R - L) / 4;
    ulong Lhi = L + (R - L) % 4;
    for (ulong n = L; n <= Lhi; ++n) {
        if ((n % 4) == 2) {
            ++count;
        }
    }
    return count;
}

// X = 2^k * SUM(pi^ei)
// divisors: (k + 1)(e1 + 1)(e2 + 1)...(en + 1) = (k + 1)M
// even divisors: kM; odd divisors: M.
// M = 1 --> k = 0,1,2,3 --> X = 1,2,4,8.
//  > X = 1 is already counted by sieve(L, R).
//  > X = 2 is already counted by count_k1(L, R).
//  > X = 4 is already counted by sieve(L / 4, R / 4).
//  > X = 8 is not already, so it is checked manually.
// M = 2 --> k = 0,1,2 --> X = p, 2p, 4p for each p.
//  > k = 0 --> X = p solutions are counted with sieve(L, R).
//  > k = 1 --> X = 2p solutions are already counted in count_k1(L, R).
//  > k = 2 --> X = 4p solutions are counted with sieve(L / 4, R / 4).
// M > 2 --> k = 1 --> X = 2p^2, 2pq, etc.
//  > Counted in count_k1(L, R).

auto solve() {
    ulong L, R;
    cin >> L >> R >> ws;

    ulong A8 = ordered(L, 8, R);          // O(1)
    ulong B1 = sieve(L, R);               // O((R - L) sqrt R)
    ulong B2 = sieve((L + 3) / 4, R / 4); // O((R - L) sqrt R)
    ulong C = count_k1(L, R);             // O(1)
    ulong total = A8 + B1 + B2 + C;

    return total;
}

// *****

int main() {
    N = 31623UL; // sqrt(1'000'000'000) rounded up
    small_odd_sieve();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
