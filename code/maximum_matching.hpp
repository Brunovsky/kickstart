#ifndef MAXIMUM_MATCHING_HPP
#define MAXIMUM_MATCHING_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Simple maximum bipartite matching
 * Complexity: O(VE)
 */
struct maximum_matching {
    int U, V;
    vector<vector<int>> adj;

    maximum_matching(int U, int V) : U(U), V(V) { adj.assign(U, {}); }

    void add(int u, int v) {
        assert(0 <= u && u < U && 0 <= v && v < V);
        adj[u].push_back(v);
    }

    vector<int> mu, mv, vis;
    int iteration;
    static inline constexpr int inf = INT_MAX / 2;

    bool dfs(int u) {
        vis[u] = iteration;
        for (int v : adj[u]) {
            if (mv[v] == -1) {
                mu[u] = v;
                mv[v] = u;
                return true;
            }
        }
        for (int v : adj[u]) {
            if (vis[mv[v]] != iteration && dfs(mv[v])) {
                mu[u] = v;
                mv[v] = u;
                return true;
            }
        }
        return false;
    }

    int compute() {
        mu.assign(U, -1);
        mv.assign(V, -1);
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

/**
 * Hopcroft-Karp maximum bipartite matching
 * Complexity: O(V^1/2 E)
 */
struct hopcroft_karp {
    int U, V;
    vector<vector<int>> adj;

    hopcroft_karp(int U, int V) : U(U), V(V), adj(U + 1) {}

    void add(int u, int v) {
        assert(0 <= u && u < U && 0 <= v && v < V);
        adj[u + 1].push_back(v + 1);
    }

    vector<int> mu, mv, vis, dist;
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
                for (int v : adj[u]) {
                    // note: the check v != mu[u] is implicit in dist[mv[v]] == inf
                    if (dist[mv[v]] == inf) {
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
        mu.assign(U + 1, 0);
        mv.assign(V + 1, 0);
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

#endif // MAXIMUM_MATCHING_HPP
