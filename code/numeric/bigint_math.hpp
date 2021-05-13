#ifndef BIGINT_MATH_HPP
#define BIGINT_MATH_HPP

#include "bigint.hpp"

// Compute b^e
bigint intpow(bigint b, long e) {
    bigint power = 1;
    while (e > 0) {
        if (e & 1) {
            power = power * b;
        }
        e >>= 1;
        b = b * b;
    }
    return power;
}

// Compute b^e (mod m)
bigint modpow(bigint b, long e, bigint m) {
    bigint power = 1;
    b = b % m;
    while (e > 0) {
        if (e & 1)
            power = (power * b) % m;
        e >>= 1;
        b = (b * b) % m;
    }
    return power;
}

// Compute n!
bigint intfac(int n) {
    bigint f = 1;
    while (n > 1) {
        f = f * n--;
    }
    return f;
}

// Compute n! (mod m)
bigint modfac(uint n, const bigint& m) {
    bigint f = 1;
    while (n > 1) {
        f = (f * n--) % m;
    }
    return f;
}

// Compute lcm(a,b)
bigint lcm(const bigint& a, const bigint& b) { return a * (b / gcd(a, b)); }

// Compute x, y such that ax + by = gcd(a,b)
bigint gcd(bigint a, bigint b, bigint& x, bigint& y) {
    bigint xn = 1, yn = 0;
    x = 0, y = 1;
    while (a != 0) {
        bigint q = b / a;
        b = b % a;
        x = x - q * xn, y = y - q * yn;
        swap(a, b), swap(x, xn), swap(y, yn);
    }
    if (b < 0) {
        b = -b, x = -x, y = -y;
    }
    return b;
}

// Compute x such that ax = 1 (mod m) (modular multiplicative inverse)
bigint invmod(const bigint& a, const bigint& m) {
    bigint x, y, g = gcd(a, m, x, y);
    (void)g, assert(g == 1);
    x = x % m;
    return x >= 0 ? x : x + m;
}

/**
 * Solve the system a = b[i] (mod p[i]), i = 0,...,n-1
 * Complexity: O(n log p)
 */
bigint chinese(int n, bigint* remainders, bigint* primes) {
    bigint p = 1, a = 0;
    for (int i = 0; i < n; i++) {
        bigint q = primes[i], b = remainders[i];
        bigint x, y, g = gcd(p, q, x, y);
        (void)g, assert(g == 1);
        a = a * q * y + b * p * x;
        p = p * q;
        a = a % p, a = a >= 0 ? a : a + p;
    }
    return a;
}

/**
 * Compute (n choose k)
 */
bigint choose(int n, int k) {
    if (k < 0 || k > n)
        return 0;
    k = min(k, n - k);
    n = n - k + 1;
    bigint binom = 1;
    int i = 1;
    while (i <= k) {
        binom = (binom * n++) / i++;
    }
    return binom;
}

/**
 * Compute n!/(k1!k2!k3!...)
 */
bigint choose(int n, const vector<int>& k) {
    assert(accumulate(begin(k), end(k), 0L) <= n);
    bigint multi = 1;
    int m = 1, r = 1;
    for (int i = 0, K = k.size(); i < K; i++) {
        for (int j = 1; j <= k[i]; j++)
            multi = multi * m++ / j;
    }
    while (m < n)
        multi = multi * m++ / r++;
    return multi;
}

/**
 * Compute (n choose k) (mod m)
 * k! and (n-k)! should be coprime with m (preferably m prime)
 */
bigint choosemod(int n, int k, bigint m) {
    if (k < 0 || k > n)
        return 0;
    bigint x = modfac(n, m);
    x = x * invmod(modfac(n - k, m), m) % m;
    x = x * invmod(modfac(k, m), m) % m;
    return x;
}

/**
 * Partitions of an integer n.
 */
bigint partitions(int n) {
    static int m = 5;
    static vector<bigint> table = {1, 1, 2, 3, 5, 7};
    while (m < n) {
        m++;
        for (int k = 1, t = 1; k * (3 * k - 1) / 2 <= m; k++, t = -t)
            table[m] += t * table[m - k * (3 * k - 1) / 2];
        for (int k = -1, t = 1; k * (3 * k - 1) / 2 <= m; k--, t = -t)
            table[m] += t * table[m - k * (3 * k - 1) / 2];
    }
    return table[n];
}

struct bdmodnum {
    bigint n, mod;

    static bigint fit(bigint v, bigint mod) { return v = v % mod, v >= 0 ? v : v + mod; }

    bdmodnum(bigint v) : n(v), mod(0) {}
    bdmodnum(bigint v, bigint mod) : n(fit(v, mod)), mod(mod) {}
    explicit operator bigint() const { return n; }
    bdmodnum& operator+=(bdmodnum v) { return n = fit(n + v.n, mod), *this; }
    bdmodnum& operator-=(bdmodnum v) { return n = fit(n - v.n, mod), *this; }
    bdmodnum& operator*=(bdmodnum v) { return n = (n * v.n) % mod, *this; }
    bdmodnum& operator/=(bdmodnum v) { return n = (n * invmod(v.n, mod)) % mod, *this; }
    friend bdmodnum operator+(bdmodnum lhs, bdmodnum rhs) { return lhs += rhs; }
    friend bdmodnum operator-(bdmodnum lhs, bdmodnum rhs) { return lhs -= rhs; }
    friend bdmodnum operator*(bdmodnum lhs, bdmodnum rhs) { return lhs *= rhs; }
    friend bdmodnum operator/(bdmodnum lhs, bdmodnum rhs) { return lhs /= rhs; }
    friend bool operator==(bdmodnum lhs, bdmodnum rhs) { return lhs.n == rhs.n; }
    friend bool operator!=(bdmodnum lhs, bdmodnum rhs) { return lhs.n != rhs.n; }
};

#endif // BIGINT_MATH_HPP
