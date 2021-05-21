#include <bits/stdc++.h>

using namespace std;

// *****

using edges_t = vector<array<int, 2>>;

struct hopcroft_karp {
    int U, V;
    vector<int> off, mu, mv;
    edges_t edge;

    hopcroft_karp(int U, int V, const edges_t& g)
        : U(U), V(V), off(U + 2, 0), mu(U + 1, 0), mv(V + 1, 0), edge(g.size()) {
        for (auto [u, v] : g)
            off[u + 2]++;
        partial_sum(begin(off), end(off), begin(off));
        auto cur = off;
        for (auto [u, v] : g) {
            edge[cur[u + 1]++] = {u + 1, v + 1};
        }
    }

    vector<int> vis, dist;
    int iteration;
    static inline constexpr int inf = INT_MAX / 2;

    bool bfs() {
        queue<int> Q;
        for (int u = 1; u <= U; u++) {
            if (mu[u] == 0) {
                dist[u] = 0;
                Q.push(u);
            } else {
                dist[u] = inf;
            }
        }
        dist[0] = inf;
        while (!Q.empty()) {
            int u = Q.front();
            Q.pop();
            if (dist[u] < dist[0]) {
                for (int e = off[u]; e < off[u + 1]; e++) {
                    // note: the check v != mu[u] is implicit in dist[mv[v]] == inf
                    if (int v = edge[e][1]; dist[mv[v]] == inf) {
                        dist[mv[v]] = dist[u] + 1;
                        Q.push(mv[v]);
                    }
                }
            }
        }
        return dist[0] != inf;
    }

    bool dfs(int u) {
        if (u == 0) {
            return true;
        }
        if (vis[u] == iteration) {
            return false;
        }
        vis[u] = iteration;
        for (int e = off[u]; e < off[u + 1]; e++) {
            if (int v = edge[e][1]; dist[mv[v]] == dist[u] + 1 && dfs(mv[v])) {
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
        int matching = 0;
        iteration = 0;
        while (bfs()) {
            iteration++;
            for (int u = 1; u <= U; u++) {
                if (mu[u] == 0 && dfs(u)) {
                    matching++;
                }
            }
        }
        return matching;
    }
};

int N, K;
vector<vector<int>> A;
vector<vector<bool>> row, col;

void put(int i, int j, int a) {
    assert(A[i][j] != 0 || (row[i][a - 1] && col[j][a - 1]));
    A[i][j] = a, row[i][a - 1] = col[j][a - 1] = false;
}

auto solve_2() {
    if (K == 2) {
        cout << "POSSIBLE\n1 2\n2 1\n";
    } else if (K == 3) {
        cout << "IMPOSSIBLE\n";
    } else if (K == 4) {
        cout << "POSSIBLE\n2 1\n1 2\n";
    }
}

auto solve() {
    cin >> N >> K;
    A.assign(N, vector<int>(N, 0));
    row.assign(N, vector<bool>(N, true));
    col.assign(N, vector<bool>(N, true));

    if (N == 2)
        return solve_2();

    bool found = false;
    int a, b, c;
    for (a = 1; a <= N && !found; a++) {
        if ((N - 2) * a + 2 <= K && K <= (N - 2) * a + 2 * N) {
            for (b = 1; b <= N && !found; b++) {
                c = K - ((N - 2) * a + b);
                if (a == b && a == c) {
                    for (int i = 0; i < N; i++)
                        put(i, i, a);
                    found = true;
                    break;
                } else if (a != b && a != c && 1 <= c && c <= N && (N > 3 || b != c)) {
                    put(0, 0, b), put(0, 1, a), put(1, 0, a), put(1, 1, c);
                    for (int i = 2; i < N; i++)
                        put(i, i, a);
                    found = true;
                    break;
                }
            }
        }
    }
    if (!found) {
        cout << "IMPOSSIBLE\n";
        return;
    }

    for (int i = 0; i < N; i++) {
        edges_t g;

        for (int v = 0; v < N; v++)
            if (row[i][v])
                for (int u = 0; u < N; u++)
                    if (col[u][v])
                        g.push_back({u, v});
        for (int j = 0; j < N; j++)
            if (A[i][j] != 0)
                g.push_back({j, A[i][j] - 1});

        hopcroft_karp mm(N, N, g);
        int maximum = mm.compute();
        assert(maximum == N);
        for (int u = 0; u < N; u++) {
            put(i, u, mm.mu[u + 1]);
        }
    }

    cout << "POSSIBLE\n";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << (j == 0 ? "" : " ") << A[i][j];
        }
        cout << endl;
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ": ";
        solve();
    }
    return 0;
}
