#pragma once

#include "../hash.hpp"

struct ett_node_sum {
    int subt_size = 0;
    long self = 0;
    long subt = 0;
    long lazy = 0;

    long subtree() const { return subt; }
    int subtree_size() const { return subt_size; }

    void pushdown(bool is_node, bool, ett_node_sum& lhs, ett_node_sum& rhs) {
        if (lazy) {
            lhs.lazy += lazy;
            rhs.lazy += lazy;
            if (is_node)
                self += lazy;
            subt += lazy * subt_size;
            lazy = 0;
        }
    }
    void pushup(bool is_node, bool, const ett_node_sum& lhs, const ett_node_sum& rhs) {
        subt_size = is_node + lhs.subt_size + rhs.subt_size;
        subt = self + lhs.subt + rhs.subt;
    }
};

struct ett_node_empty {
    void pushdown(bool, bool, ett_node_empty&, ett_node_empty&) {}
    void pushup(bool, bool, ett_node_empty&, ett_node_empty&) {}
};

/**
 * Unrooted euler tour tree
 */
template <typename ETTNode>
struct euler_tour_tree {
    struct Node {
        int parent = 0, child[2] = {};
        ETTNode node;
    };
    struct access_manager {
      private:
        friend euler_tour_tree;
        euler_tour_tree* ett;
        int u, v, l, r;
        access_manager(euler_tour_tree* ett, int u, int v) : ett(ett), u(u), v(v) {
            int uv = ett->get_edge(u, v), vu = uv ^ 1;
            assert(uv > 0);
            l = ett->splay_split<0>(vu);
            r = ett->splay_split<1>(uv);
            ett->splay(u);
        }
        access_manager(const access_manager&) = delete;
        access_manager(access_manager&&) = delete;

      public:
        ETTNode* operator->() { return &ett->t[u].node; }
        ~access_manager() noexcept { ett->splay_join(l, u, r); }
    };

    int N, F;
    vector<Node> t;
    unordered_map<pair<int, int>, int> edgemap;
    vector<int> freelist;

    explicit euler_tour_tree(int N = 0) : N(N), F(N), t(3 * N + 3), freelist(N) {
        for (int i = 0, f = (N | 1) + 1; i < N; i++) {
            freelist[i] = f + 2 * i;
        }
    }

    int get_edge(int u, int v) const {
        auto ituv = edgemap.find(minmax(u, v));
        return ituv == edgemap.end() ? 0 : ituv->second ^ (u > v);
    }
    int add_edge(int u, int v) {
        int uv = freelist[--F], vu = uv ^ 1;
        t[uv] = t[vu] = {};
        edgemap.emplace(minmax(u, v), uv ^ (u > v));
        return uv;
    }
    void rem_edge(int u, int v) {
        auto ituv = edgemap.find(minmax(u, v));
        freelist[F++] = ituv->second;
        edgemap.erase(ituv);
    }

    void pushdown(int u) {
        auto [l, r] = t[u].child;
        if (u != 0) {
            t[u].node.pushdown(u <= N, u > N && !(u & 1), t[l].node, t[r].node);
        }
    }

    void pushup(int u) {
        auto [l, r] = t[u].child;
        pushdown(l), pushdown(r);
        t[u].node.pushup(u <= N, u > N && !(u & 1), t[l].node, t[r].node);
    }

  public:
    bool link(int u, int v) {
        reroot(u), splay(v);
        if (t[u].parent)
            return false;
        int uv = add_edge(u, v), vu = uv ^ 1;
        int r = splay_split<1>(v); //
        splay_join(v, splay_join(vu, u, uv), r);
        return true;
    }

    bool cut(int u, int v) {
        int uv = get_edge(u, v), vu = uv ^ 1;
        if (uv == 0)
            return false;

        reroot(v); // v ..A.. vu u ..B.. uv ..C.. --> v ..A.. ..C.. | u ..B..
        splay_join(splay_erase(vu).first, splay_erase(uv).second);
        rem_edge(u, v);
        return true;
    }

    void reroot(int u) { shift_to_front(u); }

    int findroot(int u) {
        splay(u);
        return min_node(u);
    }

    bool conn(int u, int v) {
        if (u == v)
            return true;
        splay(u), splay(v);
        return t[u].parent;
    }

    ETTNode* access_node(int u) {
        splay(u);
        return &t[u].node;
    }
    ETTNode* access_edge(int u, int v) {
        int uv = get_edge(min(u, v), max(u, v));
        assert(uv > 0), splay(uv);
        return &t[uv].node;
    }
    ETTNode* access_tree(int u) {
        reroot(u);
        return &t[u].node;
    }
    access_manager access_subtree(int u, int v) {
        reroot(v);
        return access_manager(this, u, v);
    }

  private:
    bool is_root(int u) const { return !t[u].parent; }
    bool is_left(int u) const { return t[u].parent && u == t[t[u].parent].child[0]; }
    bool is_right(int u) const { return t[u].parent && u == t[t[u].parent].child[1]; }
    int root_node(int u) const { return t[u].parent ? root_node(t[u].parent) : u; }
    int min_node(int u) const { return t[u].child[0] ? min_node(t[u].child[0]) : u; }
    int max_node(int u) const { return t[u].child[1] ? max_node(t[u].child[1]) : u; }

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
        adopt(g, u, g ? p == t[g].child[1] : -1);
        adopt(u, p, !uside);
        pushup(p);
    }

    void splay(int u) {
        int p = t[u].parent, g = t[p].parent;
        while (p && g) {
            pushdown(g), pushdown(p), pushdown(u);
            bool zigzig = (u == t[p].child[1]) == (p == t[g].child[1]);
            rotate(zigzig ? p : u), rotate(u);
            p = t[u].parent, g = t[p].parent;
        }
        if (p) {
            pushdown(p), pushdown(u), rotate(u);
        }
        pushdown(u), pushup(u);
    }

    template <bool after>
    int splay_split(int u) {
        splay(u);
        int v = t[u].child[after];
        t[v].parent = t[u].child[after] = 0;
        pushup(u);
        return v;
    }

    int splay_join(int l, int r) {
        if (l == 0 || r == 0)
            return l ? l : r;
        assert(is_root(l) && is_root(r));
        int root = max_node(l);
        splay(root);
        adopt(root, r, 1);
        pushup(root);
        return root;
    }

    int splay_join(int l, int m, int r) { return splay_join(splay_join(l, m), r); }

    auto splay_erase(int u) {
        int r = splay_split<1>(u), l = splay_split<0>(u);
        return make_pair(l, r);
    }

    void shift_to_front(int u) { splay_join(u, splay_split<0>(u)), splay(u); }
    void shift_to_back(int u) { splay_join(splay_split<1>(u), u), splay(u); }
};
