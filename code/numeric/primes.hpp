#pragma once

#include "math.hpp"

auto factor_simple(long n) {
    vector<long> primes;
    for (long p = 2; p * p <= n; p++) {
        while (n % p == 0) {
            primes.push_back(p);
            n = n / p;
        }
    }
    if (n > 1) {
        primes.push_back(n);
    }
    return primes;
}

auto factor_map(long n) {
    unordered_map<long, int> primes;
    for (long p = 2; p * p <= n; p++) {
        while (n % p == 0) {
            primes[p]++;
            n = n / p;
        }
    }
    if (n > 1) {
        primes[n]++;
    }
    return primes;
}

template <typename T>
auto get_divisors(const unordered_map<T, int>& factors, bool one, bool self) {
    vector<T> divs = {1};
    for (const auto& [p, e] : factors) {
        int D = divs.size();
        divs.resize(D * (e + 1));
        for (int n = 1; n <= e; n++) {
            for (int d = 0; d < D; d++) {
                divs[d + n * D] = divs[d + (n - 1) * D] * p;
            }
        }
    }
    if (!one) {
        divs.erase(begin(divs));
    }
    if (!self && !divs.empty()) {
        divs.erase(begin(divs) + (divs.size() - 1));
    }
    return divs;
}

/**
 * Compute the jacobi (a/b) = ±1 (from quadratic reciprocity)
 * Complexity: O(log ab)
 */
int jacobi(long a, long b) {
    assert(b & 1);
    int t = 1, r;
    a = a % b;
    while (a) {
        while ((a & 1) == 0) {
            a >>= 1, r = b % 8;
            if (r == 3 || r == 5)
                t = -t;
        }
        swap(a, b);
        if ((a % 4) == 3 && (b % 4) == 3)
            t = -t;
        a = a % b;
    }
    return b == 1 ? t : 0;
}

/**
 * Determines if a is a quadratic residue mod p (p prime)
 * Complexity: O(log p)
 */
bool is_quadratic_residue(long a, long p) {
    a = a % p, a = a >= 0 ? a : a + p;
    return a != 0 && modpow(a, (p - 1) / 2, p) == 1;
}

long huge_modpow(long base, long e, long mod) {
    long x = 1;
    base = base % mod;
    while (e > 0) {
        if (e & 1)
            x = __int128_t(x) * __int128_t(base) % mod;
        if (e >>= 1)
            base = __int128_t(base) * __int128_t(base) % mod;
    }
    return x;
}

/**
 * Fast miller-rabin test for compositeness of n
 * Returns true if n is a prime.
 * Complexity: O(log n)
 */
bool miller_rabin(long n) {
    if (n < 5)
        return n == 2 || n == 3;
    if (n % 2 == 0)
        return false;
    int r = __builtin_ctzll(n - 1);
    long d = n >> r;

    for (int witness : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
        if (witness > n)
            break;
        if (witness == n)
            return true;
        auto x = huge_modpow(witness, d, n);
        if (x == 1 || x == n - 1)
            continue;
        for (int i = 0; i < r - 1 && x != n - 1; i++) {
            x = __int128_t(x) * __int128_t(x) % n;
        }
        if (x != n - 1)
            return false;
    }
    return true;
}
