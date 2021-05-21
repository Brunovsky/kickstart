#ifndef TIDAL_FLOW_HPP
#define TIDAL_FLOW_HPP

#include <bits/stdc++.h>

using namespace std;

/**
 * Simple tidal flow algorithm
 * Complexity: O(V E^2)
 * Based on "Tidal Flow: A Fast and Teachable Maximum Flow Algorithm" by Fontaine, M.C.
 * Faster than dinitz and comparable to push-relabel on random graphs
 */
template <typename Flow = long, typename FlowSum = Flow>
struct tidal_flow {
    struct Edge {
        int node[2];
        Flow cap, flow = 0;
    };
    int V, E = 0;
    vector<vector<int>> res;
    vector<Edge> edge;

    explicit tidal_flow(int V) : V(V), res(V) {}

    void add(int u, int v, Flow capacity, bool bothways = false) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0);
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0});
        res[v].push_back(E++), edge.push_back({{v, u}, bothways ? capacity : 0, 0});
    }

    vector<int> level, edges, Q;
    vector<Flow> p;
    vector<FlowSum> h, l;
    static constexpr FlowSum flowsuminf = numeric_limits<FlowSum>::max() / 2;

    bool bfs(int s, int t) {
        level.assign(V, -1);
        edges.clear();
        level[s] = 0;
        Q[0] = s;
        int i = 0, S = 1;
        while (i < S && level[Q[i]] != level[t]) {
            int u = Q[i++];
            for (int e : res[u]) {
                int v = edge[e].node[1];
                if (edge[e].flow < edge[e].cap) {
                    if (level[v] == -1) {
                        level[v] = level[u] + 1;
                        Q[S++] = v;
                    }
                    if (level[v] == level[u] + 1) {
                        edges.push_back(e);
                    }
                }
            }
        }
        return level[t] != -1;
    }

    FlowSum tide(int s, int t) {
        fill(begin(h), end(h), 0);
        h[s] = flowsuminf;
        for (int e : edges) {
            auto [w, v] = edge[e].node;
            p[e] = min(FlowSum(edge[e].cap - edge[e].flow), h[w]);
            h[v] = h[v] + p[e];
        }
        if (h[t] == 0) {
            return 0;
        }
        fill(begin(l), end(l), 0);
        l[t] = h[t];
        for (auto it = edges.rbegin(); it != edges.rend(); it++) {
            int e = *it;
            auto [w, v] = edge[e].node;
            p[e] = min(FlowSum(p[e]), min(h[w] - l[w], l[v]));
            l[v] -= p[e];
            l[w] += p[e];
        } // 1=push phase, 0=recover phase
        fill(begin(h), end(h), 0);
        h[s] = l[s];
        for (auto e : edges) {
            auto [w, v] = edge[e].node;
            p[e] = min(FlowSum(p[e]), h[w]);
            h[w] -= p[e];
            h[v] += p[e];
            edge[e].flow += p[e];
            edge[e ^ 1].flow -= p[e];
        }
        return h[t];
    }

    FlowSum maxflow(int s, int t) {
        h.assign(V, 0);
        l.assign(V, 0);
        p.assign(E, 0);
        Q.resize(V);
        FlowSum max_flow = 0, df;
        while (bfs(s, t)) {
            do {
                df = tide(s, t);
                max_flow += df;
            } while (df > 0);
        }
        return max_flow;
    }

    Flow get_flow(int e) const { return edge[2 * e].flow; }
    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

#endif // TIDAL_FLOW_HPP
