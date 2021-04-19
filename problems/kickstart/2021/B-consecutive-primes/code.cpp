#include <bits/stdc++.h>

using namespace std;

// *****

auto classic_sieve(int N) {
    vector<int> primes;
    vector<bool> isprime(N + 1, true);

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

vector<int> P;

auto solve() {
    int K = P.size();
    long Z;
    cin >> Z;
    int L = 0, R = K - 2;
    while (L <= R) {
        int i = (L + R) / 2;
        if (1L * P[i] * P[i + 1] <= Z) {
            L = i + 1;
        } else {
            R = i - 1;
        }
    }
    return 1L * P[L - 1] * P[L];
}

// *****

int main() {
    P = classic_sieve(1'000'500'000);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
