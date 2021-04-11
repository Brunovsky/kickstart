#ifndef PATH_FLOYD_WARSHALL_HPP
#define PATH_FLOYD_WARSHALL_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Implementation for directed graphs
 * For undirected graphs insert edges both ways
 * Negative loops are detected
 * Complexity: O(V^3)
 */
struct floyd_warshall {
    int V, E = 0;
    vector<tuple<int, int, long>> edge;

    floyd_warshall(int V, const edges_t& g, const vector<long>& weight)
        : V(V), E(size(g)), edge(E) {
        for (int e = 0; e < E; e++) {
            auto [u, v] = g[e];
            edge[e] = {u, v, weight[e]};
        }
    }

    void add(int u, int v, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        edge.push_back({u, v, w}), E++;
    }

    vector<vector<long>> dist;
    vector<vector<int>> next;
    static inline constexpr long inf = LONG_MAX / 2;

    bool compute() {
        dist.assign(V, vector<long>(V, inf));
        next.assign(V, vector<int>(V, -1));

        for (auto [u, v, w] : edge) {
            dist[u][v] = min(w, dist[u][v]);
            next[u][v] = v;
        }
        for (int u = 0; u < V; u++) {
            dist[u][u] = 0;
            next[u][u] = u;
        }

        for (int k = 0; k < V; k++) {
            for (int u = 0; u < V; u++) {
                for (int v = 0; v < V; v++) {
                    if (dist[u][v] > dist[u][k] + dist[k][v]) {
                        dist[u][v] = dist[u][k] + dist[k][v];
                        next[u][v] = next[u][k];
                    }
                }
                if (dist[u][u] < 0)
                    return false;
            }
        }
        fix_infinities();
        return true;
    }

    void fix_infinities() {
        for (int u = 0; u < V; u++) {
            for (int v = 0; v < V; v++) {
                if (dist[u][v] > inf / 2)
                    dist[u][v] = inf;
            }
        }
    }

    auto path(int u, int v) const {
        vector<int> nodes;
        if (next[u][v] == -1) {
            return nodes;
        }
        while (u != v) {
            nodes.push_back(u);
            u = next[u][v];
        }
        nodes.push_back(v);
        return nodes;
    }
};

#endif // PATH_FLOYD_WARSHALL_HPP
