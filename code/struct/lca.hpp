#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * LCA on a tree, 1-indexed
 */
struct lca_tree {
    int N, B;
    vector<vector<int>> up;
    vector<int> subt, depth;

    static int need_bits(int n) {
        return 1 + (n > 1 ? 8 * sizeof(n) - __builtin_clz(n - 1) : 0);
    }

    explicit lca_tree(const vector<vector<int>>& tree, int root)
        : N(tree.size()), B(need_bits(N)), up(B, vector<int>(N)), subt(N), depth(N) {
        init_dfs(tree, root, 0);

        for (int b = 1; b < B; b++) {
            for (int i = 1; i < N; i++) {
                int p = up[b - 1][i];
                up[b][i] = (p >= 1) ? up[b - 1][p] : 0;
            }
        }
    }

    void init_dfs(const vector<vector<int>>& tree, int u, int p) {
        up[0][u] = p;
        depth[u] = (p >= 1) ? depth[p] + 1 : 0;
        subt[u] = 1;
        for (int v : tree[u]) {
            if (v != p) {
                init_dfs(tree, v, u);
                subt[u] += subt[v];
            }
        }
    }

    int ancestor(int u, int steps) const {
        for (int b = B - 1; b >= 0; b--) {
            if (steps & (1 << b)) { u = up[b][u]; }
        }
        return u;
    }

    int parent(int u) const { return up[0][u]; }

    int lca(int u, int v) const {
        if (depth[u] < depth[v]) {
            v = ancestor(v, depth[v] - depth[u]);
        } else if (depth[u] > depth[v]) {
            u = ancestor(u, depth[u] - depth[v]);
        }
        for (int k = B - 1; k >= 0; k--) {
            if (up[k][u] != up[k][v]) { u = up[k][u], v = up[k][v]; }
        }
        return u == v ? u : up[0][u];
    }

    int dist(int u, int v) const { // # edges on path
        return depth[u] + depth[v] - 2 * depth[lca(u, v)];
    }
};
