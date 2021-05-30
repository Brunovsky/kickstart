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
    vector<bool> has_lazy;
    vector<array<int, 2>> range;

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
        has_lazy.assign(4 * N, false);
        range.resize(4 * N);
        build(1, L, R, arr);
    }

    void assign(int L, int R) {
        int N = R - L;
        node.assign(4 * N, Node());
        update.assign(4 * N, Update());
        has_lazy.assign(4 * N, false);
        range.resize(4 * N);
        build(1, L, R);
    }

    auto query_range(int L, int R) { return query_range(1, L, R); }
    void update_range(int L, int R, const Update& add) { update_range(1, L, R, add); }

  private:
    template <typename Arr>
    void build(int u, int L, int R, const Arr& arr) {
        range[u] = {L, R};
        if (L + 1 == R) {
            node[u] = Node(arr[L]);
        } else {
            int M = (L + R + 1) / 2;
            int cl = u << 1, cr = u << 1 | 1;
            build(cl, L, M, arr);
            build(cr, M, R, arr);
            node[u].merge(node[cl], node[cr]);
        }
    }

    void build(int u, int L, int R) {
        range[u] = {L, R};
        if (L + 1 != R) {
            int M = (L + R + 1) / 2;
            int cl = u << 1, cr = u << 1 | 1;
            build(cl, L, M);
            build(cr, M, R);
        }
    }

    void pushdown(int u) {
        if (has_lazy[u]) {
            int cl = u << 1, cr = u << 1 | 1;
            has_lazy[cl] = has_lazy[cr] = 1;
            update[u].apply(node[cl], range[cl]);
            update[u].apply(node[cr], range[cr]);
            update[cl].merge(update[u], range[cl]);
            update[cr].merge(update[u], range[cr]);
            has_lazy[u] = 0;
            update[u] = Update();
        }
    }

    void update_range(int u, int L, int R, const Update& add) {
        if (R <= range[u][0] || range[u][1] <= L) {
            return;
        }
        if (L <= range[u][0] && range[u][1] <= R) {
            add.apply(node[u], range[u]);
            update[u].merge(add, range[u]);
            has_lazy[u] = 1;
            return;
        }
        pushdown(u);
        int cl = u << 1, cr = u << 1 | 1;
        update_range(cl, L, R, add);
        update_range(cr, L, R, add);
        node[u].merge(node[cl], node[cr]);
    }

    auto query_range(int u, int L, int R) {
        if (R <= range[u][0] || range[u][1] <= L) {
            return Node();
        }
        if (L <= range[u][0] && range[u][1] <= R) {
            return node[u];
        }
        pushdown(u);
        int cl = u << 1, cr = u << 1 | 1;
        Node ans;
        ans.merge(query_range(cl, L, R), query_range(cr, L, R));
        return ans;
    }
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
