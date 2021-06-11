#pragma once

#include <bits/stdc++.h>
using namespace std;

template <typename Node, typename Update>
struct sparse_segtree {
    array<int, 2> range;
    unique_ptr<sparse_segtree> link[2];

    Node node = Node();
    Update update = Update(), child_update = Update();
    bool lazy = false, child_lazy = false;

    explicit sparse_segtree(int L = 0, int R = 0) : range({L, R}) {}

    void make_children() {
        auto [L, R] = range;
        if (!link[0] && L + 1 < R) {
            int M = (L + R + 1) / 2;
            link[0] = make_unique<sparse_segtree>(L, M);
            link[1] = make_unique<sparse_segtree>(M, R);
            pushdown();
        }
    }

    void pushdown() {
        if (lazy) {
            child_lazy = 1;
            child_update.merge(update, range);
            update.apply(node, range);
            lazy = 0;
            update = Update();
        }
        if (child_lazy && link[0]) {
            link[0]->lazy = link[1]->lazy = true;
            link[0]->update.merge(child_update, link[0]->range);
            link[1]->update.merge(child_update, link[1]->range);
            child_lazy = 0;
            child_update = Update();
        }
    }

    void update_range(int L, int R, const Update& add) {
        pushdown();
        if (R <= range[0] || range[1] <= L) return;
        if (L <= range[0] && range[1] <= R) {
            update.merge(add, range);
            lazy = 1, pushdown();
            return;
        }
        make_children();
        link[0]->update_range(L, R, add);
        link[1]->update_range(L, R, add);
        node.merge(link[0]->node, link[1]->node);
    }

    auto query_range(int L, int R) {
        pushdown();
        if (R <= range[0] || range[1] <= L) { return Node(); }
        if (L <= range[0] && range[1] <= R) { return node; }
        make_children();
        auto a = link[0]->query_range(L, R);
        auto b = link[1]->query_range(L, R);
        Node ans;
        ans.merge(a, b);
        return ans;
    }
};

namespace samples_sparse_segtree {

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

} // namespace samples_sparse_segtree
