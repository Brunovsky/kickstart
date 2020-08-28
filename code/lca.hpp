#ifndef LCA_HPP
#define LCA_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100'000

vector<int> children[MAXN];

template <int N, int BITS>
struct lca_tree {
    int parent[BITS][N];
    int depth[N];

    void init_dfs(int u, int w) {
        parent[0][u] = w;
        depth[u] = (w == -1) ? 0 : depth[w] + 1;
        for (int v : children[u]) {
            init_dfs(v, u);
        }
    }

    void init(int root) {
        memset(parent, 0, sizeof(parent));
        memset(depth, 0, sizeof(depth));
        init_dfs(root, -1);
        for (int k = 1; k < BITS; k++) {
            for (int i = 0; i < N; i++) {
                int t = parent[k - 1][i];
                parent[k][i] = (t == -1) ? -1 : parent[k - 1][t];
            }
        }
    }

    // b=1 for parent, b=2 for grandparent, etc.
    int ancestor(int u, int b) {
        for (int k = BITS - 1; k >= 0; k--) {
            if (b & (1 << k)) {
                u = parent[k][u];
            }
        }
        return u;
    }

    int lca(int u, int v) {
        if (depth[u] < depth[v]) {
            v = ancestor(v, depth[v] - depth[u]);
        } else if (depth[u] > depth[v]) {
            u = ancestor(u, depth[u] - depth[v]);
        }
        for (int k = BITS - 1; k >= 0; k--) {
            if (parent[k][u] != parent[k][v]) {
                u = parent[k][u], v = parent[k][v];
            }
        }
        return u == v ? u : parent[0][u];
    }

    int dist(int u, int v) { // # edges on path
        return depth[u] + depth[v] - 2 * depth[lca(u, v)];
    }
};

#endif // LCA_HPP
