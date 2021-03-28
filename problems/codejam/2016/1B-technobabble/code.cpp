#include <bits/stdc++.h>

using namespace std;

// *****

using edges_t = vector<array<int, 2>>;

struct maximum_matching {
    int U, V;
    vector<int> off, mu, mv;
    edges_t edge;

    maximum_matching(int U, int V, const edges_t& g)
        : U(U), V(V), off(U + 1, 0), mu(U, -1), mv(V, -1), edge(g.size()) {
        for (auto [u, v] : g)
            off[u + 1]++;
        inclusive_scan(begin(off), end(off), begin(off));
        auto cur = off;
        for (auto [u, v] : g) {
            edge[cur[u]++] = {u, v};
        }
    }

    vector<int> vis;
    int iteration;
    static inline constexpr int inf = INT_MAX / 2;

    bool dfs(int u) {
        vis[u] = iteration;
        for (int e = off[u]; e < off[u + 1]; e++) {
            int v = edge[e][1];
            if (mv[v] == -1) {
                mu[u] = v, mv[v] = u;
                return true;
            }
        }
        for (int e = off[u]; e < off[u + 1]; e++) {
            int v = edge[e][1];
            if (vis[mv[v]] != iteration && dfs(mv[v])) {
                mu[u] = v, mv[v] = u;
                return true;
            }
        }
        return false;
    }

    int compute() {
        vis.assign(U, 0);
        iteration = 0;
        int new_matchings = 0;
        int matchings = 0;
        do {
            iteration++;
            new_matchings = 0;
            for (int u = 0; u < U; u++) {
                if (mu[u] == -1 && dfs(u)) {
                    new_matchings++;
                }
            }
            matchings += new_matchings;
        } while (new_matchings);
        return matchings;
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
    maximum_matching mm(U, V, g);
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
