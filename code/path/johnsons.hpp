#ifndef JOHNSONS_HPP
#define JOHNSONS_HPP

#include "../struct/integer_heaps.hpp" // pairing_int_heap

using edges_t = vector<array<int, 2>>;

/**
 * Implementation for directed graphs
 * For undirected graphs insert edges both ways
 * Negative loops are detected
 * Complexity: O(EV + V^2 log(V))
 */
struct johnsons {
    int V, E = 0;
    vector<vector<pair<int, long>>> adj;

    johnsons(int V, const edges_t& g, const vector<long>& weight)
        : V(V), E(size(g)), adj(V + 1) {
        for (int e = 0; e < E; e++) {
            auto [u, v] = g[e];
            adj[u].push_back({v, weight[e]});
        }
    }

    void add(int u, int v, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        adj[u].push_back({v, w}), E++;
    }

    vector<vector<long>> dist;
    vector<vector<int>> prev;
    vector<long> pi;
    static inline constexpr long inf = LONG_MAX / 2;

    bool bellman_ford() {
        pi.assign(V + 1, 0);
        adj[V].resize(V);
        for (int v = 0; v < V; v++)
            adj[V].push_back({v, 0});

        bool stop = false;
        for (int k = 1; k <= V + 1 && !stop; k++) {
            stop = true;
            for (int u = 0; u <= V; u++) {
                for (auto [v, w] : adj[u]) {
                    if (pi[v] > pi[u] + w) {
                        pi[v] = pi[u] + w;
                        stop = false;
                    }
                }
            }
        }
        return stop;
    }

    void dijkstra(int s) {
        dist[s].assign(V, inf);
        prev[s].assign(V, -1);
        dist[s][s] = 0, prev[s][s] = s;

        pairing_int_heap<less_container<vector<long>>> Q(V, dist[s]);
        Q.push(s);

        while (!Q.empty()) {
            int u = Q.pop();
            for (auto [v, w] : adj[u]) {
                if (dist[s][v] > dist[s][u] + pi[u] - pi[v] + w) {
                    dist[s][v] = dist[s][u] + pi[u] - pi[v] + w;
                    prev[s][v] = u;
                    Q.push_or_improve(v);
                }
            }
        }
        for (int u = 0; u < V; u++) {
            if (prev[s][u] == -1) {
                assert(dist[s][u] == inf);
            } else {
                dist[s][u] += pi[u] - pi[s];
            }
        }
    }

    bool compute() {
        if (!bellman_ford())
            return false;
        dist.assign(V, {});
        prev.assign(V, {});
        for (int u = 0; u < V; u++)
            dijkstra(u);
        return true;
    }

    auto path(int u, int v) const {
        vector<int> nodes;
        if (prev[u][v] == -1) {
            return nodes;
        }
        int t;
        do {
            nodes.push_back(v);
            t = v;
            v = prev[u][v];
        } while (t != v);
        reverse(begin(nodes), end(nodes));
        return nodes;
    }
};

#endif // JOHNSONS_HPP
