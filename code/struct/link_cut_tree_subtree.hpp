#ifndef LINK_CUT_TREE_SUBTREE_HPP
#define LINK_CUT_TREE_SUBTREE_HPP

#include <bits/stdc++.h>
using namespace std;

struct lct_node_subtree_sum {
    int subt_size = 1; // size of splay tree below u
    int virt_size = 0; // size of subtree below u
    long self = 0;     // this node's value
    long subt = 0;     // subtree aggregate ~= aggregate of splay + virtuals
    long virt = 0;     // virtual aggregate ~= aggregate of virtuals
    // subtree query is self + virt / subtree size query is 1 + virt_size

    long subtree() const { return self + virt; }
    int subtree_size() const { return 1 + virt_size; }

    void pushdown(lct_node_subtree_sum&, lct_node_subtree_sum&) {}

    void pushup(lct_node_subtree_sum& lhs, lct_node_subtree_sum& rhs) {
        subt_size = 1 + lhs.subt_size + rhs.subt_size + virt_size;
        subt = self + lhs.subt + rhs.subt + virt;
    }

    void add_virtual_subtree(lct_node_subtree_sum& child) {
        virt += child.subt;
        virt_size += child.subt_size;
    }

    void rem_virtual_subtree(lct_node_subtree_sum& child) {
        virt -= child.subt;
        virt_size -= child.subt_size;
    }

    void clear() { subt_size = 0; } // for 0 node
};

/**
 * Unrooted link cut tree: subtree queries + point updates.
 */
template <typename LCTNode>
struct link_cut_tree_subtree {
    struct Node {
        int parent = 0, child[2] = {};
        int8_t flip = 0; // splay tree is flipped due to reroot
        LCTNode node;
    };

    vector<Node> t;

    explicit link_cut_tree_subtree(int N = 0) : t(N + 1) { t[0].node.clear(); }

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
        t[u].node.pushdown(t[l].node, t[r].node);
    }

    // Update node from splay children and virtual updates
    void pushup(int u) {
        auto [l, r] = t[u].child;
        pushdown(l), pushdown(r);
        t[u].node.pushup(t[l].node, t[r].node);
    }

    // ***** Interface

    void link(int u, int v) {
        reroot(u), access(v);
        t[u].parent = v;
        t[v].node.add_virtual_subtree(t[u].node);
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

    LCTNode& access_node(int u) {
        access(u);
        return t[u].node;
    }
    LCTNode& access_subtree(int u, int v) {
        reroot(v), access(u);
        return t[u].node;
    }
    const LCTNode& access_node(int u) const {
        access(u);
        return t[u].node;
    }
    const LCTNode& access_subtree(int u, int v) const {
        reroot(v), access(u);
        return t[u].node;
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
            t[v].node.add_virtual_subtree(t[t[v].child[1]].node);
            t[v].node.rem_virtual_subtree(t[last].node);
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
