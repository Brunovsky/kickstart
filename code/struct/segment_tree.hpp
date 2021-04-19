#ifndef SEGMENT_TREE_HPP
#define SEGMENT_TREE_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

#define OVERLAPS(a, b) ((a).L < (b).R && (b).L < (a).R)
#define CONTAINS(a, b) ((a).L <= (b).L && (b).R <= (a).R)
#define LENGTH(a)      ((a).R - (a).L)

struct range_t {
    int L, R;
};

bool operator==(range_t a, range_t b) { return a.L == b.L && a.R == b.R; }
bool operator<(range_t a, range_t b) { return tie(a.L, a.R) < tie(b.L, b.R); }

struct range_segtree {
    struct node_t {
        int L, R;
        long data = 0, uplazy = 0, downlazy = 0;
    };

    int O, Q; // O = #leaf interval nodes
    vector<node_t> tree;

    void apply(int i, long v) {
        tree[i].data += v * LENGTH(tree[i]);
        tree[i].uplazy += v * LENGTH(tree[i]);
        tree[i].downlazy += v;
    }
    void pushup(int i) {
        for (int j : {i << 1, i << 1 | 1}) {
            tree[i].data += tree[j].uplazy;
            tree[i].uplazy += tree[j].uplazy;
            tree[j].uplazy = 0;
        }
    }
    void pushdown(int i) {
        for (int j : {i << 1, i << 1 | 1}) {
            tree[j].data += tree[i].downlazy * LENGTH(tree[j]);
            tree[j].downlazy += tree[i].downlazy;
        }
        tree[i].downlazy = 0;
    }

    auto query(int i, range_t range) {
        if (OVERLAPS(range, tree[i])) {
            if (CONTAINS(range, tree[i])) {
                return tree[i].data;
            } else {
                assert(i < O);
                pushdown(i);
                auto lhs = query(i << 1, range);
                auto rhs = query(i << 1 | 1, range);
                pushup(i);
                return lhs + rhs;
            }
        }
        return 0L;
    }

    void update(int i, range_t range, long v) {
        if (OVERLAPS(range, tree[i])) {
            if (CONTAINS(range, tree[i])) {
                apply(i, v);
            } else {
                assert(i < O);
                pushdown(i);
                update(i << 1, range, v);
                update(i << 1 | 1, range, v);
                pushup(i);
            }
        }
    }

    int lower_bound(long v) {
        int i = 1;
        while (i < O) {
            if (tree[i << 1].data >= v)
                i = i << 1;
            else
                v -= tree[i << 1].data, i = i << 1 | 1;
        }
        return i;
    }
    int upper_bound(long v) {
        int i = 1;
        while (i < O) {
            if (tree[i << 1].data > v)
                i = i << 1;
            else
                v -= tree[i << 1].data, i = i << 1 | 1;
        }
        return i;
    }

    range_segtree() = default;
    range_segtree(const vector<int>& endp) {
        O = endp.size() - 1, Q = 1;
        tree.assign(2 * O, {});
        while (Q < O)
            Q <<= 1;

        for (int i = 0; i < O; ++i) {
            tree[i + O].L = endp[i];
            tree[i + O].R = endp[i + 1];
        }
        align_leaves_to_tree();
        for (int i = O - 1; i > 0; i--) {
            tree[i].L = tree[i << 1].L;
            tree[i].R = tree[i << 1 | 1].R;
        }
    }

    void pushup_all() {
        for (int i = O - 1; i > 0; i--) {
            pushup(i);
        }
    }
    void pushdown_all() {
        for (int i = 1; i < O; i++) {
            pushdown(i);
        }
    }

    int to_leaf(int j) const { return j < 2 * O - Q ? (j + Q) : (j + Q - O); }
    int to_array(int i) const { return i >= Q ? (i - Q) : (i + O - Q); }
    void align_leaves_to_tree() {
        rotate(begin(tree) + O, begin(tree) + (3 * O - Q), begin(tree) + 2 * O);
    }
    void align_leaves_to_array() {
        rotate(begin(tree) + O, begin(tree) + Q, begin(tree) + 2 * O);
    }
};

struct dyn_segtree {
    int L, R;
    unique_ptr<dyn_segtree> link[2];

    long data = 0;

    void apply(long v) { data += v; }
    auto merge(long a, long b) { return a + b; }

    explicit dyn_segtree(int L = 0, int R = 0) : L(L), R(R) {}

    void update(int i, long v) {
        assert(L <= i && i < R);
        if (R - L == 1) {
            return apply(v);
        }
        int M = (L + R + 1) / 2;
        if (!link[0]) {
            link[0] = make_unique<dyn_segtree>(L, M);
            link[1] = make_unique<dyn_segtree>(M, R);
        }
        link[i >= M]->update(i, v);
        data = merge(link[0]->data, link[1]->data);
    }

    auto query(int l, int r) {
        if (r <= L || R <= l)
            return 0L;
        if (l <= L && R <= r)
            return data;
        return link[0] ? merge(link[0]->query(l, r), link[1]->query(l, r)) : 0L;
    }
};

// *****

#endif // SEGMENT_TREE_HPP
