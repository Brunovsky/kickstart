#ifndef SEGMENT_TREE_HPP
#define SEGMENT_TREE_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXNODES       100000
#define OVERLAPS(a, b) ((a).L <= (b).R && (b).L <= (a).R)
#define CONTAINS(a, b) ((a).L <= (b).L && (b).R <= (a).R)
#define LENGTH(a)      ((a).R - (a).L + 1)

struct node_t {
    int L, R;
    int data, uplazy, downlazy;
};

struct range_t {
    int L, R;
};

bool operator==(range_t a, range_t b) { return a.L == b.L && a.R == b.R; }
bool operator<(range_t a, range_t b) { return tie(a.L, a.R) < tie(b.L, b.R); }

struct segment_tree {
    int O, Q;
    node_t tree[4 * MAXNODES];

    void setup(const vector<int>& endp) {
        clear();
        O = endp.size() - 1;
        Q = 1;
        while (Q < O)
            Q <<= 1;

        for (int i = 0; i < O; ++i) {
            tree[i + O].L = endp[i];
            tree[i + O].R = endp[i + 1] - 1;
        }
        align_leaves_to_tree();
        tree_lr();
    }

    void clear() {
        memset(tree, 0, sizeof(tree));
        O = Q = 0;
    }

    void tree_lr() {
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

    void pushup(int i) {
        // TODO: pushup data to tree[i] from tree[i << 1] and tree[i << 1 | 1]
        for (int j : {i << 1, i << 1 | 1}) {
            tree[i].uplazy += tree[j].uplazy;
            tree[j].uplazy = 0;
        }
    }

    void pushdown(int i) {
        // TODO: pushdown data from tree[i] to tree[i << 1] and tree[i << 1 | 1]
        for (int j : {i << 1, i << 1 | 1}) {
            tree[j].downlazy += tree[i].downlazy;
        }
        tree[i].downlazy = 0;
    }

    auto query_range(int i, range_t range) {
        if (OVERLAPS(range, tree[i])) {
            if (CONTAINS(range, tree[i])) {
                // TODO: query tree[i], contained in [L,R]
                return tree[i].data;
            } else {
                assert(i < O);
                pushdown(i);
                auto lhs = query_range(i << 1, range);
                auto rhs = query_range(i << 1 | 1, range);
                pushup(i);
                // TODO: aggregate results
                return lhs + rhs;
            }
        }
        return 0;
    }

    void update_range(int i, range_t range, int data) {
        if (OVERLAPS(range, tree[i])) {
            if (CONTAINS(range, tree[i])) {
                // TODO: update tree[i], contained in [L,R]
                tree[i].data += data * LENGTH(tree[i]);
                tree[i].downlazy += data;
            } else {
                assert(i < O);
                pushdown(i);
                update_range(i << 1, range, data);
                update_range(i << 1 | 1, range, data);
                pushup(i);
            }
        }
    }

    inline int to_leaf(int j) { return j < 2 * O - Q ? (j + Q) : (j + Q - O); }
    inline int to_array(int i) { return i >= Q ? (i - Q) : (i + O - Q); }
    inline void align_leaves_to_tree() { rotate(tree + O, tree + (3 * O - Q), tree); }
    inline void align_leaves_to_array() { rotate(tree + O, tree + Q, tree); }
};

// *****

struct Data {
    int L, R;
    int data;
    bool query;
};

vector<int> endp;
vector<Data> range_data;
segment_tree stree;

void driver() {
    // .....

    sort(begin(endp), end(endp));
    endp.erase(unique(begin(endp), end(endp)), end(endp));

    stree.setup(endp);
    // ready
}

void driver_usage() {
    // Case 1.
    // Multiple Inserts, 0 Queries
    // We care only about the values in the leaves
    for (auto entry : range_data) {
        stree.update_range(1, {entry.L, entry.R}, entry.data);
    }
    stree.pushdown_all();
    stree.align_leaves_to_array();
    // now query tree[O]..tree[2O] which is ordered

    // Case 2.
    // Multiple Inserts, Queries afterwards, prefix sum form exists
    for (auto entry : range_data) {
        stree.update_range(1, {entry.L, entry.R}, entry.data);
    }
    stree.pushdown_all();
    stree.align_leaves_to_array();
    // now transform tree[O]..tree[2O] data into prefix sum form
    int O = stree.O;
    stree.tree[O - 1].data = 0;
    for (int i = O; i < 2 * O; i++) {
        stree.tree[i].data += stree.tree[i - 1].data;
    }
    // now query [L,R] is tree[R] - tree[L-1].

    // Case 3.
    // Multiple Inserts, Multiple Queries, interleaved or afterwards
    for (auto entry : range_data) {
        if (entry.query) {
            stree.query_range(1, {entry.L, entry.R});
        } else {
            stree.update_range(1, {entry.L, entry.R}, entry.data);
        }
    }
    // done
}

#endif // SEGMENT_TREE_HPP
