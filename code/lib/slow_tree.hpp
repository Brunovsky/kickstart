#pragma once

#include "graph_formats.hpp"
#include "../random.hpp"

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
namespace gnu = __gnu_pbds;

template <typename T, typename CompareFn = less<T>>
using ordered_set = gnu::tree<T, gnu::null_type, CompareFn, gnu::rb_tree_tag,
                              gnu::tree_order_statistics_node_update>;

/**
 * An implementation of common link-cut tree mechanics using trivial data structures,
 * along with helpful utilities.
 */
template <bool rooted>
struct slow_tree {
    int N, S;
    vector<long> val;
    vector<int> parent;
    ordered_set<int> roots, non_roots;
    vector<ordered_set<int>> children;

    explicit slow_tree(int N)
        : N(N), S(N), val(N + 1), parent(N + 1, 0), children(N + 1) {
        for (int u = 1; u <= N; u++) {
            roots.insert(u);
        }
    }

    int num_nodes() const { return N; }
    int num_components() const { return S; }

    // ***** UTILS

    int random_root() const {
        int V = roots.size();
        assert(V > 0);
        return *roots.find_by_order(intd(0, V - 1)(mt));
    }

    int random_non_root() const {
        int V = non_roots.size();
        assert(V > 0);
        return *non_roots.find_by_order(intd(0, V - 1)(mt));
    }

    int random_adjacent(int u) {
        assert(!children[u].empty() || parent[u] != 0);
        if (parent[u] != 0) {
            children[u].insert(parent[u]);
        }
        int V = children[u].size();
        assert(V > 0);
        int v = *children[u].find_by_order(intd(0, V - 1)(mt));
        if (parent[u] != 0) {
            children[u].erase(parent[u]);
        }
        return v;
    }

    auto random_connected_distinct() {
        int r = findroot(random_non_root());
        auto subtree = get_subtree(r);
        int V = subtree.size();
        assert(V > 1);
        auto [a, b] = different(0, V);
        return make_pair(*next(begin(subtree), a), *next(begin(subtree), b));
    }

    auto random_connected() {
        int r = random_root();
        auto subtree = get_subtree(r);
        int V = subtree.size();
        assert(V > 0);
        int a = intd(0, V - 1)(mt);
        int b = intd(0, V - 1)(mt);
        return make_pair(*next(begin(subtree), a), *next(begin(subtree), b));
    }

    auto random_unconnected() {
        assert(S > 1);
        auto [i, j] = different(0, S);
        int u = *roots.find_by_order(i), v = *roots.find_by_order(j);
        return make_pair(random_in_subtree(u), random_in_subtree(v));
    }

    int random_in_tree(int u) {
        int r = findroot(u);
        return random_in_subtree(r);
    }

    int random_distinct_in_tree(int u) {
        int r = findroot(u);
        auto subtree = get_subtree(r);
        int V = subtree.size();
        assert(V > 1);
        int i = distance(begin(subtree), find(begin(subtree), end(subtree), u));
        int j = different(i, 0, V);
        return *next(begin(subtree), j);
    }

    int random_in_subtree(int u) {
        auto nodes = get_subtree(u);
        int V = nodes.size();
        return *next(begin(nodes), intd(0, V - 1)(mt));
    }

    int random_distinct_in_subtree(int u) {
        auto subtree = get_subtree(u);
        int V = subtree.size();
        assert(S > 1);
        int i = distance(begin(subtree), find(begin(subtree), end(subtree), u));
        int j = different(i, 0, V);
        return *next(begin(subtree), j);
    }

    int random_unconnected(int u) {
        int V = roots.size();
        assert(V > 1);
        int r = findroot(u);
        int i = roots.order_of_key(r);
        int j = different(i, 0, V);
        return random_in_subtree(*roots.find_by_order(j));
    }

    int random_ancestor(int u) {
        auto nodes = get_root_path(u);
        int V = nodes.size();
        return *next(begin(nodes), intd(0, V - 1)(mt));
    }

    int random_distinct_ancestor(int u) {
        assert(parent[u] != 0);
        auto nodes = get_root_path(parent[u]);
        int V = nodes.size();
        return *next(begin(nodes), intd(0, V - 1)(mt));
    }

    // ***** GETTERS

    list<int> get_subtree(int u, int v) {
        static_assert(!rooted);
        reroot(v);
        return get_subtree(u);
    }

    list<int> get_tree(int u) {
        u = findroot(u);
        return get_subtree(u);
    }

    list<int> get_subtree(int u) {
        list<int> nodes = {u};
        for (int v : children[u]) {
            nodes.splice(end(nodes), get_subtree(v));
        }
        return nodes;
    }

    list<int> get_root_path(int u) {
        list<int> nodes = {u};
        while (parent[u]) {
            nodes.push_back(u = parent[u]);
        }
        return nodes;
    }

    list<int> get_path(int u, int v) {
        int c = lca(u, v);
        list<int> a = {u}, b = {v};
        while (u != c && u) {
            a.push_back(u = parent[u]);
        }
        while (v != c && v) {
            b.push_front(v = parent[v]);
        }
        assert(a.back() == b.front());
        b.pop_front();
        a.splice(end(a), b);
        return a;
    }

    // ***** REROOTING

    void flip_to_child(int u, int c) {
        if (parent[u]) {
            flip_to_child(parent[u], u);
        }
        parent[u] = c;
        children[c].insert(u);
        children[u].erase(c);
        roots.erase(u), non_roots.insert(u);
    }

    // ***** CORE

    void link(int u, int v) {
        assert(u != v && findroot(u) != findroot(v));
        if constexpr (!rooted)
            reroot(u);
        else if constexpr (rooted)
            if (parent[u] != 0)
                cut(u);
        parent[u] = v;
        children[v].insert(u);
        roots.erase(u), non_roots.insert(u);
        S--;
    }

    void cut(int u) {
        static_assert(rooted);
        int v = parent[u];
        assert(v != 0);
        parent[u] = 0, children[v].erase(u);
        non_roots.erase(u), roots.insert(u);
        S++;
    }

    void cut(int u, int v) {
        static_assert(!rooted);
        assert(u != v);
        reroot(v);
        assert(parent[u] == v);
        parent[u] = 0, children[v].erase(u);
        non_roots.erase(u), roots.insert(u);
        S++;
    }

    void reroot(int u) {
        if (u && parent[u]) {
            flip_to_child(parent[u], u);
            parent[u] = 0;
            roots.insert(u), non_roots.erase(u);
        }
    }

    int lca(int u, int v) const {
        int a = u, b = v, ra = 0, rb = 0;
        while (a != b) {
            a = parent[a] ? parent[a] : (ra = a, v);
            b = parent[b] ? parent[b] : (rb = b, u);
            if (ra != 0 && rb != 0 && ra != rb)
                return 0; // different trees
        }
        return a;
    }

    int findroot(int u) const {
        while (parent[u]) {
            u = parent[u];
        }
        return u;
    }

    // ***** NODE QUERIES

    auto query_node(int u) const { return val[u]; }

    void update_node(int u, long new_value) { val[u] = new_value; }

    // ***** TREES

    auto query_tree(int u) {
        long sum = 0;
        for (int w : get_tree(u)) {
            sum += val[w];
        }
        return sum;
    }

    int tree_size(int u) { return get_tree(u).size(); }

    void update_tree(int u, long value) {
        for (int w : get_tree(u)) {
            val[w] += value;
        }
    }

    // ***** ROOTED SUBTREES

    auto query_subtree(int u) {
        static_assert(rooted);
        long sum = 0;
        for (int w : get_subtree(u)) {
            sum += val[w];
        }
        return sum;
    }

    int subtree_size(int u) {
        static_assert(rooted);
        return get_subtree(u).size();
    }

    void update_subtree(int u, long value) {
        static_assert(rooted);
        for (int w : get_subtree(u)) {
            val[w] += value;
        }
    }

    // ***** UNROOTED SUBTREES

    auto query_subtree(int u, int v) {
        static_assert(!rooted);
        reroot(v);
        long sum = 0;
        for (int w : get_subtree(u)) {
            sum += val[w];
        }
        return sum;
    }

    int subtree_size(int u, int v) {
        static_assert(!rooted);
        reroot(v);
        return get_subtree(u).size();
    }

    void update_subtree(int u, int v, long value) {
        static_assert(!rooted);
        reroot(v);
        for (int w : get_subtree(u)) {
            val[w] += value;
        }
    }

    // ***** PATHS

    auto query_path(int u, int v) {
        long sum = 0;
        for (int w : get_path(u, v)) {
            sum += val[w];
        }
        return sum;
    }

    int path_length(int u, int v) { return get_path(u, v).size(); }

    void update_path(int u, int v, long value) {
        for (int w : get_path(u, v)) {
            val[w] += value;
        }
    }
};
