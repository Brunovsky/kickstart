#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXA 1010
#define MAXN 102

int N, K, Q;
int A[MAXN];
int R[MAXN];
int P[MAXN];
int S[MAXN];
int H[MAXN];
int dp[MAXN][MAXN][2];

auto solve() {
    Q = 1;
    memset(A, 0, sizeof(A));
    memset(H, 0, sizeof(H));
    memset(R, 0, sizeof(R));
    memset(S, 0, sizeof(S));
    memset(P, 0, sizeof(P));
    memset(dp, 0, sizeof(dp));

    cin >> N >> K;
    for (int i = 0; i < N; ++i) {
        cin >> A[i];
    }

    int cur = A[0], len = 1;
    for (int i = 1; i < N; i++) {
        if (A[i] != cur) {
            H[Q] = cur;
            R[Q] = len;
            S[Q] = S[Q - 1] + len;
            Q++;
            cur = A[i];
            len = 0;
        }
        len++;
    }

    H[Q] = cur;
    R[Q] = len;
    S[Q] = S[Q - 1] + len;
    Q++;

    int visit[MAXA] = {0};
    for (int i = 1; i < Q; ++i) {
        P[i] = visit[H[i]];
        visit[H[i]] = i;
    }

    for (int i = 1; i < Q; ++i) {
        int valley = S[i] - S[P[i]] - R[i];
        assert(valley <= N);

        dp[i][0][1] = dp[P[i]][0][1] + valley;
        dp[i][0][0] = min(dp[i][0][1], dp[i - 1][0][0] + R[i]);

        for (int k = 1; k <= K; ++k) {
            dp[i][k][1] = min(dp[i - 1][k - 1][0], dp[P[i]][k][1] + valley);
            dp[i][k][0] = min(dp[i][k][1], dp[i - 1][k][0] + R[i]);
        }
    }

    return min(dp[Q - 1][K][0], dp[Q - 1][K][1]);
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
