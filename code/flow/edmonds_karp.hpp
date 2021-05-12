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
    struct Edge : array<int, 2> {
        long cap, flow = 0;
    };
    int V, E = 0;
    vector<vector<int>> res;
    vector<Edge> edge;

    explicit edmonds_karp(int V) : V(V), res(V) {}

    void add(int u, int v, Flow c) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        res[u].push_back(E++), edge.push_back({{u, v}, c, 0});
        res[v].push_back(E++), edge.push_back({{v, u}, 0, 0});
    }

    vector<int> pred, Q;
    static constexpr FlowSum inf = numeric_limits<FlowSum>::max() / 2;

    bool bfs(int s, int t) {
        fill(begin(pred), end(pred), -1);
        Q[0] = s;
        int j = 0, S = 1;
        while (j < S && pred[t] == -1) {
            int u = Q[j++];
            for (auto e : res[u]) {
                int v = edge[e][1];
                if (pred[v] == -1 && v != s && edge[e].flow < edge[e].cap) {
                    pred[v] = e;
                    Q[S++] = v;
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
            aug_flow = min(aug_flow, edge[e].cap - edge[e].flow);
        }
        for (int e = pred[t]; e != -1; e = pred[edge[e][0]]) {
            edge[e].flow += aug_flow;
            edge[e ^ 1].flow -= aug_flow;
        }
        return aug_flow;
    }

    FlowSum maxflow(int s, int t) {
        pred.resize(V);
        Q.resize(V);
        FlowSum max_flow = 0;
        while (bfs(s, t)) {
            max_flow += augment(t);
        }
        return max_flow;
    }

    Flow get_flow(int e) const { return edge[e].flow; }
    bool left_of_mincut(int u) const { return pred[u] >= 0; }
};

#endif // EDMONDS_KARP_HPP
