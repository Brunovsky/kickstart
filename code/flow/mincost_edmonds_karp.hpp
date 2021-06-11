#pragma once

#include "../struct/integer_heaps.hpp"

/**
 * Edmonds-Karp augmenting paths for simple mincost flow.
 * Complexity: O(V E^2 log V)
 * For min-cost flow problems with one source, one sink, no supplies or demands.
 */
template <typename Flow = long, typename Cost = long, typename FlowSum = Flow,
          typename CostSum = Cost>
struct mincost_edmonds_karp {
    struct Edge {
        int node[2];
        Flow cap, flow = 0;
        Cost cost;
    };
    int V, E = 0;
    vector<vector<int>> res;
    vector<Edge> edge;

    explicit mincost_edmonds_karp(int V) : V(V), res(V) {}

    void add(int u, int v, Flow capacity, Cost cost) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0 && cost >= 0);
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0, cost});
        res[v].push_back(E++), edge.push_back({{v, u}, 0, 0, -cost});
    }

    vector<CostSum> dist, pi;
    vector<int> prev;
    pairing_int_heap<less_container<vector<CostSum>>> heap;
    static inline constexpr Flow flowinf = numeric_limits<Flow>::max() / 2;
    static inline constexpr FlowSum flowsuminf = numeric_limits<FlowSum>::max() / 2;
    static inline constexpr CostSum costsuminf = numeric_limits<CostSum>::max() / 3;

    bool dijkstra(int s, int t) {
        dist.assign(V, costsuminf);
        prev.assign(V, -1);
        dist[s] = 0;

        heap.push(s);

        while (!heap.empty()) {
            auto u = heap.pop();
            for (int e : res[u]) {
                int v = edge[e].node[1];
                CostSum w = min(dist[u] + pi[u] - pi[v] + edge[e].cost, costsuminf);
                if (edge[e].flow < edge[e].cap && dist[v] > w) {
                    dist[v] = w;
                    prev[v] = e;
                    heap.push_or_improve(v);
                }
            }
        }
        reprice();
        return prev[t] != -1;
    }

    void reprice() {
        for (int u = 0; u < V; u++) {
            pi[u] = min(dist[u] + pi[u], costsuminf);
        }
    }

    auto path(int v) const {
        vector<int> path;
        while (prev[v] != -1) {
            path.push_back(prev[v]);
            v = edge[prev[v]].node[0];
        }
        return path;
    }

    auto mincost_flow(int s, int t, FlowSum F = flowsuminf) {
        pi.assign(V, 0);
        heap = pairing_int_heap<less_container<vector<CostSum>>>(V, dist);

        FlowSum sflow = 0;
        while (sflow < F && dijkstra(s, t)) {
            auto augmenting_path = path(t);
            Flow df = min(F - sflow, FlowSum(flowinf));
            for (int e : augmenting_path) {
                df = min(df, edge[e].cap - edge[e].flow);
            }
            sflow += df;
            for (int e : augmenting_path) {
                edge[e].flow += df;
                edge[e ^ 1].flow -= df;
            }
        }
        CostSum scost = 0;
        for (int e = 0; e < E; e += 2) {
            scost += FlowSum(edge[e].flow) * CostSum(edge[e].cost);
        }
        return make_pair(sflow, scost);
    }

    void clear_flow() {
        for (int e = 0; e < E; e++) {
            edge[e].flow = 0;
        }
    }

    Flow get_flow(int e) const { return edge[2 * e].flow; }
    bool left_of_mincut(int u) const { return dist[u] < costsuminf; }
};
