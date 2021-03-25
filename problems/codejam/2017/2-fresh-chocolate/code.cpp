#include <bits/stdc++.h>

using namespace std;

// *****

int dp[101][101][101][4];

int N, P;
vector<int> G, cnt;

void setup() {
    memset(dp, 0, sizeof(dp));
    for (int a = 0; a <= 100; a++) {
        for (int b = 0; b <= 100; b++) {
            for (int c = 0; c <= 100; c++) {
                for (int d = 0, e; d < 4; d++) {
                    auto& n = dp[a][b][c][d];
                    if (a > 0)
                        e = (d + 1) % 4, n = max(n, dp[a - 1][b][c][e] + (d == 0));
                    if (b > 0)
                        e = (d + 2) % 4, n = max(n, dp[a][b - 1][c][e] + (d == 0));
                    if (c > 0)
                        e = (d + 3) % 4, n = max(n, dp[a][b][c - 1][e] + (d == 0));
                }
            }
        }
    }
}

auto solve() {
    cin >> N >> P;
    G.assign(N, 0);
    cnt.assign(P, 0);
    for (int g : G)
        cin >> g, cnt[g % P]++;

    if (P == 2)
        return cnt[0] + (cnt[1] + 1) / 2;
    else if (P == 3) {
        int a = min(cnt[1], cnt[2]), b = max(cnt[1], cnt[2]);
        return cnt[0] + a + (b - a + 2) / 3;
    } else if (P == 4) {
        return cnt[0] + dp[cnt[1]][cnt[2]][cnt[3]][0];
    }

    return 0;
}

// *****

int main() {
    setup();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
