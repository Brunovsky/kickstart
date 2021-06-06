#include <bits/stdc++.h>

using namespace std;

// *****

constexpr ulong MOD = 1'000'000'007UL;

ulong N, K;
vector<ulong> A;

// compute n^k modulo MOD
ulong expo(ulong n, ulong k) {
    ulong b = n, power = 1;
    while (k > 0) {
        if (k & 1) {
            power = (power * b) % MOD;
        }
        k >>= 1;
        b = (b * b) % MOD;
    }
    return power;
}

// compute (n^(K + 1) - 1) / (n - 1) - 1 modulo MOD
// c = (n^(K + 1) - 1) * (n - 1)^-1 - 1 modulo MOD
ulong power(ulong n) {
    if (n == 1) {
        return K;
    }

    ulong a = expo(n, K + 1);
    ulong b = expo(n - 1, MOD - 2);
    ulong c = ((a + MOD - 1) * b + MOD - 1) % MOD;
    return c;
}

void read() {
    ulong x, y, C, D, E1, E2, F;
    cin >> N >> K >> x >> y >> C >> D >> E1 >> E2 >> F >> ws;
    A.assign(N + 1, 0);
    A[1] = (x + y) % F;
    for (ulong n = 2; n <= N; ++n) {
        ulong X = (C * x + D * y + E1) % F;
        ulong Y = (D * x + C * y + E2) % F;
        x = X;
        y = Y;
        A[n] = (x + y) % F;
    }
}

auto solve() {
    read();
    ulong weight_sum = 0, POWER_SUM = 0;

    for (ulong n = N, c = 1; n > 0; --n, ++c) {
        weight_sum = (weight_sum + c * A[n]) % MOD;
        POWER_SUM = (POWER_SUM + power(n) * weight_sum) % MOD;
    }

    return POWER_SUM;
}

// (n^1 + n^n + n^3 + ... + n^K) = (n^(K+1) - 1) / (n - 1) - 1
// Base 1: N.A1 + (N-1).A2 + ... + 1.An
// Base 2: (N-1).A2 + ... + 1.An
// Base 3: (N-2).A3 + ... + 1.An
// ...
// Base n: 1.An

// *****

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
