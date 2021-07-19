#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Compute all primes p<=N.
 * This allows querying primes or number of primes in a range [L,R] at most N*N.
 * Complexity: O(N log log N)
 *    time       N
 *     30ms      10'000'000
 *    100ms      32'000'000
 *    500ms      100'000'000
 */
auto classic_sieve(int N) {
    vector<int> primes;
    vector<bool> isprime;
    isprime.assign(N + 1, true);

    for (int n = 4; n <= N; n += 2)
        isprime[n] = false;
    for (int n = 3; n * n <= N; n += 2)
        if (isprime[n])
            for (int i = n * n; i <= N; i += 2 * n)
                isprime[i] = false;
    if (N > 2)
        primes.push_back(2);
    for (int n = 3; n <= N; n += 2)
        if (isprime[n])
            primes.push_back(n);

    return primes;
}

/**
 * Compute least prime divisor of all n<=N.
 * Computes primes as a side-effect as well (like classic sieve)
 * Complexity: O(N)
 * Source: https://github.com/indy256/codelibrary
 *         https://cp-algorithms.com/algebra/prime-sieve-linear.html
 *    time       N
 *     90ms      10'000'000
 *    300ms      32'000'000
 *   1100ms      100'000'000
 */
auto least_prime_sieve(int N) {
    vector<int> primes, lp, nxt;
    lp.assign(N + 1, 0), nxt.assign(N + 1, 0);
    nxt[1] = 1;

    for (int P = 0, n = 2; n <= N; n++) {
        if (lp[n] == 0) {
            lp[n] = n, primes.push_back(n), P++;
        }
        for (int i = 0; i < P && primes[i] <= lp[n] && n * primes[i] <= N; ++i) {
            lp[n * primes[i]] = primes[i], nxt[n * primes[i]] = n;
        }
    }

    return lp;
}

/**
 * Compute the number of unique prime divisors of all n<=N.
 * Complexity: O(N log log N)
 *    time       N
 *     60ms      3'200'000
 *    250ms      10'000'000
 *    850ms      32'000'000
 */
auto num_prime_divisors_sieve(int N) {
    vector<int> num_prime_divisors;
    num_prime_divisors.assign(N + 1, 0);

    for (int n = 2; n <= N; n++) {
        if (num_prime_divisors[n] == 0) {
            for (int i = 1; i * n <= N; i++) {
                num_prime_divisors[i * n]++;
            }
        }
    }

    return num_prime_divisors;
}

/**
 * Compute the number of divisors of all n<=N, including trivial divisors.
 * Complexity: O(N log log N)
 *    time       N
 *     80ms      3'200'000
 *    300ms      10'000'000
 *    950ms      32'000'000
 */
auto num_divisors_sieve(int N) {
    vector<int> num_divisors;
    num_divisors.assign(N + 1, 1);
    num_divisors[0] = 0;

    for (int n = 2; n <= N; n++) {
        if (num_divisors[n] == 1) {
            for (long m = n, k = 2; m <= N; m *= n, k++) {
                for (int i = 1; i * m <= N; i++) {
                    for (int b = 1; b < n && i * m <= N; i++, b++) {
                        num_divisors[i * m] *= k;
                    }
                }
            }
        }
    }

    return num_divisors;
}

/**
 * Compute the sum of divisors of all n<=N, including trivial divisors.
 * Complexity: O(N log log N)
 *    time       N
 *    100ms      3'200'000
 *    350ms      10'000'000
 *   1250ms      32'000'000
 */
auto sum_divisors_sieve(int N) {
    vector<long> sum_divisors;
    sum_divisors.assign(N + 1, 1);
    sum_divisors[0] = 0;

    for (int n = 2; n <= N; n++) {
        if (sum_divisors[n] == 1) {
            for (long m = n, k = 1 + n; m <= N; m *= n, k += m) {
                for (int i = 1; i * m <= N; i++) {
                    for (int b = 1; b < n && i * m <= N; i++, b++) {
                        sum_divisors[i * m] *= k;
                    }
                }
            }
        }
    }

    return sum_divisors;
}

/**
 * Compute phi(n) of all n<=N.
 * Complexity: O(N log log N)
 *    time       N
 *    100ms      3'200'000
 *    350ms      10'000'000
 *   1200ms      32'000'000
 */
auto phi_sieve(int N) {
    vector<int> phi;
    phi.resize(N + 1);
    iota(begin(phi), end(phi), 0);

    for (int n = 2; n <= N; n++) {
        if (phi[n] == n) {
            for (int i = n; i <= N; i += n) {
                phi[i] -= phi[i] / n;
            }
        }
    }

    return phi;
}

/**
 * Compute modinv(n, m) of all n<=N where m is a prime.
 * Complexity: O(N)
 *    time       N (mod=1e9+7)
 *    150ms      10'000'000
 *    400ms      32'000'000
 *   1000ms      100'000'000
 */
auto modinv_sieve(int N, int mod) {
    vector<int> inv;
    N = min(N, mod - 1);
    inv.resize(N + 1);
    inv[1] = 1;

    for (int n = 2; n <= N; n++) {
        inv[n] = mod - (mod / n) * inv[mod % n] % mod;
    }

    return inv;
}

/**
 * Compute log(n!) of all n<=N.
 * Complexity: O(N)
 *    time       N (long doubles)
 *    140ms      3'200'000
 *    380ms      10'000'000
 *   1240ms      32'000'000
 */
template <typename D = double>
auto logfac_sieve(int N) {
    vector<D> logfac;
    logfac.resize(N + 1);
    logfac[0] = logfac[1] = 1;

    for (int n = 2; n <= N; n++) {
        logfac[n] = logfac[n - 1] + log(D(n));
    }

    return logfac;
}

/**
 * Compute n! and inv(n!) for all n<=N.
 * Complexity: O(N log N)
 *    time       N (mod=1e9+7)
 *    120ms      10'000'000
 *    430ms      32'000'000
 *   1600ms      100'000'000
 */
template <typename num>
auto pascal_sieve(int N) {
    vector<num> fac, ifac;
    fac.resize(N + 1), ifac.resize(N + 1);
    fac[0] = fac[1] = ifac[1] = 1;

    for (int n = 1; n <= N; n++) {
        fac[n] = n * fac[n - 1];
    }
    ifac[N] = fac[N].inv();
    for (int n = N; n >= 1; n--) {
        ifac[n - 1] = n * ifac[n];
    }

    return make_pair(fac, ifac);
}

/**
 * Count primes in the range [L,R], both inclusive.
 * Requires sieving first, such that primes[] contains all primes at least up to sqrt(R).
 * Complexity: O(R^1/2 + K log log K) where K=R-L.
 */
int count_primes(long L, long R, const vector<int>& primes) {
    assert(1 <= L && L <= R);
    vector<bool> isprime(R - L + 1, true);

    for (long p : primes) {
        if (p * p > R)
            break;
        long k = max((L + p - 1) / p, p);
        for (long n = k * p; n <= R; n += p)
            isprime[n - L] = false;
    }
    isprime[0] = isprime[0] & (L > 1);

    return count(begin(isprime), end(isprime), true);
}

/**
 * Get primes in the range [L,R], both inclusive.
 * Requires sieving first, such that primes[] contains all primes at least up to sqrt(R).
 * Complexity: O(R^1/2 + K log log K) where K=R-L.
 */
auto get_primes(long L, long R, const vector<int>& primes) {
    assert(1 <= L && L <= R);
    vector<bool> isprime(R - L + 1, true);

    for (long p : primes) {
        if (p * p > R)
            break;
        long k = max((L + p - 1) / p, p);
        for (long n = k * p; n <= R; n += p)
            isprime[n - L] = false;
    }

    vector<long> new_primes;
    for (long n = L; n <= R; n++)
        if (isprime[n - L])
            new_primes.push_back(n);
    return new_primes;
}

/**
 * Factor primes out of n.
 */
auto factor_primes(int n, const vector<int>& lp, const vector<int>& nxt) {
    unordered_map<int, int> primes;
    while (n > 1) {
        primes[lp[n]]++, n = nxt[n];
    }
    return primes;
}
