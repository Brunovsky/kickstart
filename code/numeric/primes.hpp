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

/**
 * Compute primes dividing n naively (best for small prime divisors or small n)
 * Complexity: O(n^1/2) worst case (prime)
 */
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

#endif // PRIMES_HPP
