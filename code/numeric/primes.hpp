#ifndef PRIMES_HPP
#define PRIMES_HPP

#include "math.hpp"

// *****

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

auto factor_simple(long n) {
    vector<long> primes;
    while (n && (n & 1) == 0) {
        primes.push_back(2);
        n >>= 1;
    }
    for (long p = 3; p * p <= n; p += 2) {
        while ((n % p) == 0) {
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

auto get_divisors(const unordered_map<long, int>& factors, bool one, bool self) {
    vector<long> divs = {1};
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

#endif // PRIMES_HPP
