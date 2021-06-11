#pragma once

#include "splay_tree.hpp" // splay_tree_base, splay_order

/**
 * Euler tour tree implementation based on splay tree
 * Node should inherit from splay_order<itself> (CRTP)
 */

inline namespace ett_examples {

struct ett_node_sum : splay_order<ett_node_sum> {
    bool is_first = false;
    int firsts = 0;
    long self = 0;
    long subt = 0;
    long lazy = 0;

    ett_node_sum(bool is_first = false) : is_first(is_first) {}

    int subtree_size() const { return firsts; }
    long subtree() const { return subt; }

    void pushdown() {
        splay_order<ett_node_sum>::pushdown();
        if (lazy) {
            if (child[0])
                child[0]->lazy += lazy;
            if (child[1])
                child[1]->lazy += lazy;
            if (is_first)
                self += lazy;
            subt += lazy * firsts;
            lazy = 0;
        }
    }

    void pushup() {
        splay_order<ett_node_sum>::pushup();
        if (child[0])
            child[0]->pushdown();
        if (child[1])
            child[1]->pushdown();
        firsts = is_first + get_firsts(child[0]) + get_firsts(child[1]);
        subt = self + get_subt(child[0]) + get_subt(child[1]);
    }

  private:
    static int get_firsts(const ett_node_sum* n) { return n ? n->firsts : 0; }
    static long get_subt(const ett_node_sum* n) { return n ? n->subt : 0; }
};

struct ett_node_empty : splay_order<ett_node_empty> {
    ett_node_empty(bool = false) {}
};

} // namespace ett_examples

template <typename ETTNode>
struct euler_tour_tree {
  private:
    using ett_node = ETTNode;
    struct subt_handler {
      private:
        friend euler_tour_tree;
        euler_tour_tree* ett;
        int u;
        ett_node *L, *R;
        subt_handler(euler_tour_tree* ett, int u) : ett(ett), u(u) {
            R = ett->last[u].split_after();   // ...u1 | R
            L = ett->first[u].split_before(); // L | u0...u1 | R
            ett->root = &ett->first[u];
        }
        subt_handler(const subt_handler&) = delete;
        subt_handler(subt_handler&&) = delete;

      public:
        ett_node* operator->() noexcept { return &ett->first[u]; }
        ~subt_handler() noexcept {
            assert(ett->root == &ett->first[u]);
            ett->root = ett_node::join(ett->root, R); // L | [u0...u1] R
            ett->root = ett_node::join(L, ett->root); // L [u0...u1] R
        }
    };

    ett_node* root = nullptr;
    vector<ett_node> first, last;

  public:
    euler_tour_tree(int N = 0) : first(N + 1, 1), last(N + 1, 0) {
        for (int u = 1; u <= N; u++) {
            root = ett_node::join(root, ett_node::join(&first[u], &last[u]));
        }
    }

    // relink u as a child of v. u does not need to be a root (no need to cut it first)
    void link(int u, int v) {
        ett_node* R = last[u].split_after();    // ...u1 | R
        ett_node* L = first[u].split_before();  // L | u0...u1 | R
        ett_node::join(L, R);                   // ...v0...v1... | u0...u1
        root = first[v].split_after();          // ...v0 | ...v1... | u0...u1
        root = ett_node::join(&first[u], root); // ...v0 | u0...u1...v1...
        root = ett_node::join(&first[v], root); // ...v0u0...u1...v1...

        // ett_node* L = first[u].split_before(); // L | u0...
        // ett_node* R = last[u].split_after();   // L | [u0...u1] | R
        // ett_node::join(L, R);                  // [...v0...v1...] | [u0...u1]
        // root = last[v].split_before();         // [...v0...] | [v1...] | [u0...u1]
        // root = ett_node::join(root, &last[u]); // [...v0...u0...u1] | [v1...]
        // root = ett_node::join(root, &last[v]); // [...v0...u0...u1v1...]
    }

    void cut(int u) {
        ett_node* L = first[u].split_before(); // L | u0...
        ett_node* R = last[u].split_after();   // L | [u0...u1] | R
        root = ett_node::join(L, R);           // L R | [u0...u1]
        root = ett_node::join(root, &last[u]); // L R [u0...u1]
    }

    bool in_subtree(int u, int a) const {
        if (u == a) {
            return true;
        }
        int fu = first[u].order_of_node();
        return first[a].order_of_node() < fu && fu < last[a].order_of_node();
    }

    ett_node* access_node(int u) { return first[u].splay(), &first[u]; }
    subt_handler access_subtree(int u) { return subt_handler(this, u); }
};

struct simple_euler_tour_tree {
  private:
    struct ett_node : splay_order<ett_node> {};
    ett_node* root = nullptr;
    vector<ett_node> first, last;

  public:
    simple_euler_tour_tree(int N = 0) : first(N + 1), last(N + 1) {
        for (int u = 1; u <= N; u++) {
            root = ett_node::join(root, ett_node::join(&first[u], &last[u]));
        }
    }
    simple_euler_tour_tree(const simple_euler_tour_tree&) = delete;

    // relink u as a child of v. v need not be a root.
    void link(int u, int v) {
        ett_node* L = first[u].split_before(); // L | u0...
        ett_node* R = last[u].split_after();   // L | [u0...u1] | R
        ett_node::join(L, R);                  // [...v0...v1...] | [u0...u1]
        root = last[v].split_before();         // [...v0...] | [v1...] | [u0...u1]
        root = ett_node::join(root, &last[u]); // [...v0...u0...u1] | [v1...]
        root = ett_node::join(root, &last[v]); // [...v0...u0...u1v1...]
    }

    void cut(int u) {
        ett_node* L = first[u].split_before(); // L | u0...
        ett_node* R = last[u].split_after();   // L | [u0...u1] | R
        root = ett_node::join(L, R);           // L R | [u0...u1]
        root = ett_node::join(root, &last[u]); // L R [u0...u1]
    }

    bool in_subtree(int u, int a) const {
        if (u == a) {
            return true;
        }
        int fu = first[u].order_of_node();
        return first[a].order_of_node() < fu && fu < last[a].order_of_node();
    }
};
