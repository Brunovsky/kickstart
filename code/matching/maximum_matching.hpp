#pragma once

#include <bits/stdc++.h>
using namespace std;

using edges_t = vector<array<int, 2>>;

/**
 * Simple maximum bipartite matching
 * Complexity: O(VE)
 */
struct maximum_matching {
    int U, V;
    vector<vector<int>> adj;
    vector<int> mu, mv;

    maximum_matching(int U, int V, const edges_t& g = {}) : U(U), V(V), adj(U) {
        for (auto [u, v] : g)
            add(u, v);
    }

    void add(int u, int v) {
        assert(0 <= u && u < U && 0 <= v && v < V);
        adj[u].push_back(v);
    }

    vector<int> vis;
    int iteration;
    static inline constexpr int inf = INT_MAX / 2;

    bool dfs(int u) {
        vis[u] = iteration;
        for (int v : adj[u]) {
            if (mv[v] == -1) {
                mu[u] = v, mv[v] = u;
                return true;
            }
        }
        for (int v : adj[u]) {
            if (vis[mv[v]] != iteration && dfs(mv[v])) {
                mu[u] = v, mv[v] = u;
                return true;
            }
        }
        return false;
    }

    int compute() {
        vis.assign(U, 0);
        mu.assign(U, -1);
        mv.assign(V, -1);
        iteration = 0;
        int new_matchings = 0, matchings = 0;
        do {
            iteration++, new_matchings = 0;
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
