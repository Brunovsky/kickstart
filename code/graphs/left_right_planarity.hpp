#ifndef LEFT_RIGHT_PLANARITY_HPP
#define LEFT_RIGHT_PLANARITY_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

using edges_t = vector<array<int, 2>>;

/**
 * Left right planarity test
 * Complexity: O(V) expensive
 * Based on: The Left-Right Planarity Test, Ulrik Brandes
 */
struct left_right {
    int V, E = 0;
    vector<vector<int>> adj;
    vector<int> source, target;

    explicit left_right(int V) : V(V), adj(V) {}

    left_right(int V, const edges_t& g)
        : V(V), E(g.size()), adj(V), source(E), target(E) {
        for (int e = 0; e < E; e++) {
            auto [u, v] = g[e];
            adj[u].push_back(e), adj[v].push_back(e), source[e] = u, target[e] = v;
        }
    }

    int other(int e, int u) const { return u == target[e] ? source[e] : target[e]; }

    struct interval_t {
        int low = -1, high = -1;
        inline explicit operator bool() const { return low != -1 || high != -1; }
        inline bool operator!=(interval_t b) { return low != b.low || high != b.high; }
        inline bool operator==(interval_t b) { return low == b.low && high == b.high; }
    };
    struct conflict_t {
        interval_t L, R;
        inline explicit operator bool() const { return L || R; }
        inline bool operator!=(conflict_t b) { return L != b.L || R != b.R; }
        inline bool operator==(conflict_t b) { return L == b.L && R == b.R; }
    };

    vector<int> parent_edge, height, lowpoint, lowpoint2, nesting;
    vector<bool> vis;
    vector<int> ref, side, lowpoint_edge;
    stack<conflict_t> S;
    vector<conflict_t> stack_bottom;
    vector<vector<int>> dfs_adj;

    bool conflicting(interval_t I, int e) { return I && lowpoint[I.high] > lowpoint[e]; }
    int lowest(conflict_t P) {
        assert(P);
        int l = !P.L ? INT_MAX : lowpoint[P.L.low];
        int r = !P.R ? INT_MAX : lowpoint[P.R.low];
        return min(l, r);
    }

    void orient_dfs(int v) {
        int e = parent_edge[v];
        for (int vw : adj[v]) {
            if (vis[vw]) {
                continue;
            }
            vis[vw] = true;
            dfs_adj[v].push_back(vw);
            int w = other(vw, v);
            source[vw] = v, target[vw] = w;
            lowpoint[vw] = lowpoint2[vw] = height[v];

            if (height[w] == -1) {
                parent_edge[w] = vw;
                height[w] = height[v] + 1;
                orient_dfs(w);
            } else {
                lowpoint[vw] = height[w];
            }
            nesting[vw] = 2 * lowpoint[vw] + (lowpoint2[vw] < height[v]);
            if (e == -1) {
                continue;
            }
            if (lowpoint[vw] < lowpoint[e]) {
                lowpoint2[e] = min(lowpoint[e], lowpoint2[vw]);
                lowpoint[e] = lowpoint[vw];
            } else if (lowpoint[vw] > lowpoint[e]) {
                lowpoint2[e] = min(lowpoint2[e], lowpoint[vw]);
            } else {
                lowpoint2[e] = min(lowpoint2[e], lowpoint2[vw]);
            }
        }
        sort(begin(dfs_adj[v]), end(dfs_adj[v]),
             [&](int e1, int e2) { return nesting[e1] < nesting[e2]; });
    }

    bool add_constraints(int ei, int e) {
        conflict_t P;
        do {
            auto Q = S.top();
            S.pop();
            if (Q.L) {
                swap(Q.L, Q.R);
            }
            if (Q.L) {
                return false;
            }
            if (lowpoint[Q.R.low] > lowpoint[e]) {
                if (!P.R) {
                    P.R = Q.R;
                } else {
                    ref[P.R.low] = Q.R.high;
                }
                P.R.low = Q.R.low;
            } else {
                ref[Q.R.low] = lowpoint_edge[e];
            }
        } while (S.top() != stack_bottom[ei]);

        while (conflicting(S.top().L, ei) || conflicting(S.top().R, ei)) {
            auto Q = S.top();
            S.pop();
            if (conflicting(Q.R, ei)) {
                swap(Q.L, Q.R);
            }
            if (conflicting(Q.R, ei)) {
                return false;
            }
            ref[P.R.low] = Q.R.high;
            if (Q.R.low != -1) {
                P.R.low = Q.R.low;
            }
            if (!P.L) {
                P.L = Q.L;
            } else {
                ref[P.L.low] = Q.L.high;
            }
            P.L.low = Q.L.low;
        }
        if (P) {
            S.push(P);
        }
        return true;
    }

    void trim_back_edges(int u) {
        while (!S.empty() && lowest(S.top()) == height[u]) {
            auto P = S.top();
            S.pop();
            if (P.L.low != -1) {
                side[P.L.low] = -1;
            }
        }
        if (!S.empty()) {
            auto P = S.top();
            S.pop();
            while (P.L.high != -1 && target[P.L.high] == u) {
                P.L.high = ref[P.L.high];
            }
            if (P.L.high == -1 && P.L.low != -1) {
                ref[P.L.low] = P.R.low;
                side[P.L.low] = -1;
                P.L.low = -1;
            }
            while (P.R.high != -1 && target[P.R.high] == u) {
                P.R.high = ref[P.R.high];
            }
            if (P.R.high == -1 && P.R.low != -1) {
                ref[P.R.low] = P.L.low;
                side[P.R.low] = -1;
                P.R.low = -1;
            }
            S.push(P);
        }
    }

    bool test_dfs(int v) {
        int e = parent_edge[v];
        bool first = true;
        for (int ei : dfs_adj[v]) {
            if (!S.empty()) {
                stack_bottom[ei] = S.top();
            }
            if (ei == parent_edge[target[ei]]) {
                if (!test_dfs(target[ei])) {
                    return false;
                }
            } else {
                lowpoint_edge[ei] = ei;
                S.push({{}, {ei, ei}});
            }
            if (lowpoint[ei] < height[v]) {
                if (first) {
                    first = false;
                    lowpoint_edge[e] = lowpoint_edge[ei];
                } else if (!add_constraints(ei, e)) {
                    return false;
                }
            }
        }
        if (e != -1) {
            int u = source[e];
            trim_back_edges(u);

            if (lowpoint[e] < height[u]) {
                int hL = S.top().L.high;
                int hR = S.top().R.high;
                if (hL != -1 && (hR == -1 || lowpoint[hL] > lowpoint[hR])) {
                    ref[e] = hL;
                } else {
                    ref[e] = hR;
                }
            }
        }
        return true;
    }

    bool is_planar() {
        if (V <= 4) {
            return true;
        }
        if (E > 3 * V - 6) {
            return false;
        }
        parent_edge.assign(V, -1);
        height.assign(V, -1);
        lowpoint.assign(E, -1);
        lowpoint2.assign(E, -1);
        nesting.assign(E, 0);
        vis.assign(E, false);
        ref.assign(E, -1);
        side.assign(E, +1);
        S = stack<conflict_t>();
        stack_bottom.assign(E, conflict_t());
        lowpoint_edge.assign(E, -1);
        dfs_adj.assign(V, {});

        vector<int> roots;

        for (int u = 0; u < V; u++) {
            if (height[u] == -1) {
                height[u] = 0;
                orient_dfs(u);
                roots.push_back(u);
            }
        }

        for (int r : roots) {
            if (!test_dfs(r)) {
                return false;
            }
        }

        return true;
    }
};

#endif // LEFT_RIGHT_PLANARITY_HPP
