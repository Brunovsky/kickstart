#pragma once

#include "disjoint_set.hpp"
#include "y_combinator.hpp"

/**
 * LCA on a tree, 0/1-indexed, binary lifting
 * Complexity: O(N log N) construction, O(log N) query (lca or ancestor)
 */
struct lca_binary {
    int N, B;
    vector<vector<int>> up;
    vector<int> depth;

    static int need_bits(int n) {
        return 1 + (n > 1 ? 8 * sizeof(n) - __builtin_clz(n - 1) : 0);
    }

    explicit lca_binary(const vector<vector<int>>& tree, int root, int zero = 0)
        : N(tree.size()), B(need_bits(N)), up(B, vector<int>(N)), depth(N) {
        init_dfs(tree, root, 0);

        for (int b = 1; b < B; b++) {
            for (int i = 0; i < N; i++) {
                int p = up[b - 1][i];
                up[b][i] = (p > zero) ? up[b - 1][p] : 0;
            }
        }
    }

    void init_dfs(const vector<vector<int>>& tree, int u, int p) {
        up[0][u] = p;
        depth[u] = (p >= 1) ? depth[p] + 1 : 0;
        for (int v : tree[u]) {
            if (v != p) {
                init_dfs(tree, v, u);
            }
        }
    }

    int ancestor(int u, int steps) const {
        for (int b = B - 1; b >= 0; b--) {
            if (steps & (1 << b)) {
                u = up[b][u];
            }
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
            if (up[k][u] != up[k][v]) {
                u = up[k][u], v = up[k][v];
            }
        }
        return u == v ? u : up[0][u];
    }

    int dist(int u, int v) const { return depth[u] + depth[v] - 2 * depth[lca(u, v)]; }
};

/**
 * LCA on a tree, 0/1-indexed, RMQ based
 * Complexity: O(N log N) construction, O(1) query
 * Computes repeated euler tour as well.
 */
struct lca_rmq {
    int N, timer = 0;
    vector<int> tour, first, last, up, depth;
    vector<vector<int>> jmp;

    static int need_bits(int n) {
        return 1 + (n > 1 ? 8 * sizeof(n) - __builtin_clz(n - 1) : 0);
    }

    explicit lca_rmq(const vector<vector<int>>& tree, int root, int zero = 0)
        : N(tree.size()), first(N), last(N), up(N), depth(N) {
        init_dfs(tree, root, zero);

        jmp = {tour};

        for (int len = 1, k = 1, M = tour.size(); 2 * len <= M; len *= 2, k++) {
            int J = M - 2 * len + 1;
            jmp.emplace_back(J);
            for (int j = 0; j < J; j++) {
                int l = jmp[k - 1][j];
                int r = jmp[k - 1][j + len];
                jmp[k][j] = depth[l] < depth[r] ? l : r;
            }
        }
    }

    void init_dfs(const vector<vector<int>>& tree, int u, int p) {
        up[u] = p;
        first[u] = last[u] = timer++;
        tour.push_back(u);
        for (int v : tree[u]) {
            if (v != p) {
                depth[v] = depth[u] + 1;
                init_dfs(tree, v, u);
                last[u] = timer++;
                tour.push_back(u);
            }
        }
    }

    int parent(int u) const { return up[u]; }

    int lca(int u, int v) const {
        static constexpr int BITS = CHAR_BIT * sizeof(int) - 1;
        if (u == v)
            return u;
        auto [a, b] = minmax(first[u], first[v]);
        int bits = BITS - __builtin_clz(b - a);
        int l = jmp[bits][a];
        int r = jmp[bits][b - (1 << bits)];
        return depth[l] < depth[r] ? l : r;
    }

    int dist(int u, int v) const { return depth[u] + depth[v] - 2 * depth[lca(u, v)]; }
};

/**
 * LCA on a tree, 0/1-indexed, Schieber-Vishkin
 * Complexity: O(N) construction, O(1) query
 * Reference: https://github.com/indy256/codelibrary
 * Computes preorder as well.
 */
struct lca_schieber_vishkin {
    int N, timer = 0;
    vector<int> preorder, up, I, A, head, depth;

    static int lowest_one_bit(int n) { return n & -n; }
    static int highest_one_bit(int n) { return n ? 1 << (31 - __builtin_clz(n)) : 0; }

    explicit lca_schieber_vishkin(const vector<vector<int>>& tree, int root, int zero = 0)
        : N(tree.size()), preorder(N), up(N), I(N), A(N), head(N), depth(N) {
        init_dfs1(tree, root, zero);
        init_dfs2(tree, root, zero, 0);
    }

    void init_dfs1(const vector<vector<int>>& tree, int u, int p) {
        up[u] = p;
        I[u] = preorder[u] = timer++;
        for (int v : tree[u]) {
            if (v != p) {
                depth[v] = depth[u] + 1;
                init_dfs1(tree, v, u);
                if (lowest_one_bit(I[u]) < lowest_one_bit(I[v])) {
                    I[u] = I[v];
                }
            }
        }
        head[I[u]] = u;
    }

    void init_dfs2(const vector<vector<int>>& tree, int u, int p, int up) {
        A[u] = up | lowest_one_bit(I[u]);
        for (int v : tree[u]) {
            if (v != p) {
                init_dfs2(tree, v, u, A[u]);
            }
        }
    }

    int parent(int u) const { return up[u]; }

    int enter_into_strip(int u, int hz) const {
        if (lowest_one_bit(I[u]) == hz)
            return u;
        int hw = highest_one_bit(A[u] & (hz - 1));
        return parent(head[(I[u] & -hw) | hw]);
    }

    int lca(int u, int v) const {
        int hb = I[u] == I[v] ? lowest_one_bit(I[u]) : highest_one_bit(I[u] ^ I[v]);
        int hz = lowest_one_bit(A[u] & A[v] & -hb);
        int eu = enter_into_strip(u, hz);
        int ev = enter_into_strip(v, hz);
        return preorder[eu] < preorder[ev] ? eu : ev;
    }

    int dist(int u, int v) const { return depth[u] + depth[v] - 2 * depth[lca(u, v)]; }
};

/**
 * Tarjan's offline LCA algorithm.
 * Complexity: O(N + Q)
 */
auto lca_tarjan(const vector<vector<int>>& tree, int root,
                const vector<array<int, 2>>& queries) {
    int N = tree.size(), Q = queries.size();

    vector<vector<int>> want(N);
    vector<int> lca(queries.size());

    for (int i = 0; i < Q; i++) {
        auto [u, v] = queries[i];
        if (u == v) {
            lca[i] = u;
        } else {
            want[u].push_back(i);
            want[v].push_back(i);
        }
    }

    vector<bool> color(N);
    disjoint_set dsu(N);

    y_combinator([&](auto self, int u, int p) -> void {
        dsu.reroot(u);
        for (int v : tree[u]) {
            if (v != p) {
                self(v, u);
                dsu.join(u, v);
                dsu.reroot(u);
            }
        }
        color[u] = 1;
        for (int i : want[u]) {
            int v = u ^ queries[i][0] ^ queries[i][1];
            if (color[v]) {
                lca[i] = dsu.find(v);
            }
        }
    })(root, -1);

    return lca;
}
