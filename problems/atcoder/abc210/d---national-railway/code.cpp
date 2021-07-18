#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int H, W;
    long C;
    cin >> H >> W >> C;
    vector<vector<int>> A(H, vector<int>(W));
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            cin >> A[i][j];
        }
    }
    // i,j
    //     x,y
    constexpr long inf = LONG_MAX / 4;
    long ans = inf;
    vector<long> dp(W, inf);

    for (int x = 0; x < H; x++) {
        for (int y = 0; y < W; y++) {
            long plus = A[x][y] + C * (x + y);
            ans = min(ans, plus + dp[y]);
            long minus = A[x][y] - C * (x + y);
            dp[y] = min(dp[y], minus);
            if (y + 1 < W) {
                dp[y + 1] = min(dp[y + 1], dp[y]);
            }
        }
    }

    //    i,j
    // x,y
    dp.assign(W, inf);
    for (int x = 0; x < H; x++) {
        for (int y = W - 1; y >= 0; y--) {
            long plus = A[x][y] + C * (x - y);
            ans = min(ans, plus + dp[y]);
            long minus = A[x][y] + C * (y - x);
            dp[y] = min(dp[y], minus);
            if (y > 0) {
                dp[y - 1] = min(dp[y - 1], dp[y]);
            }
        }
    }

    cout << ans << '\n';
    return 0;
}
