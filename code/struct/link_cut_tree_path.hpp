#ifndef LINK_CUT_TREE_PATH_HPP
#define LINK_CUT_TREE_PATH_HPP

#include <bits/stdc++.h>
using namespace std;

/**
 * Unrooted link cut tree: lazy path queries + path/point updates.
 */
struct link_cut_tree_path {
    struct Node {
        int parent = 0, child[2] = {};
        int8_t flip = 0;   // splay tree is flipped due to reroot
        int path_size = 1; // size of splay tree below u
        long self = 0;     // this node's value
        long path = 0;     // path aggregate ~= aggregate of splay tree below u
        long lazy = 0;     // path lazy; will apply to all children
    };

    vector<Node> t;

    explicit link_cut_tree_path(int N = 0) : t(N + 1) { t[0].path_size = 0; }

    // ***** Node updates

    // Apply lazy updates stored at u and push them to its children
    void pushdown(int u) {
        auto& [l, r] = t[u].child;
        if (t[u].flip) {
            swap(l, r);
            t[l].flip ^= 1;
            t[r].flip ^= 1;
            t[u].flip = 0;
        }
        if (t[u].lazy) {
            t[l].lazy += t[u].lazy;
            t[r].lazy += t[u].lazy;
            t[u].self += t[u].lazy;
            t[u].path += t[u].lazy * t[u].path_size;
            t[u].lazy = t[0].lazy = 0;
        }
    }

    // Update node from splay children and virtual updates
    void pushup(int u) {
        auto [l, r] = t[u].child;
        pushdown(l), pushdown(r);
        t[u].path_size = 1 + t[l].path_size + t[r].path_size;
        t[u].path = t[u].self + t[l].path + t[r].path;
    }

    // ***** Interface

    void link(int u, int v) {
        reroot(u); // no way to detect cycles without doing extra work
        t[u].parent = v;
    }

    void cut(int u, int v) {
        reroot(u), access(v);
        assert(t[u].parent == v && !t[u].child[1] && u == t[v].child[0]); // no edge!
        t[u].parent = t[v].child[0] = 0;
    }

    void reroot(int u) {
        access(u);
        t[u].flip ^= 1;
    }

    int findroot(int u) {
        access(u);
        while (t[u].child[0])
            u = t[u].child[0], pushdown(u);
        return u;
    }

    int lca(int u, int v) {
        if (u == v)
            return u;
        access(u), v = access(v);
        return t[u].parent ? v : 0;
    }

    void update_node(int u, long value) {
        access(u);
        t[u].self = value;
    }

    void update_path(int u, int v, long value) {
        reroot(u), access(v);
        t[v].lazy += value;
    }

    auto query_path(int u, int v) {
        reroot(u), access(v);
        return t[v].path;
    }

    int path_length(int u, int v) {
        reroot(u), access(v);
        return t[v].path_size;
    }

  private:
    bool is_root(int u) const {
        return t[t[u].parent].child[0] != u && t[t[u].parent].child[1] != u;
    }

    void adopt(int parent, int child, int8_t side) {
        if (side >= 0)
            t[parent].child[side] = child;
        if (child)
            t[child].parent = parent;
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
