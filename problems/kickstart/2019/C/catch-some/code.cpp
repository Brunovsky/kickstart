#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 1001
#define MAXP 100'001
#define MAXA 1001

struct Node {
    int H; // home
    int T; // home or elsewhere
};

int N, K;
int P[MAXN];
int A[MAXN];
vector<vector<int>> B;
set<int> S;
Node dp[MAXA][MAXN];

auto solve() {
    cin >> N >> K >> ws;
    S.clear();
    B.assign(MAXP, {});
    for (int i = 0; i < N; ++i) {
        cin >> P[i];
    }
    for (int i = 0; i < N; ++i) {
        cin >> A[i];
    }
    for (int i = 0; i < N; ++i) {
        B[A[i]].push_back(P[i]);
        S.insert(A[i]);
    }

    const int C = S.size();

    memset(dp, 0x7f, sizeof(dp));
    // dp[c][k]: minimum distance for colors 1..c, to find k dogs.

    dp[0][0] = {0, 0};

    int a = 1, seen = 0;
    for (int c : S) {
        const int dogs = B[c].size();
        seen += dogs;
        dp[a][0] = {0, 0};
        sort(begin(B[c]), end(B[c]));

        for (int k = 1, maxk = min(K, seen); k <= maxk; ++k) {
            // stay put and skip these dogs
            dp[a][k].H = dp[a - 1][k].H;
            dp[a][k].T = dp[a - 1][k].T;

            // consider going to see n dogs of color c
            for (int n = 1, maxn = min(dogs, k); n <= maxn; ++n) {
                int distance = B[c][n - 1];
                int go_and_return_home = dp[a - 1][k - n].H + 2 * distance;
                int go_home_change = dp[a - 1][k - n].T + 2 * distance;
                int come_changed = dp[a - 1][k - n].H + 1 * distance;
                dp[a][k].H = min(dp[a][k].H, go_and_return_home);
                dp[a][k].T = min(dp[a][k].T, min(go_home_change, come_changed));
            }
        }
        a++;
    }
    return dp[C][K].T;
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
