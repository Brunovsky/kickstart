#ifndef DINITZ_FLOW_HPP
#define DINITZ_FLOW_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Dinitz's blocking flows
 * Complexity: O(V^2 E), close to push relabel in practice
 */
template <typename Flow = long, typename FlowSum = Flow>
struct dinitz_flow {
    using edges_t = vector<array<int, 2>>;
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<Flow> flow, cap;

    dinitz_flow(int V, const edges_t& g = {}, const vector<Flow>& caps = {})
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

    vector<int> level, arc;
    static constexpr FlowSum inf = numeric_limits<FlowSum>::max() / 2;

    bool bfs(int s, int t) {
        fill(begin(level), end(level), -1);
        level[s] = 0;
        vector<int> bfs{s};
        int j = 0, S = 1;
        while (j < S) {
            int u = bfs[j++];
            for (int e : res[u]) {
                int v = edge[e][1];
                if (level[v] == -1 && flow[e] < cap[e]) {
                    level[v] = level[u] + 1;
                    bfs.push_back(v), S++;
                    if (v == t)
                        return true;
                }
            }
        }
        return false;
    }

    auto dfs(int u, int t, Flow mincap) {
        if (u == t) {
            return mincap;
        }
        FlowSum preflow = 0;
        for (int &i = arc[u], vsize = res[u].size(); i < vsize; i++) {
            int e = res[u][i], v = edge[e][1];
            if (flow[e] < cap[e] && level[u] < level[v]) {
                FlowSum df = dfs(v, t, min(mincap, cap[e] - flow[e]));
                flow[e] += df;
                flow[e ^ 1] -= df;
                preflow += df, mincap -= df;
                if (mincap == 0)
                    break;
            }
        }
        return preflow;
    }

    FlowSum maxflow(int s, int t) {
        level.assign(V, 0);
        arc.assign(V, 0);
        FlowSum max_flow = 0;
        while (bfs(s, t)) {
            max_flow += dfs(s, t, inf);
            fill(begin(arc), end(arc), 0);
        }
        return max_flow;
    }

    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

#endif // DINITZ_FLOW_HPP
