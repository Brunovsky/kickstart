#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN   100'010
#define MAXLEN 200
#define INF    0x7f7f7f7f7f7f7f7e

int N;
long X[MAXN];
long dp[2][MAXLEN];

auto solve() {
    cin >> N;
    for (int i = 0; i < N; i++) {
        cin >> X[i];
    }

    memset(dp, 0x7f, sizeof(dp));
    dp[0][0] = 0;

    for (int i = 1; i <= N; i++) {
        long weight = X[i - 1];
        bool b = i & 1;

        auto& prev = dp[!b];
        auto& next = dp[b];
        next[0] = 0;

        for (int k = 1;; k++) {
            next[k] = prev[k];
            if (prev[k - 1] <= 6 * weight) {
                next[k] = min(next[k], prev[k - 1] + weight);
            }
            if (next[k] >= INF) {
                break;
            }
        }
    }
    int k = 0;
    while (dp[N & 1][k] < INF) {
        k++;
    }
    return k - 1;
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
