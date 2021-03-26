#ifndef MATH_HPP
#define MATH_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

// Compute b^e
long intpow(long b, long e) {
    long power = 1;
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
long modpow(long b, long e, long m) {
    long power = 1;
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
ulong intfac(ulong n) {
    ulong f = 1;
    while (n > 1) {
        f = f * n--;
    }
    return f;
}

// Compute n! (mod m)
ulong modfac(ulong n, ulong m) {
    ulong f = 1;
    while (n > 1) {
        f = (f * n--) % m;
    }
    return f;
}

// Compute x, y such that ax + by = gcd(a,b)
long gcd(long a, long b, long& x, long& y) {
    long xn = 1, yn = 0;
    x = 0, y = 1;
    while (a != 0) {
        long q = b / a;
        b = b % a;
        x = x - q * xn;
        y = y - q * yn;
        swap(a, b);
        swap(x, xn);
        swap(y, yn);
    }
    if (b < 0) {
        b = -b, x = -x, y = -y;
    }
    return b;
}

// Compute x such that ax = 1 (mod m) (modular multiplicative inverse)
long invmod(long a, long m) {
    long x, y;
    auto g = gcd(a, m, x, y);
    (void)g, assert(g == 1);
    x = x % m;
    return x >= 0 ? x : x + m;
}

// Compute gcd(a,b)
long gcd(long a, long b) {
    while (a != 0) {
        b = b % a;
        swap(a, b);
    }
    return abs(b);
}

// Compute lcm(a,b)
ulong lcm(ulong a, ulong b) { return a * (b / gcd(a, b)); }

/**
 * Compute the smallest exponent x such that a^x = b (mod m)
 * Complexity: O(sqrt(m)), uses square root decomposition
 * Source: kth (https://github.com/kth-competitive-programming/kactl)
 */
long modlog(long a, long b, long m) {
    long n = long(sqrt(m)) + 1, x = 1, f = 1, j = 1;
    unordered_map<long, long> A;
    while (j <= n && (x = f = x * a % m) != b % m)
        A[x * b % m] = j++;
    if (x == b % m)
        return j;
    if (__gcd(m, x) == __gcd(m, b))
        for (int i = 2; i < n + 2; i++)
            if (A.count(x = x * f % m))
                return n * i - A[x];
    return -1;
}

/**
 * Compute x such that x^2 = a (mod p) where p is prime
 * Complexity: O(log^2 p) heavy
 * Source: kth (https://github.com/kth-competitive-programming/kactl)
 */
long modsqrt(long a, long p) {
    a = a % p, a = a >= 0 ? a : a + p;
    if (a == 0)
        return 0;
    if (modpow(a, (p - 1) / 2, p) != 1)
        return -1; // not a quadratic residue
    if (p % 4 == 3)
        return modpow(a, (p + 1) / 4, p);
    // a^(n+3)/8 or 2^(n+3)/8 * 2^(n-1)/4 works if p % 8 == 5
    long s = p - 1, n = 2;
    int r = 0, m;
    while (s % 2 == 0)
        ++r, s /= 2;
    /// find a non-square mod p
    while (modpow(n, (p - 1) / 2, p) != p - 1)
        ++n;
    long x = modpow(a, (s + 1) / 2, p);
    long b = modpow(a, s, p), g = modpow(n, s, p);
    for (;; r = m) {
        long t = b;
        for (m = 0; m < r && t != 1; ++m)
            t = t * t % p;
        if (m == 0)
            return x;
        long gs = modpow(g, 1L << (r - m - 1), p);
        g = gs * gs % p;
        x = x * gs % p;
        b = b * g % p;
    }
}

/**
 * Solve the system a = b[i] (mod p[i]), i = 0,...,n-1
 * Complexity: O(n log p)
 */
long chinese(int n, long remainders[], long primes[]) {
    long p = 1, a = 0;
    for (int i = 0; i < n; i++) {
        long q = primes[i], b = remainders[i];
        long x, y;
        long g = gcd(p, q, x, y);
        (void)g, assert(g == 1);
        a = a * q * y + b * p * x;
        p = p * q;
        a = a % p, a = a >= 0 ? a : a + p;
    }
    return a;
}

/**
 * Compute phi(n) (totient function), naively
 */
ulong totient(ulong n) {
    ulong tot = 1;
    if ((n & 1) == 0) {
        n >>= 1;
        while ((n & 1) == 0) {
            tot <<= 1;
            n >>= 1;
        }
    }
    for (ulong p = 3; n > 1; p += 2) {
        if ((n % p) == 0) {
            tot *= p - 1;
            n = n / p;
            while ((n % p) == 0) {
                tot *= p;
                n = n / p;
            }
        }
    }
    return tot;
}

/**
 * Compute (n choose k)
 */
long choose(long n, long k) {
    if (k < 0 || k > n)
        return 0;
    k = min(k, n - k);
    n = n - k + 1;
    long binom = 1;
    long i = 1;
    while (i <= k) {
        binom = (binom * n++) / i++;
    }
    return binom;
}

/**
 * Compute n!/(k1!k2!k3!)
 */
long choose(long n, const vector<long>& k) {
    assert(accumulate(begin(k), end(k), 0L) <= n);
    long multi = 1, m = 1, r = 1;
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
long choosemod(long n, long k, long m) {
    if (k < 0 || k > n)
        return 0;
    long x = modfac(n, m);
    x = x * invmod(modfac(n - k, m), m) % m;
    x = x * invmod(modfac(k, m), m) % m;
    return x;
}

/**
 * Partitions of an integer n.
 */
long partitions(long n) {
    static int m = 5;
    static long table[121] = {1, 1, 2, 3, 5, 7};
    assert(n <= 120);
    while (m < n) {
        m++;
        for (int k = 1, t = 1; k * (3 * k - 1) / 2 <= m; k++, t = -t)
            table[m] += t * table[m - k * (3 * k - 1) / 2];
        for (int k = -1, t = 1; k * (3 * k - 1) / 2 <= m; k--, t = -t)
            table[m] += t * table[m - k * (3 * k - 1) / 2];
    }
    return table[n];
}

template <long mod>
struct modnum {
    long n;

    static long fit(long v) { return v >= mod ? v - mod : (v < 0 ? v + mod : v); }

    modnum() : n(0) {}
    modnum(long v) : n(fit(v % mod)) {}
    explicit operator long() const { return n; }
    modnum& operator+=(modnum v) { return n = fit(n + v.n), *this; }
    modnum& operator-=(modnum v) { return n = fit(n - v.n), *this; }
    modnum& operator*=(modnum v) { return n = (n * v.n) % mod, *this; }
    modnum& operator/=(modnum v) { return n = (n * invmod(v.n, mod)) % mod, *this; }
    friend modnum operator+(modnum lhs, modnum rhs) { return lhs += rhs; }
    friend modnum operator-(modnum lhs, modnum rhs) { return lhs -= rhs; }
    friend modnum operator*(modnum lhs, modnum rhs) { return lhs *= rhs; }
    friend modnum operator/(modnum lhs, modnum rhs) { return lhs /= rhs; }
    friend bool operator==(modnum lhs, modnum rhs) { return lhs.n == rhs.n; }
    friend bool operator!=(modnum lhs, modnum rhs) { return lhs.n != rhs.n; }
};

struct dmodnum {
    long n, mod;

    static long fit(long v, long mod) { return v = v % mod, v >= 0 ? v : v + mod; }

    dmodnum(long v) : n(v), mod(0) {}
    dmodnum(long v, long mod) : n(fit(v, mod)), mod(mod) {}
    explicit operator long() const { return n; }
    dmodnum& operator+=(dmodnum v) { return n = fit(n + v.n, mod), *this; }
    dmodnum& operator-=(dmodnum v) { return n = fit(n - v.n, mod), *this; }
    dmodnum& operator*=(dmodnum v) { return n = (n * v.n) % mod, *this; }
    dmodnum& operator/=(dmodnum v) { return n = (n * invmod(v.n, mod)) % mod, *this; }
    friend dmodnum operator+(dmodnum lhs, dmodnum rhs) { return lhs += rhs; }
    friend dmodnum operator-(dmodnum lhs, dmodnum rhs) { return lhs -= rhs; }
    friend dmodnum operator*(dmodnum lhs, dmodnum rhs) { return lhs *= rhs; }
    friend dmodnum operator/(dmodnum lhs, dmodnum rhs) { return lhs /= rhs; }
    friend bool operator==(dmodnum lhs, dmodnum rhs) { return lhs.n == rhs.n; }
    friend bool operator!=(dmodnum lhs, dmodnum rhs) { return lhs.n != rhs.n; }
};

#endif // MATH_HPP
