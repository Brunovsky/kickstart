#include <bits/stdc++.h>

using namespace std;

// *****

// dp[i][l][r]: minops to create s[1..i] while having [l..r] on the
// clipboard. dp[i][0][0]: minops to create s[1..i] with garbage on the
// clipboard.
int dp[302][302][302];

auto solve() {
    int S;
    string s;

    cin >> s >> ws;
    S = s.length();

    memset(dp, 0x7f, sizeof(dp));
    dp[0][0][0] = 0;
    dp[1][0][0] = 1;
    dp[1][1][1] = 2;

    for (int i = 2; i <= S; i++) {
        // append s[i] char to s[1..i-1] and maintain the clipboard
        dp[i][0][0] = dp[i - 1][0][0] + 1;
        for (int l = 1; l < i; l++) {
            for (int r = l; r < i; r++) {
                dp[i][l][r] = dp[i - 1][l][r] + 1;
            }
        }
        // try to paste the clipboard containing s[l..r] to s[1..j-1]
        for (int r = i - 1; r >= 1; r--) {
            int l = r;
            int j = i;
            do {
                if (s[l - 1] != s[j - 1]) {
                    break;
                }
                // s[l..r] == s[j..i]
                // if s[l..r] is in the clipboard we can paste!
                dp[i][l][r] = dp[j - 1][l][r] + 1;
                // now the clipboard contains s[j..i] also!
                dp[i][j][i] = min(dp[i][j][i], dp[i][l][r]);
                --j, --l;
            } while (l >= 1 && j > r);
        }
        // find minops with whatever on the clipboard
        int ops = dp[i][0][0];
        for (int l = 1; l <= i; l++) {
            for (int r = l; r <= i; r++) {
                ops = min(ops, dp[i][l][r]);
            }
        }
        // copy s[l..r] to the clipboard
        for (int l = 1; l <= i; l++) {
            for (int r = l; r <= i; r++) {
                dp[i][l][r] = min(dp[i][l][r], ops + 1);
            }
        }
        dp[i][0][0] = ops;
    }

    return dp[S][0][0];
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
