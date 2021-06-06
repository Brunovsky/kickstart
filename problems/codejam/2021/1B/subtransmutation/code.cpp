#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, A, B;
    cin >> N >> A >> B;
    vector<int> U(N + 1, 0);
    for (int i = 1; i <= N; i++) {
        cin >> U[i];
    }
    int g = gcd(A, B);
    for (int i = 1; i <= N; i++) {
        if ((N - i) % g != 0 && U[i] != 0) {
            return "IMPOSSIBLE"s;
        }
    }

    int x = N;
    do {
        vector<int> dp(x + 2, 0);
        dp[x] = 1;
        bool ok = true;
        for (int i = x; i >= 1 && ok; i -= g) {
            int a = min(x + 1, i + A), b = min(x + 1, i + B);
            dp[i] += dp[a] + dp[b];
            if (i <= N) {
                dp[i] -= U[i];
                ok = dp[i] >= 0;
            }
        }

        if (ok) {
            return to_string(x);
        }
        x += g;
    } while (true);
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
