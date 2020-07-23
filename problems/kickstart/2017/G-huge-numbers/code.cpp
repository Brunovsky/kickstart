#include <bits/stdc++.h>

using namespace std;

// *****

ulong power_mod(ulong b, ulong e, ulong mod) {
    ulong power = 1;
    ulong mul = b % mod;
    while (e > 0) {
        if (e & 1) {
            power = (power * mul) % mod;
        }
        e >>= 1;
        mul = (mul * mul) % mod;
    }
    return power;
}

auto solve() {
    ulong A, N, P;
    cin >> A >> N >> P;
    for (ulong n = 1; n <= N; ++n) {
        A = power_mod(A, n, P);
    }
    return A;
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
