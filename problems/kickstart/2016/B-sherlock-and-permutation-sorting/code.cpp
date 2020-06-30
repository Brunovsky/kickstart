#include <bits/stdc++.h>

using namespace std;

// *****

using u64 = uint64_t;

u64 N, M;
vector<vector<u64>> f;

auto solve() {
    cin >> N >> M >> ws;
    f.assign(N + 1, {});
    f[0].assign(N + 1, 0);

    for (u64 n = 1; n <= N; ++n) {
        f[n].assign(n + 1, 0);
        u64 fac = 1;
        u64 sum = 0;
        for (u64 k = 1; k < n; ++k) {
            fac = (fac * k) % M;
            for (uint i = 1; i <= n - k; ++i) {
                // 1 chunk with k numbers + i chunks covering n - k numbers
                u64 combs = f[k][1] * f[n - k][i];
                f[n][i + 1] = (f[n][i + 1] + combs) % M;
                sum = (sum + combs) % M;
            }
        }
        fac = (fac * n) % M;
        f[n][1] = (fac + M - sum) % M;
    }

    u64 sum = 0;
    for (u64 i = 1; i <= N; ++i) {
        sum = (sum + f[N][i] * i * i) % M;
    }
    return sum;
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
