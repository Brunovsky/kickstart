#include <bits/stdc++.h>

using namespace std;

// *****

#define ALL(s) begin(s), end(s)

auto solve() {
    int R, C;
    cin >> R >> C;
    int N = 2 * (R + C);
    vector<int> couple(N);
    for (int i = 0, u, v; i < R + C; i++)
        cin >> u >> v, u--, v--, couple[u] = v, couple[v] = u;

    vector<int> couple_dist(N);
    for (int u = 0; u < N; u++) {
        int v = couple[u], d = abs(u - v);
        couple_dist[u] = couple_dist[v] = min(d, N - d);
    }

    vector<int> close_couples;
    for (int u = 0; u < N; u++) {
        if (u < couple[u]) {
            close_couples.push_back(u);
        }
    }
    stable_sort(ALL(close_couples),
                [&](int u, int v) { return couple_dist[u] < couple_dist[v]; });

    constexpr int dd[4][2] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}}; // S, W, N, E
    constexpr int mapd[4][2] = {{1, 3}, {0, 2}, {3, 1}, {2, 0}}; // d -> mapd[d][c]
    constexpr char chard[4] = {'\\', '/', '\\', '/'};            // try to turn 'left'
    vector<string> board(R, string(C, ' '));
    vector<bool> vis(N);

    auto all_visited = [&vis, N](int s, int t) {
        bool ok = true;
        for (int i = s; i != t && ok; i = (i + 1) % N)
            ok = vis[i];
        return ok;
    };

    auto facing = [R, C](int u) -> tuple<int, int, int> {
        if (u < C)
            return {0, -1, u};
        u -= C;
        if (u < R)
            return {1, u, C};
        u -= R;
        if (u < C)
            return {2, R, C - 1 - u};
        u -= C;
        return {3, R - 1 - u, -1};
    };

    for (int u : close_couples) {
        int v = couple[u];
        vis[u] = vis[v] = true;

        if (!all_visited(u, v)) {
            if (!all_visited(v, u)) {
                cout << "IMPOSSIBLE\n";
                return;
            }
            swap(u, v);
        }

        auto [d, r, c] = facing(u);
        r += dd[d][0], c += dd[d][1];
        while (0 <= r && r < R && 0 <= c && c < C) {
            if (board[r][c] == ' ') {
                board[r][c] = chard[d];
            }
            d = mapd[d][board[r][c] == '\\'];
            r += dd[d][0], c += dd[d][1];
        }
        auto [dv, rv, cv] = facing(v);
        if (d != (dv ^ 0b10) || r != rv || c != cv) {
            cout << "IMPOSSIBLE\n";
            return;
        }
    }

    for (int r = 0; r < R; r++)
        for (int c = 0; c < C; c++)
            if (board[r][c] == ' ')
                board[r][c] = '/';

    for (int r = 0; r < R; r++)
        cout << board[r] << endl;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":\n";
        solve();
    }
    return 0;
}
