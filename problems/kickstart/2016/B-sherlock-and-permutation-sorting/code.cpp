#include <bits/stdc++.h>

using namespace std;

// *****

using u64 = uint64_t;

struct sum_t {
    u64 one = 0; // c[n][1]
    u64 C0 = 0;  // SUM_i (c[n][i])          - actually n!
    u64 C1 = 0;  // SUM_i (2i * c[n][i])
    u64 C2 = 0;  // SUM_i (i^2 * c[n][i])
};

auto solve() {
    u64 N, M;
    cin >> N >> M >> ws;

    vector<sum_t> f(N + 1, sum_t{});
    f[1] = {1, 1, 2, 1};

    u64 fac = 1;

    for (u64 n = 2; n <= N; ++n) {
        auto &F = f[n];
        for (u64 i = 1; i < n; ++i) {
            const auto &L = f[i];
            const auto &R = f[n - i];
            u64 one = L.one;
            u64 C0 = L.one * (R.C0) % M;
            u64 C1 = L.one * (R.C1 + 2 * R.C0) % M;
            u64 C2 = L.one * (R.C2 + R.C1 + R.C0) % M;
            F.C0 = (F.C0 + C0) % M;
            F.C1 = (F.C1 + C1) % M;
            F.C2 = (F.C2 + C2) % M;
        }
        fac = (fac * n) % M;
        F.one = (fac + M - F.C0) % M;
        F.C0 = (F.C0 + F.one) % M;
        F.C1 = (F.C1 + 2 * F.one) % M;
        F.C2 = (F.C2 + F.one) % M;
    }

    return f[N].C2;
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
