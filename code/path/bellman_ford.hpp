#ifndef BELLMAN_FORD_HPP
#define BELLMAN_FORD_HPP

#include <bits/stdc++.h>

using namespace std;

using edges_t = vector<array<int, 2>>;

/**
 * For directed graphs with negative weights
 * For undirected graphs insert edges both ways
 * Negative cycles are detected
 * Complexity: O(EV)
 */
struct bellman_ford {
    int V, E = 0;
    vector<tuple<int, int, long>> edge;

    bellman_ford(int V, const edges_t& g, const vector<long>& weight)
        : V(V), E(size(g)), edge(E) {
        assert(int(weight.size()) == E);
        for (int e = 0; e < E; e++) {
            auto [u, v] = g[e];
            edge[e] = {u, v, weight[e]};
        }
    }

    void add(int u, int v, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        edge.push_back({u, v, w}), E++;
    }

    vector<long> dist;
    vector<int> prev;
    static inline constexpr long inf = LONG_MAX / 2;

    bool compute(int s) {
        dist.assign(V, inf);
        prev.assign(V, -1);
        dist[s] = 0, prev[s] = s;

        bool stop = false;
        for (int k = 1; k <= V && !stop; k++) {
            stop = true;
            for (auto [u, v, w] : edge) {
                if (dist[v] > dist[u] + w && dist[u] < inf) {
                    dist[v] = dist[u] + w;
                    prev[v] = u;
                    stop = false;
                }
            }
        }
        return stop;
    }

    auto path(int v) const {
        vector<int> nodes;
        if (prev[v] == -1) {
            return nodes;
        }
        int t;
        do {
            nodes.push_back(v);
            t = v;
            v = prev[v];
        } while (t != v);
        reverse(begin(nodes), end(nodes));
        return nodes;
    }
};

#endif // BELLMAN_FORD_HPP
