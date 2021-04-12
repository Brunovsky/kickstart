#ifndef HOPCROFT_KARP_HPP
#define HOPCROFT_KARP_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

using edges_t = vector<array<int, 2>>;

/**
 * Hopcroft-Karp maximum bipartite matching
 * Complexity: O(V^1/2 E)
 */
struct hopcroft_karp {
    int U, V;
    vector<vector<int>> adj;
    vector<int> mu, mv;

    hopcroft_karp(int U, int V, const edges_t& g = {}) : U(U), V(V), adj(U) {
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

#endif // HOPCROFT_KARP_HPP
