#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Simple array-based segment tree template with range updates and range queries
 */
template <typename Node, typename Update>
struct segtree_beats {
    vector<Node> node;
    vector<Update> update;
    vector<bool> has_lazy;
    vector<array<int, 2>> range;

    segtree_beats() = default;
    segtree_beats(int L, int R) { assign(L, R); }

    template <typename Arr>
    segtree_beats(int L, int R, const Arr& arr) {
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
        if (R <= range[u][0] || range[u][1] <= L || add.can_break(node[u], range[u])) {
            return;
        }
        if (L <= range[u][0] && range[u][1] <= R && add.can_update(node[u], range[u])) {
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

namespace sample_jidriver {

struct ji_segnode {
    int sum = 0;
    int maximum = 0;
    int second_max = 0;
    int count_max = 1;
    explicit ji_segnode(int value = 0) : sum(value), maximum(value) {}
    void merge(const ji_segnode& lhs, const ji_segnode& rhs) {
        sum = lhs.sum + rhs.sum;
        maximum = max(lhs.maximum, rhs.maximum);
        if (maximum == lhs.maximum && maximum == rhs.maximum) {
            second_max = max(lhs.second_max, rhs.second_max);
            count_max = lhs.count_max + rhs.count_max;
        } else if (maximum == lhs.maximum) {
            second_max = max(lhs.second_max, rhs.maximum);
            count_max = lhs.count_max;
        } else {
            second_max = max(lhs.maximum, rhs.second_max);
            count_max = rhs.count_max;
        }
    }
};

struct ji_segupdate {
    int value = 0;
    ji_segupdate(int value = 0) : value(value) {}
    void merge(const ji_segupdate& parent, array<int, 2> /*range*/) {
        value = min(value, parent.value);
    }
    bool can_break(const ji_segnode& node, array<int, 2> /*range*/) const {
        return node.maximum <= value;
    }
    bool can_update(const ji_segnode& node, array<int, 2> /*range*/) const {
        return node.second_max < value && value < node.maximum;
    }
    void apply(ji_segnode& node, array<int, 2> /*range*/) const {
        assert(can_update(node, {}));
        node.sum -= node.count_max * (node.maximum - value);
        node.maximum = value;
    }
};

} // namespace sample_jidriver
