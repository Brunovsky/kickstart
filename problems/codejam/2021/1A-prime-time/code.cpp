#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXP 499

unordered_map<int, int> factor_simple(long n) {
    unordered_map<int, int> primes;
    while ((n & 1) == 0) {
        primes[2]++;
        n >>= 1;
    }
    for (long p = 3; p * p <= n && p <= MAXP; p += 2) {
        while ((n % p) == 0) {
            primes[p]++;
            n = n / p;
        }
    }
    if (n > MAXP) {
        return {};
    } else if (n > 1) {
        primes[n]++;
    }
    return primes;
}

auto solve() {
    unordered_map<int, long> lib;
    int M;
    cin >> M;
    long sum = 0;
    for (int i = 0; i < M; i++) {
        long p, n;
        cin >> p >> n;
        lib[p] = n;
        sum += p * n;
    }

    for (long s = sum - 2; s >= 2 && sum - s <= 10'000; s--) {
        auto primes = factor_simple(s);
        bool ok = !primes.empty();
        for (auto [p, n] : primes)
            if (n > lib[p])
                ok = false;

        long m = 0;
        for (auto [p, n] : primes)
            m += p * n;

        if (ok && sum == s + m)
            return s;
    }

    return 0L;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
