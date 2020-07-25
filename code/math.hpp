#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Compute b^e (mod m)
 */
long power(long base, long exp, long mod) {
    long power = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp & 1) {
            power = (power * base) % mod;
        }
        exp >>= 1;
        base = (base * base) % mod;
    }
    return power;
}

/**
 * Compute b^e
 */
long power(long base, long exp) {
    long power = 1;
    while (exp > 0) {
        if (exp & 1) {
            power = power * base;
        }
        exp >>= 1;
        base = base * base;
    }
    return power;
}

// Compute n!
ulong factorial(ulong n) {
    ulong f = 1;
    while (n > 1) {
        f = f * n--;
    }
    return f;
}

// Compute n! (mod m)
ulong factorial(ulong n, ulong mod) {
    ulong f = 1;
    while (n > 1) {
        f = (f * n--) % mod;
    }
    return f;
}

/**
 * Compute x, y such that ax + by = gcd(a,b)
 */
long gcd(long a, long b, long &x, long &y) {
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

/**
 * Compute x such that ax = 1 (mod m) (modular multiplicative inverse)
 */
long invmod(long a, long mod) {
    long x, y;
    auto g = gcd(a, mod, x, y);
    assert(g == 1);
    x = x % mod;
    return x >= 0 ? x : x + mod;
}

/**
 * Compute gcd(a,b)
 */
long gcd(long a, long b) {
    while (a != 0) {
        b = b % a;
        swap(a, b);
    }
    return abs(b);
}

/**
 * Compute lcm(a,b)
 */
ulong lcm(ulong a, ulong b) {
    return a * (b / gcd(a, b));
}

/**
 * Compute primes dividing n
 */
map<ulong, int> factorize(ulong n) {
    map<ulong, int> primes;
    while ((n & 1) == 0) {
        primes[2]++;
        n >>= 1;
    }
    for (int p = 3; n > 1; p += 2) {
        while ((n % p) == 0) {
            primes[p]++;
            n = n / p;
        }
    }
    return primes;
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
ulong binomial(ulong n, ulong k) {
    assert(k <= n);
    k = min(k, n - k);
    n = n - k + 1;
    ulong binom = 1;
    ulong i = 1;
    while (i <= k) {
        binom = (binom * n++) / i++;
    }
    return binom;
}
