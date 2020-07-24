#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100'000
#define BITS 18

vector<int> children[MAXN];
int parent[BITS][MAXN];
int depth[MAXN];

void init_lca_dfs(int u, int w) {
    parent[0][u] = w;
    depth[u] = (w == -1) ? 0 : depth[w] + 1;
    for (int v : children[u]) {
        init_lca_dfs(v, u);
    }
}

void init_lca(int N, int root) {
    init_lca_dfs(root, -1);
    for (int k = 1; k < BITS; k++) {
        for (int i = 0; i < N; i++) {
            int t = parent[k - 1][i];
            parent[k][i] = (t == -1) ? -1 : parent[k - 1][t];
        }
    }
}

int ancestor(int u, int b) {
    // go up the tree b nodes
    for (int k = BITS - 1; k >= 0; k--) {
        if (b & (1 << k)) {
            u = parent[k][u];
        }
    }
    return u;
}

int lca(int u, int v) {
    if (depth[u] < depth[v]) {
        swap(u, v);
    }
    u = ancestor(u, depth[u] - depth[v]);
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

// *****

void driver() {
    // populate N children...
    int N = 100;
    init_lca(N, 0);
}
