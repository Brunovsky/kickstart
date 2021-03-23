#ifndef MINCOST_FLOW_HPP
#define MINCOST_FLOW_HPP

#include "graph.hpp"

// *****

/**
 * Edmonds-Karp augmenting paths for simple mincost flow.
 * Complexity: O(V E^2 log V)
 * For min-cost flow problems with one source, one sink, no supplies or demands.
 */
struct mincost_edmonds_karp {
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<long> flow, cap, cost;

    mincost_edmonds_karp(int V, const edges_t& g, const vector<long>& caps,
                         const vector<long>& costs)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E),
          cost(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v};
            cost[e] = costs[c], cap[e++] = caps[c];
            res[v].push_back(e), edge[e] = {v, u};
            cost[e] = -costs[c++], cap[e++] = 0;
        }
    }

    vector<long> dist, pi;
    vector<int> prev;
    static inline constexpr long inf = LONG_MAX / 3;

    bool dijkstra(int s, int t) {
        dist.assign(V, inf);
        prev.assign(V, -1);
        dist[s] = 0;

        using int2 = pair<long, int>;
        priority_queue<int2, vector<int2>, greater<int2>> Q;
        Q.push({0, s});

        while (!Q.empty()) {
            auto [ucost, u] = Q.top();
            Q.pop();
            if (dist[u] < ucost) {
                continue;
            } else if (u == t) {
                break;
            }
            for (int e : res[u]) {
                int v = edge[e][1];
                long w = dist[u] + pi[u] - pi[v] + cost[e];
                if (flow[e] < cap[e] && dist[v] > w) {
                    dist[v] = w;
                    prev[v] = e;
                    Q.push({w, v});
                }
            }
        }
        reprice();
        return prev[t] != -1;
    }

    void reprice() {
        for (int u = 0; u < V; u++) {
            pi[u] = min(dist[u] + pi[u], inf);
        }
    }

    auto path(int v) const {
        vector<int> path;
        while (prev[v] != -1) {
            path.push_back(prev[v]);
            v = edge[prev[v]][0];
        }
        return path;
    }

    pair<long, long> mincost_flow(int s, int t, long F = inf) {
        pi.assign(V, 0);

        long sflow = 0;
        while (sflow < F && dijkstra(s, t)) {
            auto augmenting_path = path(t);
            long df = F - sflow;
            for (int e : augmenting_path) {
                df = min(df, cap[e] - flow[e]);
            }
            sflow += df;
            for (int e : augmenting_path) {
                flow[e] += df;
                flow[e ^ 1] -= df;
            }
        }
        long scost = 0;
        for (int e = 0; e < 2 * E; e += 2)
            scost += flow[e] * cost[e];
        return {sflow, scost};
    }

    bool left_of_mincut(int u) const { return dist[u] < inf; }
};

#endif // MINCOST_FLOW_HPP
