#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    using stack_t = vector<int>;
    vector<stack_t> stacks;
    int N, K, P;

    cin >> N >> K >> P;
    stacks.assign(N + 1, stack_t(K + 1, 0));
    for (int n = 1; n <= N; ++n)
        for (int k = 1; k <= K; ++k) {
            cin >> stacks[n][k];
            stacks[n][k] += stacks[n][k - 1];
        }

    // stacks[n][1] is at the top, stacks[n][K] is at the bottom

    // dp on the number of plates increasing, on the left n stacks increasing
    // if we use p plates for the first n stacks, for the n+1 stack and p
    // use the best of
    // { 0 + dp[p][n], n[1] + dp[p-1][n], n[1] + n[2] + dp[p-2][n]... }
    vector<vector<int>> dp(N + 1, vector<int>(P + 1, 0));

    for (int n = 1; n <= N; ++n) {
        for (int p = 1; p <= P; ++p) {
            int best = dp[n - 1][p];
            for (int np = 1; np <= min(p, K); ++np)
                best = max(best, stacks[n][np] + dp[n - 1][p - np]);
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
