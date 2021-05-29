#ifndef SEGTREE_HPP
#define SEGTREE_HPP

#include <bits/stdc++.h>
using namespace std;

/**
 * Simple array-based segment tree template with range updates and range queries
 */
template <typename Node, typename Update>
struct segtree {
    vector<Node> node;
    vector<Update> update;
    vector<bool> lazy;
    vector<array<int, 2>> range;
    vector<int> leaf_index;

    segtree() = default;
    segtree(int L, int R) { assign(L, R); }

    template <typename Arr>
    segtree(int L, int R, const Arr& arr) {
        assign(L, R, arr);
    }

    template <typename Arr>
    void assign(int L, int R, const Arr& arr) {
        int N = R - L;
        node.assign(4 * N, Node());
        update.assign(4 * N, Update());
        lazy.assign(4 * N, false);
        range.resize(4 * N);
        leaf_index.assign(4 * N, 0);
        build(1, L, R, arr);
    }

    template <typename Arr>
    void build(int u, int L, int R, const Arr& arr) {
        range[u] = {L, R};
        if (L + 1 == R) {
            node[u] = Node(arr[L]);
            leaf_index[L] = u;
        } else {
            int M = (L + R + 1) / 2;
            build(u << 1, L, M, arr);
            build(u << 1 | 1, M, R, arr);
            node[u].merge(node[u << 1], node[u << 1 | 1]);
        }
    }

    void assign(int L, int R) {
        int N = R - L;
        node.assign(4 * N, Node());
        update.assign(4 * N, Update());
        lazy.assign(4 * N, false);
        range.resize(4 * N);
        leaf_index.assign(4 * N, 0);
        build(1, L, R);
    }

    void build(int u, int L, int R) {
        range[u] = {L, R};
        if (L + 1 == R) {
            leaf_index[L] = u;
        } else {
            int M = (L + R + 1) / 2;
            build(u << 1, L, M);
            build(u << 1 | 1, M, R);
        }
    }

    void pushdown(int u) {
        if (lazy[u]) {
            if (range[u][0] + 1 < range[u][1]) {
                lazy[u << 1] = lazy[u << 1 | 1] = 1;
                update[u << 1].merge(update[u], range[u << 1]);
                update[u << 1 | 1].merge(update[u], range[u << 1 | 1]);
            }
            update[u].apply(node[u], range[u]);
            lazy[u] = 0;
            update[u] = Update();
        }
    }

    void update_range(int u, int L, int R, const Update& add) {
        pushdown(u);
        if (R <= range[u][0] || range[u][1] <= L) {
            return;
        }
        if (L <= range[u][0] && range[u][1] <= R) {
            update[u].merge(add, range[u]);
            lazy[u] = 1, pushdown(u);
            return;
        }
        update_range(u << 1, L, R, add);
        update_range(u << 1 | 1, L, R, add);
        node[u].merge(node[u << 1], node[u << 1 | 1]);
    }

    auto query_range(int u, int L, int R) {
        pushdown(u);
        if (R <= range[u][0] || range[u][1] <= L) {
            return Node();
        }
        if (L <= range[u][0] && range[u][1] <= R) {
            return node[u];
        }
        auto a = query_range(u << 1, L, R);
        auto b = query_range(u << 1 | 1, L, R);
        Node ans;
        ans.merge(a, b);
        return ans;
    }

    auto query_range(int L, int R) { return query_range(1, L, R); }
    void update_range(int L, int R, const Update& add) { update_range(1, L, R, add); }
};

namespace samples_segtree {

struct min_segnode {
    int value = INT_MAX / 2;
    explicit min_segnode(int value = INT_MAX / 2) : value(value) {}
    void merge(const min_segnode& lhs, const min_segnode& rhs) {
        value = min(lhs.value, rhs.value);
    }
};

struct max_segnode {
    int value = 0;
    explicit max_segnode(int value = 0) : value(value) {}
    void merge(const max_segnode& lhs, const max_segnode& rhs) {
        value = max(lhs.value, rhs.value);
    }
};

struct sum_segnode {
    int value = 0;
    explicit sum_segnode(int value = 0) : value(value) {}
    void merge(const sum_segnode& lhs, const sum_segnode& rhs) {
        value = lhs.value + rhs.value;
    }
};

struct add_segupdate {
    int value = 0;
    add_segupdate(int value = 0) : value(value) {}
    void merge(const add_segupdate& parent, array<int, 2> /*range*/) {
        value += parent.value;
    }
    void apply(sum_segnode& node, array<int, 2> range) const {
        node.value += value * (range[1] - range[0]);
    }
    void apply(min_segnode& node, array<int, 2> /*range*/) const { node.value += value; }
};

struct set_segupdate {
    int value = 0;
    set_segupdate(int value = 0) : value(value) {}
    void merge(const set_segupdate& parent, array<int, 2> /*range*/) {
        value = parent.value;
    }
    void apply(sum_segnode& node, array<int, 2> range) const {
        node.value = value * (range[1] - range[0]);
    }
    void apply(min_segnode& node, array<int, 2> /*range*/) const { node.value = value; }
};

} // namespace samples_segtree

#endif // SEGTREE_HPP
