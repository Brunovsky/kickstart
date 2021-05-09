#include "../struct/link_cut_tree.hpp"

#include "../random.hpp"
#include "test_utils.hpp"

#define SHOW(x)  print(#x "\n"), x, tree.debug()
#define QUERY(x) print(#x ": {}\n", x), tree.debug()

struct slow_tree {
    int N;
    vector<int> val, parent;
    unordered_set<int> orphans, non_orphans;
    vector<unordered_set<int>> children;

    slow_tree(int N) : N(N), val(N + 1), parent(N + 1, 0), children(N + 1) {
        for (int u = 1; u <= N; u++) {
            orphans.insert(u);
        }
    }

    // ***** UTILS

    int random_root() const {
        assert(!orphans.empty());
        return *next(begin(orphans), ulongd(0, orphans.size() - 1)(mt));
    }

    int random_child() const {
        assert(!non_orphans.empty());
        return *next(begin(non_orphans), ulongd(0, non_orphans.size() - 1)(mt));
    }

    int random_not_in_subtree(int u) const {
        auto nodes = get_subtree(u);
        unordered_set<int> nodeset(begin(nodes), end(nodes));
        vector<int> available;
        for (int v = 1; v <= N; v++) {
            if (!nodeset.count(v)) {
                available.push_back(v);
            }
        }
        int S = available.size();
        return S == 0 ? -1 : available[intd(0, S - 1)(mt)];
    }

    int random_in_subtree(int u) const {
        auto nodes = get_subtree(u);
        vector<int> available(begin(nodes), end(nodes));
        int S = available.size();
        return available[intd(0, S - 1)(mt)];
    }

    list<int> get_subtree(int u) const {
        list<int> nodes = {u};
        for (int v : children[u]) {
            nodes.splice(end(nodes), get_subtree(v));
        }
        return nodes;
    }

    list<int> get_root_path(int u) const {
        list<int> nodes = {u};
        while (parent[u]) {
            nodes.push_back(u = parent[u]);
        }
        return nodes;
    }

    list<int> get_root_path(int u, int v) const {
        list<int> nodes = {u};
        while (u != v && u) {
            nodes.push_back(u = parent[u]);
        }
        assert(u == v);
        return nodes;
    }

    int query_node(int u) const { return val[u]; }

    void flip_to_child(int u, int c) {
        if (parent[u]) {
            flip_to_child(parent[u], u);
        }
        parent[u] = c;
        children[c].insert(u);
    }

    // ***** CORE

    void link(int u, int v) {
        assert(u != v && parent[u] == 0 && findroot(v) != u);
        parent[u] = v, children[v].insert(u);
        orphans.erase(u), non_orphans.insert(u);
    }

    void cut(int u, int v) {
        assert(u != v && parent[u] == v);
        parent[u] = 0, children[v].erase(u);
        non_orphans.erase(u), orphans.insert(u);
    }

    void update_node(int u, int new_value) { val[u] = new_value; }

    void reroot(int u) {
        if (parent[u]) {
            flip_to_child(parent[u], u);
        }
    }

    // ***** SUBTREES

    int query_subtree(int u) const {
        int sum = 0;
        for (int w : get_subtree(u)) {
            sum += val[w];
        }
        return sum;
    }

    int subtree_size(int u) const { return get_subtree(u).size(); }

    void update_subtree(int u, int value) {
        for (int w : get_subtree(u)) {
            val[w] += value;
        }
    }

    // ***** PATHS

    int query_path(int u, int v) const {
        int sum = 0;
        for (int w : get_root_path(u, v)) {
            sum += val[w];
        }
        return sum;
    }

    int query_root_path(int u) const { return query_path(u, findroot(u)); }

    int path_length(int u, int v) const { return get_root_path(u, v).size(); }

    void update_path(int u, int v, int value) {
        for (int w : get_root_path(u, v)) {
            val[w] += value;
        }
    }

    // ***** OTHER

    int lca(int u, int v) const {
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

    int findroot(int u) const {
        while (parent[u]) {
            u = parent[u];
        }
        return u;
    }
};

bool show_query_all(int lo, int hi, slow_tree& slow, link_cut_tree& tree, int D = 3) {
    assert(1 <= D && D <= 9 && lo <= hi);
    int N = hi - lo + 1;

    vector<vector<int>> subt(D), rootpath(D), path(D), subt_size(D), path_length(D);
    vector<int> exp_subt(N), exp_rootpath(N), exp_path(N), exp_subt_size(N);
    vector<int> exp_path_length(N), nodes(N), above(N);
    iota(begin(nodes), end(nodes), lo);

    bool ok_subt = true, ok_rootpath = true, ok_path = true;
    bool ok_subt_size = true, ok_path_length = true;

    for (int u = lo; u <= hi; u++) {
        auto on_path = slow.get_root_path(u);
        int S = on_path.size(), i = intd(0, S - 1)(mt);
        above[u - lo] = *next(begin(on_path), i);

        exp_subt[u - lo] = slow.query_subtree(u);
        exp_rootpath[u - lo] = slow.query_root_path(u);
        exp_path[u - lo] = slow.query_path(u, above[u - lo]);
        exp_subt_size[u - lo] = slow.subtree_size(u);
        exp_path_length[u - lo] = slow.path_length(u, above[u - lo]);
    }
    for (int d = 0; d < D; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        subt[d].resize(N);
        for (int u : order) {
            subt[d][u - lo] = tree.query_subtree(u);
        }
        if (subt[d] != exp_subt) {
            if (ok_subt) {
                ok_subt = false;
                print("expect_subtree: {}\n", exp_subt);
            }
            print("got_subtree[{}]: {}\n", d + 1, subt[d]);
            tree.debug();
        }
    }
    for (int d = 0; d < D; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        rootpath[d].resize(N);
        for (int u : order) {
            rootpath[d][u - lo] = tree.query_root_path(u);
        }
        if (rootpath[d] != exp_rootpath) {
            if (ok_rootpath) {
                ok_rootpath = false;
                print("expect_rootpath: {}\n", exp_rootpath);
            }
            print("got_rootpath[{}]: {}\n", d + 1, rootpath[d]);
            tree.debug();
        }
    }
    for (int d = 0; d < D; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        path[d].resize(N);
        for (int u : order) {
            path[d][u - lo] = tree.query_path(u, above[u - lo]);
        }
        if (path[d] != exp_path) {
            if (ok_path) {
                ok_path = false;
                print("      above: {}\n", above);
                print("expect_path: {}\n", exp_path);
            }
            print("got_path[{}]: {}\n", d + 1, path[d]);
            tree.debug();
        }
    }
    for (int d = 0; d < D; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        subt_size[d].resize(N);
        for (int v : order) {
            subt_size[d][v - lo] = tree.subtree_size(v);
        }
        if (subt_size[d] != exp_subt_size) {
            if (ok_subt_size) {
                ok_subt_size = false;
                print("expect_subtree_size: {}\n", exp_subt_size);
            }
            print("got_subtree_size[{}]: {}\n", d + 1, subt_size[d]);
            tree.debug();
        }
    }
    for (int d = 0; d < D; d++) {
        vector<int> order = nodes;
        shuffle(begin(order), end(order), mt);
        path_length[d].resize(N);
        for (int u : order) {
            path_length[d][u - lo] = tree.path_length(u, above[u - lo]);
        }
        if (path_length[d] != exp_path_length) {
            if (ok_path_length) {
                ok_path_length = false;
                print("             above: {}\n", above);
                print("expect_path_length: {}\n", exp_path_length);
            }
            print("got_path_length[{}]: {}\n", d + 1, path_length[d]);
            tree.debug();
        }
    }

    return ok_subt && ok_rootpath && ok_path && ok_subt_size && ok_path_length;
}

void stress_test_link_cut_tree() {
    constexpr int N = 15, minv = 1, maxv = 50, T = 1000;
    intd noded(1, N), vald(minv, maxv);

    vector<int> initial_target(N + 1);
    for (int u = 2; u <= N; u++) {
        initial_target[u] = intd(1, u - 1)(mt);
    }
    vector<int> initial_val(N + 1);
    for (int u = 1; u <= N; u++) {
        initial_val[u] = vald(mt);
    }

    slow_tree slow(N);
    link_cut_tree tree(N);

    for (int u = 1; u <= N; u++) {
        print("initial_val[{}] = {}\n", u, initial_val[u]);
        tree.update_node(u, initial_val[u]);
        slow.update_node(u, initial_val[u]);
    }

    for (int u = 2; u <= N; u++) {
        int v = initial_target[u];

        print("LINK {} -> {}\n", u, v);
        tree.link(u, v);
        slow.link(u, v);
        bool ok = show_query_all(1, u, slow, tree);
        assert(ok);
    }

    enum Actions { LINK, CUT, LINK_CUT, UPDATE_NODE, UPDATE_SUBTREE, UPDATE_PATH };
    action_selector as({300, 300, 1000, 1000, 0, 0});

    for (int t = 0; t < T; t++) {
        Actions action = Actions(as.select());

        switch (action) {
        case LINK: {
            if (!slow.orphans.empty()) {
                int u = slow.random_root();
                if (int v = slow.random_not_in_subtree(u); v != -1) {
                    print("LINK {} -> {}\n", u, v);
                    slow.link(u, v);
                    tree.link(u, v);
                }
            }
        } break;
        case CUT: {
            if (!slow.non_orphans.empty()) {
                int u = slow.random_child();
                int v = slow.parent[u];
                print("CUT {} -> {}\n", u, v);
                slow.cut(u, v);
                tree.cut(u, v);
            }
        } break;
        case LINK_CUT: {
            int u = noded(mt);
            if (int v = slow.parent[u]; v) {
                print("CUT {} -> {}\n", u, v);
                slow.cut(u, v);
                tree.cut(u, v);
            } else if (v = slow.random_not_in_subtree(u); v != -1) {
                print("LINK {} -> {}\n", u, v);
                slow.link(u, v);
                tree.link(u, v);
            }
        } break;
        case UPDATE_NODE: {
            int u = noded(mt);
            int value = vald(mt);
            print("UPDATE {}: {} -> {}\n", u, slow.query_node(u), value);
            slow.update_node(u, value);
            tree.update_node(u, value);
        } break;
        case UPDATE_SUBTREE: {
        } break;
        case UPDATE_PATH: {
        } break;
        }

        tree.verify_invariants();
        bool ok = show_query_all(1, N, slow, tree);
        assert(ok);
    }
}

void unit_test_link_cut_tree() {
    link_cut_tree tree(12);
    for (int u = 1; u <= 12; u++) {
        tree.update_node(u, u);
    }
    tree.debug();

    SHOW(tree.link(1, 9));
    SHOW(tree.link(9, 4));
    SHOW(tree.link(6, 4));
    SHOW(tree.link(4, 2));
    SHOW(tree.link(11, 10));
    SHOW(tree.link(12, 10));
    SHOW(tree.link(10, 5));
    SHOW(tree.link(5, 2));
    SHOW(tree.link(2, 7));
    SHOW(tree.link(8, 3));
    SHOW(tree.link(3, 7));
    QUERY(tree.query_subtree(7));
    QUERY(tree.query_subtree(4));
    QUERY(tree.query_subtree(5));
    QUERY(tree.query_subtree(2));
    SHOW(tree.cut(4, 2));
    QUERY(tree.query_subtree(7));
}

int main() {
    setbuf(stdout, nullptr);
    mt.seed(73);
    RUN_SHORT(stress_test_link_cut_tree());
    // RUN_SHORT(unit_test_link_cut_tree());
    return 0;
}
