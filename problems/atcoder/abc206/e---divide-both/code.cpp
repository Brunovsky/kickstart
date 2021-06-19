#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

vector<int> primes, lp, nxt;

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
}

auto factor_primes(int n) {
    unordered_map<int, int> primes;
    while (n > 1) {
        primes[lp[n]]++, n = nxt[n];
    }
    return primes;
}

int mobius(int n) {
    auto primes = factor_primes(n);
    for (auto [p, c] : primes) {
        if (c > 1) {
            return 0;
        }
    }
    return primes.size() % 2 == 0 ? 1 : -1;
}

/**
 * Number of pairs of integers 1<=x<=n and 1<=y<=m such that coprime(x,y)
 */
long count_coprime_pairs(int n, int m) {
    long ans = 0;
    if (n > m) {
        swap(n, m);
    }
    for (int d = 1; d <= n; d++) {
        ans += 1L * mobius(d) * (n / d) * (m / d);
    }
    return ans;
}

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    least_prime_sieve(1'000'000);
    int L, R;
    cin >> L >> R;
    long coprime = count_coprime_pairs(R, R) - 2 * count_coprime_pairs(L - 1, R) +
                   count_coprime_pairs(L - 1, L - 1);
    long multiples = 0;
    for (int x = L; x <= R; x++) {
        // count multiples of x greater than x
        multiples += R / x - 1;
    }
    // add x,x and y,x
    multiples = 2 * multiples + (R - L + 1);
    long total = 1L * (R - L + 1) * (R - L + 1);
    long ans = total - coprime - multiples;
    if (L == 1) {
        ans += 2 * (R - L) + 1;
    }
    cout << ans << '\n';
    return 0;
}
