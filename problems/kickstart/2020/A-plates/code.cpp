#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 51
#define MAXK 31
#define MAXP 1501

int N, K, P;
int dp[MAXN][MAXP];
int stacks[MAXN][MAXK];
// stacks[n][1] is at the top, stacks[n][K] is at the bottom

auto solve() {
    cin >> N >> K >> P;
    memset(dp, 0, sizeof(dp));
    memset(stacks, 0, sizeof(stacks));

    for (int n = 1; n <= N; ++n) {
        for (int k = 1; k <= K; ++k) {
            cin >> stacks[n][k];
            stacks[n][k] += stacks[n][k - 1];
        }
    }

    for (int n = 1; n <= N; ++n) {
        for (int p = 1; p <= P; ++p) {
            int best = dp[n - 1][p];
            for (int np = 1; np <= min(p, K); ++np) {
                best = max(best, stacks[n][np] + dp[n - 1][p - np]);
            }
            dp[n][p] = best;
        }
    }

    return dp[N][P];
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
