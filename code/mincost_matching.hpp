#ifndef MINCOST_MATCHING_HPP
#define MINCOST_MATCHING_HPP

#include "hash.hpp"

// *****

constexpr long inf = LONG_MAX / 3;

/**
 * Min-cost maximum bipartite matching (hungarian, dijkstra-based)
 * Based on: https://web.stanford.edu/class/cs261/min_cost_bipartite_matching.pdf
 * Complexity: O(EW log E), W=max(U,V).
 *
 * If the bipartite graph is not balanced (U != V)
 * it will be padded with infinite weight edges.
 * A perfect matching must exist on the padded graph, otherwise the answer risks being
 * bogus. To obtain a min-cost non-perfect matching of arbitrary size use reduction
 * to mincost flow instead.
 */
struct mincost_hungarian {
    int U, V, E = 0, W;
    vector<vector<int>> adj;
    unordered_map<pair<int, int>, long, pair_hasher> cost;

    mincost_hungarian(int U, int V) : U(U), V(V), W(max(U, V)), adj(W) { pad(); }
    explicit mincost_hungarian(int W = 0) : mincost_hungarian(W, W) {}

    void add(int u, int v, long w) {
        assert(0 <= u && u < U && 0 <= v && v < V && w >= 0);
        assert(!cost.count({u, v}));
        adj[u].push_back(v);
        cost[{u, v}] = w;
        E++;
    }

    void pad() {
        for (int u = U; u < W; u++)
            for (int v = 0; v < W; v++)
                adj[u].push_back(v), cost[{u, v}] = inf;
        for (int v = V; v < W; v++)
            for (int u = 0; u < W; u++)
                adj[u].push_back(v), cost[{u, v}] = inf;
    }

    vector<int> m[2], prev[2];
    vector<long> pi[2], dist[2];

    long reduced_cost(int u, int v) { return cost.at({u, v}) + pi[0][u] - pi[1][v]; }

    bool dijkstra() {
        dist[0].assign(W + 1, inf);
        dist[1].assign(W, inf);
        prev[0].assign(W + 1, -1);
        prev[1].assign(W, -1);

        using int2 = pair<long, int>;
        vector<bool> vis(W, false);
        priority_queue<int2, vector<int2>, greater<int2>> Q;
        for (int u = 0; u < W; u++)
            if (m[0][u] == W)
                dist[0][u] = 0, Q.push({0, u});

        while (!Q.empty()) {
            int u = Q.top().second;
            Q.pop();
            if (u == W || vis[u]) {
                continue;
            }
            vis[u] = true;
            for (int v : adj[u]) {
                int y = m[1][v];
                long w = min(dist[0][u] + reduced_cost(u, v), inf);
                if (dist[0][y] > w) {
                    dist[0][y] = w, prev[0][y] = v;
                    Q.push({w, y});
                }
                if (dist[1][v] > w) {
                    dist[1][v] = w, prev[1][v] = u;
                }
            }
        }
        reprice();
        return prev[0][W] != -1;
    }

    void reprice() {
        for (int i : {0, 1}) {
            for (int u = 0; u < W; u++) {
                pi[i][u] = min(dist[i][u] + pi[i][u], inf);
            }
        }
    }

    auto path() {
        vector<array<int, 2>> path;
        int v = prev[0][W];
        while (v != -1) {
            path.push_back({prev[1][v], v});
            v = prev[0][prev[1][v]];
        }
        return path;
    }

    long mincost_max_matching() {
        m[0].assign(W, W);
        m[1].assign(W, W);
        pi[0].assign(W, 0);
        pi[1].assign(W, 0);

        int matchings = 0;
        while (matchings < W && dijkstra()) {
            for (auto edge : path()) {
                int u = edge[0], v = edge[1];
                m[0][u] = v, m[1][v] = u;
            }
            matchings++;
        }
        assert(matchings == W);

        long min_cost = 0;
        for (int u = 0; u < U; u++)
            if (m[0][u] < V)
                min_cost += cost.at({u, m[0][u]});
        return min_cost;
    }
};

#endif // MINCOST_MATCHING_HPP
