#pragma once

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
        node.assign(2 * N, Node());
        update.assign(2 * N, Update());
        has_lazy.assign(2 * N, false);
        range.resize(2 * N);
        int Q = 1 << (N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0);
        for (int i = 0; i < N; i++) {
            range[i + N] = {L + i, L + i + 1};
            node[i + N] = Node(arr[L + i]);
        }
        rotate(begin(node) + N, begin(node) + (3 * N - Q), end(node));
        rotate(begin(range) + N, begin(range) + (3 * N - Q), end(range));
        for (int u = N - 1; u >= 1; u--) {
            int cl = u << 1, cr = u << 1 | 1;
            range[u] = {range[cl][0], range[cr][1]};
            node[u].merge(node[cl], node[cr]);
        }
    }

    void assign(int L, int R) {
        int N = R - L;
        node.assign(2 * N, Node());
        update.assign(2 * N, Update());
        has_lazy.assign(2 * N, false);
        range.resize(2 * N);
        int Q = 1 << (N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0);
        for (int i = 0; i < N; i++) {
            range[i + N] = {L + i, L + i + 1};
        }
        rotate(begin(range) + N, begin(range) + (3 * N - Q), end(range));
        for (int u = N - 1; u >= 1; u--) {
            int cl = u << 1, cr = u << 1 | 1;
            range[u] = {range[cl][0], range[cr][1]};
        }
    }

    auto query_range(int L, int R) { return query_range(1, L, R); }
    void update_range(int L, int R, const Update& add) { update_range(1, L, R, add); }

  private:
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
    void apply(sum_segnode& node) const { node.value += value; }
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
    void apply(sum_segnode& node) const { node.value = value; }
};

template <typename T>
struct affine_segnode {
    T value = 0;
    explicit affine_segnode(T value = 0) : value(value) {}
    void merge(const affine_segnode& lhs, const affine_segnode& rhs) {
        value = lhs.value + rhs.value;
    }
};

template <typename T>
struct affine_add_segupdate {
    T b = 1, c = 0;
    affine_add_segupdate() : b(1), c(0) {}
    affine_add_segupdate(T b, T c) : b(b), c(c) {}
    void merge(const affine_add_segupdate& parent, array<int, 2> /*range*/) {
        c = c * parent.b + parent.c;
        b = b * parent.b;
    }
    void apply(affine_segnode<T>& node, array<int, 2> range) const {
        node.value = b * node.value + c * (range[1] - range[0]);
    }
};

} // namespace samples_segtree
