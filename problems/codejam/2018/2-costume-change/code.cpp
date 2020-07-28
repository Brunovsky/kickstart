#include <bits/stdc++.h>

using namespace std;

// *****

struct maximal_matching {
    int U, V;
    vector<vector<int>> adj;
    vector<int> mu; // mu[u]=v where the mapping is u<->v, nil if u is unmatched
    vector<int> mv; // mv[v]=u where the mapping is u<->v, nil if v is unmatched
    vector<int> vis;
    vector<int> dist;
    int iteration;
    static constexpr int nil = 0, inf = INT_MAX;

    maximal_matching(int U, int V) : U(U), V(V) {
        adj.assign(U + 1, {});
        dist.assign(U + 1, 0);
    }

    void add(int u, int v) {
        adj[u + 1].push_back(v + 1);
    }

    bool bfs() {
        queue<int> Q;
        for (int u = 1; u <= U; u++) {
            if (mu[u] == nil) {
                dist[u] = 0;
                Q.push(u);
            } else {
                dist[u] = inf;
            }
        }
        dist[nil] = inf;
        while (!Q.empty()) {
            int u = Q.front();
            Q.pop();
            if (dist[u] < dist[nil]) {
                for (int v : adj[u]) {
                    if (dist[mv[v]] == inf) {
                        dist[mv[v]] = dist[u] + 1;
                        Q.push(mv[v]);
                    }
                }
            }
        }
        return dist[nil] != inf;
    }

    bool dfs(int u) {
        if (u == nil) {
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
        mu.assign(U + 1, nil);
        mv.assign(V + 1, nil);
        vis.assign(U + 1, 0);
        int matching = 0;
        iteration = 0;
        while (bfs()) {
            iteration++;
            for (int u = 1; u <= U; u++) {
                if (mu[u] == nil && dfs(u)) {
                    matching++;
                }
            }
        }
        return matching;
    }
};
constexpr int maximal_matching::nil, maximal_matching::inf;

#define MAXN 101

int N;
vector<pair<int, int>> positions[2 * MAXN];

auto solve() {
    cin >> N;
    for (int c = -N; c <= N; c++) {
        positions[MAXN + c].clear();
    }
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            int c;
            cin >> c;
            positions[MAXN + c].push_back({x, y});
        }
    }
    assert(positions[MAXN].empty());
    int cnt = 0;
    for (int c = -N; c <= N; c++) {
        maximal_matching mm(N, N);
        for (auto pos : positions[MAXN + c]) {
            mm.add(pos.first, pos.second);
        }
        cnt += mm.compute();
    }
    return N * N - cnt;
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
