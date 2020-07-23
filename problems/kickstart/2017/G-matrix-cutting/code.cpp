#include <bits/stdc++.h>

using namespace std;

// *****

int dp[41][41][40][40];
int dm[41][41][40][40];

auto solve() {
    int N, M;
    cin >> N >> M;
    memset(dp, 0, sizeof(dp));
    memset(dm, 0x7f, sizeof(dm)); // basically INT_MAX
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            cin >> dm[1][1][i][j];
        }
    }
    for (int iS = 1; iS <= N; ++iS) {
        for (int jS = 1; jS <= M; ++jS) {
            if (iS == 1 && jS == 1) {
                continue;
            }
            for (int iL = 0, iR = iS; iR <= N; ++iL, ++iR) {
                for (int jL = 0, jR = jS; jR <= M; ++jL, ++jR) {
                    int &curr = dp[iS][jS][iL][jL];
                    int m = iS > 1 ? min(dm[1][jS][iL][jL], dm[iS - 1][jS][iL + 1][jL])
                                   : min(dm[iS][1][iL][jL], dm[iS][jS - 1][iL][jL + 1]);
                    dm[iS][jS][iL][jL] = m;
                    // horizontal cuts
                    for (int iM = iL + 1; iM < iR; ++iM) {
                        int top = dp[iM - iL][jS][iL][jL];
                        int bot = dp[iR - iM][jS][iM][jL];
                        curr = max(curr, top + bot + m);
                    }
                    // vertical cuts
                    for (int jM = jL + 1; jM < jR; ++jM) {
                        int top = dp[iS][jM - jL][iL][jL];
                        int bot = dp[iS][jR - jM][iL][jM];
                        curr = max(curr, top + bot + m);
                    }
                }
            }
        }
    }

    return dp[N][M][0][0];
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
