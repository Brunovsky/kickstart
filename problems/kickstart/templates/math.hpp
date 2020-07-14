#include <bits/stdc++.h>

using namespace std;

// *****

namespace math {

using i64 = int64_t;
using u64 = uint64_t;

/**
 * Compute b^e (mod m)
 */
i64 power(i64 base, i64 exp, i64 mod) {
    i64 power = 1;
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
i64 power(i64 base, i64 exp) {
    i64 power = 1;
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
u64 facto(u64 n) {
    u64 f = 1;
    while (n > 1) {
        f = f * n--;
    }
    return f;
}

// Compute n1 (mod m)
u64 facto(u64 n, u64 mod) {
    u64 f = 1;
    while (n > 1) {
        f = (f * n--) % mod;
    }
    return f;
}

/**
 * Compute gcd(a,b)
 */
u64 gcd(u64 a, u64 b) {
    while (a != 0) {
        b -= (b / a) * a;
        swap(a, b);
    }
    return b;
}

/**
 * Compute lcm(a,b)
 */
u64 lcm(u64 a, u64 b) {
    return a * (b / gcd(a, b));
}

/**
 * Compute primes dividing n
 */
map<u64, int> factorize(u64 n) {
    map<u64, int> primes;
    while (n & 1) {
        primes[2]++;
        n >>= 1;
    }
    for (int p = 3; n > 1; p += 2) {
        while (n % p == 0) {
            primes[p]++;
            n = n / p;
        }
    }
    return primes;
}

/**
 * Compute phi(n) (totient function), naively
 */
u64 phi(u64 n) {
    u64 tot = 1;
    if ((n & 1) == 0) {
        n >>= 1;
        while ((n & 1) == 0) {
            tot <<= 1;
            n >>= 1;
        }
    }
    for (u64 p = 3; n > 1; p += 2) {
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
u64 binomial(u64 n, u64 k) {
    assert(k <= n);
    k = min(k, n - k);
    n = n - k + 1;
    u64 binom = 1;
    u64 i = 1;
    while (i <= k) {
        binom = (binom * n++) / i++;
    }
    return binom;
}

} // namespace math
