#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Simple sparse array-based segment tree template with range queries and point updates
 */
template <typename Node>
struct dyn_segtree {
    int L, R;
    unique_ptr<dyn_segtree> link[2];

    Node node;

    explicit dyn_segtree(int L = 0, int R = 0) : L(L), R(R) {}

    template <typename Update>
    void update(int i, const Update& add) {
        assert(L <= i && i < R);
        if (R - L == 1) {
            add.apply(node);
            return;
        }
        int M = (L + R + 1) / 2;
        if (!link[0]) {
            link[0] = make_unique<dyn_segtree>(L, M);
            link[1] = make_unique<dyn_segtree>(M, R);
        }
        link[i >= M]->update(i, add);
        node.merge(link[0]->node, link[1]->node);
    }

    auto query(int l, int r) {
        if (r <= L || R <= l)
            return Node();
        if (l <= L && R <= r)
            return node;
        Node ans;
        if (link[0]) {
            ans.merge(link[0]->query(l, r), link[1]->query(l, r));
        }
        return ans;
    }
};
