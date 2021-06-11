#ifndef LINK_CUT_TREE_PATH_HPP
#define LINK_CUT_TREE_PATH_HPP

#include <bits/stdc++.h>
using namespace std;

/**
 * Maintain sum of paths, with support for path range updates
 */
struct lct_node_path_sum {
    int path_size = 1;
    long self = 0;
    long path = 0;
    long lazy = 0;

    void pushdown(lct_node_path_sum& lhs, lct_node_path_sum& rhs) {
        if (lazy) {
            lhs.lazy += lazy;
            rhs.lazy += lazy;
            self += lazy;
            path += lazy * path_size;
            lazy = 0;
        }
    }

    void pushup(const lct_node_path_sum& lhs, const lct_node_path_sum& rhs) {
        path_size = 1 + lhs.path_size + rhs.path_size;
        path = self + lhs.path + rhs.path;
    }

    void clear() { lazy = path_size = 0; } // for 0 node
};

/**
 * Maintain maximum in paths without support for path range updates
 */
struct lct_node_path_max {
    int path_size = 1;
    int self = 0;
    int path = 0;

    void pushdown(lct_node_path_max&, lct_node_path_max&) {}

    void pushup(const lct_node_path_max& lhs, const lct_node_path_max& rhs) {
        path_size = 1 + lhs.path_size + rhs.path_size;
        path = max(self, max(lhs.path, rhs.path));
    }

    void clear() { path_size = 0; } // for 0 node
};

struct lct_node_path_empty {
    void pushdown(lct_node_path_empty&, lct_node_path_empty&) {}
    void pushup(const lct_node_path_empty&, const lct_node_path_empty&) {}
    void clear() {}
};

/**
 * Unrooted link cut tree: lazy path queries + path/point updates.
 */
template <typename LCTNode>
struct link_cut_tree_path {
    struct Node {
        int parent = 0, child[2] = {};
        int8_t flip = 0; // splay tree is flipped due to reroot
        LCTNode node;
    };

    vector<Node> t;

    explicit link_cut_tree_path(int N = 0) : t(N + 1) { t[0].node.clear(); }

    // ***** Node updates
  private:
    void pushdown(int u) {
        auto& [l, r] = t[u].child;
        if (t[u].flip) {
            swap(l, r);
            t[l].flip ^= 1;
            t[r].flip ^= 1;
            t[u].flip = 0;
        }
        if (u != 0) { t[u].node.pushdown(t[l].node, t[r].node); }
    }

    void pushup(int u) {
        auto [l, r] = t[u].child;
        pushdown(l), pushdown(r);
        t[u].node.pushup(t[l].node, t[r].node);
    }

    // ***** Interface
  public:
    bool link(int u, int v) {
        reroot(u), access(v); // no way to detect cycles without doing extra work
        if (t[u].parent) return false;
        t[u].parent = v;
        return true;
    }

    bool cut(int u, int v) {
        reroot(u), access(v);
        if (!t[u].parent || t[u].child[1] || u != t[v].child[0]) return false;
        t[u].parent = t[v].child[0] = 0;
        return true;
    }

    void reroot(int u) {
        access(u);
        t[u].flip ^= 1;
    }

    int findroot(int u) {
        access(u);
        while (t[u].child[0]) u = t[u].child[0], pushdown(u);
        return u;
    }

    int lca(int u, int v) {
        if (u == v) return u;
        access(u), v = access(v);
        return t[u].parent ? v : 0;
    }

    bool conn(int u, int v) { return lca(u, v) != 0; }

    LCTNode* access_node(int u) {
        access(u);
        return &t[u].node;
    }
    LCTNode* access_path(int u, int v) {
        reroot(v), access(u);
        return &t[u].node;
    }

  private:
    bool is_root(int u) const {
        return t[t[u].parent].child[0] != u && t[t[u].parent].child[1] != u;
    }

    void adopt(int parent, int child, int8_t side) {
        if (side >= 0) t[parent].child[side] = child;
        if (child) t[child].parent = parent;
    }

    void rotate(int u) {
        int p = t[u].parent, g = t[p].parent;
        bool uside = u == t[p].child[1];
        adopt(p, t[u].child[!uside], uside);
        adopt(g, u, !is_root(p) ? p == t[g].child[1] : -1);
        adopt(u, p, !uside);
        pushup(p);
    }

    void splay(int u) {
        int p = t[u].parent, g = t[p].parent;
        while (!is_root(u) && !is_root(p)) {
            pushdown(g), pushdown(p), pushdown(u);
            bool zigzig = (u == t[p].child[1]) == (p == t[g].child[1]);
            rotate(zigzig ? p : u), rotate(u);
            p = t[u].parent, g = t[p].parent;
        }
        if (!is_root(u)) {
            pushdown(p), pushdown(u);
            rotate(u);
        }
        pushdown(u), pushup(u);
    }

    int access(int u) {
        int last = 0, v = u;
        do {
            splay(v);
            t[v].child[1] = last;
            last = v, v = t[v].parent;
        } while (v);
        splay(u);
        assert(!t[u].child[1] && !t[u].flip);
        return last;
    }
};

#endif // LINK_CUT_TREE_PATH_HPP
