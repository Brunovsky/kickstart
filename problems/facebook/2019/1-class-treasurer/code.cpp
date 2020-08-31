#include <bits/stdc++.h>

using namespace std;

// *****

constexpr long mod = 1'000'000'007;
constexpr long inv = 500'000'004;

long modpow(long b, long e) {
    long power = 1;
    b = b % mod;
    while (e > 0) {
        if (e & 1)
            power = (power * b) % mod;
        e >>= 1;
        b = (b * b) % mod;
    }
    return power;
}

// b > a + K  <=> b - a > K
// there must be no set of K+1 students all voting B
// i.e. at most K Bs

auto solve() {
    int N, K;
    cin >> N >> K >> ws;
    string votes;
    cin >> votes >> ws;
    int D = 0, i = N - 1;
    long pay = 0, pow2 = modpow(2, N);
    while (i >= 0) {
        D += (votes[i--] == 'B') ? 1 : -1;
        if (D == K + 1) {
            pay = (pay + pow2) % mod;
            D -= 2;
        }
        D = max(0, D);
        pow2 = pow2 * inv % mod;
    }
    assert(pow2 == 1);
    return pay;
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
