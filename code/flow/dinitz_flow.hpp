#ifndef DINITZ_FLOW_HPP
#define DINITZ_FLOW_HPP

#include <bits/stdc++.h>

using namespace std;

/**
 * Dinitz's blocking flows
 * Complexity: O(V^2 E), close to push relabel in practice
 */
template <typename Flow = long, typename FlowSum = Flow>
struct dinitz_flow {
    struct Edge {
        int node[2];
        Flow cap, flow = 0;
    };
    int V, E = 0;
    vector<vector<int>> res;
    vector<Edge> edge;

    explicit dinitz_flow(int V) : V(V), res(V) {}

    void add(int u, int v, Flow capacity, bool bothways = false) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0);
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0});
        res[v].push_back(E++), edge.push_back({{v, u}, bothways ? capacity : 0, 0});
    }

    vector<int> level, arc, Q;
    static constexpr Flow finf = numeric_limits<Flow>::max() / 2;

    bool bfs(int s, int t) {
        level.assign(V, -1);
        level[s] = 0;
        Q[0] = s;
        int j = 0, S = 1;
        while (j < S) {
            int u = Q[j++];
            for (int e : res[u]) {
                int v = edge[e].node[1];
                if (level[v] == -1 && edge[e].flow < edge[e].cap) {
                    level[v] = level[u] + 1;
                    Q[S++] = v;
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
        Flow preflow = 0;
        for (int &i = arc[u], vsize = res[u].size(); i < vsize; i++) {
            int e = res[u][i], v = edge[e].node[1];
            if (edge[e].flow < edge[e].cap && level[u] < level[v]) {
                Flow df = dfs(v, t, min(mincap, edge[e].cap - edge[e].flow));
                edge[e].flow += df;
                edge[e ^ 1].flow -= df;
                preflow += df;
                mincap -= df;
                if (mincap == 0)
                    break;
            }
        }
        return preflow;
    }

    FlowSum maxflow(int s, int t) {
        level.assign(V, 0);
        arc.assign(V, 0);
        Q.resize(V);
        FlowSum max_flow = 0;
        while (bfs(s, t)) {
            max_flow += dfs(s, t, finf);
            fill(begin(arc), end(arc), 0);
        }
        return max_flow;
    }

    Flow get_flow(int e) const { return edge[2 * e].flow; }
    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

#endif // DINITZ_FLOW_HPP
