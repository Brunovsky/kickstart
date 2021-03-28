#include <bits/stdc++.h>

using namespace std;

// *****

using edges_t = vector<array<int, 2>>;

struct hopcroft_karp {
    int U, V;
    vector<int> off, mu, mv;
    edges_t edge;

    hopcroft_karp(int U, int V, const edges_t& g)
        : U(U), V(V), off(U + 2, 0), edge(g.size()) {
        for (auto [u, v] : g)
            off[u + 1]++;
        partial_sum(begin(off), end(off), begin(off));
        auto cur = off;
        for (auto [u, v] : g) {
            edge[cur[u]++] = {u, v};
        }
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
                for (int e = off[u]; e < off[u + 1]; e++) {
                    // note: the check v != mu[u] is implicit in dist[mv[v]] == inf
                    if (int v = edge[e][1]; dist[mv[v]] == inf) {
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

auto solve() {
    int E;
    cin >> E;
    unordered_map<string, int> umap, vmap;
    edges_t g(E);
    for (auto& [u, v] : g) {
        string us, vs;
        cin >> us >> vs;
        u = umap.emplace(us, umap.size()).first->second;
        v = vmap.emplace(vs, vmap.size()).first->second;
    }
    int U = umap.size(), V = vmap.size();
    hopcroft_karp mm(U, V, g);
    int M = mm.compute();
    return E + M - U - V;
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
