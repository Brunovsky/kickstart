#ifndef MINCOST_MATCHING_HPP
#define MINCOST_MATCHING_HPP

#include "graph.hpp"

// *****

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
    int U, V, W, E;
    vector<int> off, m[2];
    edges_t edge;
    vector<long> cost;

    mincost_hungarian(int U, int V, const edges_t& g, const vector<long>& costs)
        : U(U), V(V), W(max(U, V)), E(g.size()), off(W + 1, W - V) {
        for (auto [u, v] : g)
            off[u + 1]++;
        for (int u = 0; u < U; u++) // edges if padding V side
            off[u + 1] += W - V;
        for (int u = U; u < V; u++) // edges if padding U side
            off[u + 1] = V;
        partial_sum(begin(off), end(off), begin(off));
        int Q = off.back();
        edge.resize(Q), cost.resize(Q);
        auto cur = off;
        for (int e = 0; e < E; e++) {
            auto [u, v] = g[e];
            edge[cur[u]] = {u, v}, cost[cur[u]++] = costs[e];
        }
        for (int v = V; v < U; v++) // edges if padding V side
            for (int u = 0; u < U; u++)
                edge[cur[u]] = {u, v}, cost[cur[u]++] = inf;
        for (int u = U; u < V; u++) // edges if padding U side
            for (int v = 0; v < V; v++)
                edge[cur[u]] = {u, v}, cost[cur[u]++] = inf;
        E = Q;
    }

    vector<int> prev[2];
    vector<long> pi[2], dist[2];
    static inline constexpr long inf = LONG_MAX / 3;

    long reduced_cost(int e) const {
        return cost[e] + pi[0][edge[e][0]] - pi[1][edge[e][1]];
    }

    bool dijkstra() {
        dist[0].assign(W + 1, inf);
        dist[1].assign(W, inf);
        prev[0].assign(W + 1, -1);
        prev[1].assign(W, -1);

        vector<bool> vis(W, false);

        using int2 = pair<long, int>;
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
            for (int e = off[u]; e < off[u + 1]; e++) {
                int v = edge[e][1], y = m[1][v];
                long w = min(dist[0][u] + reduced_cost(e), inf);
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
        edges_t path;
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
            for (auto [u, v] : path()) {
                m[0][u] = v, m[1][v] = u;
            }
            matchings++;
        }
        if (matchings < W)
            return -1;

        long min_cost = 0;
        for (int u = 0; u < U; u++)
            if (int v = m[0][u]; v < V)
                for (int e = off[u]; e < off[u + 1]; e++)
                    if (edge[e][1] == v) {
                        min_cost += cost[e];
                        break;
                    }
        return min_cost;
    }
};

#endif // MINCOST_MATCHING_HPP
