#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 101
#define MAXQ 100

constexpr double inf = numeric_limits<double>::infinity();

int N, Q;
long E[MAXN], S[MAXN];
long D[MAXN][MAXN];
double dp[2][MAXN][MAXN];

auto solve() {
    cin >> N >> Q;
    for (int i = 1; i <= N; i++)
        cin >> E[i] >> S[i];
    for (int u = 1; u <= N; u++)
        for (int v = 1; v <= N; v++)
            cin >> D[u][v];

    memset(dp, 0, sizeof(dp));
    for (int s = 1; s <= N; s++) {
        using entry_t = pair<long, int>;
        priority_queue<entry_t, vector<entry_t>, greater<>> Q;
        Q.push({0, s});

        vector<long> mindist(N + 1, LONG_MAX);
        mindist[s] = 0;

        while (!Q.empty()) {
            auto [d, u] = Q.top();
            Q.pop();
            if (d > mindist[u])
                continue;
            for (int v = 1; v <= N; v++) {
                if (D[u][v] > 0 && d + D[u][v] < mindist[v] && d + D[u][v] <= E[s]) {
                    mindist[v] = d + D[u][v];
                    Q.push({mindist[v], v});
                }
            }
        }

        for (int u = 1; u <= N; u++) {
            dp[0][s][u] = mindist[u] < LONG_MAX ? 1.0 * mindist[u] / S[s] : inf;
        }
        dp[0][s][s] = 0;
    }

    for (int u = 1; u <= N; u++)
        for (int v = 1; v <= N; v++)
            dp[1][u][v] = inf;

    auto curr = dp[0], next = dp[1];
    constexpr int maxh = 9; // log2(100)

    for (int h = 1; h <= maxh; h++) {
        for (int u = 1; u <= N; u++) {
            for (int v = 1; v <= N; v++) {
                for (int w = 1; w <= N; w++) {
                    next[u][v] = min(next[u][v], curr[u][w] + curr[w][v]);
                }
            }
        }
        swap(next, curr);
    }

    for (int q = 0; q < Q; q++) {
        int u, v;
        cin >> u >> v;
        cout << ' ' << curr[u][v];
    }
    cout << endl;
}

// *****

int main() {
    cout << setprecision(7) << fixed << showpoint;
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
    }
    return 0;
}
