#ifndef EDMONDS_KARP_HPP
#define EDMONDS_KARP_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

using edges_t = vector<array<int, 2>>;

/**
 * Edmonds-Karp augmenting paths
 * Complexity: O(VE^2), not good
 */
struct edmonds_karp {
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<long> flow, cap;

    edmonds_karp(int V, const edges_t& g, const vector<long>& caps)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = caps[c++];
            res[v].push_back(e), edge[e] = {v, u}, cap[e++] = 0;
        }
    }

    vector<int> pred;
    static constexpr long inf = LONG_MAX / 2;

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

    long augment(int t) {
        long aug_flow = inf;
        for (int e = pred[t]; e != -1; e = pred[edge[e][0]]) {
            aug_flow = min(aug_flow, cap[e] - flow[e]);
        }
        for (int e = pred[t]; e != -1; e = pred[edge[e][0]]) {
            flow[e] += aug_flow;
            flow[e ^ 1] -= aug_flow;
        }
        return aug_flow;
    }

    long maxflow(int s, int t) {
        pred.resize(V);
        long max_flow = 0;
        while (bfs(s, t)) {
            max_flow += augment(t);
        }
        return max_flow;
    }

    bool left_of_mincut(int u) const { return pred[u] >= 0; }
};

#endif // EDMONDS_KARP_HPP
