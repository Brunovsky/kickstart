#include <bits/stdc++.h>

using namespace std;

// *****

using u64 = uint64_t;

u64 power_mod(u64 b, u64 e, u64 mod) {
    u64 power = 1, exp = e;
    u64 mul = b % mod;
    while (e > 0) {
        if (e & 1)
            power = (power * mul) % mod;
        e >>= 1;
        mul = (mul * mul) % mod;
    }
    return power;
}

auto solve() {
    u64 A, N, P;
    cin >> A >> N >> P;
    for (u64 n = 1; n <= N; ++n)
        A = power_mod(A, n, P);
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
