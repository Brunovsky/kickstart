#ifndef PRIMES_HPP
#define PRIMES_HPP

#include "math.hpp"

// *****

#define MAXN 100'000'000

/**
 * Eratosthenes sieve
 * Sieve to find all primes up N. This allows querying primes or number of primes in a
 * range [L,R] at most N*N.
 * Sieve complexity: O(N log log N)
 * Query complexity: O(sqrt(R) + (R - L) log log (R - L))
 */
struct odd_sieve {
    int N;
    bitset<MAXN> isprime;
    vector<uint> odd_primes;

    void sieve() {
        isprime.set();
        for (int i = 4; i <= N; i += 2)
            isprime[i] = false;
        for (int i = 3; i * i <= N; i += 2)
            if (isprime[i])
                for (int n = i * i; n <= N; n += 2 * i)
                    isprime[n] = false;
        for (int i = 3; i <= N; i += 2)
            if (isprime[i])
                odd_primes.push_back(i);
    }

    int count_odd_primes(long L, long R) {
        assert(R <= N * N);
        L = max(3L, L | 1), R = (R - 1) | 1;
        if (R < L)
            return 0;
        vector<bool> rprime((R - L + 2) / 2, true);

        for (long p : odd_primes) {
            if (p * p > R)
                break;
            long k = max((p + L - 1) / p | 1, p);
            for (long n = k * p; n <= R; n += 2 * p)
                rprime[(n - L) >> 1] = false;
        }

        // alternatively, gather them up in a vector
        int count = 0;
        for (bool b : rprime) {
            count += b;
        }
        return count;
    }
};

/**
 * Miller-Rabin primality test
 * Complexity: O(log(p)^c)
 * Source: kth (https://github.com/kth-competitive-programming/kactl)
 */
bool miller_rabin(ulong n) {
    if (n < 2 || n % 6 % 4 != 1)
        return n == 2 || n == 3;
    ulong A[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022},
          s = __builtin_ctzll(n - 1), d = n >> s;
    for (ulong a : A) { // ^ count trailing zeroes
        ulong p = modpow(a % n, d, n), i = s;
        while (p != 1 && p != n - 1 && a % n && i--)
            p = p * p % n;
        if (p != n - 1 && i != s)
            return 0;
    }
    return 1;
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

/**
 * Compute primes dividing n naively (best for small prime divisors)
 * Complexity: O(n^1/2) worst case (prime)
 */
vector<ulong> factor_simple(ulong n) {
    vector<ulong> primes;
    while ((n & 1) == 0) {
        primes.push_back(2);
        n >>= 1;
    }
    for (ulong p = 3; p * p <= n; p += 2) {
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

/**
 * Compute a prime factor of composite n.
 * Uses polynomial x^2 + c and retries with a different c if it doesn't find an
 * answer quickly enough. Obviously fails if n is prime but might fail too if n is
 * composite.
 * Complexity: O(n^1/8 x trials), can be reduced with more square roots.
 * Returns 0 on failure.
 */
ulong pollard(ulong n, ulong c = 1) {
    static const ulong cmax = 17;
    int i = 1, iterations = 37 + int(ceil(sqrt(sqrt(sqrt(n)))));
    auto f = [n, c](ulong x) {
        return ((x * x + c) % n) + 1;
    };
    ulong x = 2, y = 2, g = 1;
    do {
        x = f(x), y = f(f(y));
        g = __gcd(max(x, y) - min(x, y), n);
    } while (i++ <= iterations && g == 1);
    return (1 < g && g < n) ? g : (c < cmax ? pollard(n, c + 2) : 0);
}

/**
 * Factorize an integer n with pollard-rho. Preferable for large prime divisors.
 * Source: kth (https://github.com/kth-competitive-programming/kactl)
 */
vector<ulong> factor(ulong n) {
    if (n == 1)
        return {};
    if (miller_rabin(n))
        return {n};
    ulong x = pollard(n);
    auto l = factor(x), r = factor(n / x);
    l.insert(end(l), begin(r), end(r));
    return l;
}

#endif // PRIMES_HPP
