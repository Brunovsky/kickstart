#ifndef FLOW_TIDAL_FLOW_HPP
#define FLOW_TIDAL_FLOW_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

using edges_t = vector<array<int, 2>>;

/**
 * Simple tidal flow algorithm
 * Complexity: O(V E^2)
 * Based on "Tidal Flow: A Fast and Teachable Maximum Flow Algorithm" by Fontaine, M.C.
 * Slower than push-relabel, comparable to dinitz
 */
struct tidal_flow {
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<long> flow, cap;

    tidal_flow(int V, const edges_t& g, const vector<long>& caps)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = caps[c++];
            res[v].push_back(e), edge[e] = {v, u}, cap[e++] = 0;
        }
    }

    vector<int> level, edges;
    vector<long> p, h, l;
    static constexpr long inf = LONG_MAX / 2;

    bool bfs(int s, int t) {
        fill(begin(level), end(level), -1);
        edges.clear();
        level[s] = 0;
        vector<int> bfs{s};
        int i = 0, S = 1;
        while (i < S && level[bfs[i]] != level[t]) {
            int u = bfs[i++];
            for (int e : res[u]) {
                int v = edge[e][1];
                if (flow[e] < cap[e]) {
                    if (level[v] == -1) {
                        level[v] = level[u] + 1;
                        bfs.push_back(v), S++;
                    }
                    if (level[v] == level[u] + 1) {
                        edges.push_back(e);
                    }
                }
            }
        }
        return level[t] != -1;
    }

    long tide(int s, int t) {
        fill(begin(h), end(h), 0);
        h[s] = inf;
        for (int e : edges) {
            auto [w, v] = edge[e];
            p[e] = min(cap[e] - flow[e], h[w]);
            h[v] = h[v] + p[e];
        }
        if (h[t] == 0) {
            return 0;
        }
        fill(begin(l), end(l), 0);
        l[t] = h[t];
        for (auto it = edges.rbegin(); it != edges.rend(); it++) {
            int e = *it;
            auto [w, v] = edge[e];
            p[e] = min(p[e], min(h[w] - l[w], l[v]));
            l[v] -= p[e];
            l[w] += p[e];
        } // 1=push phase, 0=recover phase
        fill(begin(h), end(h), 0);
        h[s] = l[s];
        for (auto e : edges) {
            auto [w, v] = edge[e];
            p[e] = min(p[e], h[w]);
            h[w] -= p[e];
            h[v] += p[e];
            flow[e] += p[e];
            flow[e ^ 1] -= p[e];
        }
        return h[t];
    }

    long maxflow(int s, int t) {
        level.assign(V, 0);
        h.assign(V, 0);
        l.assign(V, 0);
        p.assign(2 * E, 0);
        flow.assign(2 * E, 0);
        long max_flow = 0, df;
        while (bfs(s, t)) {
            do {
                df = tide(s, t);
                max_flow += df;
            } while (df > 0);
        }
        return max_flow;
    }

    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

#endif // FLOW_TIDAL_FLOW_HPP
