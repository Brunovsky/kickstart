#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, K;
    cin >> N >> K;
    vector<double> P(N);
    for (int i = 0; i < N; i++)
        cin >> P[i];
    sort(begin(P), end(P));

    double best = 0;
    for (int m = 0; m <= K; m++) {
        vector<double> Q(begin(P), begin(P) + m);
        Q.insert(end(Q), end(P) - (K - m), end(P));

        vector<double> dp(K + 1, 0);
        dp[0] = 1.0;
        for (int i = 0; i < K; i++) {
            for (int k = K; k >= 1; k--) {
                dp[k] = dp[k] * (1 - Q[i]) + dp[k - 1] * Q[i];
            }
            dp[0] *= 1 - Q[i];
        }
        best = max(best, dp[K / 2]);
    }
    return best;
}

// *****

int main() {
    cout << setprecision(7) << fixed << showpoint;
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
