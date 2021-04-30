#ifndef EDMONDS_KARP_HPP
#define EDMONDS_KARP_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Edmonds-Karp augmenting paths
 * Complexity: O(VE^2), not good
 */
template <typename Flow = long, typename FlowSum = Flow>
struct edmonds_karp {
    using edges_t = vector<array<int, 2>>;
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<Flow> flow, cap;

    edmonds_karp(int V, const edges_t& g = {}, const vector<Flow>& caps = {})
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = caps[c++];
            res[v].push_back(e), edge[e] = {v, u}, cap[e++] = 0;
        }
    }

    void add(int u, int v, Flow c) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        int uv = 2 * E, vu = 2 * E + 1;
        res[u].push_back(uv), edge.push_back({u, v}), cap.push_back(c);
        res[v].push_back(vu), edge.push_back({v, u}), cap.push_back(0), E++;
        flow.push_back(0), flow.push_back(0);
    }

    vector<int> pred;
    static constexpr FlowSum inf = numeric_limits<FlowSum>::max() / 2;

    bool bfs(int s, int t) {
        fill(begin(pred), end(pred), -1);
        vector<int> bfs{s};
        int j = 0, S = 1;
        while (j < S && pred[t] == -1) {
            int u = bfs[j++];
            for (auto e : res[u]) {
                int v = edge[e][1];
                if (pred[v] == -1 && v != s && flow[e] < cap[e]) {
                    pred[v] = e;
                    bfs.push_back(v), S++;
                    if (v == t)
                        return true;
                }
            }
        }
        return false;
    }

    Flow augment(int t) {
        Flow aug_flow = numeric_limits<Flow>::max();
        for (int e = pred[t]; e != -1; e = pred[edge[e][0]]) {
            aug_flow = min(aug_flow, cap[e] - flow[e]);
        }
        for (int e = pred[t]; e != -1; e = pred[edge[e][0]]) {
            flow[e] += aug_flow;
            flow[e ^ 1] -= aug_flow;
        }
        return aug_flow;
    }

    FlowSum maxflow(int s, int t) {
        pred.resize(V);
        FlowSum max_flow = 0;
        while (bfs(s, t)) {
            max_flow += augment(t);
        }
        return max_flow;
    }

    bool left_of_mincut(int u) const { return pred[u] >= 0; }
};

#endif // EDMONDS_KARP_HPP
