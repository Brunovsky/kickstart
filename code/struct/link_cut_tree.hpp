#ifndef LINK_CUT_TREE_HPP
#define LINK_CUT_TREE_HPP

#include <bits/stdc++.h>

#include "../formatting.hpp"

using namespace std;

/**
 * Link cut tree based on a splay forest
 * Details in Analysis.md
 */
struct link_cut_tree {
    struct Node {
        int parent = 0, child[2] = {};
        int8_t flip = 0;   // splay tree is flipped due to reroot
        int sub_size = 1;  // size of represented tree below u
        int path_size = 1; // size of splay tree below u
        int vir_size = 0;  // sum of sizes of all virtual subtrees of u
        long self = 0;     // this node's value
        long path = 0;     // path aggregate -> aggregate of splay tree below u
        long sub = 0;      // subtree aggregate -> aggregate of represented tree below u
        long vir = 0;      // virtual aggregate -> aggregate of virtual trees below u
    };

    // ***** Debugging

    void verify_invariants() const {
        int N = t.size() - 1;
        for (int u = 1; u <= N; u++) {
            int l = t[u].child[0], r = t[u].child[1], p = t[u].parent;
            assert(!l || t[l].parent == u);
            assert(!r || t[r].parent == u);
            assert(p != u);
            assert(t[u].sub_size == 1 + t[l].sub_size + t[r].sub_size + t[u].vir_size);
            assert(t[u].path_size == 1 + t[l].path_size + t[r].path_size);
            assert(!p || t[u].sub_size < t[p].sub_size);
        }
        // root path is sensible
        for (int u = 1; u <= N; u++) {
            vector<int> path;
            int v = u;
            do {
                path.push_back(v), v = t[v].parent;
            } while (v && u != v);
            assert(v == 0);
        }
    }

    void debug() const {
        int N = t.size() - 1;
        print("------------------------------------------------\n");
        print(" u   p  l  r  f | as ps vs | self path subt virt\n");
        auto g = [&](int x) { return x ? to_string(x) : " "; };
        for (int u = 0; u <= N; u++) {
            auto [par, c, flip, as, ps, vs, self, path, sub, vir] = t[u];
            auto [c0, c1] = c;
            print("{:>2}: {:>2} {:>2} {:>2} {:>2} | "
                  "{:2} {:2} {:2} | "
                  "{:4} {:4} {:4} {:4} \n",
                  u, g(par), g(c0), g(c1), g(flip), //
                  as, ps, vs,                       //
                  self, path, sub, vir);
        }
    }

    vector<Node> t;
    vector<int> scratch;

    explicit link_cut_tree(int N = 0) : t(N + 1), scratch(N + 1) {
        t[0].sub_size = t[0].path_size = 0;
    }

    // ***** Node updates

    // Directly set u's value and update aggregates
    void apply(int u, long value) {
        t[u].self = value;
        pushup(u);
    }

    // Pushdown lazy updates from u to its children (including reroot flip)
    void pushdown(int u) { pushdown_flip(u); }

    // Update node from splay children and virtual updates
    void pushup(int u) {
        auto [l, r] = t[u].child;
        t[u].sub_size = 1 + t[l].sub_size + t[r].sub_size + t[u].vir_size;
        t[u].sub = t[u].self + t[l].sub + t[r].sub + t[u].vir;
        t[u].path_size = 1 + t[l].path_size + t[r].path_size;
        t[u].path = t[u].self + t[l].path + t[r].path;
    }

    // Add c as virtual subtree of u
    void link_virtual_update(int u, int c) {
        t[u].vir_size += t[c].sub_size;
        t[u].vir += t[c].sub;
        pushup(u);
    }

    // Remove c as virtual subtree of u
    void cut_virtual_update(int u, int c) {
        t[u].vir_size -= t[c].sub_size;
        t[u].vir -= t[c].sub;
        pushup(u);
    }

    // ***** Interface

    // make u child of v
    void link(int u, int v) {
        access(u), access(v);
        assert(!t[u].parent && !t[v].parent);
        t[u].parent = v, link_virtual_update(v, u);
    }

    // cut link from child u to v
    void cut(int u, int v) {
        access(u), access(v);
        assert(t[u].parent == v && t[v].child[1] == u && !t[u].child[0]);
        t[v].child[1] = t[u].parent = 0, pushup(v);
    }

    auto query_subtree(int u) {
        access(u);
        return t[u].sub - t[t[u].child[0]].sub;
    }

    auto query_root_path(int u) {
        access(u);
        return t[u].path - t[t[u].child[1]].path;
    }

    auto query_path(int u, int v) {
        return u == v ? t[u].self : query_root_path(u) - query_root_path(v) + t[v].self;
    }

    auto subtree_size(int u) {
        access(u);
        return t[u].sub_size - t[t[u].child[0]].sub_size;
    }

    auto root_path_length(int u) {
        access(u);
        return t[u].path_size - t[t[u].child[1]].path_size;
    }

    auto path_length(int u, int v) {
        return u == v ? 1 : root_path_length(u) - root_path_length(v) + 1;
    }

    // Update node u with given value
    void update_node(int u, long value) {
        access(u);
        apply(u, value);
    }

  private:
    void pushdown_flip(int u) {
        if (u && t[u].flip) {
            auto& [l, r] = t[u].child;
            swap(l, r), t[l].flip = !t[l].flip, t[r].flip = !t[r].flip;
            t[u].flip = 0;
        }
    }

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
        assert(!is_root(u));
        int p = t[u].parent, g = t[p].parent;
        bool yside = u == t[p].child[1], top = is_root(p);
        adopt(p, t[u].child[!yside], yside);
        adopt(g, u, !top ? p == t[g].child[1] : -1);
        adopt(u, p, !yside);
        pushup(p), pushup(u);
    }

    void flush_tree(int u) {
        int i = 0;
        scratch[i++] = u;
        while (!is_root(u)) {
            scratch[i++] = u = t[u].parent;
        }
        while (i--) {
            pushdown(scratch[i]);
        }
    }

    void splay(int u) {
        flush_tree(u);
        int p = t[u].parent, g = t[p].parent;
        while (!is_root(u) && !is_root(p)) {
            bool zigzig = (u == t[p].child[1]) == (p == t[g].child[1]);
            rotate(zigzig ? p : u), rotate(u);
            p = t[u].parent, g = t[p].parent;
        }
        if (!is_root(u)) {
            rotate(u), assert(is_root(u));
        }
    }

    void access(int u) {
        int v = u;
        splay(u);
        while (t[u].parent) {
            int last = u;
            u = t[u].parent;
            splay(u);
            cut_virtual_update(u, last);
            link_virtual_update(u, t[u].child[1]);
            t[u].child[1] = last;
            pushup(u);
        }
        splay(v);
    }

    void trim(int u) {
        assert(u && !t[u].flip);
        if (t[u].child[1]) {
            int c = t[u].child[1];
            t[c].parent = t[u].child[1] = 0;
            while (t[c].child[1]) {
                c = t[c].child[1];
            }
            splay(c);
            t[c].parent = u;
            link_virtual_update(u, c);
        }
    }
};

#endif // LINK_CUT_TREE_HPP
