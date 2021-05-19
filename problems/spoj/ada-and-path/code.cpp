#include <bits/stdc++.h>

using namespace std;

// *****

inline namespace lib {

struct hopcroft_karp {
    int U, V;
    vector<vector<int>> adj;
    vector<int> mu, mv;

    hopcroft_karp(int U, int V, const vector<array<int, 2>>& g = {})
        : U(U), V(V), adj(U) {
        for (auto [u, v] : g)
            add(u, v);
    }

    void add(int u, int v) {
        assert(0 <= u && u < U && 0 <= v && v < V);
        adj[u].push_back(v);
    }

    vector<int> vis, dist;
    int iteration;
    static inline constexpr int inf = INT_MAX / 2;

    bool bfs() {
        queue<int> Q;
        for (int u = 0; u < U; u++) {
            if (mu[u] == -1) {
                dist[u] = 0;
                Q.push(u);
            } else {
                dist[u] = inf;
            }
        }
        dist[U] = inf;
        while (!Q.empty()) {
            int u = Q.front();
            Q.pop();
            if (dist[u] < dist[U]) {
                for (int v : adj[u]) {
                    // note: the check v != mu[u] is implicit in dist[mv[v]] == inf
                    if (dist[mv[v]] == inf) {
                        dist[mv[v]] = dist[u] + 1;
                        Q.push(mv[v]);
                    }
                }
            }
        }
        return dist[U] != inf;
    }

    bool dfs(int u) {
        if (u == U) {
            return true;
        }
        if (vis[u] == iteration) {
            return false;
        }
        vis[u] = iteration;
        for (int v : adj[u]) {
            if (dist[mv[v]] == dist[u] + 1 && dfs(mv[v])) {
                mv[v] = u;
                mu[u] = v;
                return true;
            }
        }
        return false;
    }

    int compute() {
        vis.assign(U + 1, 0);
        dist.assign(U + 1, 0);
        mu.assign(U + 1, -1);
        mv.assign(V, U);
        int matching = 0;
        iteration = 0;
        while (bfs()) {
            iteration++;
            for (int u = 0; u < U; u++) {
                if (mu[u] == -1 && dfs(u)) {
                    matching++;
                }
            }
        }
        mu.pop_back();
        return matching;
    }
};

} // namespace lib

int mask(int r, int c, int N) { return r * N + c; }
bool bounds(int r, int c, int N) { return r >= 0 && r < N && c >= 0 && c < N; }

auto solve() {
    int N;
    cin >> N;
    vector<vector<int>> board(N, vector<int>(N));
    vector<int> cnt(11);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cin >> board[i][j];
            cnt[board[i][j]]++;
        }
    }

    int U = 0;
    for (int v = 2; v <= 10; v++) {
        if (cnt[v - 1] < cnt[v]) {
            return "NO"s;
        }
        U += cnt[v];
    }
    int V = N * N;

    hopcroft_karp mm(U, V);

    constexpr int dd[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (int id = 0, i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] > 1) {
                int e = 0;
                for (auto d : dd) {
                    int r = i + d[0], c = j + d[1];
                    if (bounds(r, c, N) && board[i][j] == board[r][c] + 1) {
                        mm.add(id, mask(r, c, N)), e++;
                    }
                }
                if (e == 0) {
                    return "NO"s;
                }
                id++;
            }
        }
    }

    int ans = mm.compute();
    return ans == N * N - cnt[1] ? "YES"s : "NO"s;
}

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
