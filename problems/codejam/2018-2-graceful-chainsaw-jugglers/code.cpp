#include <bits/stdc++.h>

using namespace std;

// *****

int dp[501][501];

void setup() {
    for (int s = 1; s <= 100; s++) {
        for (int r = 0, b = s; r <= s; r++, b--) {
            for (int S = 1000; S >= s; S--) {
                for (int R = 0, B = S; R <= S; R++, B--) {
                    if (r <= R && b <= B && R <= 500 && B <= 500) {
                        dp[R][B] = max(dp[R][B], dp[R - r][B - b] + 1);
                    }
                }
            }
        }
    }
}

auto solve() {
    int R, B;
    cin >> R >> B;
    return dp[R][B];
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
