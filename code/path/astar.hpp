#ifndef PATH_ASTAR_HPP
#define PATH_ASTAR_HPP

#include "../integer_heaps.hpp" // pairing_int_heap

// *****

using edges_t = vector<array<int, 2>>;

/**
 * Implementation for directed graphs
 * For undirected graphs insert edges both ways
 * Complexity: O(E + V log(V))
 */
struct astar {
    int V, E = 0;
    vector<vector<pair<int, long>>> adj;
    using heuristic_t = const function<long(int)>&;

    astar(int V, const edges_t& g, const vector<long>& weight)
        : V(V), E(size(g)), adj(V) {
        for (int e = 0; e < E; e++) {
            assert(weight[e] >= 0);
            auto [u, v] = g[e];
            adj[u].push_back({v, weight[e]});
        }
    }

    void add(int u, int v, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && w >= 0);
        adj[u].push_back({v, w}), E++;
    }

    vector<long> dist, heur;
    vector<int> prev;
    static inline constexpr long inf = LONG_MAX / 2;

    long compute(int s, int t, heuristic_t h) {
        dist.assign(V, inf);
        heur.assign(V, inf);
        prev.assign(V, -1);
        dist[s] = heur[s] = 0, prev[s] = s;

        auto cmp = [&](int u, int v) { return heur[u] < heur[v]; };
        pairing_int_heap Q(V, cmp);
        Q.push(s);

        while (!Q.empty()) {
            int u = Q.pop();
            if (u == t) {
                return dist[t];
            }
            for (auto [v, w] : adj[u]) {
                long cost = dist[u] + w;
                if (dist[v] > cost) {
                    dist[v] = cost;
                    heur[v] = cost + h(v);
                    prev[v] = u;
                    Q.push_or_improve(v);
                }
            }
        }
        return inf;
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

#endif // PATH_ASTAR_HPP
