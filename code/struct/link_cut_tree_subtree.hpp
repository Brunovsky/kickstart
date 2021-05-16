#ifndef LINK_CUT_TREE_SUBTREE_HPP
#define LINK_CUT_TREE_SUBTREE_HPP

#include <bits/stdc++.h>
using namespace std;

/**
 * Unrooted link cut tree: subtree queries + point updates.
 */
struct link_cut_tree_subtree {
    struct Node {
        int parent = 0, child[2] = {};
        int8_t flip = 0;   // splay tree is flipped due to reroot
        int subt_size = 1; // size of splay tree below u
        int virt_size = 0; // size of subtree below u
        long self = 0;     // this node's value
        long subt = 0;     // subtree aggregate ~= aggregate of splay + virtuals
        long virt = 0;     // virtual aggregate ~= aggregate of virtuals
    };

    vector<Node> t;

    explicit link_cut_tree_subtree(int N = 0) : t(N + 1) { t[0].subt_size = 0; }

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
    }

    // Update node from splay children and virtual updates
    void pushup(int u) {
        auto [l, r] = t[u].child;
        pushdown(l), pushdown(r);
        t[u].subt_size = 1 + t[l].subt_size + t[r].subt_size + t[u].virt_size;
        t[u].subt = t[u].self + t[l].subt + t[r].subt + t[u].virt;
    }

    void add_virtual_subtree(int u, int child) {
        t[u].virt += t[child].subt;
        t[u].virt_size += t[child].subt_size;
    }

    void rem_virtual_subtree(int u, int child) {
        t[u].virt -= t[child].subt;
        t[u].virt_size -= t[child].subt_size;
    }

    // ***** Interface

    void link(int u, int v) {
        reroot(u), access(v);
        t[u].parent = v, add_virtual_subtree(v, u);
    }

    void cut(int u, int v) {
        reroot(u), access(v);
        assert(t[u].parent == v && !t[u].child[1] && u == t[v].child[0]);
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

    auto query_subtree(int u, int v) {
        reroot(v), access(u);
        return t[u].self + t[u].virt; // equivalent
        // return t[u].subt - t[t[u].child[0]].subt;
    }

    int subtree_size(int u, int v) {
        reroot(v), access(u);
        return 1 + t[u].virt_size; // equivalent
        // return t[u].subt_size - t[t[u].child[0]].subt_size;
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
            add_virtual_subtree(v, t[v].child[1]);
            rem_virtual_subtree(v, last);
            t[v].child[1] = last;
            pushup(v);
            last = v, v = t[v].parent;
        } while (v);
        splay(u);
        assert(!t[u].child[1] && !t[u].flip);
        return last;
    }
};

#endif // LINK_CUT_TREE_SUBTREE_HPP
