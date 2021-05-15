#include <bits/stdc++.h>

using namespace std;

// *****

vector<int> primes, lp, nxt;

auto get_divisors(const unordered_map<int, int>& factors) {
    vector<int> divs = {1};
    for (const auto& [p, e] : factors) {
        int D = divs.size();
        divs.resize(D * (e + 1));
        for (int n = 1; n <= e; n++) {
            for (int d = 0; d < D; d++) {
                divs[d + n * D] = divs[d + (n - 1) * D] * p;
            }
        }
    }
    if (!divs.empty() && divs[0] == 1) {
        divs.erase(begin(divs));
    }
    return divs;
}

auto least_prime_sieve(int N) {
    lp.assign(N + 1, 0), nxt.assign(N + 1, 0);
    nxt[1] = 1;

    for (int P = 0, n = 2; n <= N; n++) {
        if (lp[n] == 0) {
            lp[n] = n, primes.push_back(n), P++;
        }
        for (int i = 0; i < P && primes[i] <= lp[n] && n * primes[i] <= N; ++i) {
            lp[n * primes[i]] = primes[i], nxt[n * primes[i]] = n;
        }
    }

    return lp;
}

auto factor_primes(int n) {
    unordered_map<int, int> primes;
    while (n > 1) {
        primes[lp[n]]++, n = nxt[n];
    }
    return primes;
}

constexpr int MAXN = 1'000'000;
int dp3[MAXN + 1];
int dp2[MAXN + 1];

void setup_dp() {
    dp2[2] = 1;
    dp2[3] = dp3[3] = 1;
    for (int n = 4; n <= MAXN; n++) {
        auto divs = get_divisors(factor_primes(n));
        dp2[n] = dp3[n] = 1;
        for (auto d : divs) {
            if (d > 2) {
                dp3[n] = max(dp3[n], 1 + dp2[n / d - 1]);
            }
            dp2[n] = max(dp2[n], 1 + dp2[n / d - 1]);
        }
    }
}

auto solve() {
    int N;
    cin >> N;
    return dp3[N];
}

// *****

int main() {
    least_prime_sieve(2'000'000);
    setup_dp();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
