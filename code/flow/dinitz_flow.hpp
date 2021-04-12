#ifndef DINITZ_FLOW_HPP
#define DINITZ_FLOW_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

using edges_t = vector<array<int, 2>>;

/**
 * Dinitz's blocking flows
 * Complexity: O(V^2 E), close to push relabel in practice
 */
struct dinitz_flow {
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<long> flow, cap;

    dinitz_flow(int V, const edges_t& g, const vector<long>& caps)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = caps[c++];
            res[v].push_back(e), edge[e] = {v, u}, cap[e++] = 0;
        }
    }

    vector<int> level, arc;
    static constexpr long inf = LONG_MAX / 2;

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

    long dfs(int u, int t, long mincap) {
        if (u == t) {
            return mincap;
        }
        long preflow = 0;
        for (int &i = arc[u], vsize = res[u].size(); i < vsize; i++) {
            int e = res[u][i], v = edge[e][1];
            if (flow[e] < cap[e] && level[u] < level[v]) {
                long df = dfs(v, t, min(mincap, cap[e] - flow[e]));
                flow[e] += df;
                flow[e ^ 1] -= df;
                preflow += df, mincap -= df;
                if (mincap == 0)
                    break;
            }
        }
        return preflow;
    }

    long maxflow(int s, int t) {
        level.assign(V, 0);
        arc.assign(V, 0);
        long max_flow = 0;
        while (bfs(s, t)) {
            max_flow += dfs(s, t, inf);
            fill(begin(arc), end(arc), 0);
        }
        return max_flow;
    }

    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

#endif // DINITZ_FLOW_HPP
