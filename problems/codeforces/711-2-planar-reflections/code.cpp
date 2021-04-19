#include <bits/stdc++.h>

using namespace std;

// *****

constexpr size_t MAXN = 1001, MAXK = 1001;
size_t dp[MAXN][MAXK];

void setup() {
    for (int n = 0; n < MAXN; n++)
        dp[n][0] = 0, dp[n][1] = 1;

    for (int k = 2; k < MAXK; k++) {
        dp[0][k] = k;
        for (int n = 1; n < MAXN; n++) {
            dp[n][k]
        }
    }
}

auto solve() { return 0; }

// *****

int main() {
    ios::sync_with_stdio(false);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << solve() << endl;
    }
    return 0;
}
