#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100'000'000

/**
 * Eratosthenes sieve
 * Sieve to find all primes up N. This allows querying primes or number of primes in a
 * range [L,R] at most N*N.
 * Sieve complexity: O(N log log N)
 * Query complexity: O(sqrt(R) log log (R - L))
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
