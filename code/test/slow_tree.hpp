#include "../generators/graph_formats.hpp"
#include "../random.hpp"
#include "test_utils.hpp"

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
namespace gnu = __gnu_pbds;

template <typename T, typename CompareFn = less<T>>
using ordered_set = gnu::tree<T, gnu::null_type, CompareFn, gnu::rb_tree_tag,
                              gnu::tree_order_statistics_node_update>;

/**
 * An implementation of common link-cut tree mechanics using trivial data structures,
 * along with helpful utilities
 */
struct slow_tree {
    int N, S;
    bool unrooted = false;
    vector<long> val;
    vector<int> parent;
    ordered_set<int> roots, non_roots;
    vector<unordered_set<int>> children;

    slow_tree(int N, bool unrooted = false)
        : N(N), S(N), unrooted(unrooted), val(N + 1), parent(N + 1, 0), children(N + 1) {
        for (int u = 1; u <= N; u++) {
            roots.insert(u);
        }
    }

    int num_nodes() const { return N; }
    int num_components() const { return S; }

    // ***** UTILS

    string format_debug() const {
        vector<array<int, 2>> g;
        for (int u = 1; u <= N; u++) {
            for (int v : children[u]) {
                g.push_back({u, v});
            }
        }
        auto node_ann = [&](int u) { return format("label=\"{}\\nv={}\"", u, val[u]); };
        auto edge_ann = [&](int, int) { return ""s; };
        return full_dot(g, 1, N, !unrooted, node_ann, edge_ann);
    }

    int random_root() {
        assert(!roots.empty());
        return *roots.find_by_order(intd(0, roots.size() - 1)(mt));
    }

    int random_non_root() {
        assert(!non_roots.empty());
        return *non_roots.find_by_order(intd(0, non_roots.size() - 1)(mt));
    }

    array<int, 2> random_connected() {
        assert(S < N);
        int r = findroot(random_non_root());
        auto subtree = get_subtree(r);
        int S = subtree.size();
        auto [a, b] = different(0, S);
        return {*next(begin(subtree), a), *next(begin(subtree), b)};
    }

    array<int, 2> random_unconnected() {
        assert(S > 1);
        auto [i, j] = different(0, S);
        int u = *roots.find_by_order(i), v = *roots.find_by_order(j);
        return {random_in_subtree(u), random_in_subtree(v)};
    }

    int random_in_tree(int u) { return random_in_subtree(findroot(u)); }

    int random_in_subtree(int u) {
        auto nodes = get_subtree(u);
        int S = nodes.size();
        return *next(begin(nodes), intd(0, S - 1)(mt));
    }

    int random_not_in_subtree(int u) {
        list<int> available;
        int r = findroot(u);
        for (int v : roots) {
            if (v != r) {
                available.splice(end(available), get_subtree(v));
            }
        }
        int S = available.size();
        return *next(begin(available), intd(0, S - 1)(mt));
    }

    int random_ancestor(int u) {
        auto nodes = get_root_path(u);
        int S = nodes.size();
        return *next(begin(nodes), intd(0, S - 1)(mt));
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
        if (unrooted)
            reroot(v); // silently reroot on v
        list<int> nodes = {u};
        while (u != v && u) {
            nodes.push_back(u = parent[u]);
        }
        assert(u == v);
        return nodes;
    }

    auto query_node(int u) const { return val[u]; }

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
        assert(unrooted || !parent[u]);
        if (unrooted)
            reroot(u);
        parent[u] = v, children[v].insert(u);
        roots.erase(u), non_roots.insert(u);
        S--;
    }

    void cut(int u, int v) {
        assert(u != v && (parent[u] == v || (unrooted && parent[v] == u)));
        if (parent[v] == u)
            swap(u, v);
        parent[u] = 0, children[v].erase(u);
        non_roots.erase(u), roots.insert(u);
        S++;
    }

    void update_node(int u, long new_value) { val[u] = new_value; }

    void reroot(int u) {
        if (parent[u]) {
            flip_to_child(parent[u], u);
            parent[u] = 0;
            roots.insert(u), non_roots.erase(u);
        }
    }

    int lca(int u, int v) {
        list<int> unodes = get_root_path(u), vnodes = get_root_path(v);
        if (unodes.back() != vnodes.back()) {
            return 0;
        } else {
            auto uit = rbegin(unodes), vit = rbegin(vnodes);
            do {
                ++uit, ++vit;
            } while (uit != rend(unodes) && vit != rend(vnodes) && *uit == *vit);
            return *--uit;
        }
    }

    int findroot(int u) {
        while (parent[u]) {
            u = parent[u];
        }
        return u;
    }

    // ***** SUBTREES

    auto query_subtree(int u) {
        long sum = 0;
        for (int w : get_subtree(u)) {
            sum += val[w];
        }
        return sum;
    }

    int subtree_size(int u) { return get_subtree(u).size(); }

    void update_subtree(int u, long value) {
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

    auto query_root_path(int u) { return query_path(u, findroot(u)); }

    int path_length(int u, int v) { return get_path(u, v).size(); }

    int root_path_length(int u) { return get_root_path(u).size(); }

    void update_path(int u, int v, long value) {
        for (int w : get_path(u, v)) {
            val[w] += value;
        }
    }

    void update_root_path(int u, long value) {
        for (int w : get_root_path(u)) {
            val[w] += value;
        }
    }
};
