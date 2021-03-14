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
    vector<int> off, mu, mv;
    vector<array<int, 2>> edge;

    maximum_matching(int U, int V, vector<array<int, 2>> g)
        : U(U), V(V), off(U + 1, 0), mu(U, -1), mv(V, -1), edge(move(g)) {
        for (auto [u, v] : edge)
            off[u + 1]++;
        inclusive_scan(begin(off), end(off), begin(off));
        sort(begin(edge), end(edge));
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
    vector<int> off, mu, mv;
    vector<array<int, 2>> edge;

    hopcroft_karp(int U, int V, vector<array<int, 2>> g)
        : U(U), V(V), off(U + 2, 0), mu(U + 1, 0), mv(V + 1, 0), edge(move(g)) {
        for (auto [u, v] : edge)
            off[u + 2]++;
        inclusive_scan(begin(off), end(off), begin(off));
        sort(begin(edge), end(edge));
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
                    int v = edge[e][1] + 1;
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
        for (int e = off[u]; e < off[u + 1]; e++) {
            int v = edge[e][1] + 1;
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
