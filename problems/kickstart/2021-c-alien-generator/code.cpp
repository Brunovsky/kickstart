#include <bits/stdc++.h>

using namespace std;

// *****

vector<int> primes, lp, nxt;

auto classic_sieve(int N) {
    vector<int> primes;
    vector<bool> isprime;
    isprime.assign(N + 1, true);

    for (int n = 4; n <= N; n += 2)
        isprime[n] = false;
    for (int n = 3; n * n <= N; n += 2)
        if (isprime[n])
            for (int i = n * n; i <= N; i += 2 * n)
                isprime[i] = false;
    if (N > 2)
        primes.push_back(2);
    for (int n = 3; n <= N; n += 2)
        if (isprime[n])
            primes.push_back(n);

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

auto solve() {
    long G;
    cin >> G;
    auto factors = factor_map(G);
    factors.erase(2);
    long ans = 1;
    for (auto [p, e] : factors) {
        ans *= e + 1;
    }
    return ans;
}

// *****

int main() {
    classic_sieve(1'000'000);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
