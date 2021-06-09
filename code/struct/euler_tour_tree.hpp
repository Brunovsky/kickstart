#ifndef EULER_TOUR_TREE_HPP
#define EULER_TOUR_TREE_HPP

#include <bits/stdc++.h>
using namespace std;

/**
 * Maintain sum and size of subtrees
 */
struct ett_node_sum {
    int opens = 0;
    long self = 0;
    long subt = 0;
    long lazy = 0;

    int subtree_size() const { return opens; }
    long subtree() const { return subt; }

    void pushdown(bool is_open, ett_node_sum& lhs, ett_node_sum& rhs) {
        if (lazy) {
            lhs.lazy += lazy;
            rhs.lazy += lazy;
            if (is_open) self += lazy;
            subt += lazy * opens;
            lazy = 0;
        }
    }

    void pushup(bool is_open, const ett_node_sum& lhs, const ett_node_sum& rhs) {
        opens = is_open + lhs.opens + rhs.opens;
        subt = self + lhs.subt + rhs.subt;
    }

    void clear() {}
};

struct ett_node_empty {
    void pushdown(bool, ett_node_empty&, ett_node_empty&) {}
    void pushup(bool, const ett_node_empty&, const ett_node_empty&) {}
    void clear() {}
};

template <typename ETTNode>
struct euler_tour_tree {
    struct Node {
        int parent = 0, child[2] = {};
        int splay_size = 1;
        ETTNode node;
    };
    struct access_manager {
      private:
        friend euler_tour_tree;
        euler_tour_tree* ett;
        int u, l, r;
        access_manager(euler_tour_tree* ett, int u) : ett(ett), u(u) {
            r = ett->split_after(last(u));
            l = ett->split_before(first(u));
            ett->root = first(u);
        }
        access_manager(const access_manager&) = delete;
        access_manager(access_manager&&) = delete;

      public:
        ETTNode* operator->() noexcept { return &ett->t[first(u)].node; }
        ~access_manager() noexcept {
            assert(ett->root == first(u));
            ett->root = ett->splay_join(ett->root, r);
            ett->root = ett->splay_join(l, ett->root);
        }
    };

    int root = 0;
    vector<Node> t;

    explicit euler_tour_tree(int N = 0) : t(2 * N + 1) {
        t[0].splay_size = 0, t[0].node.clear();
        for (int u = 1; u <= N; u++) {
            root = splay_join(root, splay_join(first(u), last(u)));
        }
    }

  private:
    void pushdown(int u) {
        auto [l, r] = t[u].child;
        if (u != 0) { t[u].node.pushdown(u & 1, t[l].node, t[r].node); }
    }

    void pushup(int u) {
        auto [l, r] = t[u].child;
        pushdown(l), pushdown(r);
        t[u].node.pushup(u & 1, t[l].node, t[r].node);
    }

  public:
    void link(int u, int v) {
        int r = split_after(last(u));
        int l = split_before(first(u));
        splay_join(l, r);
        root = split_after(first(v));
        root = splay_join(first(u), root);
        root = splay_join(first(v), root);
    }

    void cut(int u) {
        int l = split_before(first(u));
        int r = split_after(last(u));
        root = splay_join(l, r);
        root = splay_join(root, last(u));
    }

    bool in_subtree(int u, int a) const {
        if (u == a) return true;
        int fu = order_of_node(first(u));
        return order_of_node(first(a)) < fu && fu < order_of_node(last(a));
    }

    ETTNode* access_node(int u) { return splay(first(u)), &t[first(u)].node; }
    access_manager access_subtree(int u) { return access_manager(this, u); }

  private:
    static int first(int n) { return 2 * n - 1; }
    static int last(int n) { return 2 * n; }
    bool is_root(int u) const { return !t[u].parent; }
    bool is_left(int u) const { return t[u].parent && u == t[t[u].parent].child[0]; }
    bool is_right(int u) const { return t[u].parent && u == t[t[u].parent].child[1]; }
    int min_node(int u) const { return t[u].child[0] ? min_node(t[u].child[0]) : u; }
    int max_node(int u) const { return t[u].child[1] ? max_node(t[u].child[1]) : u; }
    int next(int u) const {
        if (t[u].child[1]) return min_node(t[u].child[1]);
        while (u && is_left(u)) u = t[u].parent;
        return u;
    }
    int prev(int u) const {
        if (t[u].child[0]) return min_node(t[u].child[0]);
        while (u && is_right(u)) u = t[u].parent;
        return u;
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

    int find_by_order(int u, int order) const {
        if (order >= t[u].splay_size) return 0;
        while (true) {
            int v = t[u].child[0];
            if (order == t[v].splay_size) return u;
            if (order < t[v].splay_size) {
                u = v;
            } else {
                order -= t[v].splay_size;
                u = t[u].child[1];
            }
        }
    }

    int order_of_node(int u) const {
        int order = t[t[u].child[0]].splay_size;
        while (t[u].parent) {
            if (is_right(u)) order += t[t[u].parent].child[0] + 1;
            u = t[u].parent;
        }
        return order;
    }

    int split_before(int u) {
        splay(u);
        int v = t[u].child[0];
        t[v].parent = t[u].child[0] = 0;
        pushup(u);
        return v;
    }

    int split_after(int u) {
        splay(u);
        int v = t[u].child[1];
        t[v].parent = t[u].child[1] = 0;
        pushup(u);
        return v;
    }

    int splay_join(int l, int r) {
        if (l == 0 || r == 0) return l ? l : r;
        assert(is_root(l) && is_root(r));
        int root = max_node(l);
        splay(root);
        adopt(root, r, 1);
        pushup(root);
        return root;
    }
};

#endif // EULER_TOUR_TREE_HPP
