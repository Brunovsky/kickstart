#include <bits/stdc++.h>

using namespace std;

// *****

constexpr long MOD = 1'000'000'007;

long centers_onion(long R, long C) {
    long S = R + C;
    long n = (R - 1) / 2;
    // t2 = Sn(n+1)(2n+1)/6
    long t2 = (n * (n + 1) / 2) % MOD;
    t2 = ((t2 + (t2 % 3) * MOD) * (2 * n + 1) / 3) % MOD;
    t2 = (S * t2) % MOD;
    // t3 = -n^2(n+1)^2
    long t3 = n * (n + 1) % MOD;
    t3 = (t3 * t3) % MOD;
    // center
    long c = 0;
    if ((R & 1) == 0) {
        c = (R * R / 4) % MOD;
        c = ((C - R + 1) * c) % MOD;
    }

    return (2 * (t2 - t3) + c) % MOD;
}

long dots_onion(long R, long C) {
    long S = R + C - 2;
    long n = (R - 2) / 2;
    // t1 = Sn(n+1)/2
    long t1 = (n * (n + 1) / 2) % MOD;
    t1 = (S * t1) % MOD;
    // t2 = (S-4)n(n+1)(2n+1)/6
    long t2 = (n * (n + 1) / 2) % MOD;
    t2 = ((t2 + (t2 % 3) * MOD) * (2 * n + 1) / 3) % MOD;
    t2 = ((S - 4) * t2) % MOD;
    // t3 = -n^2(n+1)^2
    long t3 = n * (n + 1) % MOD;
    t3 = (t3 * t3) % MOD;
    // center
    long c = 0;
    if ((R & 1) == 1) {
        c = ((R - 1) * (R + 1) / 4) % MOD;
        c = ((C - R + 1) * c) % MOD;
    }

    return (2 * (t1 + t2 - t3) + c) % MOD;
}

auto solve() {
    long R, C;
    cin >> R >> C >> ws;
    if (R > C) {
        swap(R, C);
    }
    return (centers_onion(R, C) + dots_onion(R, C) + 2 * MOD) % MOD;
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
